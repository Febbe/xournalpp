#include <algorithm>
#include "XournalScheduler.h"

#include "PreviewJob.h"
#include "RenderJob.h"

XournalScheduler::XournalScheduler()
		: Scheduler()
{
	XOJ_INIT_TYPE(XournalScheduler);
}

XournalScheduler::~XournalScheduler()
{
	XOJ_RELEASE_TYPE(XournalScheduler);
}

void XournalScheduler::removeSidebar(SidebarPreviewBaseEntry* preview)
{
	XOJ_CHECK_TYPE(XournalScheduler);

	removeSource(preview, JOB_TYPE_PREVIEW, JOB_PRIORITY_HIGH);
}

void XournalScheduler::removePage(XojPageView* view)
{
	XOJ_CHECK_TYPE(XournalScheduler);

	removeSource(view, JOB_TYPE_RENDER, JOB_PRIORITY_URGENT);
}

void XournalScheduler::removeAllJobs()
{
	XOJ_CHECK_TYPE(XournalScheduler);
	std::lock_guard<std::mutex> _{this->jobQueueMutex};
	jobQueue = {};
}

/*[[nodiscard]]*/ std::unique_lock<std::mutex> XournalScheduler::finishTask()
{
	XOJ_CHECK_TYPE(XournalScheduler);
	return std::unique_lock<std::mutex>{this->jobRunningMutex};
}

void XournalScheduler::removeSource(void* source, JobType type, JobPriority priority)
{
	XOJ_CHECK_TYPE(XournalScheduler);

	std::lock_guard<std::mutex> _{this->jobQueueMutex};

	std::remove_if(begin(this->jobQueue[priority]), end(this->jobQueue[priority]),
	               [type, source](Job::pointer const& job)
	               {
		               return job->getType() == type && job->getSource() == source;
	               });

	// wait until the last job is done
	// we can be sure we don't access "source"
	finishTask();
}

bool XournalScheduler::existsSource(void* source, JobType type, JobPriority priority)
{
	XOJ_CHECK_TYPE(XournalScheduler);

	std::lock_guard<std::mutex> _{this->jobQueueMutex};
	return std::find_if(begin(this->jobQueue[priority]), end(this->jobQueue[priority]),
	                    [type, source](Job::pointer const& job)
	                    {
		                    return job->getType() == type && job->getSource() == source;
	                    }) != end(this->jobQueue[priority]);
}

void XournalScheduler::addRepaintSidebar(SidebarPreviewBaseEntry* preview)
{
	XOJ_CHECK_TYPE(XournalScheduler);

	if (existsSource(preview, JOB_TYPE_PREVIEW, JOB_PRIORITY_HIGH))
	{
		return;
	}

	auto job = PreviewJob::create(preview);
	addJob(std::move(job), JOB_PRIORITY_HIGH);
}

void XournalScheduler::addRerenderPage(XojPageView* view)
{
	XOJ_CHECK_TYPE(XournalScheduler);

	if (existsSource(view, JOB_TYPE_RENDER, JOB_PRIORITY_URGENT))
	{
		return;
	}

	auto job = RenderJob::create(view);
	addJob(std::move(job), JOB_PRIORITY_URGENT);
}
