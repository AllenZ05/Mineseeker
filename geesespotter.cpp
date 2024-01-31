#include "geesespotter_lib.h"
#include <iostream>
#include <stdexcept>

// Create the board and initialize it
char *create_board(std::size_t x_dim, std::size_t y_dim) {
  if (x_dim == 0 || y_dim == 0) {
    throw std::invalid_argument("Board dimensions must be greater than 0");
  }

  char *p_board = new (std::nothrow) char[x_dim * y_dim]{};
  if (p_board == nullptr) {
    throw std::bad_alloc();
  }

  for (std::size_t i = 0; i < x_dim * y_dim; ++i) {
    p_board[i] = '\0';
  }
  return p_board;
}

// Safely deallocate the given board
void clean_board(char *&board) {
  delete[] board;
  board = nullptr;
}

// Prints out the board: M for marked tiles, * for hidden tiles, otherwise 0-9
void print_board(const char *board, std::size_t x_dim, std::size_t y_dim) {
  if (board == nullptr) {
    throw std::invalid_argument("Board pointer is null");
  }

  std::cout << std::endl;
  for (std::size_t y = 0; y < y_dim; ++y) {
    for (std::size_t x = 0; x < x_dim; ++x) {
      std::size_t i = y * x_dim + x;
      char cell = board[i];

      if (cell & hidden_mask()) {
        std::cout << ((cell & marked_mask()) ? 'M' : '*');
      } else {
        int value = cell & value_mask();
        std::cout << value;
      }
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

// Hide all the field values
void hide_board(char *board, std::size_t x_dim, std::size_t y_dim) {
  if (board == nullptr) {
    throw std::invalid_argument("Board pointer is null");
  }

  for (std::size_t i = 0; i < x_dim * y_dim; ++i) {
    board[i] |= hidden_mask();
  }
}

// Player attempts to mark a field
int mark(char *board, std::size_t x_dim, std::size_t y_dim, std::size_t x_loc, std::size_t y_loc) {
  std::size_t i{y_loc * x_dim + x_loc}; // index
  if (!(board[i] & 0x20)) {
    // if revealed
    return 2;
  } else {
    // if hidden, toggle the marked bit
    board[i] ^= 0x10;
    return 0;
  }
};

// Updates the board array: all fields without a goose have their value set to the number of adjacent geese
void compute_neighbours(char *board, std::size_t x_dim, std::size_t y_dim) {
  for (std::size_t y = 0; y < y_dim; ++y) {
    for (std::size_t x = 0; x < x_dim; ++x) {
      std::size_t i = y * x_dim + x; // index

      if (board[i] == 9) {
        // No need to compute neighbours for a goose
        continue;
      }

      int neighbours = 0;

      // Check left
      if (x > 0 && board[i - 1] == 9) {
        neighbours++;
      }
      // Check right
      if (x < x_dim - 1 && board[i + 1] == 9) {
        neighbours++;
      }
      // Check up
      if (y > 0 && board[i - x_dim] == 9) {
        neighbours++;
      }
      // Check down
      if (y < y_dim - 1 && board[i + x_dim] == 9) {
        neighbours++;
      }
      // Check top-left
      if (x > 0 && y > 0 && board[i - x_dim - 1] == 9) {
        neighbours++;
      }
      // Check top-right
      if (x < x_dim - 1 && y > 0 && board[i - x_dim + 1] == 9) {
        neighbours++;
      }
      // Check bottom-left
      if (x > 0 && y < y_dim - 1 && board[i + x_dim - 1] == 9) {
        neighbours++;
      }
      // Check bottom-right
      if (x < x_dim - 1 && y < y_dim - 1 && board[i + x_dim + 1] == 9) {
        neighbours++;
      }

      // Assign the computed number of neighbours
      board[i] = neighbours;
    }
  }
}

// The game is won when all fields that do not have a goose have been revealed
bool is_game_won(char *board, std::size_t x_dim, std::size_t y_dim) {
  for (std::size_t i{}; i < x_dim * y_dim; ++i) {
    if ((board[i] & 0xf) == 9) {
      // if the field has a goose
      continue;
    }
    if (board[i] & 0x20) {
      // if a non-goose tile is hidden, the game cannot be won
      return false;
    }
  }
  // if all non-goose tiles are revealed, the game is won
  return true;
};

int reveal(char *board, std::size_t x_dim, std::size_t y_dim, std::size_t x_loc, std::size_t y_loc) {
  std::size_t i{y_loc * x_dim + x_loc};
  if ((board[i] & 0x20) && !(board[i] & 0x10)) { // if hidden and not marked
    // reveal field
    board[i] ^= 0x20;
    if (board[i] == 9) {
      return 9; // goose found
    } else if (board[i] == 0) {
      // Recursively reveal adjacent fields
      if (x_loc > 0)
        reveal(board, x_dim, y_dim, x_loc - 1, y_loc); // left
      if (x_loc < x_dim - 1)
        reveal(board, x_dim, y_dim, x_loc + 1, y_loc); // right
      if (y_loc > 0)
        reveal(board, x_dim, y_dim, x_loc, y_loc - 1); // up
      if (y_loc < y_dim - 1)
        reveal(board, x_dim, y_dim, x_loc, y_loc + 1); // down
      if (x_loc > 0 && y_loc > 0)
        reveal(board, x_dim, y_dim, x_loc - 1, y_loc - 1); // top-left
      if (x_loc < x_dim - 1 && y_loc > 0)
        reveal(board, x_dim, y_dim, x_loc + 1, y_loc - 1); // top-right
      if (x_loc > 0 && y_loc < y_dim - 1)
        reveal(board, x_dim, y_dim, x_loc - 1, y_loc + 1); // bottom-left
      if (x_loc < x_dim - 1 && y_loc < y_dim - 1)
        reveal(board, x_dim, y_dim, x_loc + 1, y_loc + 1); // bottom-right
    }
  } else if (board[i] & 0x10) { // if marked
    return 1;
  } else if (!(board[i] & 0x20)) { // if revealed
    return 2;
  }
  return 0;
}
