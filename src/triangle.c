typedef struct { V3 points[3]; } Triangle;

void draw_triangle(Triangle t, Color color) {
  V3 a = t.points[0];
  V3 b = t.points[1];
  V3 c = t.points[2];
  DrawTriangle((Vector2){a.x, a.y}, (Vector2){b.x, b.y}, (Vector2){c.x, c.y}, color);
}

void draw_triangle_lines(Triangle t, Color color) {
  V3 a = t.points[0];
  V3 b = t.points[1];
  V3 c = t.points[2];
  DrawLine((int)a.x, (int)a.y, (int)b.x, (int)b.y, color);
  DrawLine((int)b.x, (int)b.y, (int)c.x, (int)c.y, color);
  DrawLine((int)c.x, (int)c.y, (int)a.x, (int)a.y, color);
}

int sort_triangles(const void* tri_a, const void* tri_b) {
  Triangle a = *(Triangle*)tri_a;
  Triangle b = *(Triangle*)tri_b;

  float avg_z1 = (a.points[0].z + a.points[1].z + a.points[2].z) / 3;
  float avg_z2 = (b.points[0].z + b.points[1].z + b.points[2].z) / 3;
  // if(avg_z1 > avg_z2) return -1;
  // if(avg_z1 < avg_z2) return 1;
  return 0;
}