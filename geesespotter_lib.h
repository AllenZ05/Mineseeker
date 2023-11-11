#ifndef GEESESPOTTER_LIB_H
#define GEESESPOTTER_LIB_H

#include <iostream>

int main();
bool game();
void start_game(char *&board, std::size_t &x_dim, std::size_t &y_dim,unsigned int &num_geese);
char get_action();
void action_show(char *&board, std::size_t &x_dim, std::size_t &y_dim,unsigned int &num_geese);
void action_mark(char *board, std::size_t x_dim, std::size_t y_dim);
std::size_t x_dim_max();
std::size_t y_dim_max();
char marked_mask();
char hidden_mask();
char value_mask();
void spread_geese(char *board, std::size_t x_dim, std::size_t y_dim,unsigned int num_geese);

#endif
