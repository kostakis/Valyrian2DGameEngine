#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include "Typedefs.hpp"

#include <spdlog/spdlog.h>

#include <list>
#include <map>
#include <memory>
#include <string>

namespace ValyrianEngine {

class SoundManager final {
public:
	~SoundManager();

	void addSound(const std::string& name, const std::string& path);
	void playSound(const std::string& name, PlayMode mode, float volume = -1);
	void stopSound(const std::string& name);
	void stopAllSounds();

	int getSoundsSize() const;
	void setVolume(float v);
	float getVolume() const;

	static SoundManager& getInstance();

private:
	SoundManager();
	SoundManager(const SoundManager&) = delete;
	SoundManager(SoundManager&&) = delete;

	std::shared_ptr<spdlog::logger> m_logger;

	std::map<std::string, void*> sounds;     // void* over ALLEGRO_SAMPLE*
	std::map<std::string, SampleId> samples; // SampleId matches ALLEGRO_SAMPLE_ID layout

	float m_soundVolume{1.0};
	float m_pan{0.0};
	float m_speed{1.0};
};

} // namespace ValyrianEngine
#endif
