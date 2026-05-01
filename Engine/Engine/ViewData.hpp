#ifndef VIEWDATA_HPP
#define VIEWDATA_HPP

#include "Typedefs.hpp"

#include <cstdint>

namespace ValyrianEngine {

typedef struct ViewData {

	ViewData() = default;
	~ViewData() = default;

	std::uint16_t windowWidth{0};
	std::uint16_t windowHeight{0};
	int x{0};
	int y{0};
	int x2{0}; // The right end of the screen

	void updateX(int speed) {
		x += speed;
		x2 = x + windowWidth;
	}
} ViewData;

} // namespace ValyrianEngine
#endif
