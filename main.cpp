#include "include/raylib.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <stdio.h>
#define CLAY_IMPLEMENTATION
#include "include/clay.h"
#include "include/clay_renderer_raylib.c"
#include <array>
#include <cstddef>
#include <cstdio>
// #include <format>
#include <tuple>
#include <vector>
using namespace std;
const float WIDTH = 1280;
const float HEIGHT = 1280;
int player = 1;
int bot_player = 1;
uint32_t player_1_score = 0;
uint32_t player_2_score = 0;
stringstream score;
Clay_String score_clay;
enum CellState { Empty, X, O };
enum Sceen { Menu, M_Game, B_Game, Choice_Screan, End_Screan };
Sceen current_sceen = Menu;
Sceen prev_sceen;
bool manually_close = false;
bool muted = false;
int game_state;
struct GameAssets {
  // font
  Font font;
  // textures
  Texture2D volume;
  Texture2D mute;
  Texture2D exit;
  // sound files
  Music background_music;
  Sound button_click;
  Sound player_move;

  void loadAssets() {
    font = LoadFontEx("resourses/OpenSans-Semibold.ttf", 128, NULL, 0);
    volume = LoadTexture("resourses/volume.png");
    mute = LoadTexture("resourses/mute.png");
    exit = LoadTexture("resourses/exit_button.png");
    background_music = LoadMusicStream("resourses/background_music.mp3");
    button_click = LoadSound("resourses/background_music.mp3");
    player_move = LoadSound("resourses/player_move.mp3");
  }
  void unLoadAssets() {
    UnloadFont(font);
    UnloadTexture(volume);
    UnloadTexture(mute);
    UnloadTexture(exit);
    UnloadMusicStream(background_music);
    UnloadSound(button_click);
    UnloadSound(player_move);
  }
};
GameAssets assets;
class GridCell {
public:
  GridCell() { state = Empty; }
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
                      float cell_side_length, GameAssets assets) {
    if (mouse_x >= position_x && mouse_x <= position_x + cell_side_length &&
        mouse_y >= position_y && mouse_y <= position_y + cell_side_length &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && state == Empty) {
      if (!muted) {
        PlaySound(assets.player_move);
      }
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

  return true;
}

int checkWin(array<array<GridCell, 3>, 3> grid) {
  for (size_t i = 0; i < grid.size(); i++) {
    // check horizontal lines
    if (grid.at(i).at(0).state == grid.at(i).at(1).state &&
        grid.at(i).at(1).state == grid.at(i).at(2).state &&
        grid.at(i).at(0).state != Empty) {
      if (grid.at(i).at(0).state == X) {

        return 1;
      }
      if (grid.at(i).at(0).state == O) {

        return -1;
      }
    }
    // check vertical lines
    if (grid.at(0).at(i).state == grid.at(1).at(i).state &&
        grid.at(1).at(i).state == grid.at(2).at(i).state &&
        grid.at(0).at(i).state) {
      if (grid.at(0).at(i).state == X) {

        return 1;
      }
      if (grid.at(0).at(i).state == O) {

        return -1;
      }
    }
  }
  // check diagonals
  if (grid.at(0).at(0).state == grid.at(1).at(1).state &&
      grid.at(1).at(1).state == grid.at(2).at(2).state &&
      grid.at(0).at(0).state) {
    if (grid.at(0).at(0).state == X) {

      return 1;
    }
    if (grid.at(0).at(0).state == O) {

      return -1;
    }
  }

  if (grid.at(0).at(2).state == grid.at(1).at(1).state &&
      grid.at(1).at(1).state == grid.at(2).at(0).state &&
      grid.at(0).at(2).state) {
    if (grid.at(0).at(2).state == X) {

      return 1;
    }
    if (grid.at(0).at(2).state == O) {

      return -1;
    }
  }
  if (checkDraw(grid)) {
    return 0;
  }
  return -2;
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
                                       tuple<int, int, bool> action,
                                       GameAssets assets) {
  array<array<GridCell, 3>, 3> result = grid;
  if (get<2>(action) == true) {
    if (!muted) {
      PlaySound(assets.player_move);
    }
    result.at(get<0>(action)).at(get<1>(action)).state = X;
  } else {
    if (!muted) {
      PlaySound(assets.player_move);
    }
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
            tuple<int, int, bool> &action, GameAssets assets) {
  if (gameEnded(state))
    return checkWin(state);
  tuple<int, int, bool> curent_action(-1, -1, false);
  tuple<int, int, bool> temp_action(-1, -1, false);
  if (player_1) {
    auto move_list = extractMoves(state, player_1);
    int curent = -2, max_value = -2;
    for (auto move : move_list) {
      curent =
          min_max(applyMove(state, move, assets), false, temp_action, assets);
      if (curent > max_value) {

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
      curent =
          min_max(applyMove(state, move, assets), true, temp_action, assets);
      if (curent < min_value) {

        min_value = curent;
        curent_action = move;
      }
    }

    action = curent_action;
    return min_value;
  }
  return -2;
}
void initUpdateCells(array<array<GridCell, 3>, 3> &grid, GameAssets assets) {
  float center_x = float(GetScreenWidth()) / 2;
  float center_y = float(GetScreenHeight()) / 2;
  float cell_side_length =
      (GetScreenWidth() < GetScreenHeight() ? GetScreenWidth() / 6
                                            : GetScreenHeight() / 6);
  float devider_w = 30;
  if (current_sceen == B_Game) {
    if (player != bot_player) {
      for (size_t i = 0; i < grid.size(); i++) {
        for (size_t j = 0; j < grid.at(i).size(); j++) {
          // check for the collision on every frame
          grid.at(i).at(j).colisionDetect(player, GetMouseX(), GetMouseY(),
                                          cell_side_length, assets);
        }
      }
    } else {
      tuple<int, int, bool> bot_move(-1, -1, bot_player == 1);
      min_max(grid, bot_player == 1, bot_move, assets);
      grid = applyMove(grid, bot_move, assets);
      if (player == 1) {
        player = 2;
      } else if (player == 2) {
        player = 1;
      }
    }
  } else {
    for (size_t i = 0; i < grid.size(); i++) {
      for (size_t j = 0; j < grid.at(i).size(); j++) {
        // check for the collision on every frame
        grid.at(i).at(j).colisionDetect(player, GetMouseX(), GetMouseY(),
                                        cell_side_length, assets);
      }
    }
  }
}

void drawGrid() {
  // drawing the grid for the game
  float center_x = float(GetScreenWidth()) / 2;
  float center_y = float(GetScreenHeight()) / 2;
  float devider_w = 30;
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
  for (size_t i = 0; i < grid.size(); i++) {
    for (size_t j = 0; j < grid.at(i).size(); j++) {
      // set the correct position after scalling
      grid.at(i).at(j).setPosition(
          center_x - (cell_side_length * 3 / 2 + devider_w * 3 / 2) +
              j * (2 * devider_w + cell_side_length),
          center_y - (cell_side_length * 3 / 2 + devider_w * 3 / 2) +
              i * (2 * devider_w + cell_side_length));
    }
  }
}
void resetGrid(array<array<GridCell, 3>, 3> &grid) {
  for (auto &row : grid) {
    for (GridCell &cell : row) {
      cell.state = Empty;
    }
  }
}
void manuallyClose(Clay_ElementId elementId, Clay_PointerData pointerInfo,
                   intptr_t userData) {
  if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    manually_close = true;
  }
}
void handleClickMenu(Clay_ElementId elementId, Clay_PointerData pointerInfo,
                     intptr_t userData) {
  Sceen *sceen = (Sceen *)userData;
  if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    if (elementId.id == CLAY_ID("Multiplayer button").id) {
      *sceen = M_Game;
    }
    if (elementId.id == CLAY_ID("Bot game button").id) {
      *sceen = Choice_Screan;
    }
    if (elementId.id == CLAY_ID("Exit button").id) {
      manuallyClose(CLAY_ID("Exit button"), pointerInfo, userData);
    }
  }
}
void setSound(Clay_ElementId elementId, Clay_PointerData pointerInfo,
              intptr_t userData) {
  bool *muted = (bool *)userData;
  if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    *muted = !*muted;
  }
}
void restartGame(Clay_ElementId elementId, Clay_PointerData pointerInfo,
                 intptr_t userData) {
  Sceen *sceen = (Sceen *)userData;
  if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    *sceen = prev_sceen;
    player = 1;
  }
}
void returnToMenu(Clay_ElementId elementId, Clay_PointerData pointerInfo,
                  intptr_t userData) {
  Sceen *sceen = (Sceen *)userData;
  if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    *sceen = Menu;
    player_1_score = 0;
    player_2_score = 0;
    player = 1;
  }
}
void chosePlayer(Clay_ElementId elementId, Clay_PointerData pointerInfo,
                 intptr_t userData) {
  Sceen *sceen = (Sceen *)userData;
  if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {

    if (elementId.id == CLAY_ID("the X button").id) {
      bot_player = 2;
      *sceen = B_Game;
    } else if (elementId.id == CLAY_ID("the O button").id) {
      bot_player = 1;
      *sceen = B_Game;
    }
  }
}
void topBar(GameAssets *assets) {
  CLAY({.id = CLAY_ID("top bar"),
        .layout = {.sizing = {.width = CLAY_SIZING_GROW()},
                   .layoutDirection = CLAY_LEFT_TO_RIGHT}}) {

    CLAY({.id = CLAY_ID("exit button container"),
          .layout = {
              .sizing = {CLAY_SIZING_FIT(0), CLAY_SIZING_FIT(0)},
              .padding = CLAY_PADDING_ALL(2),
              .childGap = 10,
              .childAlignment = {.x = CLAY_ALIGN_X_LEFT},
          }}) {
      CLAY({.id = CLAY_ID("exit button"),
            .layout =
                {
                    .sizing = {CLAY_SIZING_FIT(100), CLAY_SIZING_FIT(100)},
                },
            .image = {.imageData = &assets->exit,
                      .sourceDimensions = {60, 60}}}) {
        Clay_OnHover(manuallyClose, (intptr_t)&muted);
      }
    }
    CLAY({.id = CLAY_ID("score container"),
          .layout = {
              .sizing = {CLAY_SIZING_GROW(300), CLAY_SIZING_FIT(50)},
              .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                 .y = CLAY_ALIGN_Y_CENTER},
          }}) {

      CLAY_TEXT(score_clay, CLAY_TEXT_CONFIG({
                                .textColor = {255, 255, 255, 255},
                                .fontSize = 128,
                            }));
    }
    CLAY({.id = CLAY_ID("sound button container"),
          .layout = {
              .sizing = {CLAY_SIZING_FIT(0), CLAY_SIZING_FIT(0)},
              .padding = CLAY_PADDING_ALL(2),
              .childGap = 10,
              .childAlignment = {.x = CLAY_ALIGN_X_RIGHT},
          }}) {
      CLAY({.id = CLAY_ID("sound button"),
            .layout =
                {
                    .sizing = {CLAY_SIZING_FIT(100), CLAY_SIZING_FIT(100)},
                },
            .image = {.imageData = muted ? &assets->mute : &assets->volume,
                      .sourceDimensions = {60, 60}}}) {
        Clay_OnHover(setSound, (intptr_t)&muted);
      }
    }
  }
}

void menu(GameAssets *assets) {

  Clay_BeginLayout();
  CLAY({
      .id = CLAY_ID("menu screan"),
      .layout = {.sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)},
                 .padding = CLAY_PADDING_ALL(15),
                 .layoutDirection = CLAY_TOP_TO_BOTTOM},
  }) {
    topBar(assets);
    CLAY({
        .id = CLAY_ID("menu body"),
        .layout = {.sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)},
                   .padding = {CLAY_PADDING_ALL(10)},
                   .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                      .y = CLAY_ALIGN_Y_CENTER}},
    }) {
      CLAY({
          .id = CLAY_ID("menu panel"),
          .layout =
              {
                  .sizing = {CLAY_SIZING_FIT(), CLAY_SIZING_FIT()},
                  .padding = {30, 30, 20, 20},
                  .childGap = 30,
                  .layoutDirection = CLAY_TOP_TO_BOTTOM,
              },
          .backgroundColor = {102, 191, 255, 255},
          .cornerRadius = CLAY_CORNER_RADIUS(10),
          .border = {.color = {255, 255, 255, 255}, .width = {10, 10, 10, 10}},
      }) {
        CLAY({.id = CLAY_ID("text container"),
              .layout = {
                  .sizing = {CLAY_SIZING_FIT(300), CLAY_SIZING_FIT(100)}}}) {
          CLAY_TEXT(
              CLAY_STRING("Wellcome to tic tac toe\nplease chose a game mode"),
              CLAY_TEXT_CONFIG({
                  .textColor = {255, 255, 255, 255},
                  .fontSize = 64,
              }));
        }
        CLAY({
            .id = CLAY_ID("Multiplayer button"),
            .layout =
                {
                    .sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_FIT(30)},
                    .padding = CLAY_PADDING_ALL(10),
                    .childAlignment = {.x = CLAY_ALIGN_X_CENTER},
                },
            .backgroundColor = {200, 200, 200, 255},
            .cornerRadius = CLAY_CORNER_RADIUS(10),
            .border = {.color = {255, 255, 255, 255}, .width = {5, 5, 5, 5}},
        }) {
          Clay_OnHover(handleClickMenu, (intptr_t)&current_sceen);
          CLAY_TEXT(CLAY_STRING("Multi player"),
                    CLAY_TEXT_CONFIG({
                        .textColor = {255, 255, 255, 255},
                        .fontSize = 64,
                    }));
        }
        CLAY({
            .id = CLAY_ID("Bot game button"),
            .layout =
                {
                    .sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_FIT(30)},
                    .padding = CLAY_PADDING_ALL(10),
                    .childAlignment = {.x = CLAY_ALIGN_X_CENTER},
                },
            .backgroundColor = {200, 200, 200, 255},
            .cornerRadius = CLAY_CORNER_RADIUS(10),
            .border = {.color = {255, 255, 255, 255}, .width = {5, 5, 5, 5}},
        }) {
          Clay_OnHover(handleClickMenu, (intptr_t)&current_sceen);
          CLAY_TEXT(CLAY_STRING("Bot Game"),
                    CLAY_TEXT_CONFIG({
                        .textColor = {255, 255, 255, 255},
                        .fontSize = 64,
                    }));
        }
      }
    }
  }
  Clay_RenderCommandArray render_comands = Clay_EndLayout();
  BeginDrawing();
  ClearBackground(SKYBLUE);
  Clay_Raylib_Render(render_comands, &assets->font);
  EndDrawing();
}

void mGame(GameAssets *assets) {
  prev_sceen = M_Game;
  Clay_BeginLayout();
  topBar(assets);
  Clay_RenderCommandArray render_comands = Clay_EndLayout();
  if (gameEnded(grid)) {
    if (checkWin(grid) == 1) {
      player_1_score++;
      game_state = 1;
    } else if (checkWin(grid) == -1) {
      player_2_score++;
      game_state = -1;
    } else {
      game_state = 0;
    }
    current_sceen = End_Screan;
  }
  initUpdateCells(grid, *assets);
  BeginDrawing();
  ClearBackground(SKYBLUE);
  Clay_Raylib_Render(render_comands, &assets->font);
  drawGrid();
  EndDrawing();
}

void bGame(GameAssets *assets) {

  prev_sceen = B_Game;
  Clay_BeginLayout();
  topBar(assets);
  Clay_RenderCommandArray render_comands = Clay_EndLayout();
  if (gameEnded(grid)) {
    if (checkWin(grid) == 1) {
      player_1_score++;
      game_state = 1;
    } else if (checkWin(grid) == -1) {
      player_2_score++;
      game_state = -1;
    } else {
      game_state = 0;
    }
    current_sceen = End_Screan;
  }
  initUpdateCells(grid, *assets);
  BeginDrawing();
  ClearBackground(SKYBLUE);
  Clay_Raylib_Render(render_comands, &assets->font);
  drawGrid();
  EndDrawing();
}
void choiceScrean(GameAssets *assets) {
  Clay_BeginLayout();
  CLAY({
      .id = CLAY_ID("choice screan"),
      .layout =
          {
              .sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_GROW()},
              .childAlignment = {.x = CLAY_ALIGN_X_CENTER},
              .layoutDirection = CLAY_TOP_TO_BOTTOM,
          },
  }) {
    topBar(assets);
    CLAY({.id = CLAY_ID("buttons container"),
          .layout = {
              .sizing =
                  {
                      CLAY_SIZING_FIT(30),
                      CLAY_SIZING_GROW(),
                  },
              .padding = CLAY_PADDING_ALL(10),
              .childGap = 100,
              .childAlignment = {.y = CLAY_ALIGN_Y_CENTER},
              .layoutDirection = CLAY_LEFT_TO_RIGHT,
          }}) {
      CLAY({
          .id = CLAY_ID("the X button"),
          .layout =
              {
                  .sizing = {CLAY_SIZING_FIT(), CLAY_SIZING_FIT(30)},
                  .padding = CLAY_PADDING_ALL(10),
                  .childAlignment = {.x = CLAY_ALIGN_X_CENTER},
              },
          .backgroundColor = {200, 200, 200, 255},
          .cornerRadius = CLAY_CORNER_RADIUS(10),
          .border = {.color = {255, 255, 255, 255}, .width = {5, 5, 5, 5}},
      }) {
        Clay_OnHover(chosePlayer, (intptr_t)&current_sceen);
        CLAY_TEXT(CLAY_STRING("play as X"),
                  CLAY_TEXT_CONFIG({
                      .textColor = {255, 255, 255, 255},
                      .fontSize = 64,
                  }));
      }
      CLAY({
          .id = CLAY_ID("the O button"),
          .layout =
              {
                  .sizing = {CLAY_SIZING_FIT(), CLAY_SIZING_FIT(30)},
                  .padding = CLAY_PADDING_ALL(10),
                  .childAlignment = {.x = CLAY_ALIGN_X_CENTER},
              },
          .backgroundColor = {200, 200, 200, 255},
          .cornerRadius = CLAY_CORNER_RADIUS(10),
          .border = {.color = {255, 255, 255, 255}, .width = {5, 5, 5, 5}},
      }) {
        Clay_OnHover(chosePlayer, (intptr_t)&current_sceen);
        CLAY_TEXT(CLAY_STRING("play as O"),
                  CLAY_TEXT_CONFIG({
                      .textColor = {255, 255, 255, 255},
                      .fontSize = 64,
                  }));
      }
    }
  }
  Clay_RenderCommandArray render_comands = Clay_EndLayout();
  BeginDrawing();
  ClearBackground(SKYBLUE);
  Clay_Raylib_Render(render_comands, &assets->font);
  EndDrawing();
}
void endScrean(GameAssets *assets, int game_state) {
  resetGrid(grid);
  Clay_BeginLayout();
  CLAY({.id = CLAY_ID("end sceen"),
        .layout = {.sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_GROW()},
                   .layoutDirection = CLAY_TOP_TO_BOTTOM}}) {
    topBar(assets);
    CLAY({.id = CLAY_ID("end body"),
          .layout = {
              .sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_GROW()},
              .padding = {.top = 100},
              .childGap = 100,
              .childAlignment = {.x = CLAY_ALIGN_X_CENTER},
              .layoutDirection = CLAY_TOP_TO_BOTTOM,

          }}) {
      CLAY({.id = CLAY_ID("end screan text container"),
            .layout = {
                .sizing = {CLAY_SIZING_FIT(), CLAY_SIZING_FIT()},
                .padding = {.top = 30},
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
            }}) {
        if (game_state == 1) {
          CLAY_TEXT(CLAY_STRING("Player X wins \n do you want to play again"),
                    CLAY_TEXT_CONFIG({
                        .textColor = {255, 255, 255, 255},
                        .fontSize = 64,
                    }));
        }
        if (game_state == -1) {
          CLAY_TEXT(CLAY_STRING("Player O wins \n do you want to play again"),
                    CLAY_TEXT_CONFIG({
                        .textColor = {255, 255, 255, 255},
                        .fontSize = 64,
                    }));
        }
        if (game_state == 0) {
          CLAY_TEXT(
              CLAY_STRING("The game was a draw \n do you want to play again"),
              CLAY_TEXT_CONFIG({
                  .textColor = {255, 255, 255, 255},
                  .fontSize = 64,
              }));
        }
      }

      CLAY({
          .id = CLAY_ID("Restart button"),
          .layout =
              {
                  .sizing = {CLAY_SIZING_FIT(), CLAY_SIZING_FIT(30)},
                  .padding = CLAY_PADDING_ALL(10),
                  .childAlignment = {.x = CLAY_ALIGN_X_CENTER},
              },
          .backgroundColor = {200, 200, 200, 255},
          .cornerRadius = CLAY_CORNER_RADIUS(10),
          .border = {.color = {255, 255, 255, 255}, .width = {5, 5, 5, 5}},
      }) {
        Clay_OnHover(restartGame, (intptr_t)&current_sceen);
        CLAY_TEXT(CLAY_STRING("Restart"), CLAY_TEXT_CONFIG({
                                              .textColor = {255, 255, 255, 255},
                                              .fontSize = 64,
                                          }));
      }
      CLAY({
          .id = CLAY_ID("menu button"),
          .layout =
              {
                  .sizing = {CLAY_SIZING_FIT(), CLAY_SIZING_FIT(30)},
                  .padding = CLAY_PADDING_ALL(10),
                  .childAlignment = {.x = CLAY_ALIGN_X_CENTER},
              },
          .backgroundColor = {200, 200, 200, 255},
          .cornerRadius = CLAY_CORNER_RADIUS(10),
          .border = {.color = {255, 255, 255, 255}, .width = {5, 5, 5, 5}},
      }) {
        Clay_OnHover(returnToMenu, (intptr_t)&current_sceen);
        CLAY_TEXT(CLAY_STRING("main menu"),
                  CLAY_TEXT_CONFIG({
                      .textColor = {255, 255, 255, 255},
                      .fontSize = 64,
                  }));
      }
    }
  }
  Clay_RenderCommandArray render_comands = Clay_EndLayout();
  BeginDrawing();
  ClearBackground(SKYBLUE);
  Clay_Raylib_Render(render_comands, &assets->font);
  EndDrawing();
}

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(WIDTH, HEIGHT, "min_max");
  InitAudioDevice();
  uint64_t clay_required_memory = Clay_MinMemorySize();
  Clay_Arena clay_memory =
      (Clay_Arena){.capacity = clay_required_memory,
                   .memory = (char *)malloc(clay_required_memory)};
  Clay_Initialize(clay_memory,
                  (Clay_Dimensions){.width = (float)GetScreenWidth(),
                                    .height = (float)GetScreenHeight()},
                  {});
  assets.loadAssets();
  Clay_SetMeasureTextFunction(Raylib_MeasureText, &assets.font);
  // Clay_SetDebugModeEnabled(true);
  SetTargetFPS(60);
  PlayMusicStream(assets.background_music);
  while (!WindowShouldClose() && !manually_close) {
    if (!muted) {
      UpdateMusicStream(assets.background_music);
    }
    Clay_SetLayoutDimensions((Clay_Dimensions){
        .width = (float)GetScreenWidth(), .height = (float)GetScreenHeight()});
    Vector2 mouse_pos = GetMousePosition();
    Clay_SetPointerState((Clay_Vector2){.x = mouse_pos.x, .y = mouse_pos.y},
                         IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
    score.str("");
    score.clear();
    score << " X: " << player_1_score << " O: " << player_2_score;
    score_clay = {.isStaticallyAllocated = false,
                  .length = static_cast<int32_t>(score.str().length()),
                  .chars = score.str().c_str()};
    switch (current_sceen) {
    case Menu:
      menu(&assets);
      break;
    case M_Game:
      mGame(&assets);
      break;
    case B_Game:
      bGame(&assets);
      break;
    case Choice_Screan:
      choiceScrean(&assets);
      break;
    case End_Screan:
      endScrean(&assets, game_state);
      break;
    }
  }
  assets.unLoadAssets();
  CloseWindow();
  CloseAudioDevice();
  return 0;
}
