typedef struct { float m[4][4]; } Matrix44;

Matrix44 matrix_identity() {
  Matrix44 matrix = {0};
  matrix.m[0][0] = 1;
  matrix.m[1][1] = 1;
  matrix.m[2][2] = 1;
  matrix.m[3][3] = 1;
  return matrix;
}

Matrix44 matrix_point_at(V3 position, V3 target, V3 up) {
  V3 new_forward = vector_sub(target, position);
  new_forward = vector_normalize(new_forward);

  V3 a = vector_mul_scalar(new_forward, vector_dot(up, new_forward));
  V3 new_up = vector_sub(up, a);
  new_up = vector_normalize(new_up);

  V3 new_right = vector_cross(new_up, new_forward);

  Matrix44 matrix;
  matrix.m[0][0] = new_right.x;	matrix.m[0][1] = new_right.y;	matrix.m[0][2] = new_right.z;	matrix.m[0][3] = 0.0f;
  matrix.m[1][0] = new_up.x;		matrix.m[1][1] = new_up.y;		matrix.m[1][2] = new_up.z;		matrix.m[1][3] = 0.0f;
  matrix.m[2][0] = new_forward.x;	matrix.m[2][1] = new_forward.y;	matrix.m[2][2] = new_forward.z;	matrix.m[2][3] = 0.0f;
  matrix.m[3][0] = position.x;			matrix.m[3][1] = position.y;			matrix.m[3][2] = position.z;			matrix.m[3][3] = 1.0f;
  return matrix;
}

// Only for Rotation/Translation Matrices
Matrix44 matrix_quick_inverse(Matrix44 m) {
  Matrix44 matrix;
  matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
  matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
  matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
  matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
  matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
  matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
  matrix.m[3][3] = 1.0f;
  return matrix;
}

void multiply_matrix_vector(V3 input, V3* output, Matrix44 matrix) {
  output->x = input.x * matrix.m[0][0] + input.y * matrix.m[1][0] + input.z * matrix.m[2][0] + matrix.m[3][0];
  output->y = input.x * matrix.m[0][1] + input.y * matrix.m[1][1] + input.z * matrix.m[2][1] + matrix.m[3][1];
  output->z = input.x * matrix.m[0][2] + input.y * matrix.m[1][2] + input.z * matrix.m[2][2] + matrix.m[3][2];
  float w   = input.x * matrix.m[0][3] + input.y * matrix.m[1][3] + input.z * matrix.m[2][3] + matrix.m[3][3];

  if(!FloatEquals(w, 0)) {
    *output = vector_div_scalar(*output, w);
  }
}

Matrix44 create_projection_matrix(float fov, float aspect_ratio, float z_near, float z_far) {
  float fov_rad = 1 / tanf(fov * 0.5f / 180.0f * 3.14159f);

  Matrix44 projection = {0};
  projection.m[0][0] = aspect_ratio * fov_rad;
  projection.m[1][1] = fov_rad;
  projection.m[2][2] = z_far / (z_far - z_near);
  projection.m[3][2] = (-z_far * z_near) / (z_far - z_near);
  projection.m[2][3] = 1;
  projection.m[3][3] = 0;
  return projection;
}

Matrix44 create_rotation_x_matrix(float angle) {
  Matrix44 rotation = {0};
  rotation.m[0][0] = 1;
  rotation.m[1][1] = cosf(angle * 0.5f);
  rotation.m[1][2] = sinf(angle * 0.5f);
  rotation.m[2][1] = -sinf(angle * 0.5f);
  rotation.m[2][2] = cosf(angle * 0.5f);
  rotation.m[3][3] = 1;
  return rotation;
}

Matrix44 create_rotation_y_matrix(float angle) {
  Matrix44 rotation = {0};
  rotation.m[0][0] = cosf(angle);
  rotation.m[0][2] = sinf(angle);
  rotation.m[2][0] = -sinf(angle);
  rotation.m[1][1] = 1.0f;
  rotation.m[2][2] = cosf(angle);
  rotation.m[3][3] = 1.0f;
  return rotation;
}

Matrix44 create_rotation_z_matrix(float angle) {
  Matrix44 rotation = {0};
  rotation.m[0][0] = cosf(angle);
  rotation.m[0][1] = sinf(angle);
  rotation.m[1][0] = -sinf(angle);
  rotation.m[1][1] = cosf(angle);
  rotation.m[2][2] = 1.0f;
  rotation.m[3][3] = 1.0f;
  return rotation;
}

Matrix44 create_translation_matrix(float x, float y, float z) {
  Matrix44 translation = {0};
  translation.m[0][0] = 1.0f;
  translation.m[1][1] = 1.0f;
  translation.m[2][2] = 1.0f;
  translation.m[3][3] = 1.0f;
  translation.m[3][0] = x;
  translation.m[3][1] = y;
  translation.m[3][2] = z;
  return translation;
}

Matrix44 matrix_mul(Matrix44 m1, Matrix44 m2)
	{
		Matrix44 result = {0};
		for (int c = 0; c < 4; c++)
			for (int r = 0; r < 4; r++)
				result.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
		return result;
	}