#include <limits.h>
#include <ncurses.h>
#include <stdbool.h>
#include <string.h>

#define ROWS 6
#define COLS 7

#define ai_char      'A'
#define player_char  'B'
#define default_char '?'

#define LEFT_KEY(c)   (c == 'h' || c == 'a')
#define RIGHT_KEY(c)  (c == 'l' || c == 'd')

/**
 * print in-game keys (or controls)
 */
void nprint_keys()
{
     int size = 5;

     char *keys[] = {
	  "Keys:",
	  "q - Quit\n",
	  "h (or a) - Move left\n",
	  "l (or d) - Move right\n",
	  "e - Drop disk \'B\'\n"
     };

     for (int i = 0, y = 3, x = 39; i < size; i++, y++) {
	  if (i == 0) {
	       mvprintw(y, x, "%s", keys[i]);
	  } else {
	       mvprintw(y, x + 1, "%s", keys[i]);
	  }
     }
}

/**
 * print keys (or controls) when game is over
 */
void nprint_game_over_keys()
{
     int size = 3;

     char *keys[] = {
	  "Keys:",
	  "q - Quit\n",
	  "n - New game\n"
     };

     for (int i = 0, y = 3, x = 39; i < size; i++, y++) {
	  if (i == 0) {
	       mvprintw(y, x, "%s", keys[i]);
	  } else {
	       mvprintw(y, x + 1, "%s", keys[i]);
	  }
     }
}

/**
 * print board
 */
void nprint_board(char board[ROWS][COLS])
{
     int y;
     for (y = 0; y < ROWS; y++) {
	  mvprintw(2 * (y + 1), 5, "+---+---+---+---+---+---+---+\n");
	  mvprintw(2 * (y + 1) + 1, 5, "| %c | %c | %c | %c | %c | %c | %c |\n",
		   board[y][0], board[y][1], board[y][2], board[y][3], board[y][4], board[y][5], board[y][6]);
     }
     mvprintw(2 * (y + 1), 5, "+---+---+---+---+---+---+---+\n");
}

/**
 * print score
 */
void nprint_score(int *ai_win_count, int *player_win_count)
{
     mvprintw(16, 12, "AI (%d) - YOU (%d)\n", *ai_win_count, *player_win_count);
}

/**
 * set board pieces
 */
void set(char board[ROWS][COLS], char c)
{
     for (int row = 0; row < ROWS; row++) {
	  for (int col = 0; col < COLS; col++) {
	       board[row][col] = c;
	  }
     }
}

/**
 * returns the row number of the first empty spot (top-down) in the specified column of the board if it exists
 * returns -1 otherwise
 */
int get_available_index(char board[ROWS][COLS], int col)
{
     int y;
     for (y = ROWS - 1; y >= 0; y--) {
	  if (board[y][col] == default_char) {
	       return y;
	  }
     }

     return y;
}

/**
 * drop disk into specified column on the board
 * returns true if successful
 * returns false otherwise
 */
bool drop_disk(char board[ROWS][COLS], char disk, int col)
{
     int row = get_available_index(board, col);
     if (row >= 0 && row <= ROWS - 1) {
	  board[row][col] = disk;
	  return true;
     }
     return false;
}

int game_status_helper(int counter, char curr_player, int weight_1, int weight_2)
{
     int status = 0;

     if (counter == 4 && curr_player == ai_char) {
	  return INT_MAX;
     } else if (counter == 4 && curr_player == player_char) {
	  return INT_MIN;
     } else {
	  if (counter == 3) {
	       status += weight_1;
	  } else if (counter == 2) {
	       status += weight_2;
	  }

	  if (curr_player == player_char) {
	       status *= -1;
	  }
	  return status;
     }
}

/**
 * get game status
 *
 * returns INT_MAX if ai player has won
 * returns INT_MIN if player has won
 * returns 0 if ai and player has tied
 */
int game_status(char board[ROWS][COLS])
{
     int status = 0,
	  // static eval
	  static_eval_status = 0, weight_1 = 1000, weight_2 = 1,
	  row, col;

     // vertical check
     for (col = 0; col < COLS; col++) {
	  row = 0;
	  while (row >= 0 && row <= ROWS - 1) {
	       char curr_player = board[row][col];
	       int counter = 0;

	       while (row >= 0 && row <= 5 && counter <= 3 && board[row][col] == curr_player) {
		    row++;
		    if (curr_player != default_char) {
			 counter++;
		    }
	       }

	       static_eval_status = game_status_helper(counter, curr_player, weight_1, weight_2);
	       if (counter == 4) {
		    return static_eval_status;
	       } else {
		    status += static_eval_status;
	       }
	  }
     }

     // horizontal check
     for (row = 0; row <= ROWS - 1; row++) {
	  col = 0;
	  while (col >= 0 && col <= COLS - 1) {
	       char curr_player = board[row][col];
	       int counter = 0;

	       while (col >= 0 && col <= 6 && counter <= 3 && board[row][col] == curr_player) {
		    col++;
		    if (curr_player != default_char) {
			 counter++;
		    }
	       }

	       static_eval_status = game_status_helper(counter, curr_player, weight_1, weight_2);
	       if (counter == 4) {
		    return static_eval_status;
	       } else {
		    status += static_eval_status;
	       }
	  }
     }

     // diagonal check
     for (row = 0; row < ROWS; row++) {
	  for (col = 0; col < COLS; col++) {
	       if (board[row][col] != default_char) {
		    int counter = 0;
		    int row_temp = row, col_temp = col;

		    // left-down
		    while (counter <= 3 && row_temp >= 0 && row_temp <= 5 && col_temp >= 0 &&
			   col_temp <= 6 && board[row_temp][col_temp] == board[row][col]) {
			 row_temp++;
			 col_temp--;
			 counter++;
		    }

		    static_eval_status = game_status_helper(counter, board[row][col], weight_1, weight_2);
		    if (counter == 4) {
			 return static_eval_status;
		    } else {
			 status += static_eval_status;
		    }

		    // right-down
		    counter = 0;
		    row_temp = row;
		    col_temp = col;

		    while (counter <= 3 && row_temp >= 0 && row_temp <= 5 && col_temp >= 0 &&
			   col_temp <= 6 && board[row_temp][col_temp] == board[row][col]) {
			 row_temp++;
			 col_temp++;
			 counter++;
		    }

		    static_eval_status = game_status_helper(counter, board[row][col], weight_1, weight_2);
		    if (counter == 4) {
			 return static_eval_status;
		    } else {
			 status += static_eval_status;
		    }
	       }
	  }
     }
     return status;
}

/**
 * check if the game is over
 *
 * returns true if the game is over
 * returns false if the game is not over
 */
bool game_over(char board[ROWS][COLS])
{
     int score = game_status(board);

     if (score <= -42000 || score >= 42000) {
	  return true;
     } else {
	  int row, col;
	  for (row = 0; row < ROWS; row++) {
	       for (col = 0; col < COLS; col++) {
		    if (board[row][col] == default_char) {
			 return false;
		    }
	       }
	  }
	  return true;
     }
}

/**
 * the game is over, ask the user if they want to quit or continue
 */
void game_over_handler(char board[ROWS][COLS], int *ai_win_count, int *player_win_count, bool *stay)
{
     char c, *str;
     int status = game_status(board);

     if (status >= 42000) {
	  str = "Game over: You lose!\n";
	  *ai_win_count = *ai_win_count + 1;
     } else if (status <= -42000) {
	  str = "Game over: You win!\n";
	  *player_win_count = *player_win_count + 1;
     } else {
	  str = "Game over: Tie!\n";
     }

     while (true) {
	  clear();
	  nprint_score(ai_win_count, player_win_count);
	  nprint_board(board);
	  nprint_game_over_keys();
	  mvprintw(18, 5, "%s", str);
	  move(18, strlen(str) + 5 - 1);
	  refresh();

	  c = getch();
	  if (c == 'q') {
	       *stay = false;
	       break;
	  } else if (c == 'n') {
	       set(board, default_char);
	       break;
	  }
     }
}

/**
 * the minimax algorithm
 */
int minimax(char board[ROWS][COLS], int *y, int *x, int depth, bool maximizing_player)
{
     if (depth == 6 || game_over(board)) {
	  int status = game_status(board);
	  return status;
     } else {
	  int value, row, col;

	  if (maximizing_player) {
	       value = INT_MIN;

	       for (col = 0; col < COLS; col++) {
		    row = get_available_index(board, col);
		    if (row >= 0 && row <= ROWS - 1 && board[row][col] == default_char) {
			 board[row][col] = ai_char;
			 int temp = minimax(board,  y, x, depth + 1, false);
			 board[row][col] = default_char;
			 if (temp >= value) {
			      value = temp;
			      if (depth == 0) {
				   *y = row;
				   *x = col;
			      }
			 }
		    }
	       }

	       return value;
	  } else {
	       value = INT_MAX;

	       for (col = 0; col < COLS; col++) {
		    row = get_available_index(board, col);
		    if (row >= 0 && row <= ROWS - 1 && board[row][col] == default_char) {
			 board[row][col] = player_char;
			 int temp = minimax(board, y, x, depth + 1, true);
			 board[row][col] = default_char;
			 if (temp <= value) {
			      value = temp;
			 }
		    }
	       }

	       return value;
	  }
     }
}

/**
 * the ai drops disk 'A' intelligently on the board
 */
void ai(char board[ROWS][COLS])
{
     int y = -1, x = -1;
     minimax(board, &y, &x, 0, true);
     if (y >= 0 && y <= ROWS && x >= 0 && x <= COLS) {
	  board[y][x] = ai_char;
     }
}

int main(void)
{
     char board[ROWS][COLS];
     bool ai_turn = false, stay = true;
     int ai_win_count = 0, player_win_count = 0,  y = 2, x = 7;

     set(board, default_char);

     initscr();

     while (stay) {
	  if (ai_turn) {
	       ai(board);
	       if (game_over(board)) {
		    game_over_handler(board, &ai_win_count, &player_win_count, &stay);
	       }
	       ai_turn = false;
	  }

	  clear();
	  nprint_score(&ai_win_count, &player_win_count);
	  nprint_board(board);
	  nprint_keys();
	  mvprintw(y, x, "%s", "v");
	  move(y - 1, x);
	  refresh();

	  if (stay) {
	       char c = getch();
	       if (c == 'q') {
		    stay = false;
	       } else if (RIGHT_KEY(c) && x < 31) {
		    x += 4;
	       } else if (LEFT_KEY(c) && x > 7) {
		    x -= 4;
	       } else if (c == 'e') {
		    int x_temp = x;
		    x_temp = x_temp - 7;
		    x_temp = x_temp / 4;
		    if (drop_disk(board, player_char, x_temp)) {
			 if (game_over(board)) {
			      game_over_handler(board, &ai_win_count, &player_win_count, &stay);
			      ai_turn = false;
			 } else if (stay) {
			      ai_turn = true;
			 }
			 
		    }
	       }
	  }
     }

     endwin();
     return 0;
}
