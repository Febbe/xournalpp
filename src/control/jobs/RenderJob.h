/*
 * Xournal++
 *
 * A job which redraws a page or a page region
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

class Rectangle;
class XojPageView;

struct RenderJob : public Job
{
	using pointer = std::shared_ptr<RenderJob>;

	static pointer create(XojPageView* view)
	{
		return pointer{new RenderJob{view}};
	}

	~RenderJob() override;
protected:
	explicit RenderJob(XojPageView* view);

public:
	JobType getType() const override;

	void* getSource() const override;

	void run() override;

private:
	/**
	 * Repaint the widget in UI Thread
	 */
	void repaintWidget(GtkWidget* widget);

	void rerenderRectangle(Rectangle* rect);

private:
	XOJ_TYPE_ATTRIB;

	XojPageView* view;
};
