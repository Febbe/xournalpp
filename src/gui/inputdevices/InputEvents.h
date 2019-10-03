/*
 * Xournal++
 *
 * [Header description]
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include <glib.h>
#include <gdk/gdk.h>
#include "control/settings/Settings.h"
#include "model/Point.h"

#include <memory>

enum InputEventType
{
	UNKNOWN,
	BUTTON_PRESS_EVENT,
	BUTTON_2_PRESS_EVENT,
	BUTTON_3_PRESS_EVENT,
	BUTTON_RELEASE_EVENT,
	MOTION_EVENT,
	ENTER_EVENT,
	LEAVE_EVENT,
	PROXIMITY_IN_EVENT,
	PROXIMITY_OUT_EVENT,
	SCROLL_EVENT,
	GRAB_BROKEN_EVENT,
	KEY_PRESS_EVENT,
	KEY_RELEASE_EVENT
};

enum InputDeviceClass
{
	INPUT_DEVICE_MOUSE,
	INPUT_DEVICE_PEN,
	INPUT_DEVICE_ERASER,
	INPUT_DEVICE_TOUCHSCREEN,
	INPUT_DEVICE_KEYBOARD,
	INPUT_DEVICE_IGNORE
};

inline void deleter(GdkEvent* p)
{
	gdk_event_free(p);
};

struct InputEvent
{
	void bindGdkEvent(GdkEvent* gep);
	GdkEvent* gdkEvent();

	InputEventType type = UNKNOWN;
	InputDeviceClass deviceClass = INPUT_DEVICE_IGNORE;

	gchar* deviceName = nullptr;
	gdouble absoluteX = 0;

	gdouble absoluteY = 0;
	gdouble relativeX = 0;
	gdouble relativeY = 0;
	guint button = 0;

	GdkModifierType state = (GdkModifierType) 0;
	gdouble pressure = Point::NO_PRESSURE;
	GdkEventSequence* sequence = nullptr;

	guint32 timestamp = 0;

private:
	struct GdkEventGuard
	{
		static inline GdkEvent* _safeInit(GdkEvent* p)
		{
			return p ? gdk_event_copy(p) : p;
		}
		GdkEventGuard() = default;
		explicit GdkEventGuard(GdkEvent* p)
		 : impl{_safeInit(p), &deleter}
		{
		}
		GdkEventGuard(GdkEventGuard const& g)
		 : impl{_safeInit(g.impl.get()), &deleter}
		{
		}
		GdkEventGuard(GdkEventGuard&&) = default;
		~GdkEventGuard() = default;
		GdkEventGuard& operator=(GdkEventGuard&&) = default;
		GdkEventGuard& operator=(GdkEventGuard const& o)
		{
			impl.reset(_safeInit(o.impl.get()));
			return *this;
		}
		GdkEventGuard& operator=(GdkEvent* p)
		{
			impl.reset(_safeInit(p));
			return *this;
		}
		std::unique_ptr<GdkEvent, decltype(&deleter)> impl{nullptr, &deleter};
	} sourceEvent{};
};

class InputEvents
{

	static InputEventType translateEventType(GdkEventType type);

public:

	static InputDeviceClass translateDeviceType(GdkDevice* device, Settings* settings);
	static InputDeviceClass translateDeviceType(const string& name, GdkInputSource source, Settings* settings);

	static InputEvent* translateEvent(GdkEvent* sourceEvent, Settings* settings);
};


