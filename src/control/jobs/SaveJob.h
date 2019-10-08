/*
 * Xournal++
 *
 * A job which saves a Document
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include "BlockingJob.h"

#include <XournalType.h>

struct SaveJob : public BlockingJob
{
	using pointer = std::shared_ptr<SaveJob>;

	static pointer create(Control* control)
	{
		return pointer{new SaveJob{control}};
	}

	virtual ~SaveJob();

	virtual void run();

	bool save();

	static void updatePreview(Control* control);

protected:
	SaveJob(Control* control);

	virtual void afterRun() const;

private:
	XOJ_TYPE_ATTRIB;

	string lastError;
};
