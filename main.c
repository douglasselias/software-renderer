#include "raylib.h"
#include "raymath.h"

#include <stdio.h>
#include <string.h>

#include "src/window.c"
#include "src/memory.c"
#include "src/v3.c"
#include "src/matrix.c"
#include "src/triangle.c"
#include "src/mesh.c"

int main() {
  init_window();

  float fov = 90.0f;
  float z_far = 1000.0f;
  float z_near = 0.1f;
  float aspect_ratio = window_height / (float)window_width;
  Matrix44 projection = create_projection_matrix(fov, aspect_ratio, z_near, z_far);

  V3 camera = {0};
  V3 look_dir = {0};

  _Mesh ship = load_obj("../assets/axis.obj");

  Triangle* triangles_to_raster = mem_alloc(sizeof(Triangle) * ship.count);
  int raster_index = 0;
  Color* colors_to_raster = mem_alloc(sizeof(Color) * ship.count);

  float f_theta = 0;

  bool lines_enabled = false;

  while(!WindowShouldClose()) {
    float dt = GetFrameTime();

    if(IsKeyPressed(KEY_L)) lines_enabled = !lines_enabled;

    Matrix44 rotation_z = create_rotation_z_matrix(f_theta);
    Matrix44 rotation_x = create_rotation_x_matrix(f_theta);
    Matrix44 translation_matrix = create_translation_matrix(0, 0, 0.5f);
    Matrix44 world_matrix = matrix_identity(); // ?
    world_matrix = matrix_mul(rotation_z, rotation_x);
    world_matrix = matrix_mul(world_matrix, translation_matrix);

    V3 up = {0,1,0};
    look_dir = (V3){0,0,1};
    V3 target = vector_add(camera, look_dir);

    Matrix44 camera_matrix = matrix_point_at(camera, target, up);
    Matrix44 view_matrix = matrix_quick_inverse(camera_matrix);

    BeginDrawing();
    ClearBackground(BLACK);

    for(int i = 0; i < ship.count; i++) {
      Triangle tri = ship.tris[i];
      Triangle projected_triangle, translated_triangle, tri_rot_z, tri_rot_zx;

      multiply_matrix_vector(tri.points[0], &tri_rot_z.points[0], rotation_z);
      multiply_matrix_vector(tri.points[1], &tri_rot_z.points[1], rotation_z);
      multiply_matrix_vector(tri.points[2], &tri_rot_z.points[2], rotation_z);

      multiply_matrix_vector(tri_rot_z.points[0], &tri_rot_zx.points[0], rotation_x);
      multiply_matrix_vector(tri_rot_z.points[1], &tri_rot_zx.points[1], rotation_x);
      multiply_matrix_vector(tri_rot_z.points[2], &tri_rot_zx.points[2], rotation_x);

      translated_triangle = tri_rot_zx;
      translated_triangle.points[0].z = tri_rot_zx.points[0].z + 8;
      translated_triangle.points[1].z = tri_rot_zx.points[1].z + 8;
      translated_triangle.points[2].z = tri_rot_zx.points[2].z + 8;

      V3 line_a = vector_sub(translated_triangle.points[1], translated_triangle.points[0]);
      V3 line_b = vector_sub(translated_triangle.points[2], translated_triangle.points[0]);

      V3 normal = vector_cross(line_a, line_b);

      float length = vector_length(normal);
      normal = vector_div_scalar(normal, length);

      V3 offset_point = vector_sub(translated_triangle.points[0], camera);
      float dot = vector_dot(normal, offset_point);

      if(dot < 0) {
        V3 light_direction = {0, 0, -1};
        float len = vector_length(light_direction);
        light_direction = vector_div_scalar(light_direction, len);

        float dot_light = vector_dot(normal, light_direction);

        Color shaded_color = Fade(RED, dot_light);

        // Translate
        multiply_matrix_vector(translated_triangle.points[0], &projected_triangle.points[0], projection);
        multiply_matrix_vector(translated_triangle.points[1], &projected_triangle.points[1], projection);
        multiply_matrix_vector(translated_triangle.points[2], &projected_triangle.points[2], projection);

        // Scale
        V3 scale = {1, 1, 0};
        projected_triangle.points[0] = vector_add(projected_triangle.points[0], scale);
        projected_triangle.points[1] = vector_add(projected_triangle.points[1], scale);
        projected_triangle.points[2] = vector_add(projected_triangle.points[2], scale);

        V3 window_normalize = {0.5f * window_width, 0.5f * window_height, 1};
        projected_triangle.points[0] = vector_mul(projected_triangle.points[0], window_normalize);
        projected_triangle.points[1] = vector_mul(projected_triangle.points[1], window_normalize);
        projected_triangle.points[2] = vector_mul(projected_triangle.points[2], window_normalize);

        triangles_to_raster[raster_index] = projected_triangle;
        colors_to_raster[raster_index] = shaded_color;
        raster_index++;
      }
    }

    qsort(triangles_to_raster, raster_index, sizeof(Triangle), sort_triangles);

    for(int i = 0; i < raster_index; i++) {
      draw_triangle(triangles_to_raster[i], colors_to_raster[i]);
      if(lines_enabled) draw_triangle_lines(triangles_to_raster[i], WHITE);
    }

    mem_reset(triangles_to_raster, sizeof(Triangle) * ship.count);
    mem_reset(colors_to_raster,    sizeof(Color)    * ship.count);
    raster_index = 0;

    EndDrawing();
  }

  CloseWindow();
  return 0;
}