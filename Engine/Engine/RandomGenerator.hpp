#ifndef RANDOMGENERATOR_HPP
#define RANDOMGENERATOR_HPP

#include <chrono>
#include <random>

class RandomGenerator {
private:
	std::mt19937 generator;

public:
	RandomGenerator() {
		// Cross-platform, non-blocking time seed
		auto now = std::chrono::high_resolution_clock::now();
		auto seed = static_cast<unsigned int>(now.time_since_epoch().count());
		generator.seed(seed);
	}

	// Give entities access to the core engine so they can apply their own distributions
	std::mt19937& getEngine() {
		return generator;
	}

	// (Optional) A quick convenience function if you just want a quick uniform int
	inline int getInt(int min, int max) {
		std::uniform_int_distribution<int> dist(min, max);
		return dist(generator);
	}
};

#endif
