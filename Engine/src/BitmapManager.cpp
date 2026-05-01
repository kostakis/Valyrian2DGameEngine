#include "BitmapManager.hpp"
#include "Logger.hpp"
#include "MyDebug.hpp"
#include "Renderer.hpp"

#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace ValyrianEngine {

BitmapManager::BitmapManager() {
	m_logger = getOrCreateLogger("BitmapManager");
	m_logger->trace("BitmapManager constructor");
}

BitmapManager::~BitmapManager() {
	m_logger->trace("BitmapManager destructor");
	if(m_renderer) {
		for(auto& x : bitmaps) {
			m_renderer->destroyBitmap(x.second);
		}
	} else {
		m_logger->warn("BitmapManager destructor called without renderer, bitmaps might leak!");
	}
}

void BitmapManager::initialize(Renderer* renderer) {
	m_renderer = renderer;
}

Bitmap BitmapManager::loadBitmap(const std::string& name, const std::string& fileName) {
	if(bitmaps.find(name) != bitmaps.end())
		return bitmaps[name];

	assert(m_renderer && "BitmapManager must be initialized with a renderer before loading bitmaps");
	auto bitmap = m_renderer->loadBitmap(fileName);
	assert(bitmap);

	bitmaps[name] = bitmap;
	return bitmap;
}

BitmapManager& BitmapManager::getInstance() {
	static BitmapManager instance;
	return instance;
}

Bitmap BitmapManager::getBitmap(const std::string& name) const {
#ifndef NDEBUG
	auto it = bitmaps.find(name);
	if(it == bitmaps.end()) {
		m_logger->error("BitmapManager::getBitmap: no bitmap registered with name '{}'", name);
		throw std::out_of_range("BitmapManager::getBitmap: unknown bitmap '" + name + "'");
	}
	return it->second;
#else
	return bitmaps.at(name);
#endif
}

} // namespace ValyrianEngine
