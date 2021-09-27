// Standard includes
#include <cassert>
#include <stdexcept>
#include <sstream>

// Project includes
#include "RK4.h"

IntegratorRK4::IntegratorRK4(IModel *simulationModel, double dt) : IIntegrator(simulationModel, dt)
  ,state(new double [dimension])
  ,temp(new double [dimension])
  ,k1(new double [dimension])
  ,k2(new double [dimension])
  ,k3(new double [dimension])
  ,k4(new double [dimension])
{
  if (simulationModel==NULL)
    throw std::runtime_error("Model pointer may not be NULL.");

  std::stringstream name;
  name << "RK4";
  SetName(name.str());
}

void IntegratorRK4::SingleStep()
{
  assert(model);

  // k1
  model->Evaluate(state, time, k1);
  for (std::size_t i=0; i<dimension; ++i)
    temp[i] = state[i] + timeStep*0.5 * k1[i];

  // k2
  model->Evaluate(temp, time + timeStep*0.5, k2);
  for (std::size_t i=0; i<dimension; ++i)
    temp[i] = state[i] + timeStep*0.5 * k2[i];

  // k3
  model->Evaluate(temp, time + timeStep*0.5, k3);
  for (std::size_t i=0; i<dimension; ++i)
    temp[i] = state[i] + timeStep * k3[i];

  // k4
  model->Evaluate(temp, time + timeStep, k4);

  for (std::size_t i=0; i<dimension; ++i)
    state[i] += timeStep/6 * (k1[i] + 2*(k2[i]+k3[i]) + k4[i]);

  time += timeStep;
}

void IntegratorRK4::SetInitialState(double *initialState)
{
  for (unsigned i=0; i<dimension; ++i)
  {
    state[i] = initialState[i];
    k1[i] = 0;
    k2[i] = 0;
    k3[i] = 0;
    k4[i] = 0;
  }

  time = 0;
}

double* IntegratorRK4::GetState() const
{
  return state;
}