#include "Goomba.hpp"
#include "Game.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Grid.hpp>
#include <Engine/RandomGenerator.hpp>
#include <Engine/SoundManager.hpp>

namespace ValyrianEngine {

void createGoomba(const tmx::Object& object, Game& game) {
	int x = object.getPosition().x;
	int y = object.getPosition().y;
	int factor = 1;
	Sprite::Direction dir{Sprite::Direction::LEFT};
	auto properites = object.getProperties();
	for(const auto& prop : properites) {
		if(prop.getName() == "scalefactor") {
			factor = prop.getIntValue();
		}
		if(prop.getName() == "direction") {
			auto tempstr = prop.getStringValue();
			dir = tempstr == "left" ? Sprite::Direction::LEFT : Sprite::Direction::RIGHT;
		}
	}
	auto g = new Goomba(x * factor, (y - 16) * factor, game); //-16 because we start counting from zero
	g->initialize();
	g->setDirection(dir);
}

} // namespace ValyrianEngine

Goomba::Goomba(int x, int y, Game& game)
    : Enemy(x, y, Enemy::s_goombaType, AnimationFilmHolder::getInstance().getFilm("GOOMBA_MOVING")),
      m_game(game) {
	m_logger->trace("Goomba constructor");
	m_bitmap = BitmapManager::getInstance().getBitmap("enemiesMoves");
	isVisible = true;
	directMotion = false;
	m_randomFallDegree = m_game.getEngine()->getRandomGenerator().getInt(80, 190);

	auto& renderer = m_game.getEngine()->getRenderer();
	auto* saved = renderer.saveCurrentTarget();
	bitmap = renderer.createBitmap(frameBox.width, frameBox.height);
	assert(bitmap);
	renderer.setTargetBitmap(bitmap);
	renderer.drawBitmapRegion(m_bitmap, frameBox.x, frameBox.y, frameBox.width, frameBox.height, 0, 0);
	renderer.restoreTarget(saved);
}

Goomba::~Goomba() {
	m_logger->trace("Goomba destructor");
	if(bitmap)
		m_game.getEngine()->getRenderer().destroyBitmap(bitmap);
}

void Goomba::initialize() {
	initializeAnimations();
}

void Goomba::startAnimation(uint64_t time) {
	if(!m_frameRangeAnimator->isRunnig() && m_canHurtMario) {
		m_frameRangeAnimator->start(m_frameRangeAnimation.get(), time);
		m_hasAnimationStarted = true;
	}
}

void Goomba::changeDirection() {
	Sprite::changeDirection();
	if(m_direction == Direction::LEFT)
		x -= m_dx;
	else
		x += m_dx;
}

void Goomba::initCollision() {
	initSameTypeCollision(Enemy::s_goombaType);

	// Goomba with koopa collision
	const auto& koopaList = SpriteManager::getInstance().getTypeList(Enemy::s_koopaType);
	for(const auto& koopa : koopaList) {
		CollisionChecker::getInstance().Register(koopa, this, [&](Sprite* koopaSprite, Sprite* goombaSprite) {
			m_logger->debug("Goomba Koopa collision");

			KoopaTroopa* koop = (KoopaTroopa*)koopaSprite;
			if(koop->getState() == KoopaTroopa::State::KICKED) {
				static_cast<Enemy*>(goombaSprite)->fireOrBrickCollision();
				return;
			}

			if(koopaSprite->getX() < goombaSprite->getX()) {
				if(koopaSprite->getDirection() == Direction::RIGHT)
					koopaSprite->changeDirection();
				if(goombaSprite->getDirection() == Direction::LEFT)
					goombaSprite->changeDirection();
			} else {
				if(koopaSprite->getDirection() == Direction::LEFT)
					koopaSprite->changeDirection();
				if(goombaSprite->getDirection() == Direction::RIGHT)
					goombaSprite->changeDirection();
			}
		});
	}
}

void Goomba::render(Renderer& renderer, int xOff, int yOff, const ViewData& vw) {
	if(!m_brickCollision) {
		Sprite::render(renderer, xOff, yOff, vw);
	} else {
		// drawScaledRotatedBitmap draws the point (cx, cy) at (dx, dy).
		// Since cx,cy is (8,8) and the scaled sprite is 32x32, the center is at (16,16) in destination space.
		// To keep the top-left at (x,y), we must draw the center at (x+16, y+16).
		renderer.drawScaledRotatedBitmap(bitmap, 8, 8, x + xOff + 16 - vw.x, y + yOff + 16 - vw.y, 2, 2,
		                                 static_cast<float>(m_randomFallDegree));
	}
}

void Goomba::registerCollision() {
	// Goomba with player collision
	auto* player = SpriteManager::getInstance().getTypeList(MarioPlayer::s_type).front();
	CollisionChecker::getInstance().Register(this, player, [&](Sprite* goomba, Sprite* player) {
		if(m_canHurtMario) {
			const auto yPlayerBottom = player->getY() + player->getRectangle().screenHeight;
			const auto goombaCenter = goomba->getY() + goomba->getRectangle().screenHeight / 2;

			if(yPlayerBottom < goombaCenter) {
				m_logger->info("Player and Goomba collision from UP");

				m_frameRangeAnimator->stop();
				m_movingAnimator->start(m_movingAnimation.get(), getSystemTime());
				m_canHurtMario = false;
				m_game.addDrawTextToList({"100", x + (frameBox.screenWidth / 2), y - frameBox.screenHeight, nullptr,
				                          m_game.getEngine()->color(255, 255, 255)});
				return;
			} else {
				m_logger->info("Player goomba collision, player should losse life");
				static_cast<MarioPlayer*>(player)->hit();
			}
		}
	});
}

void Goomba::initializeAnimations() {
	m_frameRangeAnimator = std::make_unique<FrameRangeAnimator>();
	m_movingAnimator = std::make_unique<MovingAnimator>();
	m_brickCollisionAnimator = std::make_unique<MovingAnimator>();

	m_frameRangeAnimation = std::make_unique<FrameRangeAnimation>("Goomba", 0, 1, 0, m_dx, m_dy, m_frameSwitchDelay);

	m_movingAnimation = std::make_unique<FrameRangeAnimation>("GoombaDestroyed", 0, 0, 2, 0, 0, m_frameSwitchDelay);
	m_brickCollisionAnimation = std::make_unique<MovingAnimation>("GoombaDestroyed", 45, 1, 2, 10);

	m_frameRangeAnimator->setOnStart([&](Animator*) {
		m_logger->trace("Goomba frame range started");
		isMoving = true;
	});

	m_frameRangeAnimator->setOnAction([&](Animator* a, const Animation& anim) {
		auto myAnimator = reinterpret_cast<FrameRangeAnimator*>(a);
		frameBox = film->getFrameBox(myAnimator->getCurrFrame());

		auto& animation = (FrameRangeAnimation&)(anim);

		auto dx = animation.getDx();
		dx = m_direction == Direction::RIGHT ? dx : -dx;

		auto dy = animation.getDy();

		Sprite::move(dx, dy);

		if(dx == 0) {
			// If we can not move anymore change direction on the animation
			Sprite::changeDirection();
		}
	});

	m_frameRangeAnimator->setOnFinish([&](Animator* a) {
		m_logger->trace("Goomba frame range stopped");
	});

	m_movingAnimator->setOnStart([&](Animator*) {
		m_logger->trace("Goomba moving animator started");
		m_logger->trace("Goomba destoyed X {}", x);
		m_logger->trace("Goomba destoyed Y {}", y);

		auto previousY = frameBox.screenHeight;
		film = AnimationFilmHolder::getInstance().getFilm("GOOMBA_DESTROYED");
		assert(film);
		frameBox = film->getFrameBox(0);

		y += previousY - frameBox.screenHeight;
		SoundManager::getInstance().playSound("Stomp", PlayMode::ONCE);
		Enemy::bounceEnemyFromUp();
	});

	m_movingAnimator->setOnFinish([&](Animator* a) {
		m_logger->info("Goomba Moving animator stopped");
		Destroy();
	});

	m_brickCollisionAnimator->setOnStart([&](Animator* a) {
		m_brickCollision = true;
		directMotion = true;
		m_canHurtMario = false;
		m_frameRangeAnimator->stop();
		m_movingAnimator->stop();
		stopFalling();
		y -= 5;
		m_logger->debug("m_brickCollisionAnimator started");
	});

	m_brickCollisionAnimator->setOnAction([&](Animator* a, const Animation& anim) {
		const auto& moveAnim = (MovingAnimation&)(anim);

		int dx = moveAnim.getDx();
		int dy = moveAnim.getDy();
		Sprite::move(dx, dy);
	});

	m_brickCollisionAnimator->setOnFinish([&](Animator* a) {
		Destroy();
	});
}
