#include "KoopaTroopa.hpp"

void KoopaTroopa::s_koopaTroopaFactory(const tmx::Layer::Ptr& spritesLayer) {
	const auto& objectLayer = spritesLayer->getLayerAs<tmx::ObjectGroup>();
	const auto& objects = objectLayer.getObjects();
	for(const auto& object : objects) {
		if(object.getType() == "Koopatroopa") {
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

			auto g = new KoopaTroopa(x * factor, (y - 24) * factor); // -16 because we start counting from zero
			g->initialize();
			g->setDirection(dir);
		}
	}
}

KoopaTroopa::KoopaTroopa(int x, int y)
    : Enemy(x, y, Enemy::s_koopaType, AnimationFilmHolder::getInstance().getFilm("KOOPA_TROOPA_MOVING")) {
	m_logger->trace("KoopaTroopa constructor");
}

KoopaTroopa::~KoopaTroopa() {
	m_logger->trace("KoopaTroopa destructor");
}

void KoopaTroopa::initialize() {
	m_koopaBitmap = BitmapManager::getInstance().getBitmap("enemiesMoves");
	auto film1 = AnimationFilmHolder::getInstance().getFilm("KOOPA_TROOPA_MOVING");
	frameBox = film1->getFrameBox(0);
	initializeAnimations();
}

void KoopaTroopa::render(Renderer& renderer, int xOff, int yOff, const ViewData& vw) {
	Flip flip = m_direction == Direction::LEFT ? Flip::NONE : Flip::HORIZONTAL;
	renderer.drawScaledBitmap(m_koopaBitmap, frameBox.x, frameBox.y, frameBox.width, frameBox.height, x + xOff - vw.x,
	                          y + yOff - vw.y, frameBox.screenWidth, frameBox.screenHeight, flip);
}

void KoopaTroopa::changeState(State newState) {
	switch(m_state) {
		case State::MOVING:
			if(newState == State::STUNNED) {
				m_frameRangeAnimator->stop();
				auto previousY = frameBox.screenHeight;
				film = AnimationFilmHolder::getInstance().getFilm("KOOPA_TROOPA_INSIDE");
				assert(film);
				frameBox = film->getFrameBox(0);
				y += previousY - frameBox.screenHeight;
				static_cast<MarioPlayer*>(SpriteManager::getInstance().getTypeList(MarioPlayer::s_type).front())
				    ->bounce();
			} else {
				assert(0);
			}
			break;
		case State::KICKED:
			if(newState == State::STUNNED) {
				m_movingAnimator->stop();
				static_cast<MarioPlayer*>(SpriteManager::getInstance().getTypeList(MarioPlayer::s_type).front())
				    ->bounce();
			} else {
				assert(0);
			}
			break;
		case State::STUNNED:
			if(newState == State::KICKED) {
				if(!m_movingAnimator->isRunnig()) {
					m_movingAnimator->start(m_movingAnimation.get(), getSystemTime());
				}
			} else if(newState == State::MOVING) {
				m_frameRangeAnimator->start(m_frameRangeAnimation.get(), getSystemTime());
			}
			break;
		default:
			assert(0);
			break;
	}
	m_logger->debug("Switched state from {} to {}", static_cast<int>(m_state), static_cast<int>(newState));
	m_state = newState;
}

void KoopaTroopa::registerCollision() {
	auto* player = SpriteManager::getInstance().getTypeList(MarioPlayer::s_type).front();
	CollisionChecker::getInstance().Register(this, player, [&](Sprite* goomba, Sprite* player) {
		const auto yPlayerBottom = player->getY() + player->getRectangle().screenHeight;
		const auto turtleCenter = goomba->getY() + goomba->getRectangle().screenHeight / 2;
		const auto hitFromAbove = yPlayerBottom < turtleCenter;
		auto* _player = static_cast<MarioPlayer*>(player);
		const auto playerFallingDown = hitFromAbove && !_player->isJumping();

		switch(m_state) {
			case State::MOVING:
				if(playerFallingDown) {
					m_logger->debug("Player koopa collision from UP");
					changeState(State::STUNNED);
				} else if(!hitFromAbove) {
					m_logger->debug("Player koopa collision, player should lose life");
					_player->hit();
				}
				break;
			case State::KICKED:
				if(playerFallingDown) {
					m_logger->debug("Koopa should go stunned");
					changeState(State::STUNNED);
				} else if(!hitFromAbove) {
					m_logger->debug("Player koopa collision side, player should lose life");
					_player->hit();
				}
				break;
			case State::STUNNED:
				if(playerFallingDown) {
					m_logger->debug("Player koopa stomped stunned shell, kicking with bounce");
					m_direction = player->getX() < goomba->getX() ? Direction::RIGHT : Direction::LEFT;
					static_cast<MarioPlayer*>(SpriteManager::getInstance().getTypeList(MarioPlayer::s_type).front())
					    ->bounce();
					changeState(State::KICKED);
				} else if(!hitFromAbove) {
					m_logger->debug("Player koopa side hit stunned shell, kicking");
					m_direction = player->getX() < goomba->getX() ? Direction::RIGHT : Direction::LEFT;

					// NUDGE: Move the shell 12 pixels immediately so it stops colliding with Mario
					x += (m_direction == Direction::RIGHT ? 12 : -12);

					changeState(State::KICKED);
				}
				break;
			default:
				assert(0);
				break;
		}
	});
}

void KoopaTroopa::initializeAnimations() {
	m_frameRangeAnimator = std::make_unique<FrameRangeAnimator>();

	m_frameRangeAnimation =
	    std::make_unique<FrameRangeAnimation>("KOOPA_TROOPA_MOVING", 0, 1, 0, m_dx, m_dy, m_frameSwitchDelay);

	m_frameRangeAnimator->setOnStart([&](Animator*) {
		m_logger->debug("Started framerange animator");
	});

	m_frameRangeAnimator->setOnAction([&](Animator* a, const Animation& anim) {
		auto myAnimator = reinterpret_cast<FrameRangeAnimator*>(a);
		frameBox = film->getFrameBox(myAnimator->getCurrFrame());

		auto& animation = (FrameRangeAnimation&)(anim);

		auto dx = animation.getDx();
		dx = m_direction == Direction::RIGHT ? dx : -dx;

		auto dy = animation.getDy();

		Enemy::s_grid->FilterGridMotion(getScreenRectangle(), &dx, &dy);
		if(dx == 0) { // If we can not move anymore change direction on the animation
			Sprite::changeDirection();
			// changeDirection();
		}
		Sprite::move(dx, dy);
	});

	m_frameRangeAnimator->setOnFinish([&](Animator*) {
		m_logger->debug("Stoped framerange animator");
	});

	// Kicked Animator / Animations
	m_movingAnimator = std::make_unique<MovingAnimator>();
	m_movingAnimation = std::make_unique<MovingAnimation>("KOOPA_KICKED", 0, 2, 0, 6);

	m_movingAnimator->setOnStart([&](Animator*) {
		m_logger->debug("Started moving animator");
	});

	m_movingAnimator->setOnAction([&](Animator* a, const Animation& anim) {
		auto myAnimator = reinterpret_cast<MovingAnimator*>(a);
		auto& animation = (MovingAnimation&)(anim);

		auto dx = animation.getDx();
		dx = m_direction == Direction::RIGHT ? dx : -dx;

		auto dy = animation.getDy();

		Enemy::s_grid->FilterGridMotion(getScreenRectangle(), &dx, &dy);
		if(dx == 0) { // If we can not move anymore change direction on the animation
			Sprite::changeDirection();
			// changeDirection();
		}
		Sprite::move(dx, dy);
	});

	m_movingAnimator->setOnFinish([&](Animator*) {
		m_logger->debug("Stoped moving animator");
	});
}

void KoopaTroopa::startAnimation(uint64_t time) {
	if(!m_frameRangeAnimator->isRunnig() && m_state == State::MOVING) {
		m_frameRangeAnimator->start(m_frameRangeAnimation.get(), time);
		m_hasAnimationStarted = true;
	}
}

void KoopaTroopa::changeDirection() {
	Sprite::changeDirection();
	switch(m_state) {
		case State::MOVING:
			m_frameRangeAnimator->stop();
			startAnimation();
			break;
		case State::KICKED:
			// movingAnimator reads m_direction each tick — the flip takes effect naturally
			break;
		case State::STUNNED:
			break;
		default:
			assert(0);
			break;
	}
}

void KoopaTroopa::initCollision() {
	initSameTypeCollision(Enemy::s_koopaType);
}

KoopaTroopa::State KoopaTroopa::getState() const {
	return m_state;
}