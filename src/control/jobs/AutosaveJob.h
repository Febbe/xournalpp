/*
 * Xournal++
 *
 * Autosave job
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include "Job.h"

#include <XournalType.h>

class Control;

struct AutosaveJob : public Job
{
	using pointer = std::shared_ptr<AutosaveJob>;
	virtual ~AutosaveJob();

	static pointer create(Control* control)
	{
		new pointer{new AutosaveJob{control}};
	}

protected:
	AutosaveJob(Control* control);
public:
	virtual void run();
	void afterRun();

	JobType getType() const override;

private:
	XOJ_TYPE_ATTRIB;


	Control* control = NULL;
	string error;
};
