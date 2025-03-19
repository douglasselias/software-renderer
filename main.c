#include "raylib.h"
#include "raymath.h"

#include <stdio.h>

#include "src/window.c"

int main() {
  init_window();

  while(!WindowShouldClose()) {
    float dt = GetFrameTime();

    BeginDrawing();
    ClearBackground(BLACK);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}