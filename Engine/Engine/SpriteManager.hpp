#ifndef SPRITEMANAGER_HPP
#define SPRITEMANAGER_HPP

#include "Renderer.hpp"
#include "ViewData.hpp"

#include <list>
#include <map>
#include <string>

namespace ValyrianEngine {

class Sprite;

class SpriteManager final {
public:
	using SpriteList = std::list<Sprite*>; // List of sprites
	using TypeList = std::map<std::string, SpriteList>;

	~SpriteManager();

	void CleanUp();
	void Add(Sprite* s);
	void Remove(Sprite* s);

	void renderAll(Renderer& renderer, int xOff, int yOff, const ViewData& vw, bool renderGrid);

	SpriteList& getTypeList(const std::string& id) {
		return types[id];
	}

	const TypeList& getAllList() {
		return types;
	}

	size_t getSpriteCount() const;

	static SpriteManager& getInstance();

private:
	TypeList types;

	SpriteManager();
	SpriteManager(const SpriteManager&) = delete;
	SpriteManager(SpriteManager&&) = delete;
};

} // namespace ValyrianEngine
#endif
