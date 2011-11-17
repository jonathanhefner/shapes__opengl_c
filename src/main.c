#include <GL/glfw.h>
#include <math.h>
#include <stdlib.h>
#include "shape.h"

#ifndef PI
  #define PI 3.14159
#endif


#define LIGHT_COUNT 3

const GLenum LIGHT_ENUM[LIGHT_COUNT] = {
  GL_LIGHT0, GL_LIGHT1, GL_LIGHT2
};

const GLfloat LIGHT_COLOR[LIGHT_COUNT][4] = {
  { 1.5, 0, 0, 1 },
  { 0, 1.5, 0, 1 },
  { 0, 0, 1.5, 1 }
};


/* computes coords of Ith point on a unit "circle" with N faces */
#define UNIT_X(FACE_I, FACE_N) (cosf((2.0f * PI * (FACE_I)) / (FACE_N)))
#define UNIT_Y(FACE_I, FACE_N) (sinf((2.0f * PI * (FACE_I)) / (FACE_N)))


typedef enum _ShapeType {
  SHAPE_PYRAMID,
  SHAPE_PRISM,
  SHAPE_ANTIPRISM
} ShapeType;

#define MAX_SHAPE_TYPE SHAPE_ANTIPRISM
#define MAX_CARDINALITY 42



static Shape* shape = NULL;

/***** Settings *****/
static int wireframe = 0;
static ShapeType shape_type = SHAPE_PYRAMID;
static int shape_cardinality = 3;



static void set_wireframe(int enable) {
  if (enable) {
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glEnable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_FILL);
  }

  wireframe = enable;
}


void shape_pyramid(int n) {
  const int TOP = 0; const int BTM = 1;
  int i;

  shape = shape_define(shape, 2, /*TOP*/ GL_TRIANGLE_FAN, (n + 1) + 1,
                                 /*BTM*/ GL_TRIANGLE_FAN, (n + 1) + 1);

  shape_position_vertex(shape, TOP, 0, 0.0, 1.0, 0.0);
  shape_position_vertex(shape, BTM, 0, 0.0, -1.0, 0.0);
  for (i = 0; i < n + 1; i += 1) {
    /* NOTE UNIT_X and UNIT_Y go counter-clockwise around the unit circle, BUT
        we are using UNIT_Y as our Z value since we're describing the top and
        bottom faces, existing (mostly) in the XZ plane.  The positive Z-axis
        comes out towards our camera; thus, when looking upon the top face from
        outside the shape, following UNIT_X, UNIT_Y in-order takes you clockwise
        around the top.  Hence, for the top, we negate UNIT_Y. */
    shape_position_vertex(shape, BTM, i + 1, UNIT_X(i, n), -1.0, UNIT_Y(i, n));
    shape_position_vertex(shape, TOP, i + 1, UNIT_X(i, n), -1.0, -UNIT_Y(i, n));
  }
}


void shape_prism(int n) {
  const int TOP = 0; const int BTM = 1; const int SIDES = 2;
  int i;

  shape = shape_define(shape, 3, /*TOP*/ GL_TRIANGLE_FAN, (n + 1) + 1,
                                 /*BTM*/ GL_TRIANGLE_FAN, (n + 1) + 1,
                                 /*SIDES*/ GL_TRIANGLE_STRIP, (n + 1) * 2);

  shape_position_vertex(shape, TOP, 0, 0.0, 1.0, 0.0);
  shape_position_vertex(shape, BTM, 0, 0.0, -1.0, 0.0);
  for (i = 0; i < n + 1; i += 1) {
    shape_position_vertex(shape, BTM, i + 1, UNIT_X(i, n), -1.0, UNIT_Y(i, n));
    shape_position_vertex(shape, SIDES, i * 2, UNIT_X(i, n), -1.0, UNIT_Y(i, n));

    /* see note in shape_pyramid() for why UNIT_Y is negated for TOP */
    shape_position_vertex(shape, TOP, i + 1, UNIT_X(i, n), 1.0, -UNIT_Y(i, n));
    shape_position_vertex(shape, SIDES, (i * 2) + 1, UNIT_X(i, n), 1.0, UNIT_Y(i, n));
  }
}


static void shape_antiprism(int n) {
  const int TOP = 0; const int BTM = 1; const int SIDES = 2;
  int i;

  shape = shape_define(shape, 3, /*TOP*/ GL_TRIANGLE_FAN, (n + 1) + 1,
                                 /*BTM*/ GL_TRIANGLE_FAN, (n + 1) + 1,
                                 /*SIDES*/ GL_TRIANGLE_STRIP, (n + 1) * 2);

  shape_position_vertex(shape, TOP, 0, 0.0, 1.0, 0.0);
  shape_position_vertex(shape, BTM, 0, 0.0, -1.0, 0.0);
  for (i = 0; i < n + 1; i += 1) {
    shape_position_vertex(shape, BTM, i + 1, UNIT_X(i * 2, n * 2), -1.0, UNIT_Y(i * 2, n * 2));
    shape_position_vertex(shape, SIDES, i * 2, UNIT_X(i * 2, n * 2), -1.0, UNIT_Y(i * 2, n * 2));

    /* see note in shape_pyramid() for why UNIT_Y is negated for TOP */
    shape_position_vertex(shape, TOP, i + 1, UNIT_X((i * 2) + 1, n * 2), 1.0, -UNIT_Y((i * 2) + 1, n * 2));
    shape_position_vertex(shape, SIDES, (i * 2) + 1, UNIT_X((i * 2) + 1, n * 2), 1.0, UNIT_Y((i * 2) + 1, n * 2));
  }
}


static void set_shape(ShapeType type, int cardinality) {
  if (type >= 0 && type <= MAX_SHAPE_TYPE
      && cardinality >= 3 && cardinality <= MAX_CARDINALITY) {
    switch (type) {
      case SHAPE_PYRAMID:
        shape_pyramid(cardinality);
        break;
      case SHAPE_PRISM:
        shape_prism(cardinality);
        break;
      case SHAPE_ANTIPRISM:
        shape_antiprism(cardinality);
        break;
    }
    shape_compute_normals(shape);

    shape_type = type;
    shape_cardinality = cardinality;
  }
}


static void handle_keyboard(int key, int action) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case 'W':
        set_wireframe(!wireframe);
        break;

      case GLFW_KEY_UP:
        set_shape(shape_type, shape_cardinality + 1);
        break;

      case GLFW_KEY_DOWN:
        set_shape(shape_type, shape_cardinality - 1);
        break;

      case GLFW_KEY_LEFT:
        set_shape(shape_type - 1, shape_cardinality);
        break;

      case GLFW_KEY_RIGHT:
        set_shape(shape_type + 1, shape_cardinality);
        break;
    }
  }
}


static void draw(double delta_time) {
  const GLfloat light_position[] = { 0, 1, 0, 0 };
  const GLfloat light_tilt[LIGHT_COUNT] = { 45, -45, 180 };
  static GLfloat light_rotate[LIGHT_COUNT] = { 45, 90, 0 };
  static float rotate_x = 0;
  static float rotate_y = 0;
  int i;

  glLoadIdentity();
  gluLookAt(0, 1, 5, 0, 0, 0, 0, 1, 0);

  if (!wireframe) {
    for (i = 0; i < LIGHT_COUNT; i += 1) {
      glPushMatrix();
      glRotatef(light_tilt[i], 0, 0, 1);
      light_rotate[i] += .8 * 360 * delta_time;
      if (light_rotate[i] > 360) {
        light_rotate[i] -= 360;
      }
      glRotatef(light_rotate[i], 1, 0, 0);
      glLightfv(LIGHT_ENUM[i], GL_POSITION, light_position);
      /* glBegin(GL_POINTS);
        glVertex3fv(light_position);
      glEnd(); */
      glPopMatrix();
    }
  }

  rotate_x += .1 * 360 * delta_time;
  if (rotate_x > 360) {
    rotate_x -= 360;
  }
  glRotatef(rotate_x, 1, 0, 0);

  rotate_y += .2 * 360 * delta_time;
  if (rotate_y > 360) {
    rotate_y -= 360;
  }
  glRotatef(rotate_y, 0, 1, 0);

  shape_draw(shape);
}


static void handle_reshape(int width, int height) {
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, (GLdouble)width/(GLdouble)height, 1, 10);
  glMatrixMode(GL_MODELVIEW);
}


int main() {
  double prev_time;
  double time;
  int i;

  if(!glfwInit() || !glfwOpenWindow(640, 480, 0, 0, 0, 0, 8, 0, GLFW_WINDOW)) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  /* window setup */
  glfwSetWindowTitle("Shapes Demo");
  glfwSetWindowSizeCallback(handle_reshape);
  handle_reshape(640, 480);

  /* input setup */
  glfwEnable(GLFW_KEY_REPEAT);
  glfwSetKeyCallback(handle_keyboard);

  /* rendering setup */
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POINT_SMOOTH);
  glPointSize(10);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  for (i = 0; i < LIGHT_COUNT; i += 1) {
    glEnable(LIGHT_ENUM[i]);
    glLightfv(LIGHT_ENUM[i], GL_DIFFUSE, LIGHT_COLOR[i]);
    glLightfv(LIGHT_ENUM[i], GL_SPECULAR, LIGHT_COLOR[i]);
  }
  set_wireframe(0);
  set_shape(shape_type, shape_cardinality);

  /* draw while the Esc key hasn't been pressed and the window is open */
  glMatrixMode(GL_MODELVIEW);
  prev_time = glfwGetTime();
  while(!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED)) {
    time = glfwGetTime();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw(time - prev_time);
    glfwSwapBuffers();

    prev_time = time;
  }

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
