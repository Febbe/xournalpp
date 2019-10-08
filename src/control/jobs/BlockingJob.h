/*
 * Xournal++
 *
 * A job which is done in the GTK main thread, but the application is blocked
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include "Job.h"

#include <XournalType.h>

#include <gtk/gtk.h>

class Control;

struct BlockingJob : public Job
{
	using pointer = std::shared_ptr<BlockingJob>;

	virtual ~BlockingJob();

	void execute();

	JobType getType() const override;

protected:

	BlockingJob(Control* control, string name);

	static bool finished(Control* control);

private:
	XOJ_TYPE_ATTRIB;

protected:
	Control* control;
};
