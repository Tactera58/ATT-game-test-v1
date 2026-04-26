# Ashes Till Tomorrow™

> A post-apocalyptic survival game built in Unreal Engine 5.

**Studio:** UNDERHEAT Studio  
**Engine:** Unreal Engine 5 (5.3+)  
**Language:** C++ / Blueprints  
**Status:** Active Development

---

## Overview

Ashes Till Tomorrow is a hardcore survival game set in a collapsed world. Players navigate radiation zones, dynamic weather, hostile factions, and scarce resources while managing health, hunger, thirst, stamina, and morale.

---

## Core Systems

| System | Status | Notes |
|---|---|---|
| Player Character | ✅ | Movement, sprinting, crouching, sliding, mantling |
| Survival Stats | ✅ | HP, hunger, thirst, stamina w/ weight formula |
| Weather Controller | ✅ | Rain, storm, snow, fog w/ Lumen/Nanite support |
| Crafting | ✅ | Recipe-based, inventory-deducting |
| NPC/AI | ✅ | Behavior tree integration |
| Items & Armor | 🔧 | In progress |
| Factions | 📋 | Designed |
| Quests | 📋 | Designed |

---

## Project Structure

```
AshesTillTomorrow/
├── Config/                   # Engine, game, and input settings
├── Docs/                     # Game design documents
└── Source/
    └── AshesTillTomorrow/
        ├── Characters/       # PlayerCharacter
        ├── Systems/          # Weather, Crafting, Survival
        ├── AI/               # NPC controllers
        └── Items/            # Item base classes
```

---

## Setup

### Requirements
- Unreal Engine 5.3 or later
- Visual Studio 2022 (Windows) or Xcode 14+ (macOS)
- Git LFS (for large binary assets)

### Steps

```bash
# Clone the repo
git clone https://github.com/UNDERHEAT/AshesTillTomorrow.git
cd AshesTillTomorrow

# Install Git LFS
git lfs install
git lfs pull

# Right-click AshesTillTomorrow.uproject → Generate Visual Studio Files
# Open the .sln in Visual Studio 2022
# Build (Development Editor | Win64)
# Launch from UE5 Editor
```

---

## Stamina Formula

The game uses a physics-aware stamina model:

```
OSDRV = (BRV × EM) - CWD
```

- **BRV** — Base Regen Value (1.25/sec)  
- **EM** — Ergonomics Multiplier (weapon/armor dependent)  
- **CWD** — Calculated Weight Drain (scales linearly with kg)

---

## Team

| Name | Role |
|---|---|
| Grayson Chambers | Project Management, Research |
| Nathaniel Roper | Design, Planning, Specifications |
| Aiden Wood | Game Testing, Research, Prototyping |
| Nathenial Wood | Marketing, Project Planning |

---

## License

© UNDERHEAT Studio. All rights reserved. Not for redistribution.
