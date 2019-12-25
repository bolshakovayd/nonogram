#include "solver.h"
#include <algorithm>
#include <iostream>

using nonogram::Solver;
using namespace std;
auto const INF = UINT64_MAX;
bool Solver::is_solved() {
  for (int i = 0; i < height; ++i) {
    if (row_strategies[i].size() > 1) {
      return false;
    }
  }
  return true;
}

void Solver::solve() {
  create_strategies();
  auto prune_results = eliminate_strategies();
  if (!prune_results.first) {
    cout << "The puzzle is impossible." << endl;
    return;
  }

  if (is_solved()) {
    print_board();
  } else {
    solve_recursive();
  }
}

bool Solver::solve_recursive() {
  bool vertical = false;
  int strategies = INF;
  int current_line = -1;
  for (int i = 0; i < height; ++i) {
    if (!solved_rows[i] && row_strategies[i].size() < strategies &&
        row_strategies[i].size() > 1) {
      strategies = row_strategies[i].size();
      current_line = i;
    }
  }
  for (int i = 0; i < width; ++i) {
    if (!solved_cols[i] && col_strategies[i].size() < strategies &&
        col_strategies[i].size() > 1) {
      strategies = col_strategies[i].size();
      current_line = i;
      vertical = true;
    }
  }

  vector<vector<Tile>> prev_state(height, vector<Tile>(width));
  vector<vector<int>> prev_row_strategies(height);
  vector<vector<int>> prev_col_strategies(width);
  vector<char> prev_solved_rows(height);
  vector<char> prev_solved_cols(width);
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      prev_state[i][j] = state[i][j];
    }
  }
  for (int i = 0; i < height; ++i) {
    for (int j : row_strategies[i]) {
      prev_row_strategies[i].push_back(j);
    }
    prev_solved_rows[i] = solved_rows[i];
  }
  for (int i = 0; i < width; ++i) {
    for (int j : col_strategies[i]) {
      prev_col_strategies[i].push_back(j);
    }
    prev_solved_cols[i] = solved_cols[i];
  }

  if (!vertical) {
    int count = 0;
    vector<pair<int, int>> eliminated;
    for (int current_strategy : prev_row_strategies[current_line]) {
      row_strategies[current_line].clear();
      row_strategies[current_line].push_back(current_strategy);
      solved_rows[current_line] = true;
      for (int i = 0; i < width; ++i) {
        state[current_line][i] =
            (current_strategy & (1LLU << i)) != 0 ? BLACK : WHITE;
      }
      pair<bool, int> eliminateResult = eliminate_strategies();
      if (eliminateResult.first) {
        if (is_solved()) {
          print_board();
          return true;
        } else {
          eliminated.emplace_back(eliminateResult.second, count);
        }
        restore(prev_state, prev_row_strategies, prev_col_strategies,
                prev_solved_cols, prev_solved_rows);
        count++;
      }
    }
    sort(eliminated.begin(), eliminated.end(),
         [&](const pair<int, int> &a, const pair<int, int> &b) {
           return a.first > b.first;
         });

    count = 0;
    for (auto &current_strategies : eliminated) {
      int strategy =
          prev_row_strategies[current_line][current_strategies.second];
      row_strategies[current_line].clear();
      row_strategies[current_line].push_back(strategy);
      solved_rows[current_line] = true;
      for (int i = 0; i < width; ++i) {
        state[current_line][i] = (strategy & (1LLU << i)) != 0 ? BLACK : WHITE;
      }
      eliminate_strategies();
      if (solve_recursive()) {
        return true;
      }
      restore(prev_state, prev_row_strategies, prev_col_strategies,
              prev_solved_cols, prev_solved_rows);
      count++;
    }
  } else {
    int count = 0;
    vector<pair<int, int>> eliminated;
    for (int current_strategy : prev_col_strategies[current_line]) {
      col_strategies[current_line].clear();
      col_strategies[current_line].push_back(current_strategy);
      solved_cols[current_line] = true;
      for (int i = 0; i < height; ++i) {
        state[i][current_line] =
            (current_strategy & (1LLU << i)) != 0 ? BLACK : WHITE;
      }
      auto res = eliminate_strategies();
      if (res.first) {
        if (is_solved()) {
          print_board();
          return true;
        } else {
          eliminated.emplace_back(res.second, count);
        }
      }
      restore(prev_state, prev_row_strategies, prev_col_strategies,
              prev_solved_cols, prev_solved_rows);
      ++count;
    }
    sort(eliminated.begin(), eliminated.end(),
         [&](const pair<int, int> &a, const pair<int, int> &b) {
           return a.first > b.first;
         });

    count = 0;
    for (auto &current : eliminated) {
      int strategy = prev_col_strategies[current_line][current.second];
      solved_cols[current_line] = true;
      col_strategies[current_line].clear();
      col_strategies[current_line].push_back(strategy);
      for (int i = 0; i < height; ++i) {
        state[i][current_line] = (strategy & (1LLU << i)) != 0 ? BLACK : WHITE;
      }
      eliminate_strategies();
      if (solve_recursive()) {
        return true;
      }
      restore(prev_state, prev_row_strategies, prev_col_strategies,
              prev_solved_cols, prev_solved_rows);
      count++;
    }
  }
  return false;
}

std::pair<bool, int> Solver::eliminate_strategies() {
  solved_rows = vector<char>(height, false);
  solved_cols = vector<char>(width, false);

  int eliminated = 0;
  int total_eliminated = 0;
  do {
    eliminated = 0;
    for (int i = 0; i < height; ++i) {
      if (solved_rows[i]) {
        continue;
      }
      vector<int> strategies;
      vector<Tile> current;
      for (int j = 0; j < width; ++j) {
        current.push_back(state[i][j]);
      }
      int to_fill = -1, to_empty = -1;
      for (int it : row_strategies[i]) {
        if (!is_valid(current, it)) {
          ++eliminated;
        } else {
          strategies.push_back(it);
          to_fill &= it;
          to_empty &= ~(it);
          ++it;
        }
      }
      if (strategies.empty()) {
        return {false, 0};
      }
      if (strategies.size() < row_strategies[i].size()) {
        row_strategies[i].resize(strategies.size());
        for (int j = 0; j < strategies.size(); ++j) {
          row_strategies[i][j] = strategies[j];
        }
      }
      for (int j = 0; j < width; ++j) {
        if (to_fill & (1LLU << j)) {
          state[i][j] = BLACK;
        } else if (to_empty & (1LLU << j)) {
          state[i][j] = WHITE;
        }
      }
      if (row_strategies.size() == 1) {
        solved_rows[i] = true;
      }
    }
    for (int j = 0; j < width; ++j) {
      if (solved_cols[j]) {
        continue;
      }

      vector<int> strategies;
      vector<Tile> current;
      for (int i = 0; i < height; ++i) {
        current.push_back(state[i][j]);
      }
      int to_fill = -1, to_empty = -1;
      for (int strategy : col_strategies[j]) {
        if (!is_valid(current, strategy)) {
          ++eliminated;
        } else {
          strategies.push_back(strategy);
          to_fill &= strategy;
          to_empty &= ~(strategy);
          ++strategy;
        }
      }
      if (strategies.empty()) {
        return {false, 0};
      }
      if (strategies.size() < col_strategies[j].size()) {
        col_strategies[j].resize(strategies.size());
        for (int i = 0; i < strategies.size(); ++i) {
          col_strategies[j][i] = strategies[i];
        }
      }
      for (int i = 0; i < height; ++i) {
        if (to_fill & (1LLU << i)) {
          state[i][j] = BLACK;
        } else if (to_empty & (1LLU << i)) {
          state[i][j] = WHITE;
        }
      }
      if (col_strategies[j].size() == 1) {
        solved_cols[j] = true;
      }
    }
    total_eliminated += eliminated;
  } while (eliminated);
  return {true, total_eliminated};
}

bool Solver::is_valid(const vector<Tile> &states, int strategy) {
  for (size_t i = 0; i < states.size(); ++i) {
    bool bit = ((strategy & (1LLU << i)) != 0);
    if ((bit && states[i] == WHITE) || (!bit && states[i] == BLACK)) {
      return false;
    }
  }
  return true;
}

void Solver::create_strategies() {
  for (int i = 0; i < height; ++i) {
    create_one_strategy(row_strategies[i], rows_data[i], width);
  }
  for (int i = 0; i < width; ++i) {
    create_one_strategy(col_strategies[i], cols_data[i], height);
  }
}

void Solver::create_one_strategy(vector<int> &line, const vector<int> &data,
                                 const int size) {
  bool empty = true;
  for (int e : data) {
    if (e) {
      empty = false;
      break;
    }
  }
  if (empty) {
    line.push_back(0);
    return;
  }

  vector<int> sum(data.size() + 1, 0);
  sum[data.size() - 1] = data.back() + 1;
  for (int i = data.size() - 2; i >= 0; --i) {
    sum[i] = sum[i + 1] + data[i] + 1;
  }

  int output = 0;
  strategy_recursive(size, sum, line, output, 0, 0);
}

bool Solver::strategy_recursive(int size, const vector<int> &sum,
                                vector<int> &strategies, int &output, int start,
                                int level) {
  if (level == sum.size() - 1) {
    strategies.push_back(output);
    return true;
  }

  int end = size - sum[level] + 1;
  if (end < start) {
    return false;
  }
  int num = sum[level] - sum[level + 1] - 1;
  int temp = output;
  int mask = (1LLU << num) - 1;
  for (int i = start; i <= end; ++i) {
    output = temp | (mask << i);
    if (!strategy_recursive(size, sum, strategies, output, i + num + 1,
                            level + 1)) {
      return false;
    }
  }
  output = temp;

  return true;
}

void Solver::print_board() {
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      switch (state[i][j]) {
      case WHITE:
        cout << "-";
        break;
      case BLACK:
        cout << "*";
        break;
      default:
        cout << "？";
        break;
      }
    }
    cout << endl;
  }
}

void Solver::restore(const std::vector<std::vector<Tile>> &prev_state,
                     const std::vector<std::vector<int>> &prev_row_strategies,
                     const std::vector<std::vector<int>> &prev_col_strategies,
                     const std::vector<char> &prev_solved_cols,
                     const std::vector<char> &prev_solved_rows) {
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      state[i][j] = prev_state[i][j];
    }
  }
  for (int i = 0; i < height; ++i) {
    row_strategies[i].clear();
    for (int j : prev_row_strategies[i]) {
      row_strategies[i].push_back(j);
    }
    solved_rows[i] = prev_solved_rows[i];
  }
  for (int i = 0; i < width; ++i) {
    col_strategies[i].clear();
    for (int j : prev_col_strategies[i]) {
      col_strategies[i].push_back(j);
    }
    solved_cols[i] = prev_solved_cols[i];
  }
}
