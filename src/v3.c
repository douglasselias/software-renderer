typedef struct { float x, y, z; } V3;

V3 vector_add(V3 a, V3 b) {
  return (V3){a.x + b.x, a.y + b.y, a.z + b.z};
}

V3 vector_sub(V3 a, V3 b) {
  return (V3){a.x - b.x, a.y - b.y, a.z - b.z};
}

V3 vector_mul(V3 a, V3 b) {
  return (V3){a.x * b.x, a.y * b.y, a.z * b.z};
}

V3 vector_mul_scalar(V3 v, float scalar) {
  return (V3){v.x * scalar, v.y * scalar, v.z * scalar};
}

V3 vector_div_scalar(V3 v, float scalar) {
  return (V3){v.x / scalar, v.y / scalar, v.z / scalar};
}

float vector_length(V3 v) {
  float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
  return length;
}

V3 vector_normalize(V3 v) {
  float length = vector_length(v);
  return vector_div_scalar(v, length);
}

V3 vector_cross(V3 a, V3 b) {
  V3 normal;
  normal.x = a.y * b.z - a.z * b.y;
  normal.y = a.z * b.x - a.x * b.z;
  normal.z = a.x * b.y - a.y * b.x;
  return normal;
}

float vector_dot(V3 a, V3 b) {
  float dot = a.x * b.x + a.y * b.y + a.z * b.z;
  return dot;
}