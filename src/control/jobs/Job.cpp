#include "Job.h"

#include <gtk/gtk.h>

Job::Job()
{
	XOJ_INIT_TYPE(Job);
}

Job::~Job()
{
	XOJ_CHECK_TYPE(Job);

	XOJ_RELEASE_TYPE(Job);
}

void Job::execute()
{
	XOJ_CHECK_TYPE(Job);

	this->run();
}

void* Job::getSource() const
{
	XOJ_CHECK_TYPE(Job);

	return NULL;
}

bool Job::callAfterCallback(Job* job)
{
	XOJ_CHECK_TYPE_OBJ(job, Job);

	job->afterRun();
	job->afterRunId = 0;
	return false; // do not call again
}

void Job::callAfterRun()
{
	XOJ_CHECK_TYPE(Job);

	if (this->afterRunId)
	{
		return;
	}

	this->afterRunId = gdk_threads_add_idle((GSourceFunc) Job::callAfterCallback, this);
}

/**
 * After run will be called from UI Thread after the Job is finished
 *
 * All UI Stuff should happen here
 */
void Job::afterRun()
{
}
