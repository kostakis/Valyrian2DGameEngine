#include "AnimationFilmHolder.hpp"
#include "BitmapManager.hpp"
#include "Logger.hpp"
#include "MyDebug.hpp"

#include <cassert>
#include <sstream>
#include <stdexcept>

namespace ValyrianEngine {

AnimationFilmHolder::AnimationFilmHolder() {
	m_logger = getOrCreateLogger("EngineLogger");
	m_logger->trace("AnimationFilmHolder constructor");
}

AnimationFilmHolder::~AnimationFilmHolder() {
	m_logger->trace("AnimationFilmHolder destructor");
	cleanUp();
	m_logger->debug("Cleared all films");
}

void AnimationFilmHolder::loadAll(const std::string& text) {
	IniConfig config;
	bool ok = config.load(text);
	assert(ok);
	m_configs.push_back(config);

	const char* bitmap_name = config.getValue("", "SOURCE_BITMAP");
	const char* bitmapId = config.getValue("", "BITMAP_ID");
	assert(bitmapId);
	assert(bitmap_name);

	std::string fullPath = "media/" + std::string(bitmap_name);
	auto bitmap = BitmapManager::getInstance().loadBitmap(bitmapId, fullPath);

	int i = 0;
	while(true) {
		std::string section = "ANIMATION " + std::to_string(i);
		const char* id = config.getValue(section, "NAME");
		if(!id)
			break;

		const char* spriteNumber = config.getValue(section, "NUMBER_OF_SPRITES");
		assert(spriteNumber);

		int counter = 0;
		std::stringstream(spriteNumber) >> counter;

		std::vector<Rectangle> tempRecs;
		for(int spritecounter = 0; spritecounter < counter; spritecounter++) {
			auto get = [&](const char* prefix) {
				return config.getValue(section, (std::string(prefix) + std::to_string(spritecounter)).c_str());
			};

			Rectangle tempRec;
			tempRec.x = std::stoi(get("X"));
			tempRec.y = std::stoi(get("Y"));
			tempRec.width = std::stoi(get("WIDTH"));
			tempRec.height = std::stoi(get("HEIGHT"));
			tempRec.enableScaling(std::stoi(get("SCALEDWIDTH")), std::stoi(get("SCALEDHEIGHT")));
			tempRecs.push_back(tempRec);
		}

		if(m_films.find(id) == m_films.end()) {
			m_films[id] = new AnimationFilm(bitmap, tempRecs, id);
			m_logger->info("Added Film: " + std::string(id));
		} else {
			m_logger->trace("Film '{}' already exists, skipping", id);
		}

		i++;
	}
}

void AnimationFilmHolder::cleanUp() {
	m_configs.clear();

	for(auto& x : m_films) {
		delete x.second;
	}
	m_films.clear();
}

AnimationFilmHolder& AnimationFilmHolder::getInstance() {
	static AnimationFilmHolder instance;
	return instance;
}

AnimationFilm* AnimationFilmHolder::getFilm(const std::string& id) {
	auto i = m_films.find(id);
#ifndef NDEBUG
	if(i == m_films.end()) {
		m_logger->error("AnimationFilmHolder::getFilm: no film registered with id '{}'", id);
		throw std::out_of_range("AnimationFilmHolder::getFilm: unknown film '" + id + "'");
	}
	return i->second;
#else
	return i != m_films.end() ? i->second : nullptr;
#endif
}

} // namespace ValyrianEngine
