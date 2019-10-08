/*
 * Xournal++
 *
 * A customized export
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include "BaseExportJob.h"
#include "ImageExport.h"

#include "view/DocumentView.h"

#include <PageRange.h>
#include <i18n.h>
#include <map>


struct CustomExportJob : public BaseExportJob
{
	using pointer = std::shared_ptr<CustomExportJob>;

	static pointer create(Control* control)
	{
		return pointer{new CustomExportJob{control}};
	}

	virtual ~CustomExportJob();

	void run();

	virtual bool showFilechooser();

protected:

	CustomExportJob(Control* control);
	virtual void afterRun();

	virtual void addFilterToDialog();

	/**
	 * Create one Graphics file per page
	 */
	void exportGraphics();

	virtual bool isUriValid(string& uri);

private:
	XOJ_TYPE_ATTRIB;

	/**
	 * The range to export
	 */
	PageRangeVector exportRange;

	/**
	 * PNG dpi
	 */
	int pngDpi = 300;

	/**
	 * Export graphics format
	 */
	ExportGraphicsFormat format = EXPORT_GRAPHICS_UNDEFINED;

	/**
	 * XOJ Export, else PNG Export
	 */
	bool exportTypeXoj = false;

	string lastError;

	string chosenFilterName;

	std::map<string, ExportType*> filters;
};
