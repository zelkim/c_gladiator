# Gladiator!

## Overview
Gladiator! is a console combat game developed as a submission for CCPROG1 during Term 1, Year 2023. The game is implemented in C and features a variety of gameplay mechanics, including armor and weapon choices, difficulty levels, and strategic in-game moves.

## How to Play
Upon starting the game, players are presented with choices for armor and weapons. Two difficulty levels are available:
- **Easy Difficulty:** Purely random weapon and move selection.
- **Hard Difficulty:** AI opponent with strategic decision-making based on player moves, matching weapons, and armor.

## Game Mechanics
### Stats
Players must consider four key stats:
- **ATK:** Base damage dealt to the opponent with each attack.
- **DEF:** Armor that absorbs damage before affecting main health.
- **SPD:** Determines attack order, with higher speed attacking first.
- **HP:** Base health, potentially increased with certain armor.

### In-Game Moves
1. **ATTACK:** Damages the enemy, with a chance for a critical hit.
2. **BLOCK:** Doubles DEF but prevents damage to the enemy.
3. **CHARGE:** Skips a round, doubling SPD and ATK in the next round.

### Passive Ability
Players have a 15% chance to land a critical hit, ignoring opponent DEF and dealing 100% ATK damage.

## Winning Conditions
- **Losing:** Reach 0 HP.
- **Tie:** Same-speed attacks result in a draw.
- **Speed Priority:** In case of simultaneous attacks, the player with higher speed wins.

## Extra Features
### Statistics
Track the following stats even after exiting the program:
- Games played
- Games won
- Games drawn
- Games lost
- Lifetime Damage Dealt
- Lifetime Damage Received

### Developer Mode
Access a developer command console to modify game variables and enhance the gaming experience.

Commands:
```
?, help         - Show list of commands.
(a)rmor         - Manage game armor.
(w)eapon        - Manage game weapons.
(s)ettings      - Manage game settings.
ver             - Show game version.
cls             - Clear dev terminal.
```

## How to Run
1. Clone the repository to your local machine.
```
> git clone https://github.com/sampdevi/c_gladiator
```

2. Compile the code with the GCC compiler with the `-Wall` and `-std=c99` args.
```
> gcc -Wall -std=c99 -o "gladiator"
```

4. Run the executable to start playing Gladiator!
```
> gladiator.exe
```

Enjoy the thrilling combat experience in Gladiator!
