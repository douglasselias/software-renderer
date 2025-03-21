#include <time.h>

int window_width = 1280;
int window_height = 720;

void init_window() {
  SetTraceLogLevel(LOG_WARNING);
  SetConfigFlags(FLAG_WINDOW_UNDECORATED);
  InitWindow(window_width, window_height, "Software Renderer");
  SetRandomSeed((int)time(NULL));
}

Vector2 cartesian_to_screen(Vector2 v) {
  return (Vector2){v.x + window_width  / 2, -v.y + window_height / 2};
}

Vector2 screen_to_cartesian(Vector2 v) {
  return (Vector2){v.x - window_width  / 2, -v.y + window_height / 2};
}