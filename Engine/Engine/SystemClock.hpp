#ifndef SYSTEMCLOCK_HPP
#define SYSTEMCLOCK_HPP

#include <chrono>

namespace ValyrianEngine {

class SystemClock final {
public:
	static SystemClock& get();
	uint64_t milli_secs() const;
	uint64_t micro_secs() const;
	uint64_t nano_secs() const;
	uint64_t secs() const;

private:
	std::chrono::high_resolution_clock clock;
};

uint64_t getSystemTime(void);

} // namespace ValyrianEngine
#endif
