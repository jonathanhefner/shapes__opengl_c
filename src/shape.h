#ifndef SHAPE_H
#define SHAPE_H

#include <GL/gl.h>


typedef struct _Vertex {
  GLfloat position[3];
} Vertex;

typedef struct _Surface {
  GLenum type;
  int vertex_count;
  Vertex* vertices;
} Surface;

typedef struct _Shape {
  size_t memsize;
  int surface_count;
  Surface surfaces[1];
  /* rest of surfaces beyond this point */
} Shape;


Shape* shape_define(Shape* existing_shape, int surface_count, .../* int ith_surface_type, int ith_vertex_count */);
void shape_position_vertex(Shape* shape, int surface_id, int vertex_id, float x, float y, float z);
void shape_draw(Shape* shape);


#endif
