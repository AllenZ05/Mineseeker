#include "geesespotter_lib.h"
#include <iostream>

// Create the board
char *create_board(std::size_t x_dim, std::size_t y_dim) {
  char *p_board = new char[x_dim * y_dim]{};
  for (std::size_t i{}; i < x_dim * y_dim; ++i) {
    p_board[i] = '\0';
  }
  return p_board;
};

// Deallocate the given board
void clean_board(char *board) {
  delete[] board;
  board = nullptr;
};

// Prints out the board: M for marked tiles, * for hidden tiles, otherwise 0-9
void print_board(char *board, std::size_t x_dim, std::size_t y_dim) {
  std::cout << std::endl;
  for (std::size_t y{}; y < y_dim; ++y) {
    for (std::size_t x{}; x < x_dim; ++x) {
      std::size_t i{y * x_dim + x}; // index
      if (board[i] & 0x20) {
        // if hidden
        if (board[i] & 0x10) {
          // if marked
          std::cout << "M";
        } else {
          // not marked
          std::cout << "*";
        }
      } else {
        // if revealed
        int value = board[i]; // cast char to int
        std::cout << value;
      }
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
};

// Hide all the field values
void hide_board(char *board, std::size_t x_dim, std::size_t y_dim) {
  for (std::size_t i{}; i < x_dim * y_dim; ++i) {
    // hide all values
    board[i] |= 0x20;
  }
};

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
  int neighbours{};
  for (std::size_t y{}; y < y_dim; ++y) {
    for (std::size_t x{}; x < x_dim; ++x) {
      std::size_t i{y * x_dim + x}; // index
      if (board[i] == 9) {
        neighbours = 9;
      } else {
        // check adjacent fields for geese
        if (board[i - 1] == 9 && x > 0) { // check left
          neighbours++;
        }
        if (board[i + 1] == 9 && x < x_dim - 1) { // check right
          neighbours++;
        }
        if (board[i - x_dim] == 9 && y > 0) { // check up
          neighbours++;
        }
        if (board[i + x_dim] == 9 && y < y_dim - 1) { // check bottom
          neighbours++;
        }
        if (board[i - x_dim - 1] == 9 && x > 0 && y > 0) { // check top-left
          neighbours++;
        }
        if (board[i - x_dim + 1] == 9 && x < x_dim - 1 && y > 0) { // check top-right
          neighbours++;
        }
        if (board[i + x_dim - 1] == 9 && x > 0 && y < y_dim - 1) { // check bottom-left
          neighbours++;
        }
        if (board[i + x_dim + 1] == 9 && x < x_dim - 1 && y < y_dim - 1) { // check bottom-right
          neighbours++;
        }
      }
      board[i] = neighbours;
      neighbours = 0; // reset neighbours
    }
  }
};

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
      return 9;
    } else if (board[i] == 0) {
      // reveal adjacent fields if it is not marked, and if it is hidden
      if ((!(board[i - 1] & 0x10)) && (board[i - 1] & 0x20) && x_loc > 0) { // check left
        board[i - 1] ^= 0x20;
      }
      if ((!(board[i + 1] & 0x10)) && (board[i + 1] & 0x20) && x_loc < x_dim - 1) { // check right
        board[i + 1] ^= 0x20;
      }
      if ((!(board[i - x_dim] & 0x10)) && (board[i - x_dim] & 0x20) && y_loc > 0) { // check up
        board[i - x_dim] ^= 0x20;
      }
      if ((!(board[i + x_dim] & 0x10)) && (board[i + x_dim] & 0x20) && y_loc < y_dim - 1) { // check bottom
        board[i + x_dim] ^= 0x20;
      }
      if ((!(board[i - x_dim - 1] & 0x10)) && (board[i - x_dim - 1] & 0x20) && x_loc > 0 &&
          y_loc > 0) { // check top-left
        board[i - x_dim - 1] ^= 0x20;
      }
      if ((!(board[i - x_dim + 1] & 0x10)) && (board[i - x_dim + 1] & 0x20) && x_loc < x_dim - 1 &&
          y_loc > 0) { // check top-right
        board[i - x_dim + 1] ^= 0x20;
      }
      if ((!(board[i + x_dim - 1] & 0x10)) && (board[i + x_dim - 1] & 0x20) && x_loc > 0 &&
          y_loc < y_dim - 1) { // check bottom-left
        board[i + x_dim - 1] ^= 0x20;
      }
      if ((!(board[i + x_dim + 1] & 0x10)) && (board[i + x_dim + 1] & 0x20) && x_loc < x_dim - 1 &&
          y_loc < y_dim - 1) { // check bottom-right
        board[i + x_dim + 1] ^= 0x20;
      }
    }
  } else if (board[i] & 0x10) { // if marked
    return 1;
  } else if (!(board[i] & 0x20)) { // if revealed
    return 2;
  }
  return 0;
};
