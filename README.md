# ❌⭕ Tic-Tac-Toe vs. an Unbeatable AI (C)

A terminal Tic-Tac-Toe game in C. You play against a computer opponent that uses **minimax with alpha-beta pruning**, and on Hard it never loses. A built-in self-test proves it by playing out every possible game.

This started as a group project for the Programming Methodology module at the Singapore Institute of Technology, and I have since rewritten and extended it.

## ✨ Features

- **Three difficulty levels:** Easy plays random moves, Medium plays optimally about 60% of the time, and Hard plays perfectly.
- **Minimax with alpha-beta pruning:** depth is folded into the score, so the AI goes for the fastest win and drags out a loss. Ties between equally good moves are broken randomly, so games don't repeat.
- **You choose who moves first**, with play-again and a running scoreboard.
- **Robust input:** non-numeric input, out-of-range numbers, taken cells and end-of-input (Ctrl+D) are all handled without crashing or looping forever.
- **Built-in self-test** (`--selftest`):
  - Win detection on all 8 lines
  - The AI takes an immediate win and blocks an immediate loss
  - An **exhaustive search over every possible game** confirms the Hard AI never loses, whoever goes first
- **No dependencies:** a single C11 file using only the standard library, which compiles cleanly with `-Wall -Wextra -Wpedantic`.

## 🚀 Build and run

```bash
make            # build
make run        # play
make test       # run the self-test
```

Without `make`:

```bash
gcc -std=c11 -Wall -Wextra -O2 -o tictactoe tictactoe.c
./tictactoe
./tictactoe --selftest
```

Pick a cell by its number. Empty cells show their number on the board:

```
      1 | 2 | 3
     ---+---+---
      4 | X | 6
     ---+---+---
      7 | 8 | O
```

Self-test output:

```
Exhaustive check: 16752 games played, 0 lost by the AI
All self-tests passed.
```

## 🧠 How the AI works

- The board is a flat array of 9 cells, and the 8 winning lines are stored in a lookup table.
- `minimax()` scores a position from the AI's point of view:
  - an AI win scores `10 − depth`
  - a human win scores `depth − 10`
  - a draw scores `0`
- **Alpha-beta pruning** stops exploring a branch as soon as it is clear the opponent would never allow it. This cuts the search well below the roughly 550,000 nodes of a plain minimax from an empty board.
- `best_move()` collects every move tied for the best score and picks one of them at random.

## 📁 Structure

```
tictactoe.c   # game, AI and self-test
Makefile      # build / run / test targets
```
