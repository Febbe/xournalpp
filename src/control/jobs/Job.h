/*
 * Xournal++
 *
 * A job which is done
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include <XournalType.h>
#include <memory>

enum JobType
{
	JOB_TYPE_BLOCKING, JOB_TYPE_PREVIEW, JOB_TYPE_RENDER, JOB_TYPE_AUTOSAVE
};

struct Job : public std::enable_shared_from_this<Job>
{
	using pointer = std::shared_ptr<Job>;

	virtual JobType getType() const = 0;

	/**
	 * this method is called
 	 */
	virtual void execute();

	virtual void* getSource() const;

protected:
	/**
	 * Virtual class..., childs shall only construct it via a create function
	 */
	Job();
	virtual ~Job();

	/**
	 * override this method
	 */
	virtual void run() = 0;

	/**
	 * This method should be called as _last_ operation in run
	 *
	 * If you call it in another position the object will be deleted before run is finished!
	 */
	void callAfterRun();

	/**
	 * After run will be called from UI Thread after the Job is finished
	 *
	 * All UI Stuff should happen here
	 */
	virtual void afterRun();

private:
	static bool callAfterCallback(Job* job);

private:
	XOJ_TYPE_ATTRIB;
	int afterRunId = 0;
};
