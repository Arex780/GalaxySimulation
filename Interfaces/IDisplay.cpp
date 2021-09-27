// Standard includes
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <ctime>
#include <cmath>

// Library includes
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

// Project includes
#include "IDisplay.h"

IDisplay::IDisplay(int windowWidth, int windowHeight, double fieldOfView, const std::string &name)
  :event()
  ,width(windowWidth)
  ,height(windowHeight)
  ,fov(fieldOfView)
  ,fps(0)
  ,surfaceScreen(NULL)
  ,isRunning(true)
{
  if (SDL_Init(SDL_INIT_VIDEO) == -1)
    throw std::runtime_error(SDL_GetError());
  atexit(SDL_Quit);

  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  surfaceScreen = SDL_SetVideoMode(width, height, 16, SDL_OPENGL);
  if (!surfaceScreen)
    throw std::runtime_error(SDL_GetError());

  SetWindowName(name);
  InitGL();
}

void IDisplay::InitGL()
{
  glShadeModel(GL_SMOOTH);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // black background
  glViewport(0, 0, width, height);
}

void IDisplay::ScaleAxis(double scale)
{
  fov *= scale;
  AdjustCamera();
}

void IDisplay::SetCamera(const Vector3D &position, const Vector3D &lookAt, const Vector3D &orientation)
{
  cameraOrientation = orientation;
  cameraPosition = position;
  cameraLookAt = lookAt;
  AdjustCamera();
}

void IDisplay::AdjustCamera()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  double l = fov/2.0;
  glOrtho(-l, l, -l, l, -l, l);
  gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z,
            cameraLookAt.x, cameraLookAt.y, cameraLookAt.z,
            cameraOrientation.x, cameraOrientation.y, cameraOrientation.z);
  glMatrixMode(GL_MODELVIEW);
}

void IDisplay::SetWindowName(const std::string &name)
{
  SDL_WM_SetCaption(name.c_str(), NULL);
}

double IDisplay::GetFOV() const
{
  return fov;
}

int IDisplay::GetFPS() const
{
  return fps;
}

void IDisplay::DrawAxis(const Vector3D &origin)
{
  glColor3f(0.3, 0.3, 0.3); // grey axis

  double s = std::pow(10, (int)(log10(fov/2))),
         l = fov/100,
         p = 0;

  glPushMatrix();
  glTranslated(origin.x, origin.y, origin.z);

  for (int i=0; p<fov; ++i)
  {
    p += s;

    glBegin(GL_LINES);
      glVertex3f(p, -l, 0);
      glVertex3f(p,  l, 0);
      glVertex3f(-p, -l, 0);
      glVertex3f(-p,  0, 0);
      glVertex3f(-l, p, 0);
      glVertex3f( 0, p, 0);
      glVertex3f(-l, -p, 0);
      glVertex3f( 0, -p, 0);
      glVertex3f(-l, 0, p);
      glVertex3f( 0, 0, p);
      glVertex3f(-l, 0, -p);
      glVertex3f( 0, 0, -p);
    glEnd();

  }

  glBegin(GL_LINES);
    glVertex3f(-fov, 0, 0);
    glVertex3f(fov, 0, 0);
    glVertex3f(0, -fov, 0);
    glVertex3f(0, fov, 0);
    glVertex3f(0, 0, -fov);
    glVertex3f(0, 0, fov);
  glEnd();

  glPopMatrix();
}

void IDisplay::MainLoop()
{
  int ct = 0;
  double dt = 0;
  time_t t1(time(NULL)), t2;

  while (isRunning)
  {
    Render();
    PollEvents();
    ++ct;

    // calculate fps
    t2 = time(NULL);
    dt = difftime(t2, t1);
    if (dt>1)
    {
      fps = (double)ct / dt;
      ct = 0;
      t1 = t2;
    }
  }
}

void IDisplay::ExitMainLoop()
{
  isRunning = false;
}

void IDisplay::OnProcessEvents(uint8_t type)
{}

void IDisplay::PollEvents()
{
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
    case SDL_QUIT:
        ExitMainLoop();
        break;

    default:
        OnProcessEvents(event.type);
        break;
    }
  }
}

