#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

namespace ValyrianEngine {

struct Rectangle {
	Rectangle() = default;

	Rectangle(int _x, int _y, int w, int h) {
		x = _x;
		y = _y;
		width = w;
		height = h;
		screenWidth = w;
		screenHeight = h;
	}

	// Call after setting width/height to enable 2x (or any) scaling.
	// screenWidth/screenHeight will then reflect the display size;
	// width/height always stay as the source bitmap crop dims.
	// When scalingEnabled is false, screenWidth == width (native pixel).
	void enableScaling(int w, int h) {
		scalingEnabled = true;
		screenWidth = w;
		screenHeight = h;
	}

	bool operator==(const Rectangle& rhs) const {
		if((x == rhs.x) && (y == rhs.y) && (width == rhs.width) && (height == rhs.height)) {
			return true;
		}
		return false;
	}

	int x{0};      // source bitmap crop x
	int y{0};      // source bitmap crop y
	int width{0};  // source bitmap crop width
	int height{0}; // source bitmap crop height
	bool scalingEnabled{false};
	int screenWidth{0};  // display width  (== width when scalingEnabled is false)
	int screenHeight{0}; // display height (== height when scalingEnabled is false)
};

} // namespace ValyrianEngine

#endif
