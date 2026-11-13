/*
 * Tic-Tac-Toe: play against a minimax AI in the terminal.
 *
 * Features
 *   - Three difficulty levels (Easy / Medium / Hard)
 *   - Hard mode uses minimax with alpha-beta pruning and never loses
 *   - Choose who moves first, play again, running scoreboard
 *   - Robust input handling (no infinite loops on bad input, EOF-safe)
 *   - Built-in exhaustive self-test:  ./tictactoe --selftest
 *
 * Build:  gcc -std=c11 -Wall -Wextra -O2 -o tictactoe tictactoe.c
 */

#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIDE 3
#define CELLS (SIDE * SIDE)
#define EMPTY ' '
#define HUMAN_MARK 'X'
#define AI_MARK 'O'
#define WIN_SCORE 10

typedef enum { EASY = 1, MEDIUM = 2, HARD = 3 } Difficulty;
typedef enum { ONGOING, HUMAN_WINS, AI_WINS, DRAW } Outcome;

/* All 8 winning lines, as cell indices 0-8. */
static const int LINES[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, /* rows      */
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, /* columns   */
    {0, 4, 8}, {2, 4, 6}             /* diagonals */
};

/* ------------------------------------------------------------------ */
/* Board logic                                                         */
/* ------------------------------------------------------------------ */

static void clear_board(char board[CELLS])
{
    memset(board, EMPTY, CELLS);
}

/* Returns the mark that has three in a row, or EMPTY if nobody has. */
static char winner(const char board[CELLS])
{
    for (int i = 0; i < 8; i++) {
        char a = board[LINES[i][0]];
        if (a != EMPTY && a == board[LINES[i][1]] && a == board[LINES[i][2]])
            return a;
    }
    return EMPTY;
}

static bool board_full(const char board[CELLS])
{
    for (int i = 0; i < CELLS; i++)
        if (board[i] == EMPTY)
            return false;
    return true;
}

static Outcome outcome(const char board[CELLS])
{
    char w = winner(board);
    if (w == HUMAN_MARK)
        return HUMAN_WINS;
    if (w == AI_MARK)
        return AI_WINS;
    return board_full(board) ? DRAW : ONGOING;
}

/* ------------------------------------------------------------------ */
/* AI                                                                  */
/* ------------------------------------------------------------------ */

/*
 * Minimax with alpha-beta pruning, scored from the AI's point of view.
 * Depth is folded into the score so the AI prefers faster wins and
 * slower losses.
 */
static int minimax(char board[CELLS], int depth, bool ai_turn, int alpha, int beta)
{
    char w = winner(board);
    if (w == AI_MARK)
        return WIN_SCORE - depth;
    if (w == HUMAN_MARK)
        return depth - WIN_SCORE;
    if (board_full(board))
        return 0;

    int best = ai_turn ? INT_MIN : INT_MAX;
    for (int i = 0; i < CELLS; i++) {
        if (board[i] != EMPTY)
            continue;
        board[i] = ai_turn ? AI_MARK : HUMAN_MARK;
        int score = minimax(board, depth + 1, !ai_turn, alpha, beta);
        board[i] = EMPTY;

        if (ai_turn) {
            if (score > best) best = score;
            if (best > alpha) alpha = best;
        } else {
            if (score < best) best = score;
            if (best < beta) beta = best;
        }
        if (alpha >= beta)
            break; /* prune: the opponent will never allow this line */
    }
    return best;
}

static int random_empty_cell(const char board[CELLS])
{
    int empty[CELLS], n = 0;
    for (int i = 0; i < CELLS; i++)
        if (board[i] == EMPTY)
            empty[n++] = i;
    return n ? empty[rand() % n] : -1;
}

/* Best move for the AI. Ties are broken randomly so games vary. */
static int best_move(char board[CELLS])
{
    int best_score = INT_MIN, candidates[CELLS], n = 0;
    for (int i = 0; i < CELLS; i++) {
        if (board[i] != EMPTY)
            continue;
        board[i] = AI_MARK;
        int score = minimax(board, 1, false, INT_MIN, INT_MAX);
        board[i] = EMPTY;

        if (score > best_score) {
            best_score = score;
            n = 0;
        }
        if (score == best_score)
            candidates[n++] = i;
    }
    return n ? candidates[rand() % n] : -1;
}

static int ai_move(char board[CELLS], Difficulty level)
{
    switch (level) {
    case EASY:
        return random_empty_cell(board);
    case MEDIUM: /* plays perfectly about 60% of the time */
        return (rand() % 100 < 60) ? best_move(board) : random_empty_cell(board);
    case HARD:
    default:
        return best_move(board);
    }
}

/* ------------------------------------------------------------------ */
/* Terminal UI                                                         */
/* ------------------------------------------------------------------ */

static void print_board(const char board[CELLS])
{
    printf("\n");
    for (int r = 0; r < SIDE; r++) {
        printf("     ");
        for (int c = 0; c < SIDE; c++) {
            int i = r * SIDE + c;
            /* Show the cell number on empty squares to make input easy. */
            char shown = board[i] == EMPTY ? (char)('1' + i) : board[i];
            printf(" %c %s", shown, c < SIDE - 1 ? "|" : "");
        }
        printf("\n%s", r < SIDE - 1 ? "     ---+---+---\n" : "\n");
    }
}

/*
 * Reads one line and parses an integer in [lo, hi].
 * Returns -1 on EOF so the caller can exit cleanly.
 */
static int read_int(const char *prompt, int lo, int hi)
{
    char line[64];
    for (;;) {
        printf("%s", prompt);
        fflush(stdout);
        if (!fgets(line, sizeof line, stdin))
            return -1;

        char *end;
        long value = strtol(line, &end, 10);
        while (isspace((unsigned char)*end))
            end++;
        if (end != line && *end == '\0' && value >= lo && value <= hi)
            return (int)value;
        printf("  Please enter a number from %d to %d.\n", lo, hi);
    }
}

static bool read_yes_no(const char *prompt)
{
    char line[16];
    for (;;) {
        printf("%s", prompt);
        fflush(stdout);
        if (!fgets(line, sizeof line, stdin))
            return false;
        char c = (char)tolower((unsigned char)line[0]);
        if (c == 'y')
            return true;
        if (c == 'n')
            return false;
        printf("  Please answer y or n.\n");
    }
}

static int human_move(const char board[CELLS])
{
    for (;;) {
        int cell = read_int("Your move (1-9): ", 1, 9);
        if (cell < 0)
            return -1;
        if (board[cell - 1] == EMPTY)
            return cell - 1;
        printf("  Cell %d is already taken.\n", cell);
    }
}

/* Plays one game. Returns the outcome, or ONGOING if input ended (EOF). */
static Outcome play_game(Difficulty level, bool human_first)
{
    char board[CELLS];
    clear_board(board);
    bool human_turn = human_first;

    print_board(board);
    while (outcome(board) == ONGOING) {
        if (human_turn) {
            int cell = human_move(board);
            if (cell < 0)
                return ONGOING;
            board[cell] = HUMAN_MARK;
        } else {
            int cell = ai_move(board, level);
            board[cell] = AI_MARK;
            printf("Computer plays %d.\n", cell + 1);
        }
        print_board(board);
        human_turn = !human_turn;
    }

    Outcome result = outcome(board);
    if (result == HUMAN_WINS)
        printf("You win!\n");
    else if (result == AI_WINS)
        printf("Computer wins.\n");
    else
        printf("It's a draw.\n");
    return result;
}

/* ------------------------------------------------------------------ */
/* Self-test: proves Hard mode never loses                             */
/* ------------------------------------------------------------------ */

/*
 * Explores every possible sequence of human moves against the Hard AI
 * (with the AI's own random tie-breaking replaced by trying every
 * equally good move). Counts games and any losses.
 */
static void explore(char board[CELLS], bool human_turn, long *games, long *losses)
{
    Outcome o = outcome(board);
    if (o != ONGOING) {
        (*games)++;
        if (o == HUMAN_WINS)
            (*losses)++;
        return;
    }

    if (human_turn) {
        for (int i = 0; i < CELLS; i++) {
            if (board[i] != EMPTY)
                continue;
            board[i] = HUMAN_MARK;
            explore(board, false, games, losses);
            board[i] = EMPTY;
        }
        return;
    }

    /* Try every move the AI considers optimal. */
    int best_score = INT_MIN, scores[CELLS];
    for (int i = 0; i < CELLS; i++) {
        scores[i] = INT_MIN;
        if (board[i] != EMPTY)
            continue;
        board[i] = AI_MARK;
        scores[i] = minimax(board, 1, false, INT_MIN, INT_MAX);
        board[i] = EMPTY;
        if (scores[i] > best_score)
            best_score = scores[i];
    }
    for (int i = 0; i < CELLS; i++) {
        if (scores[i] != best_score)
            continue;
        board[i] = AI_MARK;
        explore(board, true, games, losses);
        board[i] = EMPTY;
    }
}

static int self_test(void)
{
    int failures = 0;
    char board[CELLS];

    /* 1. Win detection on every line. */
    for (int l = 0; l < 8; l++) {
        clear_board(board);
        for (int k = 0; k < 3; k++)
            board[LINES[l][k]] = AI_MARK;
        if (winner(board) != AI_MARK) {
            printf("FAIL: line %d not detected\n", l);
            failures++;
        }
    }

    /* 2. AI takes an immediate win. */
    const char take_win[CELLS] = {'O', 'O', ' ', 'X', 'X', ' ', ' ', ' ', ' '};
    memcpy(board, take_win, CELLS);
    if (best_move(board) != 2) {
        printf("FAIL: AI missed the winning move\n");
        failures++;
    }

    /* 3. AI blocks an immediate loss. */
    const char must_block[CELLS] = {'X', 'X', ' ', ' ', 'O', ' ', ' ', ' ', ' '};
    memcpy(board, must_block, CELLS);
    if (best_move(board) != 2) {
        printf("FAIL: AI did not block\n");
        failures++;
    }

    /* 4. Exhaustive: Hard AI never loses, whoever moves first. */
    long games = 0, losses = 0;
    clear_board(board);
    explore(board, true, &games, &losses);
    clear_board(board);
    explore(board, false, &games, &losses);
    printf("Exhaustive check: %ld games played, %ld lost by the AI\n", games, losses);
    if (losses)
        failures++;

    printf(failures ? "SELF-TEST FAILED (%d)\n" : "All self-tests passed.\n", failures);
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}

/* ------------------------------------------------------------------ */

int main(int argc, char *argv[])
{
    srand((unsigned)time(NULL));

    if (argc > 1 && strcmp(argv[1], "--selftest") == 0)
        return self_test();

    printf("==============================\n");
    printf("     T I C - T A C - T O E\n");
    printf("==============================\n");
    printf("You are %c, the computer is %c.\n", HUMAN_MARK, AI_MARK);
    printf("Pick a cell by its number:\n");
    char demo[CELLS];
    clear_board(demo);
    print_board(demo);

    int wins = 0, losses = 0, draws = 0;
    do {
        int level = read_int("Difficulty - 1) Easy  2) Medium  3) Hard: ", 1, 3);
        if (level < 0)
            break;
        int first = read_int("Who starts - 1) You  2) Computer: ", 1, 2);
        if (first < 0)
            break;

        Outcome result = play_game((Difficulty)level, first == 1);
        if (result == ONGOING)
            break;
        if (result == HUMAN_WINS)
            wins++;
        else if (result == AI_WINS)
            losses++;
        else
            draws++;

        printf("\nScore - You: %d  Computer: %d  Draws: %d\n\n", wins, losses, draws);
    } while (read_yes_no("Play again? (y/n): "));

    printf("Thanks for playing!\n");
    return EXIT_SUCCESS;
}
