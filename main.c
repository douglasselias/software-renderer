#include "raylib.h"
#include "raymath.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

_Mesh load_obj(char* filename) {
  char* obj_text = LoadFileText(filename);
  char* copy_obj_text = strdup(obj_text);

  int total_vertices = 0, total_faces = 0;

  {
    char* line = strtok(obj_text, "\n");
    while(line != NULL) {
      if(line[0] == 'v') total_vertices++;
      if(line[0] == 'f') total_faces++;
      line = strtok(NULL, "\n");
    }
  }

  V3* vertices    = mem_alloc(sizeof(V3)       * total_vertices);
  Triangle* faces = mem_alloc(sizeof(Triangle) * total_faces);
  int vi = 0, fi = 0;

  char* line = strtok(copy_obj_text, "\n");
  while(line != NULL) {
    if(line[0] == 'v') { 
      char junk;
      sscanf(line, "%c %f %f %f", &junk, &vertices[vi].x, &vertices[vi].y, &vertices[vi].z);
      vi++;
    }

    if(line[0] == 'f') {
      char junk;
      int x, y, z;
      sscanf(line, "%c %d %d %d", &junk, &x, &y, &z);
      faces[fi].points[0] = vertices[x-1];
      faces[fi].points[1] = vertices[y-1];
      faces[fi].points[2] = vertices[z-1];
      fi++;
    }

    line = strtok(NULL, "\n");
  }

  _Mesh mesh = {.count = total_faces, .tris = faces};
  return mesh;
}

int sort_triangles(const void* tri_a, const void* tri_b) {
  Triangle a = *(Triangle*)tri_a;
  Triangle b = *(Triangle*)tri_b;

  float avg_z1 = (a.points[0].z + a.points[1].z + a.points[2].z) / 3;
  float avg_z2 = (b.points[0].z + b.points[1].z + b.points[2].z) / 3;
  // return (avg_z2 - avg_z1) > 0 ? 1 : -1;  // Sort farthest to nearest
  // if(avg_z1 > avg_z2) return -1;
  // if(avg_z1 < avg_z2) return 1;

  // float min_z1 = fminf(fminf(a.points[0].z, a.points[1].z), a.points[2].z);
  // float min_z2 = fminf(fminf(b.points[0].z, b.points[1].z), b.points[2].z);
  // if (min_z1 > min_z2) return -1;
  // if (min_z1 < min_z2) return 1;

  return 0;
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

  V3 camera = {0};

  _Mesh ship = load_obj("../assets/ship.obj");

  _Mesh cube = {};
  cube.count = 12;
  cube.tris = mem_alloc(sizeof(Triangle) * cube.count);
  {
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
  }

  Triangle* triangles_to_raster = mem_alloc(sizeof(Triangle) * ship.count);
  int raster_index = 0;
  Color* colors_to_raster = mem_alloc(sizeof(Color) * ship.count);

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

      V3 normal, line_a, line_b;
      line_a.x = translated_triangle.points[1].x - translated_triangle.points[0].x;
      line_a.y = translated_triangle.points[1].y - translated_triangle.points[0].y;
      line_a.z = translated_triangle.points[1].z - translated_triangle.points[0].z;

      line_b.x = translated_triangle.points[2].x - translated_triangle.points[0].x;
      line_b.y = translated_triangle.points[2].y - translated_triangle.points[0].y;
      line_b.z = translated_triangle.points[2].z - translated_triangle.points[0].z;

      normal.x = line_a.y * line_b.z - line_a.z * line_b.y;
      normal.y = line_a.z * line_b.x - line_a.x * line_b.z;
      normal.z = line_a.x * line_b.y - line_a.y * line_b.x;

      float length = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
      normal.x /= length;
      normal.y /= length;
      normal.z /= length;

      float dot = normal.x * (translated_triangle.points[0].x - camera.x) +
                  normal.y * (translated_triangle.points[0].y - camera.y) +
                  normal.z * (translated_triangle.points[0].z - camera.z);

      if(dot < 0) {
        V3 light_direction = {0, 0, -1};
        float len = sqrtf(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
        light_direction.x /= len;
        light_direction.y /= len;
        light_direction.z /= len;

        float dot_light = normal.x * light_direction.x +
                          normal.y * light_direction.y +
                          normal.z * light_direction.z;

        Color shaded_color = Fade(RED, dot_light);

        // Translate
        multiply_matrix_vector(translated_triangle.points[0], &projected_triangle.points[0], projection);
        multiply_matrix_vector(translated_triangle.points[1], &projected_triangle.points[1], projection);
        multiply_matrix_vector(translated_triangle.points[2], &projected_triangle.points[2], projection);

        // Scale
        projected_triangle.points[0].x += 1; projected_triangle.points[0].y += 1;
        projected_triangle.points[1].x += 1; projected_triangle.points[1].y += 1;
        projected_triangle.points[2].x += 1; projected_triangle.points[2].y += 1;

        projected_triangle.points[0].x *= 0.5f * window_width;
        projected_triangle.points[0].y *= 0.5f * window_height;
        projected_triangle.points[1].x *= 0.5f * window_width;
        projected_triangle.points[1].y *= 0.5f * window_height;
        projected_triangle.points[2].x *= 0.5f * window_width;
        projected_triangle.points[2].y *= 0.5f * window_height;

        triangles_to_raster[raster_index] = projected_triangle;
        colors_to_raster[raster_index] = shaded_color;
        raster_index++;

        // V3 a = projected_triangle.points[0];
        // V3 b = projected_triangle.points[1];
        // V3 c = projected_triangle.points[2];

        // DrawTriangle((Vector2){a.x, a.y}, (Vector2){b.x, b.y}, (Vector2){c.x, c.y}, shaded_color);

        // DrawLine((int)a.x, (int)a.y, (int)b.x, (int)b.y, WHITE);
        // DrawLine((int)b.x, (int)b.y, (int)c.x, (int)c.y, WHITE);
        // DrawLine((int)c.x, (int)c.y, (int)a.x, (int)a.y, WHITE);
      }
    }

    qsort(triangles_to_raster, raster_index, sizeof(Triangle), sort_triangles);

    for(int i = 0; i < raster_index; i++) {
      V3 a = triangles_to_raster[i].points[0];
      V3 b = triangles_to_raster[i].points[1];
      V3 c = triangles_to_raster[i].points[2];
      DrawTriangle((Vector2){a.x, a.y}, (Vector2){b.x, b.y}, (Vector2){c.x, c.y}, colors_to_raster[i]);
    }

    memset(triangles_to_raster, 0, sizeof(Triangle) * ship.count);
    memset(colors_to_raster,    0, sizeof(Color)    * ship.count);
    raster_index = 0;

    EndDrawing();
  }

  CloseWindow();
  return 0;
}