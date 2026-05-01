#ifndef ALLEGRORENDERER_HPP
#define ALLEGRORENDERER_HPP

#include "../../Rectangle.hpp"
#include "../../Typedefs.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <string>

namespace ValyrianEngine {

// The "Truth" for Allegro backend
class Texture {
public:
	ALLEGRO_BITMAP* handle{nullptr};
};

class InternalDisplay {
public:
	ALLEGRO_DISPLAY* handle{nullptr};
};

class InternalTimer {
public:
	ALLEGRO_TIMER* handle{nullptr};
};

class InternalFont {
public:
	ALLEGRO_FONT* handle{nullptr};
};

class InternalEventQueue {
public:
	ALLEGRO_EVENT_QUEUE* handle{nullptr};
};

union Event {
	ALLEGRO_EVENT_TYPE type;
	ALLEGRO_ANY_EVENT any;
	ALLEGRO_DISPLAY_EVENT display;
	ALLEGRO_KEYBOARD_EVENT keyboard;
	ALLEGRO_MOUSE_EVENT mouse;
	ALLEGRO_TIMER_EVENT timer;
	ALLEGRO_TOUCH_EVENT touch;
	ALLEGRO_USER_EVENT user;

	bool isTimerSource(Timer t) const {
		return t && t->handle && timer.source == t->handle;
	}
};

namespace EventType {
constexpr uint32_t TIMER = ALLEGRO_EVENT_TIMER;
constexpr uint32_t KEY_DOWN = ALLEGRO_EVENT_KEY_DOWN;
constexpr uint32_t KEY_UP = ALLEGRO_EVENT_KEY_UP;
constexpr uint32_t DISPLAY_CLOSE = ALLEGRO_EVENT_DISPLAY_CLOSE;
} // namespace EventType

namespace KeyCode {
constexpr int UP = ALLEGRO_KEY_UP;
constexpr int DOWN = ALLEGRO_KEY_DOWN;
constexpr int LEFT = ALLEGRO_KEY_LEFT;
constexpr int RIGHT = ALLEGRO_KEY_RIGHT;
constexpr int ESCAPE = ALLEGRO_KEY_ESCAPE;
constexpr int ENTER = ALLEGRO_KEY_ENTER;
constexpr int PAD_ENTER = ALLEGRO_KEY_PAD_ENTER;
constexpr int SPACE = ALLEGRO_KEY_SPACE;
constexpr int LSHIFT = ALLEGRO_KEY_LSHIFT;
constexpr int G = ALLEGRO_KEY_G;
constexpr int W = ALLEGRO_KEY_W;
constexpr int A = ALLEGRO_KEY_A;
constexpr int S = ALLEGRO_KEY_S;
constexpr int D = ALLEGRO_KEY_D;
} // namespace KeyCode

class AllegroRenderer {
public:
	AllegroRenderer() = default;

	// Rendering Operations
	void drawBitmap(Bitmap b, float x, float y, Flip flip = Flip::NONE);
	void drawScaledBitmap(Bitmap b, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh,
	                      Flip flip = Flip::NONE);
	void drawBitmapRegion(Bitmap b, float sx, float sy, float sw, float sh, float dx, float dy, Flip flip = Flip::NONE);
	void drawText(Font f, Color c, float x, float y, Flip flip, const char* text);
	void drawRectangle(float x1, float y1, float x2, float y2, Color c, float thickness);
	void clear(Color c);
	void clearCurrentBitmap(Color c);
	void setTargetBitmap(Bitmap b);
	void updateDisplay();
	void holdDrawing(bool hold);

	int getBitmapWidth(Bitmap b);
	int getBitmapHeight(Bitmap b);

	// Resource Management
	Bitmap createBitmap(int w, int h);
	void destroyBitmap(Bitmap b);
	Bitmap loadBitmap(const std::string& path);
	Font loadFont(const std::string& path, int size, int flags = 0);
	void destroyFont(Font f);

	// Offscreen rendering helpers — save/restore current render target by raw handle
	void* saveCurrentTarget() const;
	void restoreTarget(void* savedHandle);

	// Extended draw operation for rotating sprites
	void drawScaledRotatedBitmap(Bitmap b, float cx, float cy, float dx, float dy, float xscale, float yscale,
	                             float angle, Flip flip = Flip::NONE);

	// System Initialization
	void initSystem();
	void initAudio(void** outVoice, void** outMixer);
	void destroyAudio(void* voice, void* mixer);

	// Display / Window
	Display createDisplay(int width, int height, bool fullscreen = false);
	void destroyDisplay(Display d);
	void makeDisplayCurrentBitmap(Display d);
	void setDisplayIcon(Display d, Bitmap icon);
	void getMonitorSize(int monitorIndex, int* outWidth, int* outHeight);

	// Timers
	Timer createTimer(double fps);
	void destroyTimer(Timer t);
	void startTimer(Timer t);

	// Event Queue
	EventQueue createEventQueue();
	void destroyEventQueue(EventQueue eq);
	void registerEventSources(EventQueue eq, Display d, Timer fpsTimer, Timer logicTimer);
	void waitForEvent(EventQueue eq, Event* ev);

	// Helper for Color conversion
	static ALLEGRO_COLOR ToAllegroColor(Color c) {
		return al_map_rgba(c.r, c.g, c.b, c.a);
	}
	static int ToAllegroFlags(Flip flip) {
		int flags = 0;
		if(flip == Flip::HORIZONTAL)
			flags |= ALLEGRO_FLIP_HORIZONTAL;
		if(flip == Flip::VERTICAL)
			flags |= ALLEGRO_FLIP_VERTICAL;
		if(flip == Flip::BOTH)
			flags |= (ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL);
		return flags;
	}
};

} // namespace ValyrianEngine

#endif
