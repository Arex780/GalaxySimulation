#ifndef _IDISPLAY
#define _IDISPLAY

// Standard includes
#include <string>

// Library includes
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glu.h>

// Project includes
#include "../Structs/Vectors.h"

class IDisplay
{
public:

  IDisplay(int windowWidth, int windowHeight, double fieldOfView, const std::string &name);
  void MainLoop();
  void ExitMainLoop();
  void SetWindowName(const std::string &name);
  virtual void Render() = 0;

protected:

  virtual void PollEvents();
  virtual void OnProcessEvents(uint8_t type);
  void SetCamera(const Vector3D &position, const Vector3D &lookAt, const Vector3D &orientation);
  void AdjustCamera();
  void DrawAxis(const Vector3D &origin);
  int GetFPS() const;
  void ScaleAxis(double scale);
  double GetFOV() const;
  SDL_Event event;

private:

  void InitGL();

  double fov;
  int fps;
  int width;
  int height; 

  Vector3D cameraPosition;
  Vector3D cameraLookAt; 
  Vector3D cameraOrientation;

  SDL_Surface *surfaceScreen;

  volatile bool isRunning;
};

#endif
