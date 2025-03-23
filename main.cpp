#include "raylib.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
// #include <iostream>
#include <tuple>
#include <vector>

using namespace std;
const float WIDTH = 1280;
const float HEIGHT = 1280;
int player = 1;
int bot_player = 2;
bool bot_game = true;
enum CellState { Empty, X, O };

class GridCell {
public:
  GridCell() { state = Empty; };
  void setPosition(int px, int py) {
    position_x = px;
    position_y = py;
  }
  void draw(float cell_side_length) {
    if (state == X) {
      DrawRectanglePro(
          {position_x + cell_side_length / 2, position_y + cell_side_length / 2,
           30, cell_side_length * 9 / 10},
          {15, cell_side_length * 9 / 20}, RAD2DEG * PI / 4, WHITE);
      DrawRectanglePro(
          {position_x + cell_side_length / 2, position_y + cell_side_length / 2,
           30, cell_side_length * 9 / 10},
          {15, cell_side_length * 9 / 20}, RAD2DEG * (PI - PI / 4), WHITE);
    } else if (state == O) {
      DrawCircle(position_x + cell_side_length / 2,
                 position_y + cell_side_length / 2, cell_side_length / 2 - 10,
                 WHITE);
      DrawCircle(position_x + cell_side_length / 2,
                 position_y + cell_side_length / 2, cell_side_length / 2 - 30,
                 SKYBLUE);
    }
  }
  void colisionDetect(int &player, int mouse_x, int mouse_y,
                      float cell_side_length) {
    if (mouse_x >= position_x && mouse_x <= position_x + cell_side_length &&
        mouse_y >= position_y && mouse_y <= position_y + cell_side_length &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && state == Empty) {
      if (player == 1) {
        state = X;
        player = 2;
      } else if (player == 2) {
        state = O;
        player = 1;
      }
    }
  }
  int position_x, position_y;
  CellState state;
};

array<array<GridCell, 3>, 3> grid;
bool checkDraw(array<array<GridCell, 3>, 3> grid) {
  for (auto row : grid) {
    for (GridCell cell : row) {
      if (cell.state == Empty) {
        return false;
      }
    }
  }
  printf("its a draw\n");
  return true;
}

int checkWin(array<array<GridCell, 3>, 3> grid) {
  if (!checkDraw(grid)) {

    for (size_t i = 0; i < grid.size(); i++) {
      // check horizontal lines
      if (grid.at(i).at(0).state == grid.at(i).at(1).state &&
          grid.at(i).at(1).state == grid.at(i).at(2).state &&
          grid.at(i).at(0).state != Empty) {
        if (grid.at(i).at(0).state == X) {
          printf("X wins\n");
          return 1;
        }
        if (grid.at(i).at(0).state == O) {
          printf("O wins\n");
          return -1;
        }
      }
      // check vertical lines
      if (grid.at(0).at(i).state == grid.at(1).at(i).state &&
          grid.at(1).at(i).state == grid.at(2).at(i).state &&
          grid.at(0).at(i).state) {
        if (grid.at(0).at(i).state == X) {
          printf("X wins\n");
          return 1;
        }
        if (grid.at(0).at(i).state == O) {
          printf("O wins\n");
          return -1;
        }
      }
    }
    // check diagonals
    if (grid.at(0).at(0).state == grid.at(1).at(1).state &&
        grid.at(1).at(1).state == grid.at(2).at(2).state &&
        grid.at(0).at(0).state) {
      if (grid.at(0).at(0).state == X) {
        printf("X wins\n");
        return 1;
      }
      if (grid.at(0).at(0).state == O) {
        printf("O wins\n");
        return -1;
      }
    }

    if (grid.at(0).at(2).state == grid.at(1).at(1).state &&
        grid.at(1).at(1).state == grid.at(2).at(0).state &&
        grid.at(0).at(2).state) {
      if (grid.at(0).at(2).state == X) {
        printf("X wins\n");
        return 1;
      }
      if (grid.at(0).at(2).state == O) {
        printf("O wins\n");
        return -1;
      }
    }
  } else {
    return 0;
  }
  return -5;
}

vector<tuple<int, int, bool>> extractMoves(array<array<GridCell, 3>, 3> grid,
                                           bool player_1) {
  vector<tuple<int, int, bool>> result;
  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      if (grid.at(i).at(j).state == Empty) {
        result.push_back(tuple(i, j, player_1));
      }
    }
  }
  return result;
}

array<array<GridCell, 3>, 3> applyMove(array<array<GridCell, 3>, 3> grid,
                                       tuple<int, int, bool> action) {
  array<array<GridCell, 3>, 3> result = grid;
  if (get<2>(action) == true) {
    result.at(get<0>(action)).at(get<1>(action)).state = X;
  } else {
    result.at(get<0>(action)).at(get<1>(action)).state = O;
  }
  return result;
}

bool gameEnded(array<array<GridCell, 3>, 3> grid) {
  int game_state = -2;
  game_state = checkWin(grid);
  if (game_state == 1 || game_state == -1 || game_state == 0) {
    return true;
  }
  return false;
}

int min_max(array<array<GridCell, 3>, 3> state, bool player_1,
            tuple<int, int, bool> &action) {
  if (gameEnded(state))
    return checkWin(state);
  tuple<int, int, bool> curent_action(-1, -1, false);
  tuple<int, int, bool> temp_action(-1, -1, false);
  if (player_1) {
    auto move_list = extractMoves(state, player_1);
    int curent = -2, max_value = -2;
    for (auto move : move_list) {
      curent = min_max(applyMove(state, move), false, temp_action);
      if (curent > max_value) {
        printf("this is happening max");
        max_value = curent;
        curent_action = move;
      }
    }
    action = curent_action;
    return max_value;
  }

  if (!player_1) {
    auto move_list = extractMoves(state, player_1);
    int curent = 2, min_value = 2;
    for (auto move : move_list) {
      curent = min_max(applyMove(state, move), true, temp_action);
      if (curent < min_value) {
        printf("this is happening min");
        min_value = curent;
        curent_action = move;
      }
    }

    action = curent_action;
    return min_value;
  }
  return -2;
}
void initUpdateCells(array<array<GridCell, 3>, 3> &grid) {
  float center_x = float(GetScreenWidth()) / 2;
  float center_y = float(GetScreenHeight()) / 2;
  float cell_side_length =
      (GetScreenWidth() < GetScreenHeight() ? GetScreenWidth() / 6
                                            : GetScreenHeight() / 6);
  float devider_w = 30;
  float devider_h = cell_side_length * 3 + devider_w * 6;
  if (bot_game) {
    if (player != bot_player) {
      for (size_t i = 0; i < grid.size(); i++) {
        for (size_t j = 0; j < grid.at(i).size(); j++) {
          // set the correct position after scalling
          grid.at(i).at(j).setPosition(
              center_x - (cell_side_length * 3 / 2 + devider_w * 3 / 2) +
                  j * (2 * devider_w + cell_side_length),
              center_y - (cell_side_length * 3 / 2 + devider_w * 3 / 2) +
                  i * (2 * devider_w + cell_side_length));
          // check for the collision on every frame
          grid.at(i).at(j).colisionDetect(player, GetMouseX(), GetMouseY(),
                                          cell_side_length);
        }
      }
    } else {
      tuple<int, int, bool> bot_move(-1, -1, false);
      min_max(grid, false, bot_move);
      grid = applyMove(grid, bot_move);
      if (player == 1) {
        player = 2;
      } else if (player == 2) {
        player = 1;
      }
    }
  } else {
    for (size_t i = 0; i < grid.size(); i++) {
      for (size_t j = 0; j < grid.at(i).size(); j++) {
        // set the correct position after scalling
        grid.at(i).at(j).setPosition(
            center_x - (cell_side_length * 3 / 2 + devider_w * 3 / 2) +
                j * (2 * devider_w + cell_side_length),
            center_y - (cell_side_length * 3 / 2 + devider_w * 3 / 2) +
                i * (2 * devider_w + cell_side_length));
        // check for the collision on every frame
        grid.at(i).at(j).colisionDetect(player, GetMouseX(), GetMouseY(),
                                        cell_side_length);
      }
    }
  }
}

void drawGrid() {
  // drawing the grid for the game
  float center_x = float(GetScreenWidth()) / 2;
  float center_y = float(GetScreenHeight()) / 2;
  float devider_w = 30;
  float spacer = 10;
  float cell_side_length =
      (GetScreenWidth() < GetScreenHeight() ? GetScreenWidth() / 6
                                            : GetScreenHeight() / 6);
  float devider_h = cell_side_length * 3 + devider_w * 6;
  DrawRectangleRounded({center_x - (cell_side_length / 2 + devider_w),
                        center_y - (devider_h / 2) + devider_w / 2, devider_w,
                        devider_h},
                       1, 10, WHITE);
  DrawRectangleRounded({float(center_x) + (cell_side_length / 2 + devider_w),
                        center_y - (devider_h / 2) + devider_w / 2, devider_w,
                        devider_h},
                       1, 10, WHITE);
  DrawRectangleRounded({center_x - (devider_h / 2) + devider_w / 2,
                        center_y - (cell_side_length / 2 + devider_w),
                        devider_h, devider_w},
                       1, 10, WHITE);
  DrawRectangleRounded({center_x - (devider_h / 2) + devider_w / 2,
                        center_y + (cell_side_length / 2 + devider_w),
                        devider_h, devider_w},
                       1, 10, WHITE);
  // drawing the cells
  for (auto row : grid) {
    for (GridCell cell : row) {
      cell.draw(cell_side_length);
    }
  }
}

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(WIDTH, HEIGHT, "min_max");
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    if (!gameEnded(grid)) {
      initUpdateCells(grid);
    }
    BeginDrawing();
    ClearBackground(SKYBLUE);
    drawGrid();
    EndDrawing();
  }
  return 0;
}
