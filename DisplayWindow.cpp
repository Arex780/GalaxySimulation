// Standard includes
#include <iostream>
#include <cmath>
#include <cassert>
#include <limits>
#include <omp.h>

// Project includes
#include "DisplayWindow.h"
#include "Integrators/Euler.h"
#include "Integrators/Heun.h"
#include "Integrators/RK4.h"

DisplayWindow::DisplayWindow(Json::Value config) : IDisplay(config["Window size"].asInt(), config["Window size"].asInt(), config["Field of view"].asInt(), config["Simulation"].asString())
  ,model(NULL)
  ,integrator(NULL)
  ,configuration(config)
  ,cameraSettings(0)
  ,showAxis(true)
  ,showCompleteTree(false)
  ,showForceTree(false)
  ,showParticles(true)
  ,showStatistics(true)
  ,isSimulationPaused(false)
{}

void DisplayWindow::Init()
{
  // Create the model class
  if (configuration["Model"].asString() == "N-body")
    model = new NBody(configuration);
  else // default if not provided or not correct
    model = new NBody(configuration);

  // assign model to the integrator and set the time step
  delete integrator;
  if (configuration["Integrator"].asString() == "Euler")
    integrator = new IntegratorEuler(model, configuration["Time step"].asInt());
  else if (configuration["Integrator"].asString() == "Heun")
    integrator = new IntegratorHeun(model, configuration["Time step"].asInt());
  else if (configuration["Integrator"].asString() == "RK4")
    integrator = new IntegratorRK4(model, configuration["Time step"].asInt());
  else // default if not provided or not correct
    integrator = new IntegratorHeun(model, configuration["Time step"].asInt());

  integrator->SetInitialState(model->GetInitialState());

  // OpenGL initialization
  glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
  SetCamera(Vector3D(0,0,1),Vector3D(0,0,0),Vector3D(0,1,0));
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void DisplayWindow::Render()
{
  if (!isSimulationPaused)
    integrator->SingleStep();

  glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);

  // Pre-configured camera positions
  switch (cameraSettings)
  {
  case 0: SetCamera(Vector3D(0,0,1),Vector3D(0,0,0),Vector3D(0,1,0));
          break;

  case 1: SetCamera(Vector3D(0,0,1),Vector3D(0,3,0),Vector3D(0,1,0));
          break;
          
  case 2: SetCamera(Vector3D(0,0,1),Vector3D(3,3,0),Vector3D(1,1,0));
          break;

  case 3: SetCamera(Vector3D(1,0,0),Vector3D(0,0,0),Vector3D(0,0,1));
          break;
  }

  if (showAxis) // display axis on the mass center
  {
    const Vector3D &massCenter = model->GetMassCenter();
    DrawAxis(Vector3D(massCenter.x, massCenter.y, massCenter.z));
  }

  if (showCompleteTree || showForceTree)
    DrawTree();

  if (showParticles)
    DrawParticles();

  if (showStatistics)
    ShowStatisticsConsole();

  SDL_GL_SwapBuffers();
}

void DisplayWindow::DrawParticles()
{
  assert(integrator);

  ParticleState2D *state = reinterpret_cast<ParticleState2D*>(integrator->GetState());
  const ParticleParameters *parameters  = model->GetParticleParameters();

  for (int i=0; i<model->GetTotalParticles(); ++i)
  {
    if (parameters[i].radius > 0) // bulge loop
    {
      glEnable(GL_POINT_SMOOTH);
      glColor3f(1,0.5f,0); // orange color
      glPointSize(parameters[i].radius * 40);
      glBegin(GL_POINTS);
      glVertex3f(state[i].positionX, state[i].positionY, 0.0f);
      glEnd();
    }
    else
    {
      glDisable(GL_POINT_SMOOTH); // stars loop
      glColor3f(0,0,1); // blue color
      glPointSize(parameters[i].mass/10);
      glBegin(GL_POINTS);
      glVertex3f(state[i].positionX, state[i].positionY, 0.0f);
      glEnd();
    }
  }

}

void DisplayWindow::ShowStatisticsConsole()
{
  Quadtree *tree = model->GetTree();

  std::cout << "                             \n";
  std::cout << "Time: " << integrator->GetTime() << "\n";
  std::cout << "FPS: " << GetFPS() << "\n";
  std::cout << "FOV: " << GetFOV() << "\n";
  std::cout << "Axis scale: " << pow(10, (int)(log10(GetFOV()/2))) << "\n";
  std::cout << "Bodies inside tree: " << tree->GetAllNodesParticles() << "\n";
  std::cout << "Theta: " << tree->GetTheta() << "\n";
  std::cout << "Time step: " << integrator->GetTimeStep() << "\n";
  std::cout << "Integrator: " << integrator->GetName().c_str() << "\n";
  std::cout << "_____________________________\n";
}

void DisplayWindow::DrawTree()
{
  struct DrawTree
  {
    enum TreeType
    {
      COMPLETE, // Display all tree nodes
      FORCE,    // Display only tree nodes that are used to calculate force
    };

    DrawTree(Quadtree *tree, TreeType type, int fov)
    {
      DrawTreeNode(tree, 0, type, fov);
    }

    void DrawTreeNode(Quadtree *treeNode, int level, TreeType type, int fov)
    {
      assert(treeNode);

      double col = 1 - level*0.2;
      switch (type)
      {
        case COMPLETE:  glColor3f(col, 1, col); break;
        case FORCE:  glColor3f(0, 1, 0); break;
      }

      // First draw root node
      if ( type==COMPLETE ||
          (type==FORCE  && !treeNode->IsDevided()) )
      {
        const Vector2D &min = treeNode->GetMinimumDimension(), &max = treeNode->GetMaximumDimension();

        glBegin(GL_LINE_STRIP);
           glVertex3f(min.x, min.y, 0);
           glVertex3f(max.x, min.y, 0);
           glVertex3f(max.x, max.y, 0);
           glVertex3f(min.x, max.y, 0);
           glVertex3f(min.x, min.y, 0);
        glEnd();

        if (!treeNode->IsExternal())
        {
          double len = (double)fov/50 * std::max(1 - level*0.2, 0.1);
          glPointSize(4);
          glColor3f(col, 1, col);

          const Vector2D massCenter = treeNode->GetMassCenter();
          glBegin(GL_LINES);
            glVertex3f(massCenter.x-len, massCenter.y, 0);
            glVertex3f(massCenter.x+len, massCenter.y, 0);
          glEnd();
          glBegin(GL_LINES);
            glVertex3f(massCenter.x, massCenter.y-len, 0);
            glVertex3f(massCenter.x, massCenter.y+len, 0);
          glEnd();
        }
      }

      // If the node was not divided, dont draw its nodes
      if (type!=COMPLETE && !treeNode->IsDevided())
        return;

      // Draw child nodes
      for (int i=0; i<4; ++i)
      {
        if (treeNode->quadNode[i])
          DrawTreeNode(treeNode->quadNode[i], level+1, type, fov);
      }
    }
  };

  Quadtree *tree = model->GetTree();
  if (showCompleteTree)
    DrawTree DrawComplete(tree, DrawTree::COMPLETE, GetFOV());
  else if (showForceTree)
    DrawTree DrawFar(tree, DrawTree::FORCE, GetFOV());
}

void DisplayWindow::DrawTreeNode(Quadtree *treeNode, int level)
{
  assert(treeNode);
  double len = 0.01 * std::max(1 - level*0.2, 0.1);

  double col = 1 - level*0.2;

  if (treeNode->IsDevided())
    glColor3f(1, col, col);
  else
    glColor3f(col, 1, col);

  const Vector2D &min = treeNode->GetMinimumDimension(), &max = treeNode->GetMaximumDimension();
  glBegin(GL_LINE_STRIP);
    glVertex3f(min.x, min.y, 0);
    glVertex3f(max.x, min.y, 0);
    glVertex3f(max.x, max.y, 0);
    glVertex3f(min.x, max.y, 0);
    glVertex3f(min.x, min.y, 0);
  glEnd();

  if (!treeNode->IsExternal())
  {
    Vector2D massCenter = treeNode->GetMassCenter();

    glPointSize(4);
    glColor3f(col, 1, col);
    glBegin(GL_LINES);
      glVertex3f(massCenter.x-len, massCenter.y,     0);
      glVertex3f(massCenter.x+len, massCenter.y,     0);
      glVertex3f(massCenter.x,     massCenter.y-len, 0);
      glVertex3f(massCenter.x,     massCenter.y+len, 0);
    glEnd();
  }

  if (!treeNode->IsDevided())
    return;

  for (int i=0; i<4; ++i)
  {
    if (treeNode->quadNode[i])
    {
      DrawTreeNode(treeNode->quadNode[i], level+1);
    }
  }
}

void DisplayWindow::OnProcessEvents(uint8_t type)
{
  switch (type)
  {
    case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
          case SDLK_1:
               cameraSettings = 0;
               break;

          case SDLK_2:
               cameraSettings = 1;
               break;
               
          case SDLK_3:
               cameraSettings = 2;
               break;

          case SDLK_4:
               cameraSettings = 3;
               break;

          case SDLK_a:
                showAxis = !showAxis;
                break;

          case  SDLK_p:
                showParticles = !showParticles;
                break;

          case  SDLK_r:
                integrator->Reverse();
                break;

          case  SDLK_t:
                showForceTree = false;
                showCompleteTree = !showCompleteTree;
                break;

          case  SDLK_f:
                showCompleteTree = false;
                showForceTree = !showForceTree;
                break;

          case  SDLK_SPACE:
                isSimulationPaused = !isSimulationPaused;
                break;

          case  SDLK_s:
                showStatistics = !showStatistics;
                break;

          case SDLK_UP:
               model->SetTheta(model->GetTheta() + 0.1);
               break;

          case SDLK_DOWN:
               model->SetTheta(std::max(model->GetTheta() - 0.1, 0.1));
               break;

          case SDLK_RIGHT:
               integrator->SetTimeStep(integrator->GetTimeStep() + 100.0);
               break;

          case SDLK_LEFT:
               integrator->SetTimeStep(std::max(integrator->GetTimeStep() - 100.0, 100.0));
               break;

          case SDLK_RSHIFT:
               ScaleAxis(0.9);
               break;

          case SDLK_RCTRL:
               ScaleAxis(1.1);
               break;

          default:
               break;

        }

        break;
  }
}
