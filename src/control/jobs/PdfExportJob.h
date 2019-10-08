/*
 * Xournal++
 *
 * A job to export PDF
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include "BaseExportJob.h"

struct PdfExportJob : public BaseExportJob
{
	using pointer = std::shared_ptr<PdfExportJob>;

	static pointer create(Control* control)
	{
		return pointer{new PdfExportJob{control}};
	}

	virtual ~PdfExportJob();

	void run();

protected:

	PdfExportJob(Control* control);
	virtual void addFilterToDialog();
	virtual bool isUriValid(string& uri);

private:
	XOJ_TYPE_ATTRIB;
};
