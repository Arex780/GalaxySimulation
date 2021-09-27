// Standard includes
#include <cassert>
#include <cstdlib>

// Project includes
#include "Particles.h"

ParticleData2D::ParticleData2D()
  :particleState(NULL)
  ,particleParameters(NULL)
{}

ParticleData2D::ParticleData2D(ParticleState2D *state, ParticleParameters *parameters)
  :particleState(state)
  ,particleParameters(parameters)
{
  assert(particleState);
  assert(particleParameters);
}

ParticleData2D::ParticleData2D(const ParticleData2D &ref)
  :particleState(ref.particleState)
  ,particleParameters(ref.particleParameters)
{}

ParticleData2D& ParticleData2D::operator=(const ParticleData2D &ref)
{
  if (this!=&ref)
  {
    particleState    = ref.particleState;
    particleParameters = ref.particleParameters;
  }

  return *this;
}

void ParticleData2D::Reset()
{
  particleState    = NULL;
  particleParameters = NULL;
}

bool ParticleData2D::IsNull() const
{
  return particleState && particleParameters;
}

ParticleData3D::ParticleData3D()
  :particleState(NULL)
  ,particleParameters(NULL)
{}

ParticleData3D::ParticleData3D(ParticleState3D *state, ParticleParameters *parameters)
  :particleState(state)
  ,particleParameters(parameters)
{
  assert(particleState);
  assert(particleParameters);
}

ParticleData3D::ParticleData3D(const ParticleData3D &ref)
  :particleState(ref.particleState)
  ,particleParameters(ref.particleParameters)
{}

ParticleData3D& ParticleData3D::operator=(const ParticleData3D &ref)
{
  if (this!=&ref)
  {
    particleState    = ref.particleState;
    particleParameters = ref.particleParameters;
  }

  return *this;
}

void ParticleData3D::Reset()
{
  particleState    = NULL;
  particleParameters = NULL;
}

bool ParticleData3D::IsNull() const
{
  return particleState && particleParameters;
}
