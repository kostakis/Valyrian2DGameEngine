#include "SoundManager.hpp"
#include "Logger.hpp"

#include <allegro5/allegro_audio.h>

#include <cassert>
#include <cstring>
#include <memory>

namespace ValyrianEngine {

static ALLEGRO_PLAYMODE toAllegroPlayMode(PlayMode mode) {
	switch(mode) {
		case PlayMode::LOOP:
			return ALLEGRO_PLAYMODE_LOOP;
		case PlayMode::BIDIR:
			return ALLEGRO_PLAYMODE_BIDIR;
		case PlayMode::ONCE:
		default:
			return ALLEGRO_PLAYMODE_ONCE;
	}
}

SoundManager::SoundManager() {
	m_logger = getOrCreateLogger("SoundManager");
	m_logger->trace("SoundManager constructor");
}

SoundManager::~SoundManager(void) {
	m_logger->trace("SoundManager destructor");
	m_logger->debug("Destroying SoundManager with {} loaded sample(s)", sounds.size());
	for(auto& x : sounds) {
		m_logger->trace("Destroying sample '{}'", x.first);
		al_destroy_sample(static_cast<ALLEGRO_SAMPLE*>(x.second));
	}
}

SoundManager& SoundManager::getInstance() {
	static SoundManager instance;
	return instance;
}

void SoundManager::addSound(const std::string& name, const std::string& path) {
	if(sounds.find(name) != sounds.end()) {
		m_logger->trace("addSound: '{}' already loaded, skipping '{}'", name, path);
		return;
	}

	m_logger->debug("Loading sample '{}' from '{}'", name, path);
	ALLEGRO_SAMPLE* mySample = al_load_sample(path.c_str());
	assert(mySample);

	sounds[name] = mySample;
	m_logger->info("Sample '{}' loaded (total: {})", name, sounds.size());
}

void SoundManager::playSound(const std::string& name, PlayMode mode, float volume) {
	float finalVolume = (volume == -1) ? m_soundVolume : volume;
	m_logger->trace("playSound '{}' (volume={}, pan={}, speed={})", name, finalVolume, m_pan, m_speed);

	auto it = sounds.find(name);
	if(it == sounds.end()) {
		m_logger->warn("playSound: no sample registered with name '{}'", name);
		return;
	}

	ALLEGRO_SAMPLE_ID al_id;
	bool ok = al_play_sample(static_cast<ALLEGRO_SAMPLE*>(it->second), finalVolume, m_pan, m_speed,
	                         toAllegroPlayMode(mode), &al_id);
	if(!ok) {
		m_logger->warn("playSound: al_play_sample failed for '{}'", name);
		return;
	}

	static_assert(sizeof(SampleId) == sizeof(ALLEGRO_SAMPLE_ID), "SampleId layout mismatch");
	SampleId sid;
	std::memcpy(&sid, &al_id, sizeof(SampleId));
	samples[name] = sid;
}

void SoundManager::stopSound(const std::string& name) {
	auto id = samples.find(name);
	if(id == samples.end()) {
		m_logger->trace("stopSound: '{}' not playing, nothing to stop", name);
		return;
	}

	ALLEGRO_SAMPLE_ID al_id;
	std::memcpy(&al_id, &id->second, sizeof(ALLEGRO_SAMPLE_ID));
	m_logger->debug("Stopping sample '{}'", name);
	al_stop_sample(&al_id);
}

void SoundManager::stopAllSounds() {
	m_logger->debug("Stopping all samples");
	al_stop_samples();
}

int SoundManager::getSoundsSize() const {
	return static_cast<int>(sounds.size());
}

void SoundManager::setVolume(float v) {
	m_logger->debug("setVolume: {} -> {}", m_soundVolume, v);
	m_soundVolume = v;
}

float SoundManager::getVolume() const {
	return m_soundVolume;
}

} // namespace ValyrianEngine
