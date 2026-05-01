# Map Creation Guide

Levels are created using the [Tiled Map Editor](https://www.mapeditor.org/).

TODO - This needs way better documentation. Showcase how to make the tiles collision per pixel.
Showcase the properties of the enemies objects, etc.... many stuff

## Tile Setup
1. Use a **16x16** tile size.
2. Load the tilesets from the `media/` folder (e.g., `tileset_packed.png`).
3. Ensure the map is exported in **CSV format** (not Base64 or Zlib), as the current parser expects comma-separated values.

## Layer Structure
Every `.tmx` file must contain at least one layer with specific name:

### 1. `TileLayer`
- Contains all "solid" geometry (ground, pipes, blocks).
- Tiles in this layer are used for collision detection.
- **Solid Logic**: Tiles are considered solid based on their ID, configured in `GameManager::initGame()`.

### 3. `Enemies` (Object Layer)
- Used to spawn entities. Place "Point" or "Rectangle" objects here.
- The `type` property of the object must match the engine's internal strings (e.g., `Enemy`, `Powerup`).

## Loading a Map
To change the active level, modify `src/GameManager.cpp`:

```cpp
void GameManager::initGame() {
    // Path to the .csv export for visuals/collision
    const std::string tileLayer = "media/maps/LvlOne/TileLayer.csv";
    // Path to the .csv export for decoration
    const std::string backgroundLayer = "media/maps/LvlOne/BackgroundLayer.csv";
    // Path to the .tmx for object/enemy placement
    const std::string tmxConfiguration = "media/maps/LvlOne/FirstLevel.tmx";
    
    // ...
}
```

## Adding New Objects
When adding a new interactable object (like a specialized block), you must:
1. Define it in Tiled.
2. Update `Game::initEnemies` in `src/Game.cpp` to parse the new object type.
3. Create a corresponding class in `src/` that inherits from `Sprite` or `Enemy`.
