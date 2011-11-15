#include <stdlib.h>
#include <stdarg.h>
#include "shape.h"


static void fast_normalize(vec3 v) {
  /* Using fast inverse sqrt algorithm with more accurate magic number.
      See http://en.wikipedia.org/wiki/Fast_inverse_square_root#History_and_investigation */
  float f = (vx(v) * vx(v)) + (vy(v) * vy(v)) + (vz(v) * vz(v));
  float fhalf = 0.5f * f;
  int i = *(int*)&f;            /* floating point to bits */
  i = 0x5f375a86 - (i>>1);      /* newton's method initial guess */
  f = *(float*)&i;              /* bits back to floating point */
  f *= 1.5f - (fhalf * f * f);  /* newton's method first iteration */

  vec3_op_3f(v, *=, f, f, f);
}


static void tri_normal(vec3 dest, vec3 v0, vec3 v1, vec3 v2) {
  /* NOTE we follow the right-hand rule and assume triangle vertices go
      counter-clockwise around the triangle's face */
  const vec3 a = { vx(v1) - vx(v0), vy(v1) - vy(v0), vz(v1) - vz(v0) };
  const vec3 b = { vx(v2) - vx(v0), vy(v2) - vy(v0), vz(v2) - vz(v0) };

  /* normal = a cross b */
  vx(dest) = vy(a) * vz(b) - vz(a) * vy(b);
  vy(dest) = vz(a) * vx(b) - vx(a) * vz(b);
  vz(dest) = vx(a) * vy(b) - vy(a) * vx(b);
}


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

  /* point GL to vertex arrays */
  walker = (Vertex*)&(shape->surfaces[surface_count]);
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(walker->position));
  glNormalPointer(GL_FLOAT, sizeof(Vertex), &(walker->normal));

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


void shape_compute_normals(Shape* shape) {
  int i;
  int j;
  Vertex* verts;

  for (i = 0; i < shape->surface_count; i += 1) {
    if (shape->surfaces[i].vertex_count >= 3) {
      verts = shape->surfaces[i].vertices;

      vec3_op_3f(verts[0].normal, =, 0, 0, 0)
      vec3_op_3f(verts[1].normal, =, 0, 0, 0)

      switch (shape->surfaces[i].type) {
        case GL_TRIANGLE_FAN:
          for (j = 2; j < shape->surfaces[i].vertex_count; j += 1) {
            tri_normal(verts[j].normal,
                        verts[0].position, verts[j - 1].position, verts[j].position);

            vec3_op_vec3(verts[0].normal, +=, verts[j].normal)
            vec3_op_vec3(verts[j - 1].normal, +=, verts[j].normal)

            fast_normalize(verts[j - 1].normal);
          }
          fast_normalize(verts[0].normal);
          fast_normalize(verts[j - 1].normal);
          break;

        case GL_TRIANGLE_STRIP:
          for (j = 2; j < shape->surfaces[i].vertex_count; j += 1) {
              /* NOTE we alternate the order in which the j-th and (j-1)th
                  vertices are passed into tri_normal().  I.E. when j is even,
                  the vertex order is (j-2), (j-1), (j); and when j is odd, the
                  vertex order is (j-2), (j), (j-1).  This is due to the nature
                  of triangle strips, and doing otherwise would cause normals to
                  alternatingly point out from opposite sides of the strip.
                  (To see this, draw a triangle strip on paper and, using the
                  specified vertex order, apply the right-hand rule.) */
              tri_normal(verts[j].normal,
                          verts[j - 2].position, verts[j - (j%2==0)].position, verts[j - (j%2==1)].position);

            vec3_op_vec3(verts[j - 2].normal, +=, verts[j].normal)
            vec3_op_vec3(verts[j - 1].normal, +=, verts[j].normal)

            fast_normalize(verts[j - 2].normal);
          }
          fast_normalize(verts[j - 2].normal);
          fast_normalize(verts[j - 1].normal);
          break;

        default:
          /* NO normals for YOU! */
          break;
      }
    }
  }
}


void shape_draw(Shape* shape) {
  int i;
  int j;

  for (i = 0, j = 0; i < shape->surface_count; j += shape->surfaces[i].vertex_count, i += 1) {
    glDrawArrays(shape->surfaces[i].type, j, shape->surfaces[i].vertex_count);
  }

  /* FOR DEBUGGING: draw vertex normals */
  #ifdef DRAW_NORMALS
  for (i = 0; i < shape->surface_count; i += 1) {
    for (j = 0; j < shape->surfaces[i].vertex_count; j += 1) {
      glColor3f(0, 255, 0);
      glBegin(GL_LINES);
        for (j = 0; j < shape->surfaces[i].vertex_count; j += 1) {
          glVertex3fv(shape->surfaces[i].vertices[j].position);
          glVertex3f(shape->surfaces[i].vertices[j].position[0]
                      + shape->surfaces[i].vertices[j].normal[0],
                    shape->surfaces[i].vertices[j].position[1]
                      + shape->surfaces[i].vertices[j].normal[1],
                    shape->surfaces[i].vertices[j].position[2]
                      + shape->surfaces[i].vertices[j].normal[2]);
        }
      glEnd();
      glColor3f(255, 255, 255);
    }
  }
  #endif
}
