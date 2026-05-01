#ifndef RENDERER_ROUTER_HPP
#define RENDERER_ROUTER_HPP

#if defined(USE_ALLEGRO_BACKEND)
#include "Backends/Allegro/AllegroRenderer.hpp"
namespace ValyrianEngine {
using Renderer = AllegroRenderer;
}
#elif defined(USE_SDL_BACKEND)
// #include "Backends/Sdl/SdlRenderer.hpp"
// namespace ValyrianEngine {
// using Renderer = SdlRenderer;
// }
#else
#error "No rendering backend defined! Please define USE_ALLEGRO_BACKEND or USE_SDL_BACKEND in CMake."
#endif

#endif
