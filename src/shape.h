#ifndef SHAPE_H
#define SHAPE_H

#include <GL/gl.h>


typedef GLfloat vec3[3];

#define vx(V) ((V)[0])
#define vy(V) ((V)[1])
#define vz(V) ((V)[2])

#define vec3_op_vec3(V1, OP, V2)  \
  do {                            \
    vx(V1) OP vx(V2);             \
    vy(V1) OP vy(V2);             \
    vz(V1) OP vz(V2);             \
  } while (0);

#define vec3_op_3f(V, OP, F1, F2, F3) \
  do {                                \
    vx(V) OP (F1);                    \
    vy(V) OP (F2);                    \
    vz(V) OP (F3);                    \
  } while (0);


typedef struct _Vertex {
  vec3 position;
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
