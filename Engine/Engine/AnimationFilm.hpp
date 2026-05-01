#ifndef ANIMATIONFILM_HPP
#define ANIMATIONFILM_HPP

#include "MyDebug.hpp"
#include "Rectangle.hpp"
#include "Typedefs.hpp"

#include <string>
#include <vector>

namespace ValyrianEngine {

class AnimationFilm {
public:
	AnimationFilm(Bitmap b, const std::vector<Rectangle>& recs, const std::string& _id) {
		assert(b);
		m_bitmap = b;
		m_boxes = recs;
		m_id = _id;
	}

	void append(const Rectangle& r) {
		m_boxes.push_back(r);
	}

	byte getTotalFrames() const {
		return static_cast<byte>(m_boxes.size());
	}
	Bitmap getBitmap() const {
		return m_bitmap;
	}
	std::string getId() const {
		return m_id;
	}
	const Rectangle& getFrameBox(byte frameNo) const {
		assert(m_boxes.size() > frameNo);
		return m_boxes[frameNo];
	}

private:
	std::vector<Rectangle> m_boxes; // All the rectangles of the current film
	Bitmap m_bitmap{nullptr};       // Bitmap to find the rectangles
	std::string m_id;               // Id of the film
};

} // namespace ValyrianEngine
#endif
