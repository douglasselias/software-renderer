#include "raylib.h"
#include "raymath.h"

#include <stdio.h>
#include <stdlib.h>

#include "src/window.c"

void* mem_alloc(int size) {
  return calloc(size, 1);
}

typedef struct { float x, y, z; } V3;
typedef struct { V3 points[3]; } Triangle;
typedef struct { Triangle* tris; int count; } _Mesh; // Underscore to avoid name collision with Raylib Mesh type
typedef struct { float m[4][4]; } Matrix44;

void multiply_matrix_vector(V3 input, V3* output, Matrix44 matrix) {
  output->x = input.x * matrix.m[0][0] + input.y * matrix.m[1][0] + input.z * matrix.m[2][0] + matrix.m[3][0];
  output->y = input.x * matrix.m[0][1] + input.y * matrix.m[1][1] + input.z * matrix.m[2][1] + matrix.m[3][1];
  output->z = input.x * matrix.m[0][2] + input.y * matrix.m[1][2] + input.z * matrix.m[2][2] + matrix.m[3][2];
  float w   = input.x * matrix.m[0][3] + input.y * matrix.m[1][3] + input.z * matrix.m[2][3] + matrix.m[3][3];

  if(!FloatEquals(w, 0)) {
    output->x /= w;
    output->y /= w;
    output->z /= w;
  }
}

int main() {
  init_window();

  float fov = 90.0f;
  float z_far = 1000.0f;
  float z_near = 0.1f;
  float aspect_ratio = window_height / (float)window_width;
  float fov_rad = 1 / tanf(fov * 0.5f / 180.0f * 3.14159f);

  Matrix44 projection = {0};
  projection.m[0][0] = aspect_ratio * fov_rad;
  projection.m[1][1] = fov_rad;
  projection.m[2][2] = z_far / (z_far - z_near);
  projection.m[3][2] = (-z_far * z_near) / (z_far - z_near);
  projection.m[2][3] = 1;
  projection.m[3][3] = 0;

  float f_theta = 0;

  _Mesh cube = {};
  cube.count = 12;
  cube.tris = mem_alloc(sizeof(Triangle) * cube.count);

  // SOUTH
  cube.tris[0] = (Triangle){ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f };
	cube.tris[1] = (Triangle){ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f };

  // EAST
	cube.tris[2] = (Triangle){ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f };
	cube.tris[3] = (Triangle){ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f };

  // NORTH
	cube.tris[4] = (Triangle){ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f };
	cube.tris[5] = (Triangle){ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f };

  // WEST
	cube.tris[6] = (Triangle){ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f };
	cube.tris[7] = (Triangle){ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f };

  // TOP
	cube.tris[8] = (Triangle){ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f };
	cube.tris[9] = (Triangle){ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f };

  // BOTTOM
	cube.tris[10] = (Triangle){ 1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f };
	cube.tris[11] = (Triangle){ 1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f };

  while(!WindowShouldClose()) {
    float dt = GetFrameTime();

    f_theta += dt;

    Matrix44 rotation_z = {0};
    Matrix44 rotation_x = {0};
  
    rotation_z.m[0][0] = cosf(f_theta);
    rotation_z.m[0][1] = sinf(f_theta);
    rotation_z.m[1][0] = -sinf(f_theta);
    rotation_z.m[1][1] = cosf(f_theta);
    rotation_z.m[2][2] = 1;
    rotation_z.m[3][3] = 1;
  
    rotation_x.m[0][0] = 1;
    rotation_x.m[1][1] = cosf(f_theta * 0.5f);
    rotation_x.m[1][2] = sinf(f_theta * 0.5f);
    rotation_x.m[2][1] = -sinf(f_theta * 0.5f);
    rotation_x.m[2][2] = cosf(f_theta * 0.5f);
    rotation_x.m[3][3] = 1;

    BeginDrawing();
    ClearBackground(BLACK);

    for(int i = 0; i < cube.count; i++) {
      Triangle tri = cube.tris[i];
      Triangle projected_triangle, translated_triangle, tri_rot_z, tri_rot_zx;

      multiply_matrix_vector(tri.points[0], &tri_rot_z.points[0], rotation_z);
      multiply_matrix_vector(tri.points[1], &tri_rot_z.points[1], rotation_z);
      multiply_matrix_vector(tri.points[2], &tri_rot_z.points[2], rotation_z);

      multiply_matrix_vector(tri_rot_z.points[0], &tri_rot_zx.points[0], rotation_x);
      multiply_matrix_vector(tri_rot_z.points[1], &tri_rot_zx.points[1], rotation_x);
      multiply_matrix_vector(tri_rot_z.points[2], &tri_rot_zx.points[2], rotation_x);

      translated_triangle = tri_rot_zx;
      translated_triangle.points[0].z = tri_rot_zx.points[0].z + 3;
      translated_triangle.points[1].z = tri_rot_zx.points[1].z + 3;
      translated_triangle.points[2].z = tri_rot_zx.points[2].z + 3;

      multiply_matrix_vector(translated_triangle.points[0], &projected_triangle.points[0], projection);
      multiply_matrix_vector(translated_triangle.points[1], &projected_triangle.points[1], projection);
      multiply_matrix_vector(translated_triangle.points[2], &projected_triangle.points[2], projection);

      // Scale projection
      projected_triangle.points[0].x += 1; projected_triangle.points[0].y += 1;
      projected_triangle.points[1].x += 1; projected_triangle.points[1].y += 1;
      projected_triangle.points[2].x += 1; projected_triangle.points[2].y += 1;

      projected_triangle.points[0].x *= 0.5f * window_width;
      projected_triangle.points[0].y *= 0.5f * window_height;
      projected_triangle.points[1].x *= 0.5f * window_width;
      projected_triangle.points[1].y *= 0.5f * window_height;
      projected_triangle.points[2].x *= 0.5f * window_width;
      projected_triangle.points[2].y *= 0.5f * window_height;

      V3 a = projected_triangle.points[0];
      V3 b = projected_triangle.points[1];
      V3 c = projected_triangle.points[2];

      DrawLine((int)a.x, (int)a.y, (int)b.x, (int)b.y, RED);
      DrawLine((int)b.x, (int)b.y, (int)c.x, (int)c.y, RED);
      DrawLine((int)c.x, (int)c.y, (int)a.x, (int)a.y, RED);
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}