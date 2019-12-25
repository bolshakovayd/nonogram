#include "solver.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int main() {
  int height, width;
  cin >> height >> width;
  cin.ignore();

  nonogram::Solver game(height, width);

  for (int i = 0; i < height; ++i) {
    string row_data_str;
    getline(cin, row_data_str);

    vector<int> row_data;
    istringstream ss(row_data_str);
    while (!ss.eof()) {
      int temp;
      ss >> temp;
      row_data.push_back(temp);
    }
    game.rows_data.push_back(row_data);
  }

  for (int i = 0; i < width; ++i) {
    string col_data_str;
    getline(cin, col_data_str);

    vector<int> col_data;
    istringstream ss(col_data_str);
    while (!ss.eof()) {
      int temp;
      ss >> temp;
      col_data.push_back(temp);
    }
    game.cols_data.push_back(col_data);
  }

  game.solve();
}

/*
example

20
20
7 10
10
6 10
9 1
16 1
5 4
9 7
13 3
1 4 3 1
3 11
2 1
8 5 5
7 4 3
7 8
16
5 2 5
7 3 2
2 10
11 1
5 8
2 3 3 1 1
1 3 2 1 3 1 1
1 3 2 1 3 3 1
1 3 2 3 3 1
1 6 1 6 1
8 1 6 1
8 1 6 1
2 7 1 1 1
1 7 1 1 1
1 1 4 2 2 1
3 1 3 2 2 2
3 1 2 1 2 1 2
5 4 1 2 4
3 1 1 2 1 2 4
3 3 3 2 4
1 1 3 2 3 3
1 1 2 1 5 1
1 1 3 7 1
1 1 1 1 5 1 1
1 1 2 6 3
 */
