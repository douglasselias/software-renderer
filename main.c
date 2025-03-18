#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include <stdio.h>

#include "src/window.c"

void draw_line(Vector2 start, Vector2 end, Color color) {
  float dx = end.x - start.x;
  float dy = end.y - start.y;

  float step;
  if(fabs(dx) >= fabs(dy)) {
    step = fabsf(dx);
  } else {
    step = fabsf(dy);
  }

  dx /= step;
  dy /= step;

  float x = start.x;
  float y = start.y;
  int i = 0;

  while(i <= step) {
    DrawPixel((int)round(x), (int)round(y), color);
    x += dx;
    y += dy;
    i++;
  }
}

int main() {
  init_window();

  while(!WindowShouldClose()) {
    float dt = GetFrameTime();

    BeginDrawing();
    ClearBackground(BLACK);
    Vector2 start = {0,           HALF_WINDOW_HEIGHT};
    Vector2 end   = {WINDOW_WIDTH,WINDOW_HEIGHT};
    draw_line(start, end, MAGENTA);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}