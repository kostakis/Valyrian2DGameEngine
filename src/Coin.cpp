#include "Coin.hpp"
#include "MarioPlayer.hpp"

#include <Engine/AnimationFilmHolder.hpp>
#include <Engine/BitmapManager.hpp>
#include <Engine/CollisionChecker.hpp>
#include <Engine/SoundManager.hpp>
#include <Engine/SpriteManager.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <tmxlite/TileLayer.hpp>

#include <iostream>

namespace ValyrianEngine {

void Coin::s_coinFactory(const tmx::Layer::Ptr& spritesLayer) {
	const auto& objectLayer = spritesLayer->getLayerAs<tmx::ObjectGroup>();
	const auto& objects = objectLayer.getObjects();
	for(const auto& object : objects) {
		if(object.getType() == "Coin") {
			int x = object.getPosition().x;
			int y = object.getPosition().y;
			int factor = 1;

			auto properites = object.getProperties();
			for(const auto& prop : properites) {
				if(prop.getName() == "scaleFactor") {
					factor = prop.getIntValue();
				}
			}

			new Coin(x * factor, (y - 16) * factor);
		}
	}
}

Coin::Coin(int x, int y)
    : Sprite(x, y, s_type, "Created", AnimationFilmHolder::getInstance().getFilm("Coin")) {
	m_logger = getOrCreateLogger("Coin");
	m_logger->trace(__FUNCTION__);
	isVisible = true;
	directMotion = true;
	m_bitmap = BitmapManager::getInstance().getBitmap("sprites");

	initAnims();
}

Coin::~Coin() {
	m_logger->trace(__FUNCTION__);
}

void Coin::startAnimation(uint64_t time) {
	if(!m_frameRangeAnimator->isRunnig()) {
		m_frameRangeAnimator->start(m_frameRangeAnimation.get(), time);
		m_hasAnimationStarted = true;
	}
}

void Coin::take(uint64_t time) {
	m_logger->debug(__FUNCTION__);
	auto volume = SoundManager::getInstance().getVolume();
	auto volumeToPlay = volume * 0.8;
	SoundManager::getInstance().playSound("Coin", PlayMode::ONCE, volumeToPlay);
	m_movingAnimator->start(m_movingAnimation.get(), time);
	m_isTaken = true;
	m_hasAnimationStarted = true;
}

bool Coin::isTaken() const {
	return m_isTaken;
}

void Coin::registerCollision() {
	auto* player = SpriteManager::getInstance().getTypeList(MarioPlayer::s_type).front();
	CollisionChecker::getInstance().Register(this, player, [&](Sprite* coin, Sprite* player) {
		take();
	});
}

void Coin::initAnims() {
	m_frameRangeAnimator = std::make_unique<FrameRangeAnimator>();
	m_frameRangeAnimation = std::make_unique<FrameRangeAnimation>("CoinAnimation", 0, 2, 0, 0, 0, 150);

	m_movingAnimator = std::make_unique<MovingAnimator>();
	m_movingAnimation = std::make_unique<MovingAnimation>("MoveCoinAnim", 0, 0, -16, 9);

	m_frameRangeAnimator->setOnAction([&](Animator* a, const Animation& anim) {
		auto* myAnimator = reinterpret_cast<FrameRangeAnimator*>(a);
		frameBox = film->getFrameBox(myAnimator->getCurrFrame());
	});

	m_movingAnimator->setOnStart([&](Animator* a) {
		m_logger->debug("Coin moving animator started");
		m_frameRangeAnimator->stop();
		frameBox = film->getFrameBox(0);
		auto playerList = SpriteManager::getInstance().getTypeList(MarioPlayer::s_type);
		if(!playerList.empty()) {
			auto player = dynamic_cast<MarioPlayer*>(playerList.front());
			player->increaseCoins();
		}
	});

	m_movingAnimator->setOnAction([&](Animator* a, const Animation& anim) {
		int dx = 0;
		int dy = m_movingAnimation->getDy();
		Sprite::move(dx, dy);
	});
}

} // namespace ValyrianEngine
