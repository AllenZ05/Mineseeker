#ifndef GEESESPOTTER_H
#define GEESESPOTTER_H

#include <iostream>

char *create_board(std::size_t x_dim, std::size_t y_dim);
void clean_board(char *&board);
void print_board(const char *board, std::size_t x_dim, std::size_t y_dim);
void hide_board(char *board, std::size_t x_dim, std::size_t y_dim);
int mark(char *board, std::size_t x_dim, std::size_t y_dim, std::size_t x_loc, std::size_t y_loc);
void compute_neighbours(char *board, std::size_t x_dim, std::size_t y_dim);
bool is_game_won(char *board, std::size_t x_dim, std::size_t y_dim);
int reveal(char *board, std::size_t x_dim, std::size_t y_dim, std::size_t x_loc, std::size_t y_loc);

#endif
