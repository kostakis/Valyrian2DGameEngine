# Animation & Animator System

The engine's animation system is designed for maximum performance and flexibility through a strict separation of **Data**, **State**, and **Logic**.

---

## 🎞️ 1. The Data Layer: `AnimationFilm`
An `AnimationFilm` is a read-only metadata object that describes a sequence of frames on a sprite sheet.

- **The Flyweight Pattern**: Because many sprites (e.g., 20 enemies) often share the same animation sequence, the film data is stored once in the `AnimationFilmHolder`.
- **Properties**: Contains the source bitmap, and a vector of `Rectangle` objects defining the source coordinates for every frame in the sequence.
- **Configuration**: Films are defined in `.cfg` files, allowing artists to adjust frame coordinates without recompiling code.

## 📋 2. The State Layer: `Animation`
An `Animation` object defines the *parameters* of a specific movement or visual change.

- **`FrameRangeAnimation`**: Defines `startFrame`, `endFrame`, `delay`, and `isLooping`. Used for walking, spinning coins, or waving flags.
- **`MovingAnimation`**: Defines a path as a series of offsets (`dx`, `dy`) and the number of repetitions. Used for jumping arcs, fireballs, or enemy movement.
- **`FlashAnimation`**: Defines a `flashDelay` and `totalDuration`. Used for hit-flicker or invulnerability effects.

## 🧠 3. The Logic Layer: `Animator`
The `Animator` is a state machine that drives the execution of an `Animation` on a target `Sprite`.

- **`Progress(timestamp)`**: Called every frame. The animator checks the current time against the animation's timing parameters to determine if the sprite's state (frame or position) needs updating.
- **Lifecycle**:
    - `Start(animation, sprite, timestamp)`: Attaches an animation to a sprite.
    - `Stop()`: Halts execution.
    - `OnFinish`: A callback mechanism (via `NotifyStopped`) that allows the game to trigger logic when an animation ends (e.g., the player transitioning from "jumping" to "falling").

---

## 🧩 Animator Composition

The power of the system lies in **composition**. A single sprite can be controlled by multiple animators simultaneously:

1.  **Visual Animation**: A `FrameRangeAnimator` cycles through the player's walking frames.
2.  **Positional Animation**: A `MovingAnimator` handles the parabolic curve of his jump.
3.  **Effect Animation**: A `FlashAnimator` causes him to blink if he just took damage.

These three animators operate independently on the same `Sprite` instance, allowing for complex, layered behaviors without monolithic "Mega-Animator" classes.

---

## ⏱️ Timing & Synchronization
The system relies on a central **`SystemClock`**. 

Instead of simple frame counting, animators use **millisecond timestamps**. This ensures that animations play at the correct speed regardless of the frame rate, preventing "fast-forward" gameplay on high-refresh-rate monitors.

The **`AnimatorManager`** acts as the global registry for all active animators, ensuring `Progress()` is called exactly once per frame for every entity in the world.
