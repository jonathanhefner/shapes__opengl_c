#include <stdlib.h>
#include <stdarg.h>
#include "shape.h"


Shape* shape_define(Shape* existing_shape, int surface_count, .../* int ith_surface_type, int ith_vertex_count */) {
  va_list surface_args;
  Shape* shape;
  Vertex* walker;
  int memsize;
  int i;

  /* compute full size with each surface's vertices */
  memsize = sizeof(Shape) + (sizeof(Surface) * (surface_count - 1));
  va_start(surface_args, surface_count);
  for (i = 0; i < surface_count; i += 1) {
    va_arg(surface_args, int); /* skip surface_type for now */
    memsize += va_arg(surface_args, int)/*vertex_count*/ * sizeof(Vertex);
  }
  va_end(surface_args);

  /* allocate space as necessary */
  if (existing_shape == NULL || existing_shape->memsize < memsize) {
    shape = (existing_shape == NULL) ? malloc(memsize) : realloc(existing_shape, memsize);
    /* TODO error on NULL */
    shape->memsize = memsize;
  } else {
    shape = existing_shape;
  }

  /* initialize structs */
  shape->surface_count = surface_count;
  walker = (Vertex*)&(shape->surfaces[surface_count]); /* start vertices after surfaces */
  va_start(surface_args, surface_count);
  for (i = 0; i < surface_count; i += 1) {
    shape->surfaces[i].type = va_arg(surface_args, int);
    shape->surfaces[i].vertex_count = va_arg(surface_args, int);
    shape->surfaces[i].vertices = walker;
    walker += shape->surfaces[i].vertex_count;
  }
  va_end(surface_args);

  return shape;
}


void shape_position_vertex(Shape* shape, int surface_id, int vertex_id, float x, float y, float z) {
  /* TODO remove this check? */
  if (surface_id < shape->surface_count
      && vertex_id < shape->surfaces[surface_id].vertex_count) {
    shape->surfaces[surface_id].vertices[vertex_id].position[0] = x;
    shape->surfaces[surface_id].vertices[vertex_id].position[1] = y;
    shape->surfaces[surface_id].vertices[vertex_id].position[2] = z;
  }
}


void shape_draw(Shape* shape) {
  int i;
  int j;

  for (i = 0; i < shape->surface_count; i += 1) {
    glBegin(shape->surfaces[i].type);
      for (j = 0; j < shape->surfaces[i].vertex_count; j += 1) {
        glVertex3fv(shape->surfaces[i].vertices[j].position);
      }
    glEnd();
  }
}
