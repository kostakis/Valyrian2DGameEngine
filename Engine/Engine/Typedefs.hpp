#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#include <cstdint>

namespace ValyrianEngine {

class Texture;
typedef Texture* Bitmap;

class InternalDisplay;
typedef InternalDisplay* Display;

union Event;

class InternalTimer;
typedef InternalTimer* Timer;

class InternalFont;
typedef InternalFont* Font;

class InternalEventQueue;
typedef InternalEventQueue* EventQueue;

struct Color {
	std::uint8_t r, g, b, a;
	static Color RGB(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
		return {r, g, b, 255};
	}
	static Color RGBA(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) {
		return {r, g, b, a};
	}
};

typedef std::uint8_t byte; // For small positive numbers, 0-255

typedef uint64_t timeStamp;

enum class Flip { NONE = 0, HORIZONTAL = 1, VERTICAL = 2, BOTH = 3 };

enum class PlayMode { ONCE, LOOP, BIDIR };

// Opaque audio sample handle — backend stores its own type behind this.
struct SampleId {
	int _id{0};
	int _index{0};
};

// Quick Tile Maths
// Currently working only for scaled 32x32 tiles
#define DIV_TILE_WIDTH(i) ((i) >> 5)
#define DIV_TILE_HEIGHT(i) ((i) >> 5)

#define MUL_TILE_WIDTH(i) ((i) << 5)
#define MUL_TILE_HEIGHT(i) ((i) << 5)

} // namespace ValyrianEngine

#endif
