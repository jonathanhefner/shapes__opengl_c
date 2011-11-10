#include <GL/glfw.h>
#include <math.h>
#include <stdlib.h>
#include "shape.h"

#ifndef PI
  #define PI 3.14159
#endif

#define MAX_CARDINALITY 42


static Shape* shape = NULL;


/***** Settings *****/
static int wireframe = 0;
static int cardinality = 3;
/* coords around a unit polygon (a unit "circle" with cardinality faces, instead of infinite) */
static float unit_x[MAX_CARDINALITY + 1];
static float unit_y[MAX_CARDINALITY + 1];



static void set_wireframe(int enable) {
  glPolygonMode(GL_FRONT_AND_BACK, enable ? GL_LINE : GL_FILL);
  wireframe = enable;
}


void shape_pyramid() {
  const int TOP = 0; const int BTM = 1;
  int i;

  shape = shape_define(shape, 3, /*TOP*/ GL_TRIANGLE_FAN, (cardinality + 1) + 1,
                                 /*BTM*/ GL_TRIANGLE_FAN, (cardinality + 1) + 1);

  shape_position_vertex(shape, TOP, 0, 0.0, 1.0, 0.0);
  shape_position_vertex(shape, BTM, 0, 0.0, -1.0, 0.0);
  for (i = 0; i < cardinality + 1; i += 1) {
    shape_position_vertex(shape, TOP, i + 1, unit_x[i], -1.0, unit_y[i]);
    shape_position_vertex(shape, BTM, i + 1, unit_x[i], -1.0, unit_y[i]);
  }
}


void shape_prism() {
  const int TOP = 0; const int BTM = 1; const int SIDES = 2;
  int i;

  shape = shape_define(shape, 3, /*TOP*/ GL_TRIANGLE_FAN, (cardinality + 1) + 1,
                                 /*BTM*/ GL_TRIANGLE_FAN, (cardinality + 1) + 1,
                                 /*SIDES*/ GL_TRIANGLE_STRIP, (cardinality + 1) * 2);

  shape_position_vertex(shape, TOP, 0, 0.0, 1.0, 0.0);
  shape_position_vertex(shape, BTM, 0, 0.0, -1.0, 0.0);
  for (i = 0; i < cardinality + 1; i += 1) {
    shape_position_vertex(shape, TOP, i + 1, unit_x[i], 1.0, unit_y[i]);
    shape_position_vertex(shape, BTM, i + 1, unit_x[i], -1.0, unit_y[i]);
    shape_position_vertex(shape, SIDES, i * 2, unit_x[i], 1.0, unit_y[i]);
    shape_position_vertex(shape, SIDES, (i * 2) + 1, unit_x[i], -1.0, unit_y[i]);
  }
}


static void set_cardinality(int n) {
  int i;

  if (n >= 3 && n <= MAX_CARDINALITY) {
    for (i = 0; i < n; i += 1) {
      unit_x[i] = sinf((2.0f * PI * i) / n);
      unit_y[i] = cosf((2.0f * PI * i) / n);
    }

    /* add the first coord as the final to complete the loop */
    unit_x[n] = unit_x[0];
    unit_y[n] = unit_y[0];

    cardinality = n;
    shape_pyramid();
  }
}


static void handle_keyboard(int key, int action) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case 'W':
        set_wireframe(!wireframe);
        break;

      case GLFW_KEY_UP:
        set_cardinality(cardinality + 1);
        break;

      case GLFW_KEY_DOWN:
        set_cardinality(cardinality - 1);
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
  set_cardinality(cardinality);
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
