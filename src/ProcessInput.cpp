#include "Game.hpp"
#include "MarioPlayer.hpp"

#include <Engine/Sprite.hpp>

#include <cmath>
#include <iostream>

void Game::processInput() {
	switch(ev->type) {
		case EventType::TIMER:
			if(ev->isTimerSource(engine->getLogicTimer())) {

				//---------------DEBBUGGING--------------------------

				if(keysState[static_cast<int>(Keys::KEY_UP)]) {
					m_viewData.y -= 4;
				}

				if(keysState[static_cast<int>(Keys::KEY_DOWN)]) {
					if(m_viewData.y >= 0)
						m_viewData.y = 0;
					else
						m_viewData.y += 4;
				}

				if(m_player->isInputDisabled()) {
					m_logger->warn("Disabled input ignoring");
					break;
				}

				if(keysState[static_cast<int>(Keys::KEY_W)]) {
					int dx = 0;
					int dy = -m_player->getYSpeed();
					m_player->move(dx, dy);
				}

				if(keysState[static_cast<int>(Keys::KEY_S)]) {
					if(enableDownMovement) {
						int dx = 0;
						int dy = m_player->getYSpeed();
						m_player->move(dx, dy);
					} else {
						// TODO: check for nearby pipe and trigger level transition
					}
				}

				// Vertical camera (only in FREE mode)
				if(m_cameraMode == CameraMode::FREE) {
					int mapHeightPixels = m_maxTileForHeightTileLayer * m_tileLayerTileHeight;
					int screenHalfHeight = ::Engine::Engine::s_windowHeight / 2;
					int newY = m_player->getY() - screenHalfHeight;

					if(newY < 0)
						newY = 0;
					if(newY > mapHeightPixels - ::Engine::Engine::s_windowHeight)
						newY = std::max(0, mapHeightPixels - ::Engine::Engine::s_windowHeight);

					m_viewData.y = newY;
				}

				//---------------END OF DEBBUGGING--------------------------

				if((enableDownMovement || !keysState[static_cast<int>(Keys::KEY_S)]) && keysState[static_cast<int>(Keys::KEY_A)]) {
					if(keysState[static_cast<int>(Keys::LSHIFT)]) {
						if(auto* mp = dynamic_cast<MarioPlayer*>(m_player))
							mp->enableRun();
					}
					m_player->setDirection(Sprite::Direction::LEFT);
					int dx = -m_player->getXSpeed();
					int dy = 0;

					// Mario mode restricts left movement to screen edge
					if(m_cameraMode == CameraMode::MARIO) {
						if(m_player->getX() + dx < m_viewData.x) {
							dx = m_viewData.x - m_player->getX();
						}
					}

					m_player->move(dx, dy);
				}

				if((enableDownMovement || !keysState[static_cast<int>(Keys::KEY_S)]) && keysState[static_cast<int>(Keys::KEY_D)]) {
					if(keysState[static_cast<int>(Keys::LSHIFT)]) {
						if(auto* mp = dynamic_cast<MarioPlayer*>(m_player))
							mp->enableRun();
					}

					m_player->setDirection(Sprite::Direction::RIGHT);
					int dx = m_player->getXSpeed();
					int dy = 0;
					m_player->move(dx, dy);

					// Mario mode scrolls when reaching center
					if(m_cameraMode == CameraMode::MARIO) {
						if(engine->getTileLayer()->checkMiddlePosition(m_player->getX(), m_viewData)) {
							int nextTile = DIV_TILE_WIDTH(m_viewData.x + m_viewData.windowWidth + dx) + 1;
							if(nextTile < m_maxTileWidthTileLayer && (dx > 0)) {
								m_viewData.updateX(dx);
								engine->getBackgroundLayer()->scroll(dx);
							} else {
								int tmp = -dx;
								m_player->move(tmp, dy);
							}
						}
					}
				}

				// FREE mode camera horizontal tracking
				if(m_cameraMode == CameraMode::FREE) {
					int mapWidthPixels = m_maxTileWidthTileLayer * m_tileLayerTileWidth;
					if(mapWidthPixels > ::Engine::Engine::s_windowWidth) {
						int screenHalfWidth = ::Engine::Engine::s_windowWidth / 2;
						int newX = m_player->getX() - screenHalfWidth;

						if(newX < 0)
							newX = 0;
						if(newX > mapWidthPixels - ::Engine::Engine::s_windowWidth)
							newX = mapWidthPixels - ::Engine::Engine::s_windowWidth;

						if(newX != m_viewData.x) {
							int diff = newX - m_viewData.x;
							m_viewData.x = newX;
							engine->getBackgroundLayer()->scroll(diff);
						}
					} else {
						m_viewData.x = 0;
					}
				}
			}

			if(ev->isTimerSource(engine->getFPSTimer())) {
				reDraw = true;
			}
			break;

		case EventType::KEY_DOWN:
			switch(ev->keyboard.keycode) {
				case KeyCode::UP:
					keysState[static_cast<int>(Keys::KEY_UP)] = true;
					break;
				case KeyCode::DOWN:
					keysState[static_cast<int>(Keys::KEY_DOWN)] = true;
					break;
				case KeyCode::RIGHT:
					keysState[static_cast<int>(Keys::KEY_RIGHT)] = true;
					break;
				case KeyCode::LSHIFT:
					keysState[static_cast<int>(Keys::LSHIFT)] = true;
					break;
				case KeyCode::LEFT:
					keysState[static_cast<int>(Keys::KEY_LEFT)] = true;
					break;
				case KeyCode::G:
					keysState[static_cast<int>(Keys::KEY_G)] = true;
					renderGrid = true;
					break;
				case KeyCode::W:
					keysState[static_cast<int>(Keys::KEY_W)] = true;
					break;
				case KeyCode::D:
					keysState[static_cast<int>(Keys::KEY_D)] = true;
					break;
				case KeyCode::S:
					keysState[static_cast<int>(Keys::KEY_S)] = true;
					break;
				case KeyCode::A:
					keysState[static_cast<int>(Keys::KEY_A)] = true;
					break;
				case KeyCode::SPACE:
					keysState[static_cast<int>(Keys::SPACEBAR)] = true;
					m_logger->debug("Spacebar pressed");
					if(enableJump && !m_player->isInputDisabled()) {
						if(auto* mp = dynamic_cast<MarioPlayer*>(m_player))
							mp->startJumping();
					}
					break;
				case KeyCode::ESCAPE:
					finished = true;
					break;
				default:
					break;
			}
			break;
		case EventType::KEY_UP:
			switch(ev->keyboard.keycode) {
				case KeyCode::UP:
					keysState[static_cast<int>(Keys::KEY_UP)] = false;
					break;
				case KeyCode::DOWN:
					keysState[static_cast<int>(Keys::KEY_DOWN)] = false;
					break;
				case KeyCode::RIGHT:
					keysState[static_cast<int>(Keys::KEY_RIGHT)] = false;
					break;
				case KeyCode::LEFT:
					keysState[static_cast<int>(Keys::KEY_LEFT)] = false;
					break;
				case KeyCode::LSHIFT:
					keysState[static_cast<int>(Keys::LSHIFT)] = false;
					if(auto* mp = dynamic_cast<MarioPlayer*>(m_player))
						mp->stopRun();
					break;
				case KeyCode::G:
					keysState[static_cast<int>(Keys::KEY_G)] = false;
					renderGrid = false;
					break;
				case KeyCode::W:
					keysState[static_cast<int>(Keys::KEY_W)] = false;
					if(!m_player->isInputDisabled()) {
						m_player->stopMoving();
					}
					break;
				case KeyCode::D:
					keysState[static_cast<int>(Keys::KEY_D)] = false;
					if(!m_player->isInputDisabled()) {
						m_player->stopMoving();
					}
					break;
				case KeyCode::S:
					keysState[static_cast<int>(Keys::KEY_S)] = false;
					if(!m_player->isInputDisabled()) {
						m_player->stopMoving();
					}
					break;
				case KeyCode::A:
					keysState[static_cast<int>(Keys::KEY_A)] = false;
					if(!m_player->isInputDisabled()) {
						m_player->stopMoving();
					}
					break;
				case KeyCode::SPACE:
					keysState[static_cast<int>(Keys::SPACEBAR)] = false;
					m_logger->debug("Spacebar unpressed");
					if(enableJump) {
						if(auto* mp = dynamic_cast<MarioPlayer*>(m_player)) {
							mp->stopJump();
							mp->resetJump();
						}
					}
					break;
				default:
					break;
			}
			break;
		case EventType::DISPLAY_CLOSE:
			finished = true;
			state = State::Quit;
			break;
		default:
			break;
	}
}
