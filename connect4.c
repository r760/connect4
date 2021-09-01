#include "connect4.h"

int yFin, xFin;

/**
 * the minimax algorithm
 */
int minimax(int depth, bool maximizingPlayer) {
  if (depth == 6 || gameOver()) {
    int score = getScore();
    return score;
  } else {
    int value, y, x;
    if (maximizingPlayer) {
      value = INT_MIN;

      for (x = 0; x < 7; x++) {
        y = getAvailableIndex(x);
        if (y >= 0 && y <= 5 && board[y][x] == defaultCharacter) {
          board[y][x] = aiPlayer;
          int temp = minimax(depth + 1, false);
          if (temp >= value && depth == 0) {
            yFin = y;
            xFin = x;
          }
          board[y][x] = defaultCharacter;
          value = max(value, temp);
        }
      }

      return value;
    } else {
      value = INT_MAX;

      for (x = 0; x < 7; x++) {
        y = getAvailableIndex(x);
        if (y >= 0 && y <= 5 && board[y][x] == defaultCharacter) {
          board[y][x] = realPlayer;
          int temp = minimax(depth + 1, true);
          board[y][x] = defaultCharacter;
          value = min(value, temp);
        }
      }
      return value;
    }
  }
}

/**
 * using the minimax algorithm,
 * the ai drops disk A intelligently
 */
void ai() {
  minimax(0, true);
  board[yFin][xFin] = aiPlayer;
}

int main() {
  set(defaultCharacter);
  initscr();

  int y = 2, x = 7;

  while (stay) {
    if (aiTurn) {
      ai();
      if (gameOver()) {
        gameOverHandler();
        if (stay) {
          ai();
        }
      }
      aiTurn = false;
    }

    clear();
    printScore();
    printBoard();
    printBindings();
    print("v", y, x);
    move(y - 1, x);
    refresh();

    if (stay) {
      char c = getch();
      if (c == 'q') {
        stay = false;
      } else if ((c == 'l') && x < 7 + (4 * 6)) {
        x += 4;
      } else if ((c == 'h') && x > 7) {
        x -= 4;
      } else if (c == 'e') {
        int xTemp = x;
        xTemp = xTemp - 7;
        xTemp = xTemp / 4;
        dropDisk(realPlayer, xTemp);

        if (gameOver()) {
          gameOverHandler();
        }

        if (stay) {
          aiTurn = true;
        }
      }
    }
  }

  endwin();
  return 0;
}