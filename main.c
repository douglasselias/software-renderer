#include "raylib.h"
#include "raymath.h"

#include <stdio.h>

#include "src/window.c"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

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

typedef struct {
  Vector2 a, b, c;
} Triangle;

void draw_triangle(Triangle t, Color color) {
  draw_line(t.a, t.b, color);
  draw_line(t.b, t.c, color);
  draw_line(t.c, t.a, color);
}

// Copied from https://github.com/gustavopezzi/triangle-rasterizer-float/blob/main/src/main.c
bool is_top_left(Vector2 start, Vector2 end) {
  Vector2 edge = { end.x - start.x, end.y - start.y };
  bool is_top_edge = edge.y == 0 && edge.x > 0;
  bool is_left_edge = edge.y < 0;
  return is_left_edge || is_top_edge;
}

float edge_cross(Vector2 a, Vector2 b, Vector2 p) {
  Vector2 ab = { b.x - a.x, b.y - a.y };
  Vector2 ap = { p.x - a.x, p.y - a.y };
  return ab.x * ap.y - ab.y * ap.x;
}

void draw_triangle_filled(Triangle t, Color colors[3]) {
  float min_x = floorf(MIN(MIN(t.a.x, t.b.x), t.c.x));
  float min_y = floorf(MIN(MIN(t.a.y, t.b.y), t.c.y));
  float max_x = ceilf(MAX(MAX(t.a.x, t.b.x), t.c.x));
  float max_y = ceilf(MAX(MAX(t.a.y, t.b.y), t.c.y));

  float area = edge_cross(t.a, t.b, t.c);

  // Compute the constant delta_s that will be used for the horizontal and vertical steps
  float delta_w0_col = (t.b.y - t.c.y);
  float delta_w1_col = (t.c.y - t.a.y);
  float delta_w2_col = (t.a.y - t.b.y);
  float delta_w0_row = (t.c.x - t.b.x);
  float delta_w1_row = (t.a.x - t.c.x);
  float delta_w2_row = (t.b.x - t.a.x);

  // Rasterization fill rule, not 100% precise due to floating point innacuracy
  float bias0 = is_top_left(t.b, t.c) ? 0 : -0.0001f;
  float bias1 = is_top_left(t.c, t.a) ? 0 : -0.0001f;
  float bias2 = is_top_left(t.a, t.b) ? 0 : -0.0001f;

  // Compute the edge functions for the fist (top-left) point
  Vector2 p0 = { min_x + 0.5f , min_y + 0.5f };
  float w0_row = edge_cross(t.b, t.c, p0) + bias0;
  float w1_row = edge_cross(t.c, t.a, p0) + bias1;
  float w2_row = edge_cross(t.a, t.b, p0) + bias2;

  // Loop all candidate pixels inside the bounding box
  for (int y = (int)min_y; y <= max_y; y++) {
    float w0 = w0_row;
    float w1 = w1_row;
    float w2 = w2_row;
    for (int x = (int)min_x; x <= max_x; x++) {
      bool is_inside = w0 >= 0 && w1 >= 0 && w2 >= 0;
      if (is_inside) {
        float alpha = w0 / area;
        float beta  = w1 / area;
        float gamma = w2 / area;

        unsigned char a = 0xFF;
        unsigned char r = (unsigned char)((alpha) * colors[0].r + (beta) * colors[1].r + (gamma) * colors[2].r);
        unsigned char g = (unsigned char)((alpha) * colors[0].g + (beta) * colors[1].g + (gamma) * colors[2].g);
        unsigned char b = (unsigned char)((alpha) * colors[0].b + (beta) * colors[1].b + (gamma) * colors[2].b);

        // unsigned int interp_color = 0x00000000;
        // interp_color = (interp_color | a) << 8;
        // interp_color = (interp_color | b) << 8;
        // interp_color = (interp_color | g) << 8;
        // interp_color = (interp_color | r);
        Color interp_color = {r,g,b,a};
        DrawPixel(x, y, interp_color);
        // DrawPixel(x, y, RED);
      }
      w0 += delta_w0_col;
      w1 += delta_w1_col;
      w2 += delta_w2_col;
    }
    w0_row += delta_w0_row;
    w1_row += delta_w1_row;
    w2_row += delta_w2_row;
  }
}

Vector2 rotate_point(Vector2 point, float angle) {
  return (Vector2){
    point.x * cosf(angle) - point.y * sinf(angle),
    point.x * sinf(angle) + point.y * cosf(angle),
  };
}

Vector2 rotate_point_with_origin(Vector2 origin, Vector2 point, float angle) {
  float dx = point.x - origin.x;
  float dy = point.y - origin.y;

  return (Vector2){
    dx * cosf(angle) - dy * sinf(angle) + origin.x,
    dx * sinf(angle) + dy * cosf(angle) + origin.y,
  };
}

int main() {
  init_window();

  Triangle tri;
  // tri.a = (Vector2){HALF_WINDOW_WIDTH, 0};
  // tri.b = (Vector2){HALF_WINDOW_WIDTH/2, HALF_WINDOW_HEIGHT+(HALF_WINDOW_HEIGHT/2)};
  // tri.c = (Vector2){HALF_WINDOW_WIDTH+(HALF_WINDOW_WIDTH/2), HALF_WINDOW_HEIGHT+(HALF_WINDOW_HEIGHT/2)};
  Vector2 origin = {HALF_WINDOW_WIDTH, HALF_WINDOW_HEIGHT};

  tri.a = (Vector2){HALF_WINDOW_WIDTH+(HALF_WINDOW_WIDTH/2), HALF_WINDOW_HEIGHT+(HALF_WINDOW_HEIGHT/2)};
  tri.b = (Vector2){HALF_WINDOW_WIDTH/2, HALF_WINDOW_HEIGHT+(HALF_WINDOW_HEIGHT/2)};
  tri.c = (Vector2){HALF_WINDOW_WIDTH, 0};
  float angle = 1;

  Color colors[3] = {RED,GREEN,BLUE};
  while(!WindowShouldClose()) {
    float dt = GetFrameTime();

    // tri.a = rotate_point(tri.a, 1 * dt);
    // tri.b = rotate_point(tri.b, 1 * dt);
    // tri.c = rotate_point(tri.c, 1 * dt);

    // tri.a = rotate_point_with_origin(origin, tri.a, angle * dt);
    // tri.b = rotate_point_with_origin(origin, tri.b, angle * dt);
    // tri.c = rotate_point_with_origin(origin, tri.c, angle * dt);

    BeginDrawing();
    ClearBackground(BLACK);

    draw_triangle_filled(tri, colors);
    // draw_triangle(tri, MAGENTA);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}