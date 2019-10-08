#include "Scheduler.h"
#include <config-debug.h>

#include <inttypes.h>
#include <algorithm>
//#include <hwloc/helper.h>

#ifdef DEBUG_SHEDULER
#define SDEBUG g_message
#else
#define SDEBUG(msg, ...)
#endif

Scheduler::Scheduler(std::string name) : name(std::move(name))
{
	XOJ_INIT_TYPE(Scheduler);
}

Scheduler::~Scheduler()
{
	XOJ_CHECK_TYPE(Scheduler);

	SDEBUG("Destroy scheduler");

	if (this->jobRenderThreadTimerId)
	{
		g_source_remove(this->jobRenderThreadTimerId);
		this->jobRenderThreadTimerId = 0;
	}

	stop();

	if (this->blockRenderZoomTime)
	{
		g_free(this->blockRenderZoomTime);
	}

	XOJ_RELEASE_TYPE(Scheduler);
}

void Scheduler::start()
{
	SDEBUG("Starting scheduler");
	g_return_if_fail(this->thread == NULL);

	this->thread = g_thread_new(name.c_str(), (GThreadFunc) jobThreadCallback, this);
}

void Scheduler::stop()
{
	SDEBUG("Stopping scheduler");

	if (!this->threadRunning)
	{
		return;
	}
	this->threadRunning = false;
	this->cond = true;
	this->jobQueueCond.notify_all();

	if (this->thread)
	{
		g_thread_join(this->thread);
	}
}

void Scheduler::addJob(std::shared_ptr<Job> job, JobPriority priority)
{
	std::lock_guard<std::mutex> _{this->jobQueueMutex};
	XOJ_CHECK_TYPE(Scheduler);
	SDEBUG("Adding job...");

	this->jobQueue[priority].emplace_back(std::move(job));
	this->cond = true;
	this->jobQueueCond.notify_all();

	SDEBUG("add job: %" PRId64, (uint64_t) job);
}

inline Job::pointer Scheduler::getNextJobUnlocked(bool onlyNotRender)
{
	bool discard{};
	return getNextJobUnlocked(onlyNotRender, discard);
}

Job::pointer Scheduler::getNextJobUnlocked(bool onlyNotRender, bool& hasRenderJobs)
{
	XOJ_CHECK_TYPE(Scheduler);

	Job::pointer returner;

	for (auto&& queue :this->jobQueue)
	{
		auto iter = begin(queue);
		if (!returner)
		{
			iter = std::find_if(iter, end(queue), [onlyNotRender, &hasRenderJobs](Job::pointer const& job)
			{
				return !(onlyNotRender && (hasRenderJobs |= (job->getType() == JOB_TYPE_RENDER)));
			});
			if (iter == end(queue)) continue;
			returner = std::move(*iter);
			iter = queue.erase(iter);
		}

		hasRenderJobs |= (end(queue) != std::find_if(iter, end(queue), [](Job::pointer const& job)
		{
			return job->getType() == JOB_TYPE_RENDER;
		}));
	}

	return returner;
}

/**
 * Locks the complete scheduler
 */
std::unique_lock<std::mutex> Scheduler::aquire_lock()
{
	XOJ_CHECK_TYPE(Scheduler);

	return std::unique_lock<std::mutex>{this->schedulerMutex};
}

/**
 * Unlocks the complete scheduler
 */
void Scheduler::unlock(std::unique_lock<std::mutex> ignore)
{
	XOJ_CHECK_TYPE(Scheduler);
	(void) ignore;
}

constexpr auto ZOOM_WAIT_US_TIMEOUT = 300000; // 0.3s

void Scheduler::blockRerenderZoom()
{
	XOJ_CHECK_TYPE(Scheduler);

	std::lock_guard<std::mutex> _{this->blockRenderMutex};

	if (this->blockRenderZoomTime == NULL)
	{
		this->blockRenderZoomTime = g_new(GTimeVal, 1);
	}

	g_get_current_time(this->blockRenderZoomTime);
	g_time_val_add(this->blockRenderZoomTime, ZOOM_WAIT_US_TIMEOUT);
}

void Scheduler::unblockRerenderZoom()
{
	XOJ_CHECK_TYPE(Scheduler);
	{ //lock guard keep code block!
		std::lock_guard<std::mutex> _{this->blockRenderMutex};

		g_free(this->blockRenderZoomTime);
		this->blockRenderZoomTime = NULL;
		if (this->jobRenderThreadTimerId)
		{
			g_source_remove(this->jobRenderThreadTimerId);
			this->jobRenderThreadTimerId = 0;
		}
	}
	this->cond = true;
	this->jobQueueCond.notify_all();
}

/**
 * g_time_val_diff:
 * @t1: time value t1
 * @t2: time value t2
 *
 * Calculates the time difference between t1 and t2 in milliseconds.
 * The result is positive if t1 is later than t2.
 *
 * Returns:
 * Time difference in microseconds
 */
glong g_time_val_diff(GTimeVal* t1, GTimeVal* t2)
{
	g_assert(t1);
	g_assert(t2);
	return ((t1->tv_sec - t2->tv_sec) * G_USEC_PER_SEC + (t1->tv_usec - t2->tv_usec)) / 1000;
}

/**
 * If the Scheduler is blocking because we are zooming and there are only render jobs
 * we need to wakeup it later
 */
bool Scheduler::jobRenderThreadTimer(Scheduler* scheduler)
{
	XOJ_CHECK_TYPE_OBJ(scheduler, Scheduler);

	scheduler->jobRenderThreadTimerId = 0;

	{
		std::lock_guard<std::mutex> _{scheduler->blockRenderMutex};
		g_free(scheduler->blockRenderZoomTime);
		scheduler->blockRenderZoomTime = NULL;
	}

	scheduler->cond = true;
	scheduler->jobQueueCond.notify_all();
	//g_cond_broadcast(&scheduler->jobQueueCond);

	return false;
}

gpointer Scheduler::jobThreadCallback(Scheduler* scheduler)
{
	XOJ_CHECK_TYPE_OBJ(scheduler, Scheduler);

	while (scheduler->threadRunning)
	{
		// lock the whole scheduler
		// Todo: cpp17 replace with std::scoped_lock
		std::unique_lock<std::mutex> sched_m_guard{scheduler->schedulerMutex, std::defer_lock};
		std::unique_lock<std::mutex> rend_m_guard{scheduler->blockRenderMutex, std::defer_lock};
		std::lock(sched_m_guard, rend_m_guard);
		bool onlyNoneRenderJobs = false;
		glong diff = 1000;
		if (scheduler->blockRenderZoomTime)
		{
			GTimeVal time;
			g_get_current_time(&time);

			diff = g_time_val_diff(scheduler->blockRenderZoomTime, &time);
			if (diff <= 0)
			{
				g_free(scheduler->blockRenderZoomTime);
				scheduler->blockRenderZoomTime = NULL;
			}
			else
			{
				onlyNoneRenderJobs = true;
			}
		}
		rend_m_guard.unlock();

		std::unique_lock<std::mutex> job_queue_m_guard{scheduler->jobQueueMutex};
		bool hasOnlyRenderJobs = false;
		Job::pointer job = scheduler->getNextJobUnlocked(onlyNoneRenderJobs, hasOnlyRenderJobs);
		if (job)
		{
			hasOnlyRenderJobs = false;
		}

		SDEBUG("get job: %" PRId64, (uint64_t) job);

		if (!job)
		{
			// unlock the whole scheduler
			sched_m_guard.unlock();

			if (hasOnlyRenderJobs)
			{
				if (scheduler->jobRenderThreadTimerId)
				{
					g_source_remove(scheduler->jobRenderThreadTimerId);
				}
				scheduler->jobRenderThreadTimerId = g_timeout_add(diff, (GSourceFunc) jobRenderThreadTimer, scheduler);
			}

			while (!scheduler->cond)
			{
				scheduler->jobQueueCond.wait(job_queue_m_guard);
			}
			//Todo check this
			scheduler->cond = false;
			//implicid wait and scope: job_queue_m_guard.unlock()
			continue;
		}

		SDEBUG("do job: %" PRId64, (uint64_t) job);

		job_queue_m_guard.unlock();

		std::lock_guard<std::mutex> _jrm_guard(scheduler->jobRunningMutex);

		job->execute();
		//scope unlock jobRunningMutex "_jrm_guard"
		//scope unlock the whole scheduler
		SDEBUG("next");
	}

	SDEBUG("finished");

	return NULL;
}
