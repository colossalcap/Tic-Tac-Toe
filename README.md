# ❌⭕ Tic-Tac-Toe vs. a Minimax AI (C)

A terminal Tic-Tac-Toe game in C where you play against a computer opponent that picks its moves with the **minimax algorithm**. This was a group project for the Programming Methodology module at the Singapore Institute of Technology.

## ✨ Features

- **Minimax move search:** `minimax()` and `findBestMove()` recursively play out every remaining move to score each empty cell.
- **Win and draw detection:** rows, columns and diagonals are checked after every move (`rowCrossed`, `columnCrossed`, `diagonalCrossed`).
- **Input validation:** rejects out-of-range cells and cells that are already taken.
- **Plain C:** standard library only, no external dependencies.

## 🚀 Build and run

```bash
gcc "tic tac toe.c" -o tictactoe
./tictactoe
```

Enter a number from **1–9** to place your `X`:

```
 1 | 2 | 3
-----------
 4 | 5 | 6
-----------
 7 | 8 | 9
```

## 🧠 How the AI works

- `evaluate()` scores a finished board **+10 / −10 / 0** (win, loss, draw).
- `minimax()` alternates between a maximising and a minimising player, recursing through every possible continuation. A 3×3 board is small enough to search completely with no pruning.
- `findBestMove()` tries every empty cell and keeps the one with the best minimax score. The computer then places its `O` there.

---

*Forked from the team repo [BlackberryDealer/Tic-Tac-Toe](https://github.com/BlackberryDealer/Tic-Tac-Toe).*
