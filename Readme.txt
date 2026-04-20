# TikkiTaka / BattleBlaster

A top-down **tank combat game** built in **Unreal Engine with C++**, focused on **modular gameplay systems**, **intentional combat design**, and **decision-driven AI**.

The player controls a tank, aims with the mouse, and battles enemy tanks and towers in arena-style encounters. The project emphasizes readable combat, scalable architecture, and system-driven gameplay rather than one-off scripted behavior.

---

## Table of Contents

- [Overview](#overview)
- [Core Technical Gameplay Design Principle](#core-technical-gameplay-design-principle)
- [How the Principle Was Applied](#how-the-principle-was-applied)
- [How It Improved the Quality of the Game](#how-it-improved-the-quality-of-the-game)
- [How to Play](#how-to-play)
- [Controls](#controls)
- [What Makes the Game Complex](#what-makes-the-game-complex)
- [Technical Highlights](#technical-highlights)
- [Game Programming Patterns Used](#game-programming-patterns-used)
- [Why This Project Matters](#why-this-project-matters)
- [Future Improvements](#future-improvements)

---

## Overview

**TikkiTaka / BattleBlaster** is a top-down combat game where the player pilots a tank and fights enemy units in a combat arena. Enemies include AI-controlled tanks and defensive towers, each reacting to the state of the world through structured gameplay systems.

This project was designed not just as a playable game, but as a **technical gameplay systems project**. Its goal is to demonstrate how multiple systems—input, aiming, AI, health, combat logic, and runtime level setup—can work together cleanly and predictably.

The overall design philosophy is:

> **Combat should feel readable, intentional, and systems-driven.**

That means the player should be able to understand:
- why enemies attack or stop attacking,
- why movement and aiming behave the way they do,
- how positioning affects combat,
- and how the game state drives outcomes.

---

## Core Technical Gameplay Design Principle

The central technical gameplay design principle behind this project is:

### **Clarity through modular, decision-driven gameplay**

This means the game was built so that:
- player actions are predictable,
- enemy behavior is purposeful,
- major systems are separated by responsibility,
- and gameplay outcomes come from interacting systems rather than random or hardcoded scripts.

A second important principle is:

### **Separation of input, decision-making, and execution**

The project intentionally separates:
- **input** → what the player wants to do,
- **decision-making** → how AI chooses what to do,
- **execution** → how the pawn/world performs those actions.

This separation improves both gameplay feel and code quality.

---

## How the Principle Was Applied

### 1. Command-based player input

The player controller does not directly hardcode all gameplay behavior. Instead, player actions are represented through command-style execution.

Examples:
- movement command
- aim command
- fire command

This keeps the input layer cleaner and separates button/key handling from gameplay execution.

---

### 2. GOAP-based AI behavior

Enemy behavior is built around a **Goal-Oriented Action Planning (GOAP)** style architecture.

AI reasons about combat conditions such as:
- whether it has a target,
- whether the target is in range,
- whether it has line of sight,
- whether the turret is aligned,
- whether it is allowed to fire.

Instead of acting through a rigid scripted chain, the AI chooses actions based on the current world state.

This makes enemy behavior more understandable, more modular, and easier to tune.

---

### 3. Component-based gameplay systems

Important gameplay concerns are split into dedicated systems instead of being buried inside one giant actor class.

Examples include:
- health handling,
- AI brain logic,
- combat behavior,
- runtime setup logic.

This improves modularity and makes debugging much easier.

---

### 4. Event-driven system responses

The project uses event-driven logic where systems need to respond to one another without being tightly coupled.

Examples:
- destruction/death affecting game state,
- UI responding to state changes,
- spawned actors being configured after creation.

This makes the architecture more scalable and easier to maintain.

---

### 5. Runtime-authored combat setups

The project also explores a runtime/custom level setup pipeline where actor placement data can be stored and later spawned into the world.

This supports:
- testing different arena layouts,
- iterating on combat encounters,
- and building structured gameplay setups beyond a single hardcoded level.

---

## How It Improved the Quality of the Game

### 1. Combat became more readable

Combat behavior is easier to understand because AI acts according to meaningful checks:
- range,
- line of sight,
- alignment,
- cooldown,
- action validity.

This makes the game feel more fair and less arbitrary.

---

### 2. Debugging became easier

Because systems are separated cleanly, bugs can be isolated faster.

For example, if an enemy is not firing, the problem can be narrowed down to:
- target detection,
- range check,
- line of sight,
- turret alignment,
- fire command execution,
- spawn or projectile setup.

This directly improves development speed and reliability.

---

### 3. Gameplay became easier to tune

Values like:
- scan range,
- fire cooldown,
- aim speed,
- alignment tolerance,
- attack range

can be adjusted without rewriting major architecture.

This makes the project much easier to iterate on.

---

### 4. The architecture supports growth

Because the project is modular, it is easier to expand with:
- new enemy types,
- new combat actions,
- new levels,
- new control options,
- new authored scenarios,
- improved UI/settings support,
- more advanced AI.

This makes the project stronger as both a game and a technical showcase.

---

### 5. Player control quality improved

Separating input handling from gameplay execution helps the controls stay more maintainable and predictable.

That is especially important in a tank game, where:
- movement,
- turret aiming,
- firing,
- and vehicle feel

must work together smoothly.

---

## How to Play

### Objective

Destroy enemy units and survive the combat encounter.

Depending on the current scenario, the player may face:
- enemy tanks,
- enemy towers,
- multiple hostiles in the arena.

The player succeeds by eliminating threats while staying alive.

---

## Controls

### Current Core Controls

- **Move** using movement controls
- **Aim** using the mouse cursor
- **Fire** using the fire input

---

## Special Notes About Playing the Game

### 1. This is not a standard character controller

The player is controlling a **tank**, not a lightweight character.

That means:
- movement has weight,
- aiming is separate from locomotion,
- turret direction matters,
- and firing depends on positioning and orientation.

---

### 2. Movement and aiming are intentionally separated

The tank body moves through the world, while the turret aims toward the mouse cursor.

This means the player must manage:
- movement,
- target tracking,
- spacing,
- line of sight,
- and firing direction at the same time.

This creates more tactical gameplay.

---

### 3. Positioning matters a lot

Because combat depends on line of sight and range, small changes in position can completely change a fight.

The player benefits from:
- staying in good firing lanes,
- avoiding overexposure,
- maintaining useful range,
- understanding enemy coverage zones.

---

### 4. Enemy behavior is systems-based

Enemies are not just scripted to attack constantly.

They respond to world-state checks such as:
- target visibility,
- target distance,
- alignment,
- cooldown availability.

That means the player can learn enemy behavior and respond strategically.

---

## What Makes the Game Complex

This project combines several non-trivial systems into one playable experience:

- command-driven player input
- cursor-based aiming
- GOAP-based AI planning
- modular gameplay components
- event-driven reactions
- runtime spawning / authored level setup
- debugging and tuning for LOS, range, alignment, and firing logic

The challenge is not simply making a tank move and shoot.

The challenge is making all of those systems work together in a way that feels:
- coherent,
- intentional,
- readable,
- and playable.

---

## Technical Highlights

- Built in **Unreal Engine**
- Implemented in **C++**
- Uses **Enhanced Input**
- Uses **command-based gameplay input handling**
- Uses **GOAP-style AI architecture**
- Uses **component-based gameplay systems**
- Uses **runtime spawning / structured level setup**
- Supports **top-down cursor aiming**
- Focuses on **tank combat readability and control clarity**

---

## Game Programming Patterns Used

### Command Pattern
Used for player action execution such as:
- movement
- aiming
- firing

### GOAP (Goal-Oriented Action Planning)
Used for enemy decision-making based on world-state conditions and valid actions.

### Component Pattern
Used to separate major gameplay responsibilities such as health and AI logic.

### Observer / Event-Driven Design
Used where systems need to react to gameplay changes without being tightly coupled.

### Factory / Spawner-style Runtime Construction
Used in the runtime/custom level setup pipeline to create gameplay actors from stored data.

### Data-Driven Design
Used for tuning combat values and authoring encounter setups more flexibly.

---

## Why This Project Matters

This project demonstrates more than a playable prototype.

It shows the ability to:
- architect gameplay systems cleanly,
- build modular AI,
- separate responsibility across gameplay layers,
- debug combat behaviors systematically,
- and design a game where technical choices meaningfully improve player experience.

From the player perspective, this creates:
- clearer combat,
- more understandable enemies,
- more tactical encounters,
- more reliable controls.

From the engineering perspective, it creates:
- easier debugging,
- better extensibility,
- cleaner systems,
- faster iteration.

---

## Future Improvements

Potential next steps for the project include:

- expanded enemy behaviors
- more combat scenarios
- improved runtime/custom level workflows
- player-remappable controls
- additional UI polish
- gamepad-specific aiming support
- better progression structure
- more encounter variety
- additional weapons and combat roles

---

## Summary

**TikkiTaka / BattleBlaster** is a C++ Unreal Engine tank combat project built around the principle of **clear, modular, decision-driven gameplay**.

That principle was applied through:
- command-driven player input,
- GOAP-based AI,
- component-oriented systems,
- event-driven architecture,
- and structured runtime spawning.

These choices improved the game by making:
- combat more readable,
- controls more reliable,
- AI more purposeful,
- debugging easier,
- and the overall project more scalable.

The result is both a playable combat game and a strong technical gameplay systems project.