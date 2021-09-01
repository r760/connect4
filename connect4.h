#include <limits.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

char board[6][7], defaultCharacter = '?', aiPlayer = 'A', realPlayer = 'B';
bool aiTurn = true, stay = true;
int aiPlayerWinCount = 0, realPlayerWinCount = 0;

/*
 * Utility Functions
 */

/**
 * set all board pieces to c
 */
void set(char c) {
  int y, x;
  for (y = 0; y < 6; y++) {
    for (x = 0; x < 7; x++) {
      board[y][x] = c;
    }
  }
}

/**
 * return the y-index of the first empty spot of column x
 * return -1 if no such spot exists
 */
int getAvailableIndex(int x) {
  int y;
  for (y = 5; y >= 0; y--) {
    if (board[y][x] == defaultCharacter) {
      return y;
    }
  }
  return y;
}

/**
 * drop disk into specified column if possible
 */
void dropDisk(char disk, int x) {
  int y = getAvailableIndex(x);
  if (y >= 0 && y <= 5) {
    board[y][x] = disk;
  }
}

int getScoreHelper(int counter, char yOrig, int weight_1, int weight_2) {
  int tempScore = 0;
  if (counter == 4 && yOrig == aiPlayer) {
    return INT_MAX;
  } else if (counter == 4 && yOrig == realPlayer) {
    return INT_MIN;
  } else {
    if (counter == 3) {
      tempScore += weight_1;
    } else if (counter == 2) {
      tempScore += weight_2;
    }

    if (yOrig == realPlayer) {
      tempScore *= -1;
    }
    return tempScore;
  }
}

/**
 * return INT_MAX if ai player wins
 * return INT_MIN if real player wins
 * else return static eval
 */
int getScore() {
  int score = 0, tempScore = 0;
  // static eval
  int weight_1 = 1000, weight_2 = 1;

  // vertical check
  int y, x;
  for (x = 0; x < 7; x++) {
    y = 0;
    while (y >= 0 && y <= 5) {
      char yOrig = board[y][x];
      int counter = 0;

      while (y >= 0 && y <= 5 && counter <= 3 && board[y][x] == yOrig) {
        y++;
        if (yOrig != defaultCharacter) {
          counter++;
        }
      }

      tempScore = getScoreHelper(counter, yOrig, weight_1, weight_2);
      if (counter == 4) {
        return tempScore;
      } else {
        score += tempScore;
      }
    }
  }

  // horizontal check
  for (y = 0; y <= 5; y++) {
    x = 0;
    while (x >= 0 && x <= 6) {
      char yOrig = board[y][x];
      int counter = 0;

      while (x >= 0 && x <= 6 && counter <= 3 && board[y][x] == yOrig) {
        x++;
        if (yOrig != defaultCharacter) {
          counter++;
        }
      }

      tempScore = getScoreHelper(counter, yOrig, weight_1, weight_2);
      if (counter == 4) {
        return tempScore;
      } else {
        score += tempScore;
      }
    }
  }

  // diagonal check
  for (y = 0; y < 6; y++) {
    for (x = 0; x < 7; x++) {
      if (board[y][x] != defaultCharacter) {
        int counter = 0;
        int yTemp = y, xTemp = x;

        // left-down
        while (counter <= 3 && yTemp >= 0 && yTemp <= 5 && xTemp >= 0 &&
               xTemp <= 6 && board[yTemp][xTemp] == board[y][x]) {
          yTemp++;
          xTemp--;
          counter++;
        }

        tempScore = getScoreHelper(counter, board[y][x], weight_1, weight_2);
        if (counter == 4) {
          return tempScore;
        } else {
          score += tempScore;
        }

        // right-down
        counter = 0;
        yTemp = y;
        xTemp = x;

        while (counter <= 3 && yTemp >= 0 && yTemp <= 5 && xTemp >= 0 &&
               xTemp <= 6 && board[yTemp][xTemp] == board[y][x]) {
          yTemp++;
          xTemp++;
          counter++;
        }

        tempScore = getScoreHelper(counter, board[y][x], weight_1, weight_2);
        if (counter == 4) {
          return tempScore;
        } else {
          score += tempScore;
        }
      }
    }
  }
  return score;
}

/**
 * return true if the game is over
 * return false otherwise
 */
bool gameOver() {
  int score = getScore();
  if (score <= -42000 || score >= 42000) {
    return true;
  } else {
    int y, x;
    for (y = 0; y < 6; y++) {
      for (x = 0; x < 7; x++) {
        if (board[y][x] == defaultCharacter) {
          return false;
        }
      }
    }
    return true;
  }
}

/**
 * return the min between a and b
 */
int min(int a, int b) {
  if (a <= b) {
    return a;
  } else {
    return b;
  }
}

/**
 * return the max between a and b
 */
int max(int a, int b) {
  if (a >= b) {
    return a;
  } else {
    return b;
  }
}

/*
 * IO Functions
 */

void print(char *temp, int y, int x) {
  int i = 0;
  while (temp[i] != '\0') {
    mvaddch(y, x++, temp[i++]);
  }
}

void printBindings() {
  char *bindings[5] = {"Commands:", "q - Quit\n", "h - Move left\n",
                       "l - Move right\n", "e - Drop disk \'B\'\n"};
  int i, y = 3, x = 39;
  for (i = 0; i < 5; i++) {
    if (i == 1) {
      x++;
    }
    print(bindings[i], y++, x);
  }
}

void printBoard() {
  int y, temp;
  temp = 3;
  for (y = 0; y < 6; y++) {
    move(temp, 5);
    printw("+---+---+---+---+---+---+---+\n");
    move(temp + 1, 5);
    printw("| %c | %c | %c | %c | %c | %c | %c |\n", board[y][0], board[y][1],
           board[y][2], board[y][3], board[y][4], board[y][5], board[y][6]);
    temp += 2;
  }
  move(temp, 5);
  printw("+---+---+---+---+---+---+---+\n");
}

void printScore() {
  // int score = getScore();
  // move(2, 39);
  // printw("SCORE (%d)\n", score);
  move(16, 12);
  printw("AI (%d) - YOU (%d)\n", aiPlayerWinCount, realPlayerWinCount);
}

void gameOverHandler() {
  char c, *status;
  int score = getScore();
  if (score >= 42000) {
    status = "Game over: You lose!\n";
    aiPlayerWinCount++;
  } else if (score <= -42000) {
    status = "Game over: You win!\n";
    realPlayerWinCount++;
  } else {
    status = "Game over: Tie!\n";
  }

  while (true) {
    clear();
    printScore();
    printBoard();
    print("Commands:\n", 3, 39);
    print("q - Quit\n", 4, 40);
    print("n - New game\n", 5, 40);
    print(status, 18, 5);
    move(18, 25);
    refresh();

    c = getch();
    if (c == 'q') {
      stay = false;
      break;
    } else if (c == 'n') {
      set(defaultCharacter);
      break;
    }
  }
}