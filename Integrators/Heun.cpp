// Standard includes
#include <cassert>
#include <stdexcept>
#include <sstream>

// Project includes
#include "Heun.h"

IntegratorHeun::IntegratorHeun(IModel *simulationModel, double dt) : IIntegrator(simulationModel, dt)
  ,state(new double[dimension])
  ,temp(new double[dimension])
  ,k1(new double[dimension])
  ,k2(new double[dimension])
{
  if (simulationModel==NULL)
    throw std::runtime_error("Model pointer may not be NULL.");

  std::stringstream name;
  name << "Heun";
  SetName(name.str());
}

void IntegratorHeun::SingleStep()
{
  // k1
  model->Evaluate(state, time, k1);
  for (std::size_t i=0; i<dimension; ++i)
    temp[i] = state[i] + 2.0/3.0 * timeStep * k1[i];


  // k2
  model->Evaluate(temp, time + 2.0/3.0 * timeStep, k2);
  for (std::size_t i=0; i<dimension; ++i)
    state[i] += timeStep/4.0 * (k1[i] + 3*k2[i]);

  time += timeStep;
}

void IntegratorHeun::SetInitialState(double *initialState)
{
  for (unsigned i=0; i<dimension; ++i)
  {
    state[i] = initialState[i];
    k1[i] = 0;
    k2[i] = 0;
  }

  time = 0;
}

double* IntegratorHeun::GetState() const
{
  return state;
}