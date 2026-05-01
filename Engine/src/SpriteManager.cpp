#include "SpriteManager.hpp"
#include "Sprite.hpp"

#include <iostream>

namespace ValyrianEngine {

SpriteManager::SpriteManager() {}

SpriteManager::~SpriteManager() {
	CleanUp();
}

void SpriteManager::CleanUp() {
	std::list<Sprite*> allSprites;
	for(auto& lists : types) {
		for(auto* s : lists.second) {
			allSprites.push_back(s);
		}
	}
	types.clear();

	for(auto* s : allSprites) {
		s->PrepareForManualDelete();
		delete s;
	}
}

void SpriteManager::Add(Sprite* s) {
	types[s->getTypeId()].push_back(s);
}

void SpriteManager::Remove(Sprite* s) {
	auto it = types.find(s->getTypeId());
	if(it != types.end()) {
		it->second.remove(s);
	}
}

size_t SpriteManager::getSpriteCount() const {
	size_t count = 0;
	for(auto& pair : types) {
		count += pair.second.size();
	}
	return count;
}

void SpriteManager::renderAll(Renderer& renderer, int xOff, int yOff, const ViewData& vw, bool renderGrid) {
	for(const auto& lists : types) {
		for(const auto& sprite : lists.second) {
			if(sprite->getIsVisible()) {
				sprite->render(renderer, xOff, yOff, vw);
				if(renderGrid) {
					sprite->renderRec(renderer, xOff, yOff, vw);
				}
			}
		}
	}
}

SpriteManager& SpriteManager::getInstance() {
	static SpriteManager instance;
	return instance;
}

} // namespace ValyrianEngine
