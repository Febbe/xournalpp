/*
 * Xournal++
 *
 * Base class for Exports
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include "BlockingJob.h"

#include <PathUtil.h>
#include <XournalType.h>

class Control;

struct BaseExportJob : public BlockingJob
{
	using pointer = std::shared_ptr<BaseExportJob>;

	virtual ~BaseExportJob();

	virtual void afterRun();

	virtual bool showFilechooser();

	string getFilterName();

protected:

	BaseExportJob(Control* control, string name);

	void initDialog();
	virtual void addFilterToDialog() = 0;
	void addFileFilterToDialog(string name, string pattern);
	bool checkOverwriteBackgroundPDF(Path& filename);
	virtual bool isUriValid(string& uri);

private:
	XOJ_TYPE_ATTRIB;

protected:
	GtkWidget* dialog = NULL;

	Path filename;

	/**
	 * Error message to show to the user
	 */
	string errorMsg;

	class ExportType
	{
	public:
		string extension;
		bool withoutBackground;

		ExportType(string ext, bool hideBg)
		 : extension(ext),
		   withoutBackground(hideBg)
		{
		}
	};
};
