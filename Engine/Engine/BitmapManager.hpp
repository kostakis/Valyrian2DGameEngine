#ifndef BITMAPMANAGER_HPP
#define BITMAPMANAGER_HPP

#include "Renderer.hpp"
#include "Typedefs.hpp"

#include <spdlog/spdlog.h>

#include <map>
#include <memory>
#include <string>

namespace ValyrianEngine {

class BitmapManager final {
public:
	BitmapManager();
	~BitmapManager();

	void initialize(Renderer* renderer);
	Bitmap loadBitmap(const std::string& name, const std::string& fileName);

	static BitmapManager& getInstance();
	Bitmap getBitmap(const std::string& name) const;

private:
	BitmapManager(const BitmapManager&) = delete;
	BitmapManager(BitmapManager&&) = delete;

	std::shared_ptr<spdlog::logger> m_logger;
	Renderer* m_renderer{nullptr};
	std::map<std::string, Bitmap> bitmaps;
};

} // namespace ValyrianEngine
#endif
