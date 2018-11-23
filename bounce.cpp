#include <GLUT/glut.h>
#include <cmath>
#include <stdlib.h>

// Colors
GLfloat WHITE[] = {1, 1, 1};
GLfloat RED[] = {1, 0, 0};
GLfloat GREEN[] = {1, 0, 0};
GLfloat MAGENTA[] = {1, 0, 1};

// A camera.
class Camera {
  double theta;      // determines the x and z positions
  double y;          // the current y position
  double dTheta;     // increment in theta for swinging the camera around
  double dy;         // increment in y for moving the camera up/down
public:
  Camera(): theta(0), y(3), dTheta(0.04), dy(0.2) {}
  double getX() {return 10 * cos(theta);}
  double getY() {return y;}
  double getZ() {return 10 * sin(theta);}
  void moveRight() {theta += dTheta;}
  void moveLeft() {theta -= dTheta;}
  void moveUp() {y += dy;}
  void moveDown() {if (y > dy) y -= dy;}
};

/ A ball.
class Ball {
  double radius;
  GLfloat* color;
  double maximumHeight;
  double x;
  double y;
  double z;
  int direction;
public:
  Ball(double r, GLfloat* c, double h, double x, double z):
      radius(r), color(c), maximumHeight(h), direction(-1),
      y(h), x(x), z(z) {
  }
  void update() {
    y += direction * 0.05;
    if (y > maximumHeight) {
      y = maximumHeight; direction = -1;
    } else if (y < radius) {
      y = radius; direction = 1;
    }
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    glTranslated(x, y, z);
    glutSolidSphere(radius, 30, 30);
    glPopMatrix();
  }
};

// A checkerboard.  
class Checkerboard {
  int displayListId;
  int width;
  int depth;
public:
  Checkerboard(int width, int depth): width(width), depth(depth) {}
  double centerx() {return width / 2;}
  double centerz() {return depth / 2;}
  void create() {
    displayListId = glGenLists(1);
    glNewList(displayListId, GL_COMPILE);
    //GLfloat lightPosition[] = {4, 3, 7, 1};
    //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glBegin(GL_QUADS);
    glNormal3d(0, 1, 0);
    for (int x = 0; x < width - 1; x++) {
      for (int z = 0; z < depth - 1; z++) {
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
                     (x + z) % 2 == 0 ? RED : WHITE);
        glVertex3d(x, 0, z);
        glVertex3d(x+1, 0, z);
        glVertex3d(x+1, 0, z+1);
        glVertex3d(x, 0, z+1);
      }
    }
    glEnd();
    glEndList();
  }
  void draw() {
    glCallList(displayListId);
  }
};

// Global variables: a camera, a checkerboard and some balls.
Checkerboard checkerboard(8, 8);
Camera camera;
Ball balls[] = {
  Ball(1, GREEN, 7, 6, 1),
  Ball(1.5, MAGENTA, 6, 3, 4),
  Ball(0.4, WHITE, 5, 1, 7)
};


// Application-specific initialization: Set up global lighting parameters
// and create display lists.
void init() {
  glEnable(GL_DEPTH_TEST);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, WHITE);
  glLightfv(GL_LIGHT0, GL_SPECULAR, WHITE);
  glMaterialfv(GL_FRONT, GL_SPECULAR, WHITE);
  glMaterialf(GL_FRONT, GL_SHININESS, 30);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  checkerboard.create();
}
// Draws one frame, the checkerboard then the balls, from the current camera
// position.
void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  gluLookAt(camera.getX(), camera.getY(), camera.getZ(),
            checkerboard.centerx(), 0.0, checkerboard.centerz(),
            0.0, 1.0, 0.0);
  checkerboard.draw();
  for (int i = 0; i < sizeof balls / sizeof(Ball); i++) {
    balls[i].update();
  }
  glFlush();
  glutSwapBuffers();
}
// On reshape, constructs a camera that perfectly fits the window.
void reshape(GLint w, GLint h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, GLfloat(w) / GLfloat(h), 1.0, 150.0);
  glMatrixMode(GL_MODELVIEW);
}

// Requests to draw the next frame.
void timer(int v) {
  glutPostRedisplay();
  glutTimerFunc(1000/60, timer, v);
}
// Moves the camera according to the key pressed, then ask to refresh the
// display.
void special(int key, int, int) {
  switch (key) {
    case GLUT_KEY_LEFT: camera.moveLeft(); break;
    case GLUT_KEY_RIGHT: camera.moveRight(); break;
    case GLUT_KEY_UP: camera.moveUp(); break;
    case GLUT_KEY_DOWN: camera.moveDown(); break;
  }
  glutPostRedisplay();
}

void key(unsigned char key, int x, int y) {
  switch (key) {
    case 'a': camera.moveLeft(); break;
    case 'd': camera.moveRight(); break;
    case 'w': camera.moveUp(); break;
    case 's': camera.moveDown(); break;
	case 'q': exit(0);

  }

