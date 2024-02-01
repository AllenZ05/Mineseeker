#include "geesespotter_lib.h"
#include "geesespotter.h"
#include <cctype>
#include <ctime>
#include <iostream>
#include <limits>
#include <random>
#include <string>

int main() {
  std::srand(static_cast<unsigned int>(std::time(nullptr)));
  if (!game()) {
    std::cerr << "An error occurred during the game." << std::endl;
  }
  return 0;
}

bool game() {
  std::size_t x_dim{0}, y_dim{0};
  unsigned int num_geese{0};
  char *game_board = nullptr;

  if (!start_game(game_board, x_dim, y_dim, num_geese)) {
    return false;
  }

  char curr_action{0};
  while ((curr_action = get_action()) != 'Q') {
    switch (curr_action) {
    case 'S':
      action_show(game_board, x_dim, y_dim, num_geese);
      break;
    case 'M':
      action_mark(game_board, x_dim, y_dim);
      break;
    case 'R':
      std::cout << "Restarting the game." << std::endl;
      if (!start_game(game_board, x_dim, y_dim, num_geese)) {
        return false;
      }
      break;
    }
    print_board(game_board, x_dim, y_dim);

    if (is_game_won(game_board, x_dim, y_dim)) {
      std::cout << "Congratulations! You won!" << std::endl;
      if (!start_game(game_board, x_dim, y_dim, num_geese)) {
        return false;
      }
    }
  }

  clean_board(game_board);
  return true;
}

bool start_game(char *&board, std::size_t &x_dim, std::size_t &y_dim, unsigned int &num_geese) {
  std::cout << std::endl;
  std::cout << "-------------------------------------------\n";
  std::cout << "Welcome to GeeseSpotter!\n\n";
  x_dim = get_dimension_input("x");
  y_dim = get_dimension_input("y");

  num_geese = get_geese_input(x_dim, y_dim);

  clean_board(board); // Safe to call on nullptr
  board = create_board(x_dim, y_dim);
  spread_geese(board, x_dim, y_dim, num_geese);
  compute_neighbours(board, x_dim, y_dim);
  hide_board(board, x_dim, y_dim);

  return true;
}

std::size_t get_dimension_input(const std::string &dimension_name) {
  std::size_t dimension = 0;
  while (true) {
    std::cout << "Please enter the " << dimension_name << " dimension (max " << ((dimension_name == "x") ? x_dim_max() : y_dim_max())
              << "): ";
    std::cin >> dimension;

    if (std::cin.fail() || dimension < 1 || dimension > ((dimension_name == "x") ? x_dim_max() : y_dim_max())) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cerr << "Invalid input. Please enter a number between 1 and " << ((dimension_name == "x") ? x_dim_max() : y_dim_max()) << ".\n";
    } else {
      break;
    }
  }
  return dimension;
}

unsigned int get_geese_input(std::size_t x_dim, std::size_t y_dim) {
  unsigned int num_geese = 0;
  while (true) {
    std::cout << "Please enter the number of geese: ";
    std::cin >> num_geese;

    if (std::cin.fail() || num_geese > x_dim * y_dim) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cerr << "Invalid input. Please enter a valid number of geese.\n";
    } else {
      break;
    }
  }
  return num_geese;
}

char get_action() {
  char action{0};

  std::cout << "\nPlease enter the action ([S]how, [M]ark, [R]estart, [Q]uit): ";
  std::cin >> action;

  if (islower(action)) {
    action = toupper(action);
  }

  return action;
}

void action_show(char *&board, std::size_t &x_dim, std::size_t &y_dim, unsigned int &num_geese) {
  std::size_t x_reveal{0};
  std::size_t y_reveal{0};
  std::cout << "Please enter the x location to show: ";
  std::cin >> x_reveal;
  std::cout << "Please enter the y location to show: ";
  std::cin >> y_reveal;
  std::cout << std::endl;
  if (x_reveal >= x_dim || y_reveal >= y_dim) {
    std::cout << "Location entered is not on the board." << std::endl;
  } else if (board[x_dim * y_reveal + x_reveal] & marked_mask()) {
    std::cout << "Location is marked, and therefore cannot be revealed." << std::endl;
    std::cout << "Use Mark on location to unmark." << std::endl;
  } else if (reveal(board, x_dim, y_dim, x_reveal, y_reveal) == 9) {
    std::cout << "You disturbed a goose! Your game has ended." << std::endl;
    print_board(board, x_dim, y_dim);
    std::cout << "Starting a new game." << std::endl;
    start_game(board, x_dim, y_dim, num_geese);
  }

  return;
}

void action_mark(char *board, std::size_t x_dim, std::size_t y_dim) {
  std::size_t x_mark{0};
  std::size_t y_mark{0};
  std::cout << "Please enter the x location to mark: ";
  std::cin >> x_mark;
  std::cout << "Please enter the y location to mark: ";
  std::cin >> y_mark;

  if (x_mark >= x_dim || y_mark >= y_dim) {
    std::cout << "Location entered is not on the board." << std::endl;
  } else if (mark(board, x_dim, y_dim, x_mark, y_mark) == 2) {
    std::cout << "Position already revealed, so cannot be marked." << std::endl;
  }
  return;
}

std::size_t x_dim_max() { return 60; }

std::size_t y_dim_max() { return 20; }

char marked_mask() { return 0x10; }

char hidden_mask() { return 0x20; }

char value_mask() { return 0x0F; }

void spread_geese(char *board, std::size_t x_dim, std::size_t y_dim, unsigned int num_geese) {
  if (board != nullptr) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, x_dim * y_dim - 1);
    for (unsigned int gen_goose = 0; gen_goose < num_geese; ++gen_goose) {
      std::size_t try_position = 0;
      do {
        try_position = dis(gen);
      } while (board[try_position] != 0);
      board[try_position] = 9;
    }
  }
}
