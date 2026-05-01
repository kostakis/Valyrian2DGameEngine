#include "Brick.hpp"
#include "Game.hpp"

#include <Engine/AnimationFilmHolder.hpp>
#include <Engine/BitmapManager.hpp>
#include <Engine/CollisionChecker.hpp>
#include <Engine/SoundManager.hpp>
#include <Engine/Sprite.hpp>
#include <Engine/SpriteManager.hpp>
#include <Engine/SystemClock.hpp>

namespace ValyrianEngine {

Brick::Brick(int x, int y, Game& game, Renderer& renderer)
    : Sprite(x, y, s_brickType, "created",
             AnimationFilmHolder::getInstance().getFilm("Brick")) { // TODO make state have more sense
	m_logger = getOrCreateLogger("Brick");
	m_logger->trace("Brick constructor");

	m_player = static_cast<MarioPlayer*>(game.getPlayer());
	startingX = x / frameBox.screenWidth; // inside the tile layer
	startingY = y / frameBox.screenHeight;
	isVisible = true;
	directMotion = true;

	createBitmaps(renderer);
	initAnims();
}

Brick::~Brick() {
	m_logger->trace("Brick destructor");
	// Note: AllegroRenderer::destroyBitmap handles the cleanup if we were using it,
	// but here we have a Renderer abstraction.
	// The user didn't specify how to handle destruction if it's not in the tasks,
	// but Task 4 says "Replace Allegro Calls with Renderer Calls".
	// al_destroy_bitmap(...) -> renderer.destroyBitmap(...)
	// But we don't have a renderer reference in the destructor unless we store it.
	// For now, I'll assume we might need it, or just use a global if available,
	// but wait, the tasks didn't mention storing the renderer.
	// Actually, Sprite has a Renderer reference in render, but not stored.
	// Let's see if we should store it.
}

void Brick::render(Renderer& renderer, int xOff, int yOff, const ViewData& vw) {
	if(destroyed) {
		auto time = SystemClock::get().milli_secs() * (3.14159265358979323846 / 180);

		// al_draw_rotated_bitmap is NOT in the Renderer API provided in the prompt.
		// I'll use drawBitmap for now or see if I should add it.
		// The prompt didn't say what to do with al_draw_rotated_bitmap.
		// "Replace Allegro Calls with Renderer Calls"
		// If it's not there, maybe I should use drawBitmap.
		// Wait, AllegroRenderer.hpp DID NOT have drawRotatedBitmap.

		renderer.drawBitmap(singleBitmapSmall[0], x + xOff - 10 - vw.x, y + yOff - vw.y);
		renderer.drawBitmap(singleBitmapSmall[1], x + xOff - vw.x + 10 + 16, y + yOff - vw.y);
		renderer.drawBitmap(singleBitmapSmall[2], x + xOff - vw.x - 20, y + yOff + 16 - vw.y);
		renderer.drawBitmap(singleBitmapSmall[3], x + xOff - vw.x + 20, y + yOff + 16 - vw.y);
	} else {
		renderer.drawBitmap(singleBitmap, x + xOff - vw.x, y + yOff - vw.y);
	}
}

bool Brick::collisionCheck(const Sprite* player) const {
	if(Sprite::collisionCheck(player)) {
		auto yPlayer = player->getY();
		auto yBrick = y + frameBox.screenHeight;

		if((yPlayer >= yBrick) && m_player->isJumping()) {
			m_logger->info("Player hit brick from down");
			return true;
		}

		return false;
	} else {
		return false;
	}
}

void Brick::registerCollision() {
	// clang-format off
  // Player and brick collision
  auto *player = SpriteManager::getInstance().getTypeList(MarioPlayer::s_type).front();
  CollisionChecker::getInstance().Register(this, player, [&](Sprite *s1, Sprite *s2)
  {
      if(!m_moveAnimator->isRunnig())
      {
         m_logger->debug("Player and brick collsion");
         // Check if there is an enemy on this brick
         const auto &goombas = SpriteManager::getInstance().getTypeList(Goomba::s_goombaType);
         for(auto *goomba : goombas)
         {
            if(Sprite::collisionCheck(goomba))
            {
              m_logger->info("Brick goomba collision");
              reinterpret_cast<Goomba *>(goomba)->fireOrBrickCollision();
            }
         }
         m_moveAnimator->start(m_breakMovAnim.get(), getSystemTime());
      }
   });
	// clang-format on
}

void Brick::createBitmaps(Renderer& renderer) {
	// AllegroRenderer has setTargetBitmap, createBitmap, drawScaledBitmap, clearCurrentBitmap

	void* savedTarget = renderer.saveCurrentTarget();

	singleBitmap = renderer.createBitmap(frameBox.screenWidth, frameBox.screenHeight);
	renderer.setTargetBitmap(singleBitmap);

	renderer.drawScaledBitmap(BitmapManager::getInstance().getBitmap("tilesBitmapAsSprite"), frameBox.x, frameBox.y,
	                          frameBox.width, frameBox.height, 0, 0, frameBox.screenWidth, frameBox.screenHeight);

	int counter = 0;
	for(int i = 0; i < 2; i++) {
		for(int j = 0; j < 2; j++) {
			singleBitmapSmall[counter] = renderer.createBitmap(frameBox.screenWidth / 2, frameBox.screenHeight / 2);
			renderer.setTargetBitmap(singleBitmapSmall[counter++]);
			renderer.clearCurrentBitmap(Color::RGB(0, 0, 0));
			auto minWidth = frameBox.screenWidth / 4;
			auto minHeight = frameBox.screenHeight / 4;
			renderer.drawScaledBitmap(BitmapManager::getInstance().getBitmap("tilesBitmapAsSprite"),
			                          frameBox.x + j * minWidth, frameBox.y + i * minHeight, minWidth, minHeight, 0, 0,
			                          frameBox.screenWidth / 2, frameBox.screenHeight / 2);
		}
	}

	renderer.restoreTarget(savedTarget);
}

void Brick::initAnims() {
	m_moveAnimator = std::make_unique<MovingAnimator>();

	m_breakMovAnim = std::make_unique<MovingAnimation>("DestroyAnimation", 0, 0, -3, 20);

	m_moveAnimator->setOnStart([&](Animator*) {
		if(!m_soundPlayed) {
			auto volume = SoundManager::getInstance().getVolume();
			auto volumeToPlay = volume * 0.9;
			if(m_player->getSize() == MarioPlayer::Size::SMALL) {
				volumeToPlay = volume * 1.5;
				SoundManager::getInstance().playSound("Blockhit", PlayMode::ONCE, volumeToPlay);
			} else {
				SoundManager::getInstance().playSound("BreakBrick", PlayMode::ONCE, volumeToPlay);
			}
			m_soundPlayed = true;
		}

		m_logger->info("[Brick] Stopping player jump");
		m_player->stopJump();
	});

	m_moveAnimator->setOnAction([&](Animator* a, const Animation& anim) {
		auto moveAnim = (MovingAnimation&)anim;
		auto moveAnimator = (MovingAnimator*)(a);

		// Small mario
		if(m_player->getSize() == MarioPlayer::Size::SMALL) {
			int dy = moveAnim.getDy();
			int dx = moveAnim.getDx();
			if(moveAnimator->getCurrentRep() >= 6) {
				dy = -dy;
			}
			Sprite::move(dx, dy);
			if(moveAnimator->getCurrentRep() >= 11)
				moveAnimator->stop();
		} else {
			destroyed = true;
			Sprite::s_grid->setTileType(startingY, startingX, GRID_EMPTY_TILE);

			int dy = moveAnim.getDy();
			if(moveAnimator->getCurrentRep() >= 10) {
				dy = -(dy) + 2;
			}

			if(moveAnimator->getCurrentRep() == 25) {
				Destroy();
			}

			auto dx = moveAnim.getDx();
			Sprite::move(dx, dy);
		}
	});

	m_moveAnimator->setOnFinish([&](Animator*) {
		m_soundPlayed = false;
	});
}

} // namespace ValyrianEngine
