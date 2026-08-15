# TikkiTaka / BattleBlaster

**Systems-Driven Top-Down Tank Combat | Unreal Engine 5.6 | C++**

TikkiTaka / BattleBlaster is a top-down tank combat project built in **Unreal Engine 5.6 with C++**, designed around modular gameplay architecture rather than one-off scripted behavior.

The project combines several larger gameplay-engineering systems:

* a custom **Goal-Oriented Action Planning (GOAP)** framework,
* autonomous tank and tower combat AI,
* a reusable **Command-based action layer** shared between player and AI execution,
* a fully interactive **runtime level editor**,
* serialized custom-level data,
* runtime reconstruction of authored encounters,
* per-instance enemy AI tuning,
* patrol-route authoring,
* component-based health and combat behavior,
* campaign and custom-level progression.

The project was primarily built as a gameplay-systems exercise: the focus is not only on making tanks move and shoot, but on building systems that can be extended, reasoned about, debugged, and reused.

---

## Technical Overview

| System                         | Implementation                                                | Engineering Focus                 |
| ------------------------------ | ------------------------------------------------------------- | --------------------------------- |
| **GOAP Planner**               | Symbolic world state, preconditions, effects and action costs | AI planning                       |
| **GOAP Brain**                 | Runtime planning, action execution and replanning             | Decision/execution separation     |
| **Tank AI**                    | Patrol, acquisition, chase, aim, fire and health recovery     | Goal-driven autonomous behavior   |
| **Tower AI**                   | Range, LOS, alignment and cooldown-driven attacks             | Combat-state reasoning            |
| **Command Layer**              | Shared Move, Aim, Fire and Pause commands                     | Input/execution decoupling        |
| **Runtime Level Editor**       | Placement, selection, ghost previews, validation and tuning   | Gameplay tooling                  |
| **Custom Level Serialization** | Structured placed-actor records and SaveGame persistence      | Data-driven content               |
| **Runtime Level Spawner**      | Multi-pass world reconstruction and reference resolution      | Runtime content construction      |
| **AI Tuning**                  | Per-instance tank and tower tuning stored with levels         | Designer-facing iteration         |
| **Patrol Authoring**           | Persistent ID-based tank-to-patrol relationships              | Entity relationship serialization |
| **Progression Flow**           | Campaign, custom preview and campaign-to-custom modes         | Game-state architecture           |
| **Combat Foundation**          | Health component, projectiles, damage and effects             | Component-oriented gameplay       |

---

# Custom GOAP Framework

One of the central systems in TikkiTaka is a custom implementation of **Goal-Oriented Action Planning**.

Instead of encoding enemy behavior as a large collection of nested conditionals, AI actions describe:

```text
Preconditions
     +
Effects
     +
Cost
     ↓
GOAP Planner
     ↓
Action Plan
```

At its core, every GOAP action exposes:

```cpp
FName ActionName;
float Cost;

TMap<FName, bool> Preconditions;
TMap<FName, bool> Effects;
```

along with runtime behavior:

```cpp
CheckProceduralPrecondition()
Perform()
IsDone()
ResetAction()
```

This creates an important separation:

> **Symbolic planning decides what should happen; gameplay code decides whether and how the action can actually happen.**

---

## Cost-Based Plan Search

The planner begins with the current world state and recursively evaluates actions whose preconditions are satisfied.

Each valid action produces a new simulated state:

```text
Current State
     ↓
Check Preconditions
     ↓
Apply Action Effects
     ↓
New State
     ↓
Goal reached?
```

Multiple valid plans can be discovered.

The planner then compares their accumulated action costs and returns the **lowest-cost plan that satisfies the requested goal**.

Conceptually:

```text
                 Current State
                      │
             ┌────────┼─────────┐
             │        │         │
          Action A Action B  Action C
             │        │
             ▼        ▼
          State A   State B
             │        │
          Action D  Action E
             │        │
             ▼        ▼
            Goal     Goal
          Cost: 4   Cost: 7

               ↓

          Choose Cost 4
```

The planner is therefore not simply executing a predetermined sequence. It derives a sequence from available actions and the current state of the world.

---

# GOAP Brain Component

Planning and execution are handled by a reusable GOAP brain component.

The runtime loop follows roughly:

```text
Build World State
       ↓
Build Goal State
       ↓
Generate Plan
       ↓
Execute Current Action
       ↓
Procedural condition failed?
       ├── Yes → Replan
       │
       └── No
            ↓
        Action complete?
            ├── No → Continue
            └── Yes
                 ↓
          Execute next action
```

A plan is therefore not treated as permanently valid.

Actions can invalidate themselves at runtime through procedural checks, allowing the AI to rebuild its plan when gameplay conditions no longer match the assumptions used during planning.

This separation allows the same planning framework to support very different enemy types.

---

# Tank AI

Enemy tanks use the GOAP system to transition between several different behavioral objectives.

Available actions include:

```text
Find Patrol Point
Move To Patrol Point

Scan For Player
Chase Target
Aim At Target
Fire

Find Health Pickup
Move To Health Pickup
```

The AI's world state contains information such as:

```text
HasTarget
TargetInScanRange
TargetInAttackRange

LowHealth
HasHealthPickup
ReachedHealthPickup

HasPatrolPoint
ReachedPatrolPoint

CanFire
TurretAligned

WantsAttack
WantsRecovery
```

These state values allow gameplay behavior to emerge from combinations of conditions rather than a single fixed script.

---

## Dynamic Goal Selection

The tank does not pursue the same goal at all times.

Its goal depends on current gameplay state.

### Recovery

```text
Low Health
    ↓
Commit To Recovery
    ↓
Find Health Pickup
    ↓
Move To Pickup
    ↓
Recover
```

### Combat

```text
Detect Player
    ↓
Acquire Target
    ↓
Close Distance
    ↓
Align Turret
    ↓
Fire
```

### Patrol

```text
No Immediate Threat
       ↓
Find Patrol Point
       ↓
Move To Patrol Point
       ↓
Continue Patrol
```

This gives the same enemy the ability to transition between **exploration, combat and survival behavior** without implementing those modes as a monolithic state machine.

---

# Behavioral Commitment and Hysteresis

The tank AI also introduces commitment state around combat and recovery.

For example, health recovery is not immediately abandoned the instant health crosses the low-health threshold.

Instead:

```text
Health drops below threshold
        ↓
Commit To Recovery
        ↓
Acquire Health Pickup
        ↓
Recover
        ↓
Reach recovery threshold
        ↓
Release Recovery Commitment
```

This prevents unstable decision oscillation around threshold boundaries.

The same architecture maintains combat commitment while the player remains a valid threat.

That produces more consistent autonomous behavior than repeatedly switching objectives every frame.

---

# Hybrid Planned + Reactive Combat

Not every combat operation needs to become an entire GOAP planning problem.

The tank architecture uses GOAP to establish tactical state such as:

```text
Acquire Target
Reach Attack Range
Align Turret
```

while immediate firing can be handled reactively once the required combat conditions are satisfied.

This creates a useful distinction between:

```text
Strategic / Tactical Decision

        GOAP

          ↓

Immediate Gameplay Reaction

     Reactive Logic
```

For fast-changing combat situations, this avoids unnecessarily rebuilding a full plan for every weapon opportunity.

---

# Tower Combat AI

Towers use the same GOAP foundation but expose a different action set and world model.

The tower's symbolic state includes:

```text
HasTarget
TargetInRange
HasLineOfSight
TurretAligned
CanFire
AttackComplete
```

Its planned attack chain is approximately:

```text
Scan
  ↓
Validate Target
  ↓
Check Range + LOS
  ↓
Aim
  ↓
Wait For Alignment
  ↓
Fire
```

The important part is that firing is gated by multiple explicit gameplay conditions.

A tower cannot simply fire because a target exists.

It must reason about:

* target acquisition,
* range,
* line of sight,
* turret alignment,
* fire cooldown.

This makes combat behavior much easier to diagnose and tune.

---

# Line-of-Sight and Combat Validation

Tower attacks contain explicit line-of-sight evaluation rather than relying purely on distance.

The combat pipeline therefore distinguishes:

```text
Target Exists
      │
      ▼
Target In Range
      │
      ▼
Line Of Sight
      │
      ▼
Turret Aligned
      │
      ▼
Cooldown Ready
      │
      ▼
Fire
```

This makes the attack system more readable both for the player and for debugging.

A failure to fire can be traced back to a specific gameplay predicate rather than being hidden inside one large AI function.

---

# Shared Command Architecture

Player input is implemented through a lightweight **Command Pattern**.

The controller maps logical gameplay actions to command objects:

```text
EInputAction::Move  → TT_MoveCommand
EInputAction::Aim   → TT_AimCommand
EInputAction::Fire  → TT_FireCommand
EInputAction::Pause → TT_PauseCommand
```

The controller handles input acquisition.

The command handles gameplay execution.

```text
Enhanced Input
     ↓
Player Controller
     ↓
Command
     ↓
Pawn
```

This avoids directly embedding all gameplay behavior inside input callbacks.

---

# AI Reuses the Gameplay Command Layer

One of the more important architectural details is that AI does not implement an entirely separate movement/aim/fire stack.

Enemy tank AI uses the same command concepts used by the player's control layer:

```text
                   Gameplay Commands
                         │
             ┌───────────┴───────────┐
             │                       │
       Player Controller          Tank AI
             │                       │
             └───────────┬───────────┘
                         ▼
                       Pawn
```

For example:

* `TT_MoveCommand` supports both player-relative movement and AI movement toward a world location.
* `TT_AimCommand` rotates the turret toward a world-space target.
* `TT_FireCommand` performs projectile creation through the pawn's projectile spawn point.

This creates a cleaner execution boundary:

> **Humans and AI can decide differently while still operating the same gameplay verbs.**

That is a useful architecture for gameplay systems because input source and gameplay capability remain separate concerns.

---

# Top-Down Tank Controls

The player controller uses Unreal's **Enhanced Input** system.

Movement and turret aiming are deliberately independent.

```text
Keyboard
   ↓
Tank Hull Movement

Mouse Cursor
   ↓
World Hit Position
   ↓
Aim Command
   ↓
Turret Rotation
```

This supports tank-style control where vehicle orientation and firing direction are separate gameplay decisions.

The aiming command uses smooth interpolation toward the desired yaw rather than snapping the turret instantly toward the cursor.

---

# Modular Pawn Architecture

Combat entities derive shared functionality from a common base pawn.

The base pawn owns core composition such as:

```text
ATT_BasePawn
│
├── Collision Capsule
├── Base Mesh
├── Turret Mesh
├── Projectile Spawn Point
└── Health Component
```

Movement, turret aiming, projectile spawning and health can therefore operate against common pawn capabilities.

Specialized tanks and towers can build different behavior on top of the same core combat representation.

---

# Component-Based Health

Health is implemented as its own Actor Component rather than being embedded directly into every combat pawn.

The component:

* initializes health,
* listens for Unreal damage events,
* applies incoming damage,
* supports healing,
* exposes health state,
* informs the GameMode when an actor dies.

This keeps fundamental combat state reusable across multiple actor types.

It is also consumed directly by AI when making recovery decisions.

```text
Damage System
     ↓
Health Component
     ↓
Current Health
     ├──────────────→ Death / GameMode
     │
     └──────────────→ AI World State
```

So health is not merely a UI statistic—it participates directly in autonomous decision making.

---

# Projectile Combat

Weapons spawn projectile actors through a dedicated projectile spawn point attached to the turret.

The missile implementation includes:

* projectile movement,
* collision,
* owner filtering,
* Unreal damage application,
* Niagara impact effects,
* camera shake,
* finite projectile lifetime.

The owner check prevents the projectile from immediately damaging the pawn that fired it.

```text
Fire Command
     ↓
Projectile Spawn Point
     ↓
Missile
     ↓
Collision
     ↓
Apply Damage
     ↓
Health Component
```

---

# Runtime Level Editor

A major secondary system in the project is a **custom runtime level editor implemented in C++**.

This editor allows gameplay encounters to be authored from inside the running game instead of requiring every encounter to be built manually in the Unreal Editor.

Supported placeable types include:

```text
Player Start
Enemy Tower
Enemy Tank
Wall
Patrol Point
```

The editor maintains a data representation separate from its visual preview actors.

```text
Level Definition
      │
      ▼
Placed Actor Records
      │
      ├── Type
      ├── Unique Instance ID
      ├── Transform
      ├── AI Tuning
      └── Patrol Relationships
      │
      ▼
Preview Actors
```

This separation is important because the saved level is defined by gameplay data—not by serializing temporary editor-preview actors.

---

# Grid-Snapped Placement

World positions are snapped to a configurable placement grid.

```text
Mouse World Position
        ↓
Grid Snap
        ↓
Bounds Validation
        ↓
Spacing Validation
        ↓
Ghost Preview
        ↓
Placement
```

The placement system checks:

* editor boundaries,
* object spacing,
* overlap rules,
* placeable type,
* PlayerStart uniqueness.

This prevents malformed levels from being constructed accidentally.

---

# Ghost Preview System

Before an object is committed to the level, the runtime editor maintains a separate ghost actor.

The ghost:

* follows the intended world location,
* uses grid-snapped coordinates,
* has gameplay collision disabled,
* reflects whether the current placement is valid.

Committed preview actors use query-only collision so they can still be selected with mouse traces without behaving like gameplay actors.

This establishes a clean separation between:

```text
Editor Representation
```

and:

```text
Runtime Gameplay Representation
```

---

# Selection and Editing

Placed entities receive persistent instance IDs.

The editor maintains a map from:

```text
Instance ID → Preview Actor
```

which allows a selected entity to be related back to its underlying data record.

Selected actors support operations such as:

```text
Select
Delete
Move
Rotate
Edit AI Tuning
Link Patrol Point
Unlink Patrol Point
```

Selection highlighting is implemented independently from the gameplay data using custom depth rendering.

---

# Per-Instance AI Tuning

Enemy configuration can be stored directly inside each level record.

### Tower Tuning

A tower record contains values such as:

```text
Scan Range
Fire Range
Fire Cooldown
Aim Speed
Aim Tolerance
```

### Tank Tuning

A tank record contains values such as:

```text
Scan Range
Attack Range
Fire Cooldown
Aim Speed
Fire Angle Tolerance
Low Health Threshold
Recovery Threshold
Patrol Acceptance Radius
```

This means two enemies using the same actor class can still behave differently.

```text
EnemyTank Class
    │
    ├── Tank A
    │     Attack Range = X
    │
    └── Tank B
          Attack Range = Y
```

The level itself therefore stores part of the encounter's gameplay design.

---

# Persistent Patrol-Route Authoring

Enemy tanks can be linked to patrol points directly from the runtime editor.

Instead of storing raw actor pointers—which would be meaningless after saving and loading—the system stores stable logical relationships using IDs:

```text
EnemyTank_123456

LinkedPatrolPointIds
│
├── PatrolPoint_431295
├── PatrolPoint_782531
└── PatrolPoint_973125
```

That relationship survives serialization because it exists entirely as data.

This becomes particularly important during runtime reconstruction.

---

# Level Validation

A level cannot be saved successfully until structural validation passes.

The validation system checks requirements such as:

```text
At least one enemy
Exactly one PlayerStart
Valid patrol-point references
```

This places integrity checks at the content-authoring boundary instead of allowing malformed data to fail later during gameplay.

---

# SaveGame-Based Level Serialization

Custom encounters are stored using Unreal's SaveGame system.

The serialized level definition contains:

```text
FTT_CustomLevelDefinition
│
├── Level Display Name
├── Placed Actors[]
│   │
│   ├── Instance ID
│   ├── Type
│   ├── Transform
│   ├── Patrol Links
│   ├── Tower Tuning
│   └── Tank Tuning
│
└── Add To Campaign Progression
```

The editor can therefore:

```text
Author
  ↓
Validate
  ↓
Save
  ↓
Reload
  ↓
Continue Editing

or

  ↓
Play Immediately
```

---

# Multi-Pass Runtime Level Reconstruction

Saved editor data is not used as temporary preview geometry during gameplay.

A dedicated runtime spawner rebuilds the actual encounter.

The reconstruction is deliberately performed in multiple passes.

### Pass 1 — Spawn Entities

```text
Saved Records
      ↓
Walls
Patrol Points
Enemy Towers
Enemy Tanks
Player Start
```

Each spawned object is mapped back to its saved instance ID.

### Pass 2 — Resolve Relationships

After all entities exist:

```text
Tank Saved Patrol IDs
        ↓
Spawned Actor Map
        ↓
Runtime Patrol Actors
        ↓
Tank AI Component
```

This is important because references cannot be reliably resolved until their target actors have been created.

### Pass 3 — Player Setup

Finally, the player pawn is positioned at the authored PlayerStart.

This creates a clear:

```text
Deserialize
   ↓
Instantiate
   ↓
Resolve References
   ↓
Initialize Gameplay
```

pipeline.

---

# Runtime AI Configuration

When custom enemies are reconstructed, their serialized tuning data is applied directly to their AI components.

```text
Saved Enemy Record
       ↓
Spawn Enemy
       ↓
Find AI Component
       ↓
ApplyCustomTuning()
       ↓
Runtime Behavior
```

The custom level system is therefore not merely spawning actors at saved transforms.

It reconstructs **behavioral configuration** as part of the encounter.

---

# Custom Levels and Campaign Progression

Custom levels can either be:

```text
Played Immediately
```

or optionally:

```text
Inserted After The Main Campaign
```

The GameInstance tracks different play-flow states, including campaign play and custom-level preview.

Conceptually:

```text
Campaign
   │
   ├── Level 1
   ├── Level 2
   └── Final Campaign Level
             │
             ▼
   Custom Level Included?
        │           │
       No          Yes
        │           │
        ▼           ▼
    End Screen   Custom Level
                     │
                     ▼
                 End Screen
```

This connects the runtime creation tool back into the actual game loop instead of leaving it as an isolated prototype feature.

---

# Game-State Management

The GameMode coordinates combat-level state.

It tracks active enemy tanks and towers and evaluates victory once both collections are empty.

```text
Enemy Destroyed
      ↓
GameMode
      ↓
Remaining Towers?
Remaining Tanks?
      ↓
No Enemies Remaining
      ↓
Victory
      ↓
GameInstance
      ↓
Next Level
```

Player death follows the same centralized game-state path and results in the current level being restarted.

This keeps level flow outside of individual enemies.

---

# Debugging-Oriented AI Development

The AI contains explicit runtime state inspection and diagnostic logic.

Tank AI can expose high-level goals such as:

```text
RECOVERY: FindHealthPickup
RECOVERY: MoveToHealthPickup

COMBAT: ScanForTarget
COMBAT: ChaseTarget
COMBAT: AimAtTarget
COMBAT: HoldAim_Cooldown
COMBAT: FireAtTarget

PATROL: FindPatrolPoint
PATROL: MoveToPatrolPoint
```

This is valuable for GOAP debugging because it exposes the semantic reason behind an AI's current behavior rather than forcing developers to infer AI state solely from movement.

Combat code also includes diagnostics around:

* target validity,
* range,
* cooldown,
* turret alignment,
* projectile spawn state.

---

# Architecture

A simplified view of the project architecture:

```text
                         GAMEPLAY
                            │
       ┌────────────────────┼─────────────────────┐
       │                    │                     │
     PLAYER                 AI                LEVEL TOOLS
       │                    │                     │
Enhanced Input         GOAP Brain          Runtime Editor
       │                    │                     │
Player Controller      GOAP Planner        Level Records
       │                    │                     │
       │                World State             SaveGame
       │                    │                     │
       │              Goals + Actions       Runtime Spawner
       │                    │                     │
       └─────────────┬──────┴─────────────────────┘
                     │
                COMMAND LAYER
                     │
          ┌──────────┼──────────┐
          │          │          │
        Move        Aim        Fire
          │          │          │
          └──────────┴──────────┘
                     │
                 Base Pawn
                     │
          ┌──────────┴──────────┐
          │                     │
     Health Component       Projectile
```

---

# Game Programming Patterns

## Command Pattern

Used to decouple action requests from their execution.

Examples:

```text
Move
Aim
Fire
Pause
```

The same gameplay verbs can be invoked by different decision sources.

---

## GOAP

Used for dynamic AI planning.

```text
World State
     +
Goal State
     +
Available Actions
     ↓
Planner
     ↓
Lowest-Cost Valid Plan
```

---

## Component Pattern

Used for reusable gameplay responsibilities such as:

```text
Health
GOAP Brain
Tank AI
Tower AI
```

---

## Data-Driven Design

Custom-level records store transforms, relationships and enemy-specific tuning separately from the runtime actor instances that consume them.

---

## ID-Based Relationship Serialization

Patrol routes are persisted as logical IDs rather than transient runtime pointers and resolved after actors are reconstructed.

---

## Multi-Pass Construction

The runtime level loader separates:

```text
Object Creation
Reference Resolution
Gameplay Initialization
```

to avoid order-of-creation dependency problems.

---

# Project Structure

```text
Source/BattleBlaster/
│
├── AI/
│   ├── GOAP/
│   │   ├── TT_GOAPAction
│   │   ├── TT_GOAPBrainComponent
│   │   └── TT_GOAPPlanner
│   │
│   ├── Tank/
│   │   └── TT_TankAIComponent
│   │
│   └── Tower/
│       └── TT_TowerAIComponent
│
├── Commands/
│   ├── Aim/
│   ├── Fire/
│   ├── Move/
│   └── Pause/
│
├── Components/
│   └── TT_HealthComponent
│
├── Controllers/
│   └── TT_PlayerController
│
├── GameMode/
│   └── TikkiTakaGameMode
│
├── GI/
│   └── TT_GameInstance
│
├── LevelEditor/
│   ├── Data/
│   │   └── TT_CustomLevelTypes
│   │
│   ├── Editor/
│   │   └── TT_RuntimeLevelEditor
│   │
│   ├── Save/
│   │   └── TT_CustomLevelSaveGame
│   │
│   ├── Spawner/
│   │   └── TT_LevelRuntimeSpawner
│   │
│   └── UI/
│
├── Pawn/
│   ├── TT_BasePawn
│   ├── Tank/
│   └── Tower/
│
├── Pickups/
│
├── Projectile/
│   └── TT_Missile
│
└── UI/
```

---

# Technology

* **Unreal Engine 5.6**
* **C++**
* **Enhanced Input**
* **UMG**
* **Niagara**
* **Unreal SaveGame**
* **Actor Components**
* **GameplayStatics**
* Unreal collision/query systems

---

# What This Project Demonstrates

TikkiTaka is primarily a **gameplay and systems programming project**.

The most important engineering work demonstrated here includes:

* designing a reusable GOAP framework instead of hardcoding individual enemy scripts,
* representing AI knowledge as symbolic world state,
* generating lowest-cost action plans from preconditions and effects,
* separating AI planning from action execution,
* combining planned behavior with reactive combat decisions,
* preventing AI objective oscillation through behavioral commitment,
* reusing gameplay commands between human and AI controllers,
* designing separate tank and tower behaviors on the same planning framework,
* building a data-driven runtime level editor,
* implementing grid-snapped and validated placement,
* creating ghost previews and runtime selection tools,
* exposing per-instance enemy tuning,
* serializing relationships without relying on transient actor pointers,
* reconstructing authored encounters through a multi-pass spawn pipeline,
* resolving saved patrol relationships at runtime,
* integrating user-authored levels into the game's progression flow,
* structuring combat through reusable pawns, components and projectiles,
* building debugging visibility into autonomous AI behavior.

The result is not only a top-down tank combat prototype, but a collection of interconnected gameplay systems demonstrating **AI architecture, gameplay abstraction, runtime tooling, data-driven design and content-pipeline engineering**.

---

# Running the Project

### Requirements

* Unreal Engine **5.6**
* Visual Studio / compatible Unreal C++ toolchain

### Clone

```bash
git clone https://github.com/WARL0RD-11/TikkiTaka.git
```

Open:

```text
BattleBlaster.uproject
```

and allow Unreal Engine to compile the C++ module if prompted.

---

# Author

**Nishant Verma**
Gameplay Programmer / Systems Programmer

### Primary Areas Demonstrated

`Unreal Engine` · `C++` · `GOAP` · `Gameplay AI` · `Command Pattern` · `Runtime Tools` · `Level Editor` · `Data-Driven Systems` · `Gameplay Architecture` · `Enhanced Input`
