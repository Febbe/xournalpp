/*
 * Xournal++
 *
 * A job which handles preview repaint
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

class SidebarPreviewBaseEntry;
class Document;

/**
 * @brief A Job which renders a SidebarPreviewPage
 */
struct PreviewJob : public Job
{
	using pointer = std::shared_ptr<PreviewJob>;

	static pointer create(SidebarPreviewBaseEntry* sidebar)
	{
		return pointer{new PreviewJob{sidebar}};
	}

	~PreviewJob() override;

	void* getSource() const override;

	void run() override;

	JobType getType() const override;

protected:
	explicit PreviewJob(SidebarPreviewBaseEntry* sidebar);

private:
	void initGraphics();
	void drawBorder();
	void finishPaint();
	void drawBackgroundPdf(Document* doc);
	void drawPage(int layer);

private:
	XOJ_TYPE_ATTRIB;

	/**
	 * Graphics buffer
	 */
	cairo_surface_t* crBuffer = NULL;

	/**
	 * Graphics drawing
	 */
	cairo_t* cr2 = NULL;

	/**
	 * Zoom factor
	 */
	double zoom = 0;

	/**
	 * Sidebar preview
	 */
	SidebarPreviewBaseEntry* sidebarPreview = NULL;
};
