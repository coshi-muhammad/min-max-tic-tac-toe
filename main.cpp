#include "raylib.h"
#include <array>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <tuple>
#include <vector>
using namespace std;
const float WIDTH = 1280;
const float HEIGHT = 1280;
int player = 1;
bool game_won = false;
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
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && state == Empty &&
        game_won == false) {
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
void initUpdateCells() {
  float center_x = float(GetScreenWidth()) / 2;
  float center_y = float(GetScreenHeight()) / 2;
  float cell_side_length =
      (GetScreenWidth() < GetScreenHeight() ? GetScreenWidth() / 6
                                            : GetScreenHeight() / 6);
  float devider_w = 30;
  float devider_h = cell_side_length * 3 + devider_w * 6;
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
        game_won = true;
        if (grid.at(i).at(0).state == X) {
          printf("first\n");
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
        game_won = true;
        if (grid.at(0).at(i).state == X) {
          printf("second\n");
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
      game_won = true;
      if (grid.at(0).at(0).state == X) {
        printf("third\n");
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
      game_won = true;
      if (grid.at(0).at(2).state == X) {
        printf("fourth\n");
        printf("X wins\n");
        return 1;
      }
      if (grid.at(0).at(2).state == O) {
        printf("O wins\n");
        return -1;
      }
    }
  }
  return 0;
}
vector<tuple<int, int, bool>> extract_moves(array<array<GridCell, 3>, 3> grid,
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

void min_max(array<array<GridCell, 3>, 3> state, size_t depth, bool player_1) {}
int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(WIDTH, HEIGHT, "min_max");
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    initUpdateCells();
    checkWin(grid);
    BeginDrawing();
    ClearBackground(SKYBLUE);
    drawGrid();
    EndDrawing();
  }
  return 0;
}
