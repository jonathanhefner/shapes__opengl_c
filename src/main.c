#include <GL/glfw.h>
#include <math.h>
#include <stdlib.h>

#ifndef PI
  #define PI 3.14159
#endif

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define MAX_CARDINALITY 42


/* unit circle coords for start of each face + repitition of the first coord at end to complete the loop */
static float unit_circle_coords[MAX_CARDINALITY + 1][2];


/***** Settings *****/
static int wireframe = 0;
static int cardinality = 3;



static void set_cardinality(int n) {
  int i;

  if (n >= 3 && n <= MAX_CARDINALITY) {
    for (i = 0; i < n; i += 1) {
      unit_circle_coords[i][0] = sinf((2.0f * PI * i) / n);
      unit_circle_coords[i][1] = cosf((2.0f * PI * i) / n);
    }

    /* add the first coord as the final to complete the loop */
    unit_circle_coords[n][0] = unit_circle_coords[0][0];
    unit_circle_coords[n][1] = unit_circle_coords[0][1];

    cardinality = n;
  }
}


static void set_wireframe(int enable) {
  if (enable) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(3.0f);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  wireframe = enable;
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


void draw_prism() {
  int i;
  int y;

  /* draw top and bottom */
  for (y = 1; y >= -1; y -= 2) {
    glBegin(GL_TRIANGLE_FAN);
      glVertex3f(0, y, 0);
      for (i = 0; i <= cardinality; i += 1) {
        glVertex3f(unit_circle_coords[i][0], y, unit_circle_coords[i][1]);
      }
    glEnd();
  }

  /* draw sides */
  glBegin(GL_TRIANGLE_STRIP);
    for (i = 0; i <= cardinality; i += 1) {
      glVertex3f(unit_circle_coords[i][0], 1, unit_circle_coords[i][1]);
      glVertex3f(unit_circle_coords[i][0], -1, unit_circle_coords[i][1]);
    }
  glEnd();
}


void draw_pyramid() {
  int i;

  /* draw base */
  glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, -1, 0);
    for (i = 0; i <= cardinality; i += 1) {
      glVertex3f(unit_circle_coords[i][0], -1, unit_circle_coords[i][1]);
    }
  glEnd();

  /* draw conic top */
  glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 1, 0);
    for (i = 0; i <= cardinality; i += 1) {
      glVertex3f(unit_circle_coords[i][0], -1, unit_circle_coords[i][1]);
    }
  glEnd();
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
  draw_pyramid();
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

  if(!glfwInit() || !glfwOpenWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetWindowTitle("Shapes Demo");
  glfwSetWindowSizeCallback(window_reshape);
  window_reshape(WINDOW_WIDTH, WINDOW_HEIGHT);

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
