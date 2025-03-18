#include <time.h>

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720
#define HALF_WINDOW_WIDTH  (WINDOW_WIDTH  / 2)
#define HALF_WINDOW_HEIGHT (WINDOW_HEIGHT / 2)

void init_window() {
  SetTraceLogLevel(LOG_WARNING);
  SetConfigFlags(FLAG_WINDOW_UNDECORATED);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Software Renderer");
  SetRandomSeed((int)time(NULL));
}

Vector2 cartesian_to_screen(Vector2 v) {
  return (Vector2){v.x + HALF_WINDOW_WIDTH, -v.y + HALF_WINDOW_HEIGHT};
}

Vector2 screen_to_cartesian(Vector2 v) {
  return (Vector2){v.x - HALF_WINDOW_WIDTH, -v.y + HALF_WINDOW_HEIGHT};
}