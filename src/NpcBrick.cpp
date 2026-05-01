#include "NpcBrick.hpp"
#include "Coin.hpp"
#include "Goomba.hpp"

NpcBrick::NpcBrick(int x, int y, NpcType type, int occur, Game& game, bool visibility)
    : Sprite(x, y, s_brickType, "Created", AnimationFilmHolder::getInstance().getFilm("Brick")) {
	m_logger = getOrCreateLogger("NpcBrick");
	m_logger->trace("NpcBrick constructor");

	m_numberOfOcuur = occur;
	m_bitmap = BitmapManager::getInstance().getBitmap("tilesBitmapAsSprite");
	setIsVisible(visibility);
	m_type = type;
	m_player = static_cast<MarioPlayer*>(game.getPlayer());
	directMotion = true;
	assert(m_player);

	initAnimations();

	// Collision with player
	CollisionChecker::getInstance().Register(this, m_player, [&](Sprite* s1, Sprite* s2) {
		if(!m_moveAnimator->isRunnig() && !m_takenAll) {
			const auto& goombas = SpriteManager::getInstance().getTypeList(Enemy::s_goombaType);
			for(auto* goomba : goombas) {
				if(Sprite::collisionCheck(goomba))
					reinterpret_cast<Goomba*>(goomba)->fireOrBrickCollision();
			}
			m_moveAnimator->start(m_moveAnim.get(), getSystemTime());
			m_logger->debug("Player npc brick collision");
		}
	});
	;
}

NpcBrick::~NpcBrick() {
	m_logger->trace("~NpcBrick destructor");
}

void NpcBrick::render(Renderer& renderer, int xOff, int yOff, const ViewData& vw) {
	renderer.drawScaledBitmap(m_bitmap, frameBox.x, frameBox.y, frameBox.width, frameBox.height, x + xOff - vw.x,
	                          y + yOff - vw.y, frameBox.screenWidth, frameBox.screenHeight);
}

bool NpcBrick::collisionCheck(const Sprite* s) const {
	if(Sprite::collisionCheck(s)) {
		if(m_player->getY() >= y + frameBox.screenHeight && m_player->isJumping() /*&& !m_player->isFalling()*/) {
			return true;
		} else
			return false;
	} else
		return false;
}

void NpcBrick::setIsVisible(bool visible) {
	isVisible = visible;
	if(visible == true) {
		Sprite::s_grid->setTileType(y / frameBox.screenHeight, x / frameBox.screenWidth, GRID_SOLID_TILE);
	} else {
		Sprite::s_grid->setTileType(y / frameBox.screenHeight, x / frameBox.screenWidth, GRID_EMPTY_TILE);
	}
}

void NpcBrick::initAnimations() {
	m_moveAnimator = std::make_unique<MovingAnimator>();
	m_moveAnim = std::make_unique<MovingAnimation>("MoveAnimation", 0, 0, m_ySpeed, m_animationSpeed);

	m_moveAnimator->setOnStart([&](Animator*) {
		m_numberOfOcuur--;
		setIsVisible(true);

		m_logger->info("[NpcBrick] Stopping player jump");
		m_player->stopJump();

		if(m_type == NpcType::COIN) {
			auto coin = new Coin(x, y);
			coin->take();
		} // TODO rest when classes are ready
	});

	m_moveAnimator->setOnAction([&](Animator* a, const Animation& anim) {
		auto moveAnim = (MovingAnimation&)anim;
		auto moveAnimator = (MovingAnimator*)(a);

		int dy = moveAnim.getDy();
		int dx = moveAnim.getDx();

		if(moveAnimator->getCurrentRep() >= 6) {
			dy = -dy;
		}
		Sprite::move(dx, dy);
		if(moveAnimator->getCurrentRep() >= 11)
			moveAnimator->stop();
	});

	m_moveAnimator->setOnFinish([&](Animator*) {
		if(m_numberOfOcuur == 0) {
			CollisionChecker::getInstance().Cancel(this);
			m_takenAll = true;
			film = AnimationFilmHolder::getInstance().getFilm("TakenBrick");
			frameBox = film->getFrameBox(0);
		}
	});
}
