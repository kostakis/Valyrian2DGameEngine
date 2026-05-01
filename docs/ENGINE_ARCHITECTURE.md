# Engine Architecture

The ValyrianEngine is a high-performance, game-agnostic 2D framework designed for modularity and efficiency. Originally built for a classic 2D platformer clone, its core systems are entirely decoupled from game-specific logic, allowing for easy adaptation to any 2D genre, built in C++17.

## 🚀 Key Architectural Strengths

### 1. Pluggable Backend Architecture
The engine features a fully abstracted rendering and input layer. While it currently ships with an **Allegro 5** implementation, the architecture is designed for seamless switching between backends (e.g., SDL2, SFML, or DirectX/Vulkan).

- **Compile-time Switching**: Using a `Renderer` router pattern, the engine selects the appropriate backend at compile time, eliminating the overhead of virtual function calls in the hot path.
- **Backend Agnostic**: The core engine logic interacts with a unified API, ensuring that porting to a new library requires only a new backend implementation without touching the game logic.

### 2. Instant Grid System (Spatial Partitioning)
Collision detection and environmental queries are powered by a highly optimized **Grid System**.
- **$O(1)$ Lookups**: By partitioning the world into a logical grid, the engine can perform instantaneous checks for solid ground, walls, or slopes.
- **Hybrid Rasterization**: The grid supports both simple tile-based blocking and complex per-tile collision shapes (rasterized from TMX metadata), providing pixel-perfect precision with grid-based speed.
- **Motion Filtering**: The `Grid` class provides `FilterGridMotion`, which acts as a "physics barrier," preventing entities from entering solid cells before they even move, ensuring stability and preventing tunneling.

### 3. Centralized Sprite Management
The `SpriteManager` acts as the single source of truth for all active entities in the game world.
- **Easy Allocation**: Sprites are automatically registered upon creation, making life-cycle management effortless for the developer.
- **Batch Operations**: Supports efficient batch rendering and updates, categorized by `TypeId` (e.g., "Enemy", "Player").
- **Decoupled Lifecycle**: The manager facilitates global cleanups and state resets, essential for scene transitions or game restarts.

### 4. Memory Safety & The Destruction Manager
To solve the "invalidated pointer" problem common in complex game loops (e.g., an object being deleted while its collision is still being processed), the engine employs a **Deferred Destruction** pattern.
- **`LatelyDestroyable`**: Objects (like Sprites and Animators) inherit from this interface.
- **`DestructionManager`**: Instead of immediate deletion, objects are marked for destruction. The `DestructionManager` safely purges these objects only at the end of the frame, after all systems have finished their updates.

### 5. Flexibility via "Duck Typing" & Templates
The engine leverages C++ templates to provide the flexibility of duck typing without sacrificing type safety or performance.
- **Generic Callbacks**: The `CollisionChecker` and `Animator` systems use template-based registration for actions (`onFinish`, `onAction`, etc.). This allows any callable (lambdas, member functions, functors) to be attached to game events.
- **Loose Coupling**: Systems can interact with entities without requiring them to inherit from massive, complex interfaces. If it looks like a Sprite and behaves like a Sprite, the engine can handle it.

---

## 🏗️ Core System Overview

### Singleton Resource Managers
To prevent redundant I/O and memory bloat, the engine uses specialized singletons:
- **`BitmapManager`**: Implements a flyweight pattern for textures, ensuring each asset is loaded only once.
- **`SoundManager`**: Provides a high-level API for audio playback, managing samples and streams transparently.
- **`AnimationFilmHolder`**: A factory that manages `AnimationFilm` objects, separating animation metadata from the sprites themselves.

### Modular Components
- **`GravityHandler`**: A pluggable component that can be attached to any sprite to instantly grant it physics-based gravity behavior.
- **`AnimatorManager`**: A global orchestrator for all active animations (frame-range, moving, flashing), ensuring they progress in sync with the system clock.

---

## 🔄 The Execution Pipeline (The Game Loop)

Every frame follows a strict sequence to ensure consistency and safety:

1.  **Input Phase**: Hardware events are captured and mapped to game actions.
2.  **AI & Logic Phase**: Game entities update their internal state and decision-making logic.
3.  **Animation Phase**: `AnimatorManager` progresses all active animations by the elapsed time.
4.  **Collision Phase**: `CollisionChecker` evaluates registered pairs and triggers template-based callbacks.
5.  **Render Phase**: The `Renderer` draws layers (Background, Tile, Sprite) in Z-order.
6.  **Destruction Phase**: `DestructionManager` commits all pending deletions, safely cleaning up memory for the next frame.
