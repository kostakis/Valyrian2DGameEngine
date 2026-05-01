project(ValyrianEngine LANGUAGES CXX)

# Rendering / Graphics backend solution -- Currently on allego supported
set(ENGINE_BACKEND "ALLEGRO" CACHE STRING "Rendering backend")
set_property(CACHE ENGINE_BACKEND PROPERTY STRINGS "ALLEGRO" "SDL")

if(ENGINE_BACKEND STREQUAL "ALLEGRO")
    set(BACKEND_SOURCES
        src/Backends/Allegro/AllegroRenderer.cpp
    )
    find_package(Allegro CONFIG REQUIRED)
    set(BACKEND_LIBS
        Allegro::allegro
        Allegro::allegro_main
        Allegro::allegro_font
        Allegro::allegro_ttf
        Allegro::allegro_image
        Allegro::allegro_audio
        Allegro::allegro_acodec
        Allegro::allegro_primitives
    )
    set(BACKEND_DEFINE USE_ALLEGRO_BACKEND)
elseif(ENGINE_BACKEND STREQUAL "SDL")
	## THIS IS NOT SUPPORTED OR TESTED YET
    # set(BACKEND_SOURCES src/Backends/Sdl/SdlRenderer.cpp)
    # find_package(SDL2 REQUIRED)
    # set(BACKEND_LIBS SDL2::SDL2 SDL2::SDL2_image SDL2::SDL2_ttf)
    # set(BACKEND_DEFINE USE_SDL_BACKEND)
endif()

file(GLOB engine_headers Engine/*.hpp)
file(GLOB engine_sources src/*.cpp)

add_library(Engine ${engine_headers} ${engine_sources} ${BACKEND_SOURCES})

target_compile_definitions(Engine PUBLIC ${BACKEND_DEFINE})

target_include_directories(Engine
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Engine
)

target_compile_features(Engine PUBLIC cxx_std_14)

find_package(spdlog CONFIG REQUIRED)
find_package(zstd CONFIG REQUIRED)
find_package(pugixml CONFIG REQUIRED)

find_path(TMXLITE_INCLUDE_DIR tmxlite/Map.hpp REQUIRED)
find_library(TMXLITE_LIBRARY_RELEASE NAMES tmxlite-s tmxlite REQUIRED)
find_library(TMXLITE_LIBRARY_DEBUG   NAMES tmxlite-s-d tmxlite-d)

add_library(tmxlite::tmxlite STATIC IMPORTED)
set_target_properties(tmxlite::tmxlite PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${TMXLITE_INCLUDE_DIR}"
    IMPORTED_LOCATION              "${TMXLITE_LIBRARY_RELEASE}"
    IMPORTED_LOCATION_RELEASE      "${TMXLITE_LIBRARY_RELEASE}"
)
if(TMXLITE_LIBRARY_DEBUG)
    set_property(TARGET tmxlite::tmxlite APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(tmxlite::tmxlite PROPERTIES
        IMPORTED_LOCATION_DEBUG "${TMXLITE_LIBRARY_DEBUG}"
    )
endif()

target_link_libraries(Engine PUBLIC
    ${BACKEND_LIBS}
    tmxlite::tmxlite
    $<IF:$<TARGET_EXISTS:zstd::libzstd_static>,zstd::libzstd_static,zstd::libzstd_shared>
    pugixml::pugixml
    spdlog::spdlog
)
