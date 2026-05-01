#include "Backends/Allegro/AllegroRenderer.hpp"
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <cassert>
#include <cstdlib>

#ifdef _WIN32
#include <allegro5/allegro_direct3d.h>
#endif

namespace ValyrianEngine {

void AllegroRenderer::drawBitmap(Bitmap b, float x, float y, Flip flip) {
	if(b && b->handle) {
		al_draw_bitmap(b->handle, x, y, ToAllegroFlags(flip));
	}
}

void AllegroRenderer::drawScaledBitmap(Bitmap b, float sx, float sy, float sw, float sh, float dx, float dy, float dw,
                                       float dh, Flip flip) {
	if(b && b->handle) {
		al_draw_scaled_bitmap(b->handle, sx, sy, sw, sh, dx, dy, dw, dh, ToAllegroFlags(flip));
	}
}

void AllegroRenderer::drawBitmapRegion(Bitmap b, float sx, float sy, float sw, float sh, float dx, float dy,
                                       Flip flip) {
	if(b && b->handle) {
		al_draw_bitmap_region(b->handle, sx, sy, sw, sh, dx, dy, ToAllegroFlags(flip));
	}
}

void AllegroRenderer::drawText(Font f, Color c, float x, float y, Flip flip, const char* text) {
	if(f && f->handle && text) {
		al_draw_text(f->handle, ToAllegroColor(c), x, y, ToAllegroFlags(flip), text);
	}
}

void AllegroRenderer::drawRectangle(float x1, float y1, float x2, float y2, Color c, float thickness) {
	al_draw_rectangle(x1, y1, x2, y2, ToAllegroColor(c), thickness);
}

void AllegroRenderer::clear(Color c) {
	al_clear_to_color(ToAllegroColor(c));
}

void AllegroRenderer::clearCurrentBitmap(Color c) {
	al_clear_to_color(ToAllegroColor(c));
}

void AllegroRenderer::setTargetBitmap(Bitmap b) {
	if(b && b->handle) {
		al_set_target_bitmap(b->handle);
	}
}

void AllegroRenderer::updateDisplay() {
	al_flip_display();
}

void AllegroRenderer::holdDrawing(bool hold) {
	al_hold_bitmap_drawing(hold);
}

int AllegroRenderer::getBitmapWidth(Bitmap b) {
	return (b && b->handle) ? al_get_bitmap_width(b->handle) : 0;
}

int AllegroRenderer::getBitmapHeight(Bitmap b) {
	return (b && b->handle) ? al_get_bitmap_height(b->handle) : 0;
}

Bitmap AllegroRenderer::createBitmap(int w, int h) {
	ALLEGRO_BITMAP* handle = al_create_bitmap(w, h);
	if(!handle)
		return nullptr;
	Texture* t = new Texture();
	t->handle = handle;
	return t;
}

void AllegroRenderer::destroyBitmap(Bitmap b) {
	if(b) {
		if(b->handle) {
			al_destroy_bitmap(b->handle);
		}
		delete b;
	}
}

Bitmap AllegroRenderer::loadBitmap(const std::string& path) {
	ALLEGRO_BITMAP* handle = al_load_bitmap(path.c_str());
	if(!handle)
		return nullptr;
	Texture* t = new Texture();
	t->handle = handle;
	return t;
}

Font AllegroRenderer::loadFont(const std::string& path, int size, int flags) {
	ALLEGRO_FONT* handle = al_load_ttf_font(path.c_str(), size, flags);
	if(!handle)
		return nullptr;
	InternalFont* f = new InternalFont();
	f->handle = handle;
	return f;
}

void AllegroRenderer::destroyFont(Font f) {
	if(f) {
		if(f->handle) {
			al_destroy_font(f->handle);
		}
		delete f;
	}
}

void* AllegroRenderer::saveCurrentTarget() const {
	return al_get_target_bitmap();
}

void AllegroRenderer::restoreTarget(void* savedHandle) {
	if(savedHandle)
		al_set_target_bitmap(static_cast<ALLEGRO_BITMAP*>(savedHandle));
}

void AllegroRenderer::drawScaledRotatedBitmap(Bitmap b, float cx, float cy, float dx, float dy, float xscale,
                                              float yscale, float angle, Flip flip) {
	if(b && b->handle)
		al_draw_scaled_rotated_bitmap(b->handle, cx, cy, dx, dy, xscale, yscale, angle, ToAllegroFlags(flip));
}

void AllegroRenderer::initSystem() {
	if(!al_init())
		std::exit(EXIT_FAILURE);
	if(!al_install_keyboard())
		std::exit(EXIT_FAILURE);
	if(!al_install_mouse())
		std::exit(EXIT_FAILURE);
	if(!al_init_font_addon())
		std::exit(EXIT_FAILURE);
	if(!al_init_image_addon())
		std::exit(EXIT_FAILURE);
	if(!al_init_primitives_addon())
		std::exit(EXIT_FAILURE);
	if(!al_init_ttf_addon())
		std::exit(EXIT_FAILURE);
}

void AllegroRenderer::initAudio(void** outVoice, void** outMixer) {
	ALLEGRO_CONFIG* sys = al_get_system_config();
	al_set_config_value(sys, "alsa", "buffer_size", "2048");
	al_set_config_value(sys, "alsa", "frag_count", "4");

	if(!al_install_audio())
		std::exit(EXIT_FAILURE);
	if(!al_init_acodec_addon())
		std::exit(EXIT_FAILURE);

	auto voice = al_create_voice(48000, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
	assert(voice);
	*outVoice = voice;

	auto mixer = al_create_mixer(48000, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
	assert(mixer);
	*outMixer = mixer;

	al_attach_mixer_to_voice((ALLEGRO_MIXER*)mixer, (ALLEGRO_VOICE*)voice);
	al_set_default_mixer((ALLEGRO_MIXER*)mixer);
	al_reserve_samples(32);
}

void AllegroRenderer::destroyAudio(void* voice, void* mixer) {
	if(mixer)
		al_destroy_mixer((ALLEGRO_MIXER*)mixer);
	if(voice)
		al_destroy_voice((ALLEGRO_VOICE*)voice);
}

Display AllegroRenderer::createDisplay(int width, int height, bool fullscreen) {
	if(fullscreen)
		al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW | ALLEGRO_NOFRAME | ALLEGRO_OPENGL | ALLEGRO_RESIZABLE);

	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
	al_set_new_bitmap_flags(al_get_new_bitmap_flags() & ~ALLEGRO_MIN_LINEAR);
	al_set_new_bitmap_flags(al_get_new_bitmap_flags() & ~ALLEGRO_MAG_LINEAR);

	ALLEGRO_DISPLAY* handle = al_create_display(width, height);
	if(!handle)
		return nullptr;
	auto* d = new InternalDisplay();
	d->handle = handle;
	return d;
}

void AllegroRenderer::destroyDisplay(Display d) {
	if(d) {
		if(d->handle)
			al_destroy_display(d->handle);
		delete d;
	}
}

void AllegroRenderer::makeDisplayCurrentBitmap(Display d) {
	if(d && d->handle)
		al_set_target_bitmap(al_get_backbuffer(d->handle));
}

void AllegroRenderer::setDisplayIcon(Display d, Bitmap icon) {
	if(d && d->handle && icon && icon->handle)
		al_set_display_icon(d->handle, icon->handle);
}

void AllegroRenderer::getMonitorSize(int monitorIndex, int* outWidth, int* outHeight) {
	ALLEGRO_MONITOR_INFO info;
	al_get_monitor_info(monitorIndex, &info);
	*outWidth = info.x2 - info.x1;
	*outHeight = info.y2 - info.y1;
}

Timer AllegroRenderer::createTimer(double fps) {
	ALLEGRO_TIMER* handle = al_create_timer(1.0 / fps);
	if(!handle)
		return nullptr;
	auto* t = new InternalTimer();
	t->handle = handle;
	return t;
}

void AllegroRenderer::destroyTimer(Timer t) {
	if(t) {
		if(t->handle)
			al_destroy_timer(t->handle);
		delete t;
	}
}

void AllegroRenderer::startTimer(Timer t) {
	if(t && t->handle)
		al_start_timer(t->handle);
}

EventQueue AllegroRenderer::createEventQueue() {
	ALLEGRO_EVENT_QUEUE* handle = al_create_event_queue();
	if(!handle)
		return nullptr;
	auto* eq = new InternalEventQueue();
	eq->handle = handle;
	return eq;
}

void AllegroRenderer::destroyEventQueue(EventQueue eq) {
	if(eq) {
		if(eq->handle)
			al_destroy_event_queue(eq->handle);
		delete eq;
	}
}

void AllegroRenderer::registerEventSources(EventQueue eq, Display d, Timer fpsTimer, Timer logicTimer) {
	if(!eq || !eq->handle)
		return;
	if(fpsTimer && fpsTimer->handle)
		al_register_event_source(eq->handle, al_get_timer_event_source(fpsTimer->handle));
	if(logicTimer && logicTimer->handle)
		al_register_event_source(eq->handle, al_get_timer_event_source(logicTimer->handle));
	al_register_event_source(eq->handle, al_get_keyboard_event_source());
	if(d && d->handle)
		al_register_event_source(eq->handle, al_get_display_event_source(d->handle));
}

void AllegroRenderer::waitForEvent(EventQueue eq, Event* ev) {
	if(eq && eq->handle && ev)
		al_wait_for_event(eq->handle, (ALLEGRO_EVENT*)ev);
}

} // namespace ValyrianEngine
