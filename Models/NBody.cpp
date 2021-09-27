// Standrad includes
#include <cstdlib>
#include <cmath>
#include <limits>
#include <iostream>
#include <string>
#include <omp.h>

// Project includes
#include "NBody.h"

using namespace std;

NBody::NBody(Json::Value config) : IModel("N-Body simulation (2D)")
  ,particleState(NULL)
  ,particleParameters(NULL)
  ,configuration(config)
  ,quadtree(Quadtree(Vector2D(), Vector2D()))
  ,cornerNW()
  ,cornerSE()
  ,massCenter()
  ,areaOfInterest(1)
  ,year(365.25*86400) // definition of year in seconds
  ,massSun(1.988435e30) // mass of the Sun in kilograms
  ,pc(3.08567758129e16) // definion of parcecs in meters
  ,gravitationalConstant(6.67428e-11) // G
  ,g(gravitationalConstant/(pc*pc*pc)*massSun*year*year) // G but in parsecs, sun-mass and years
  ,particles(0)
{
  Quadtree::gravitationalConstant = g;

  if (configuration["Simulation"].asString() == "Single Galaxy")
    SingleGalaxy();
  else if (configuration["Simulation"].asString() == "Galaxy Collision")
    GalaxyCollision();
  else // default if not provided or not correct
    SingleGalaxy();
}

Vector3D NBody::GetMassCenter() const
{
  const Vector2D &massCenter = quadtree.GetMassCenter();
  return Vector3D(massCenter.x, massCenter.y, 0);
}

double* NBody::GetInitialState()
{
  return reinterpret_cast<double*>(particleState);
}

void NBody::GetOrbitalVelocity(const ParticleData2D &p1, const ParticleData2D &p2)
{
  double x1 = p1.particleState->positionX,
         y1 = p1.particleState->positionY,
         m1 = p1.particleParameters->mass;
  double x2 = p2.particleState->positionX,
         y2 = p2.particleState->positionY;

  // Calculate distance
  double r[2], dist;
  r[0] = x1 - x2;
  r[1] = y1 - y2;

  // distance in parsec
  dist = sqrt(r[0] * r[0] + r[1] * r[1]);

  // Based on the distance from the given body (p1) calculate the velocity needed to maintain a circular orbit
  double v = sqrt(g * m1 / dist);

  // Calculate for 2D vector
  double &vx = p2.particleState->velocityX,
         &vy = p2.particleState->velocityY;
  vx = ( r[1] / dist) * v;
  vy = (-r[0] / dist) * v;
}

void NBody::SimulationSettings(int totalParticles)
{
  particles = totalParticles;
  SetSimulationDimension(particles*4);

  particleState = new ParticleState2D[totalParticles];
  particleParameters = new ParticleParameters[totalParticles];
}

void NBody::SingleGalaxy()
{
  Json::Value simSettings = configuration["Simulation settings"]["Single Galaxy"];
  
  // Set simulation parameters
  SimulationSettings(simSettings["Number of particles"].asInt());

  // Initialize particles
  ParticleData2D galaxyCore;

  for (int i=0; i<particles; ++i)
  {
    ParticleState2D &state = particleState[i];
    ParticleParameters &parameters = particleParameters[i];

    if (i==0)
    {
      galaxyCore.particleState = &state;
      galaxyCore.particleParameters = &parameters;
      state.positionX = simSettings["Initial conditions"]["positionX"].asFloat();
      state.positionY = simSettings["Initial conditions"]["positionY"].asFloat();
      state.velocityX = simSettings["Initial conditions"]["velocityX"].asFloat(); // parsecs/year
      state.velocityY = simSettings["Initial conditions"]["velocityY"].asFloat(); // parsecs/year
      parameters.mass = simSettings["Bulge mass"].asFloat(); // times sun mass
      parameters.radius = simSettings["Bulge radius"].asFloat();
    }
    else
    {
      double radius = simSettings["Bulge radius"].asFloat() + (double)rand() / RAND_MAX * (simSettings["Disk radius"].asFloat() - simSettings["Bulge radius"].asFloat());
      double angle = rand();
      parameters.mass = simSettings["Minimum stellar mass"].asFloat() + (double)rand() / RAND_MAX * (simSettings["Maximum stellar mass"].asFloat() - simSettings["Minimum stellar mass"].asFloat());
      state.positionX = simSettings["Initial conditions"]["positionX"].asFloat() + radius*sin(angle);
      state.positionY = simSettings["Initial conditions"]["positionY"].asFloat() + radius*cos(angle);

      GetOrbitalVelocity(galaxyCore, ParticleData2D(&state, &parameters));
      state.velocityX+=galaxyCore.particleState->velocityX;
      state.velocityY+=galaxyCore.particleState->velocityY;
    }

    // Determine the size of the area including all particles
    cornerSE.x = std::max(cornerSE.x, state.positionX);
    cornerSE.y = std::max(cornerSE.y, state.positionY);
    cornerNW.x = std::min(cornerNW.x, state.positionX);
    cornerNW.y = std::min(cornerNW.y, state.positionY);
  }

  // Calculate the dimesion of the quadrant and add little bit more space to it
  areaOfInterest = 1.5 * 1.05 * std::max(cornerSE.x - cornerNW.x, cornerSE.y - cornerNW.y);
}

void NBody::GalaxyCollision()
{
  Json::Value simSettings = configuration["Simulation settings"]["Galaxy Collision"];

  // Calculate all particles in every galaxy (and initialize particles)
  int particlesNumber = 0;
  ParticleData2D galaxyCores [simSettings.size()];
  for (int i = 1; i <= simSettings.size(); i++)
  {
    particlesNumber += simSettings[to_string(i)]["Number of particles"].asInt();
  }

  int k = 0;

  // Set simulation parameters
  SimulationSettings(particlesNumber);

  for (int i = 1; i <= simSettings.size(); i++)
  {
    Json::Value galaxySettings = simSettings[to_string(i)];

    for (int j=0; j<galaxySettings["Number of particles"].asInt(); ++j)
    {
      ParticleState2D &state = particleState[k];
      ParticleParameters &parameters = particleParameters[k];
      k++;

      if (j==0)
      {
        galaxyCores[i].particleState = &state;
        galaxyCores[i].particleParameters = &parameters;
        state.positionX = galaxySettings["Initial conditions"]["positionX"].asFloat();
        state.positionY = galaxySettings["Initial conditions"]["positionY"].asFloat();
        state.velocityX = galaxySettings["Initial conditions"]["velocityX"].asFloat(); // parsecs/year
        state.velocityY = galaxySettings["Initial conditions"]["velocityY"].asFloat(); // parsecs/year
        parameters.mass = galaxySettings["Bulge mass"].asFloat(); // times sun mass
        parameters.radius = galaxySettings["Bulge radius"].asFloat();
      }
      else
      {
        double radius = galaxySettings["Bulge radius"].asFloat() + (double)rand() / RAND_MAX * (galaxySettings["Disk radius"].asFloat() - galaxySettings["Bulge radius"].asFloat());
        double angle = rand();
        parameters.mass = galaxySettings["Minimum stellar mass"].asFloat() + (double)rand() / RAND_MAX * (galaxySettings["Maximum stellar mass"].asFloat() - galaxySettings["Minimum stellar mass"].asFloat());
        state.positionX = galaxySettings["Initial conditions"]["positionX"].asFloat() + radius*sin(angle);
        state.positionY = galaxySettings["Initial conditions"]["positionY"].asFloat() + radius*cos(angle);

        GetOrbitalVelocity(galaxyCores[i], ParticleData2D(&state, &parameters));
        state.velocityX+=galaxyCores[i].particleState->velocityX;
        state.velocityY+=galaxyCores[i].particleState->velocityY;
      }

      // Determine the size of the area including all particles
      cornerSE.x = std::max(cornerSE.x, state.positionX);
      cornerSE.y = std::max(cornerSE.y, state.positionY);
      cornerNW.x = std::min(cornerNW.x, state.positionX);
      cornerNW.y = std::min(cornerNW.y, state.positionY);
    }
  }

  // Calculate the dimesion of the quadrant and add little bit more space to it
  areaOfInterest = 1.5 * 1.05 * std::max(cornerSE.x - cornerNW.x, cornerSE.y - cornerNW.y);

}

void NBody::BuiltTree(const ParticleData2D &particleData)
{
  quadtree.Reset(Vector2D(massCenter.x - areaOfInterest, massCenter.y - areaOfInterest),
               Vector2D(massCenter.x + areaOfInterest, massCenter.y + areaOfInterest));

  // Build the quadtree
  for (int i=0; i<particles; ++i)
  {
    try
    {
      // Get data of the particle
      ParticleData2D particle(&(particleData.particleState[i]), &(particleData.particleParameters[i]));

      // Insert the particle only if its inside the aoi
      quadtree.Insert(particle, 0);
    }
    catch(std::exception &exc)
    {
      // Particle outside the area of interest. Do nothing
    }
  }

  // Compute mass distribution
  quadtree.ComputeMassDistribution();

  // Update the mass center
  massCenter = quadtree.GetMassCenter();
}

const ParticleParameters* NBody::GetParticleParameters() const
{
  return particleParameters;
}

Quadtree* NBody::GetTree()
{
  return &quadtree;
}

int NBody::GetTotalParticles() const
{
  return particles;
}

double NBody::GetTheta() const
{
  return quadtree.GetTheta();
}

void NBody::SetTheta(double theta)
{
  quadtree.SetTheta(theta);
}

void NBody::Evaluate(double *state, double time, double *derivative)
{
  ParticleState2D *particleState = reinterpret_cast<ParticleState2D*>(state);
  ParticleNextState2D *particleNextState = reinterpret_cast<ParticleNextState2D*>(derivative);
  ParticleData2D particleData(particleState, particleParameters);

  BuiltTree(particleData);

  // OpenMP parallel calculation
  #pragma omp parallel for
  for (int i=1; i<particles; ++i)
  {
    ParticleData2D particle(&particleState[i], &particleParameters[i]);
    Vector2D accleration = quadtree.CalculateForce(particle);
    particleNextState[i].accelerationX = accleration.x;
    particleNextState[i].accelerationY = accleration.y;
    particleNextState[i].velocityX = particleState[i].velocityX;
    particleNextState[i].velocityY = particleState[i].velocityY;
  }

  // Particle "0" has statistics data and cannot be calculated parallel
  quadtree.ClearStatistics();
  ParticleData2D particle(&particleState[0], &particleParameters[0]);
  Vector2D acceleration = quadtree.CalculateForce(particle);
  particleNextState[0].accelerationX = acceleration.x;
  particleNextState[0].accelerationY = acceleration.y;
  particleNextState[0].velocityX = particleState[0].velocityX;
  particleNextState[0].velocityY = particleState[0].velocityY;
}
