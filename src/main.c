#include <GL/glfw.h>
#include <math.h>
#include <stdlib.h>
#include "shape.h"

#ifndef PI
  #define PI 3.14159
#endif


typedef enum _ShapeType {
  SHAPE_PYRAMID,
  SHAPE_PRISM
} ShapeType;

#define MAX_SHAPE_TYPE SHAPE_PRISM
#define MAX_CARDINALITY 42


static Shape* shape = NULL;


/***** Settings *****/
static int wireframe = 0;
static ShapeType shape_type = SHAPE_PYRAMID;
static int shape_cardinality = 3;
/* coords around a unit polygon (a unit "circle" with shape_cardinality faces, instead of infinite) */
static float unit_x[MAX_CARDINALITY + 1];
static float unit_y[MAX_CARDINALITY + 1];



static void set_wireframe(int enable) {
  glPolygonMode(GL_FRONT_AND_BACK, enable ? GL_LINE : GL_FILL);
  wireframe = enable;
}


void shape_pyramid() {
  const int TOP = 0; const int BTM = 1;
  int i;

  shape = shape_define(shape, 3, /*TOP*/ GL_TRIANGLE_FAN, (shape_cardinality + 1) + 1,
                                 /*BTM*/ GL_TRIANGLE_FAN, (shape_cardinality + 1) + 1);

  shape_position_vertex(shape, TOP, 0, 0.0, 1.0, 0.0);
  shape_position_vertex(shape, BTM, 0, 0.0, -1.0, 0.0);
  for (i = 0; i < shape_cardinality + 1; i += 1) {
    shape_position_vertex(shape, TOP, i + 1, unit_x[i], -1.0, unit_y[i]);
    shape_position_vertex(shape, BTM, i + 1, unit_x[i], -1.0, unit_y[i]);
  }
}


void shape_prism() {
  const int TOP = 0; const int BTM = 1; const int SIDES = 2;
  int i;

  shape = shape_define(shape, 3, /*TOP*/ GL_TRIANGLE_FAN, (shape_cardinality + 1) + 1,
                                 /*BTM*/ GL_TRIANGLE_FAN, (shape_cardinality + 1) + 1,
                                 /*SIDES*/ GL_TRIANGLE_STRIP, (shape_cardinality + 1) * 2);

  shape_position_vertex(shape, TOP, 0, 0.0, 1.0, 0.0);
  shape_position_vertex(shape, BTM, 0, 0.0, -1.0, 0.0);
  for (i = 0; i < shape_cardinality + 1; i += 1) {
    shape_position_vertex(shape, TOP, i + 1, unit_x[i], 1.0, unit_y[i]);
    shape_position_vertex(shape, BTM, i + 1, unit_x[i], -1.0, unit_y[i]);
    shape_position_vertex(shape, SIDES, i * 2, unit_x[i], 1.0, unit_y[i]);
    shape_position_vertex(shape, SIDES, (i * 2) + 1, unit_x[i], -1.0, unit_y[i]);
  }
}


static void set_shape(ShapeType type, int cardinality) {
  int i;

  if (type >= 0 && type <= MAX_SHAPE_TYPE
      && cardinality >= 3 && cardinality <= MAX_CARDINALITY) {
    for (i = 0; i < cardinality; i += 1) {
      unit_x[i] = sinf((2.0f * PI * i) / cardinality);
      unit_y[i] = cosf((2.0f * PI * i) / cardinality);
    }

    /* add the first coord as the final to complete the loop */
    unit_x[cardinality] = unit_x[0];
    unit_y[cardinality] = unit_y[0];

    shape_cardinality = cardinality;

    switch (type) {
      case SHAPE_PYRAMID:
        shape_pyramid();
        break;
      case SHAPE_PRISM:
        shape_prism();
        break;
    }

    shape_type = type;
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
  static float rotate_x = 0;
  static float rotate_y = 0;

  glLoadIdentity();
  gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);

  rotate_x += .1 * 360 * delta_time;
  glRotatef(rotate_x, 1, 0, 0);
  rotate_y += .2 * 360 * delta_time;
  glRotatef(rotate_y, 0, 1, 0);
  shape_draw(shape);
}


static void window_reshape(int width, int height) {
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, (GLdouble)width/(GLdouble)height, 1, 10);
  glMatrixMode(GL_MODELVIEW);
}


int main() {
  double prev_time;
  double time;

  if(!glfwInit() || !glfwOpenWindow(640, 480, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetWindowTitle("Shapes Demo");
  glfwSetWindowSizeCallback(window_reshape);
  window_reshape(640, 480);

  glfwEnable(GLFW_KEY_REPEAT);
  glfwSetKeyCallback(handle_keyboard);
  
  /* draw while the Esc key hasn't been pressed and the window is open */
  glMatrixMode(GL_MODELVIEW);
  set_shape(shape_type, shape_cardinality);
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
