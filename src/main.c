#include <GL/glfw.h>
#include <math.h>
#include <stdlib.h>

#ifndef PI
  #define PI 3.14159
#endif

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define MAX_FACES 1024
/* current number of faces */
static int faces = 5;
/* unit circle coords for start of each face + repitition of the first coord at end to complete the loop */
static float faces_coords[MAX_FACES + 1][2];



static void compute_faces() {
  static int prev_faces = -1;
  int i;

  if (faces != prev_faces) {
    for (i = 0; i < faces; i += 1) {
      faces_coords[i][0] = sinf((2.0f * PI * i) / faces);
      faces_coords[i][1] = cosf((2.0f * PI * i) / faces);
    }

    /* add the first coord as the final to complete the loop */
    faces_coords[faces][0] = faces_coords[0][0];
    faces_coords[faces][1] = faces_coords[0][1];

    prev_faces = faces;
  }
}


void draw_prism() {
  int i;
  int y;

  compute_faces();

  /* draw top and bottom */
  for (y = 1; y >= -1; y -= 2) {
    glBegin(GL_TRIANGLE_FAN);
      glVertex3f(0, y, 0);
      for (i = 0; i <= faces; i += 1) {
        glVertex3f(faces_coords[i][0], y, faces_coords[i][1]);
      }
    glEnd();
  }

  /* draw sides */
  glBegin(GL_TRIANGLE_STRIP);
    for (i = 0; i <= faces; i += 1) {
      glVertex3f(faces_coords[i][0], 1, faces_coords[i][1]);
      glVertex3f(faces_coords[i][0], -1, faces_coords[i][1]);
    }
  glEnd();
}


void draw_pyramid() {
  int i;

  compute_faces();

  /* draw base */
  glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, -1, 0);
    for (i = 0; i <= faces; i += 1) {
      glVertex3f(faces_coords[i][0], -1, faces_coords[i][1]);
    }
  glEnd();

  /* draw conic top */
  glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 1, 0);
    for (i = 0; i <= faces; i += 1) {
      glVertex3f(faces_coords[i][0], -1, faces_coords[i][1]);
    }
  glEnd();
}



void draw_square() {
  glBegin(GL_QUADS);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glVertex3f(1, 1, 0);
    glVertex3f(0, 1, 0);
  glEnd();
}


static void draw(double delta_time) {
  static float rotate_x = 0;
  static float rotate_y = 0;

  glLoadIdentity();
  gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glLineWidth(3.0f);

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
