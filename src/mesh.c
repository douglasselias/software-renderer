typedef struct { Triangle* tris; int count; } _Mesh; // Underscore to avoid name collision with Raylib Mesh type

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