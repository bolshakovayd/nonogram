#pragma once

#include <vector>

namespace nonogram {
enum Tile { WHITE, BLACK, UNKNOWN };

struct Solver {
  Solver(int height, int width)
      : height{height}, width{width},
        state(height, std::vector<Tile>(width, UNKNOWN)),
        row_strategies(height), col_strategies(width) {}

  void solve();

  std::vector<std::vector<int>> rows_data;

  std::vector<std::vector<int>> cols_data;

private:
  void create_strategies();

  void create_one_strategy(std::vector<int> &line, const std::vector<int> &data,
                           int size);

  bool strategy_recursive(int size, const std::vector<int> &sum,
                    std::vector<int> &strategies, int &output, int start,
                    int level);

  std::pair<bool, int> eliminate_strategies(); // перебрать стратегии

  static bool is_valid(const std::vector<Tile> &states, int strategy);

  bool is_solved();

  bool solve_recursive();

  void print_board();

  void restore(const std::vector<std::vector<Tile>> &prev_state,
               const std::vector<std::vector<int>> &prev_row_strategies,
               const std::vector<std::vector<int>> &prev_col_strategies,
               const std::vector<char> &prev_solved_cols,
               const std::vector<char> &prev_solved_rows); // backup

  int height, width;
  std::vector<std::vector<Tile>> state;
  std::vector<std::vector<int>> row_strategies;
  std::vector<std::vector<int>> col_strategies;
  std::vector<char> solved_rows;
  std::vector<char> solved_cols;
};
} // namespace nonogram
