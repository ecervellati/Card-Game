# Card-Game
Card Game – Programming Project

This repository contains a C implementation of a card game designed as a programming assignment for the Introduction to Programming course.

## Game Rules

### Deck:

40 cards divided into 4 suits (Clubs, Spades, Diamonds, Hearts).
Each suit contains cards numbered 1–7, plus J, Q, K.

### Players:

From 2 to 20 players.
Each player starts with 2 life points.

### Goal:
Be the last player alive.

## Gameplay
The game is played in phases. At the beginning of each phase:

- Shuffle the deck.
- Deal 2 cards to each player: one face-up, one face-down.
- Randomly choose the first player.
- Turns proceed in circular order (e.g. with 3 players: if player 2 starts → P2 → P3 → P1).

During their turn, a player must:

- Resolve the effect of their face-up card.
- Optionally reveal their face-down card to apply its effect.
- End the turn and pass it to the next player.
- At the end of the phase, any player with 0 life points is eliminated.

### Card Effects

- Cards 2–6 → No effect.
- 7 → The next player must reveal their face-down card and apply its effect.
- J → Give 1 life point to the previous player.
- Q → Give 1 life point to the player two seats ahead (skipping one).
- 1 → Lose 1 life point, which is placed on the field. (Life points on the field persist across phases until collected.)
- K → Collect all life points currently on the field.

### End of the Game
The game ends when, after a phase, only one player remains alive.

## How to Compile and Run
This project is implemented in C99 and only uses the standard library.

Compile with:
```
gcc main.c -o cardgame
```

Run with:
```
./cardgame
```
