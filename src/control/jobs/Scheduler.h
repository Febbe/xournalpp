/*
 * Xournal++
 *
 * A scheduler for background jobs
 *
 * Some code from Evince project
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include "Job.h"
#include <XournalType.h>

#include <mutex>
#include <atomic>
#include <condition_variable>
#include <deque>

/**
 * @file Scheduler.h
 * @brief A file containing the defintion of the Scheduler
*/

/**
 * @enum JobPriority
 *
 * The priority of the job affects the order of execution:
 * Jobs with higher priority are processed before the
 * lower ones.
 */
enum JobPriority
{
	/**
	 * Urgent: used for rendering the current page
	 */
	JOB_PRIORITY_URGENT,

	/**
	 * High: used for rendering thumbnail ranges
	 */
	JOB_PRIORITY_HIGH,

	/**
	 * Low: used for rendering of pages not in the current range
	 */
	JOB_PRIORITY_LOW,

	/**
	 * None: used for any other job (loading / saving / printing...)
	 */
	JOB_PRIORITY_NONE,

	/**
	 * The number of priorities
	 */
	JOB_N_PRIORITIES
};


class Scheduler
{
public:
	Scheduler(std::string name = "scheduler");
	virtual ~Scheduler();

public:
	/**
	 * Adds a Job to the Scheduler
	 *
	 * @param Job      the job
	 * @param priority the desired priority
	 *
	 * The Job is now owned by the scheduler, and automatically freed if it is done
	 */
	void addJob(Job::pointer job, JobPriority priority);

	void start();
	void stop();

	/**
	 * Locks the complete scheduler
	 */
	std::unique_lock<std::mutex> aquire_lock();

	/**
	 * Unlocks the complete scheduler
	 */
	void unlock(std::unique_lock<std::mutex> lifetime_guard);

	/**
	 * Don't render the next X ms so the scrolling performance is better
	 */
	void blockRerenderZoom();

	/**
	 * Remove the blocked rendering manually
	 */
	void unblockRerenderZoom();

private:
	static gpointer jobThreadCallback(Scheduler* scheduler);

	inline Job::pointer getNextJobUnlocked(bool onlyNotRender = false);
	Job::pointer getNextJobUnlocked(bool onlyNotRender, bool& hasRenderJobs);

	static bool jobRenderThreadTimer(Scheduler* scheduler);

protected:
	XOJ_TYPE_ATTRIB;

	std::atomic<bool> threadRunning{true};

	int jobRenderThreadTimerId = 0;

	GThread* thread = NULL;

	std::atomic<bool> cond{false};
	std::condition_variable jobQueueCond{};
	std::mutex jobQueueMutex;
	std::mutex schedulerMutex;

	/**
	 * This is need to be sure there is no job running if we delete a page, else we may access delete memory...
	 */
	std::mutex jobRunningMutex;

	std::array<std::deque<Job::pointer>, JOB_N_PRIORITIES> jobQueue;

	GTimeVal* blockRenderZoomTime = NULL;
	std::mutex blockRenderMutex;
	std::string name = "scheduler";
};
