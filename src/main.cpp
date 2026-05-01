#include "Enemy.hpp"
#include "GameManager.hpp"

#include <Engine/Logger.hpp>

#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>

int main() {
	std::shared_ptr<spdlog::logger> logger = getOrCreateLogger("main");
	logger->info("CPP MAIN");
#ifndef NDEBUG
	spdlog::set_level(spdlog::level::debug);
#else
	spdlog::set_level(spdlog::level::info);
#endif

	// Player position trace is verbose — always on for debugging.
	getOrCreateLogger("Player")->set_level(spdlog::level::trace);
	getOrCreateLogger("Game")->set_level(spdlog::level::trace);

	// Eagerly create all enemy loggers so explicit level overrides apply
	// before any enemy is spawned (loggers are otherwise created lazily).
	// for(auto* name : Enemy::allEnemysStr) {
	//	getOrCreateLogger(std::string("Enemy-") + name)->set_level(spdlog::level::info);
	//}

	// SPDLOG_LEVEL=info,EngineLogger=trace,Player=debug ./SuperMario
	// overrides any of the above at runtime.
	spdlog::cfg::load_env_levels();
	logger->info("Configured loggers");

	auto gameManager = std::make_unique<GameManager>();
	gameManager->initialize();
	logger->info("Configured GameManager. Starting main");
	gameManager->main();
}
