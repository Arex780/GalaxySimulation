// Standard includes
#include <cassert>
#include <stdexcept>
#include <sstream>

// Project includes
#include "Euler.h"

IntegratorEuler::IntegratorEuler(IModel *simulationModel, double dt) : IIntegrator(simulationModel, dt)
  ,state((simulationModel) ? new double [simulationModel->GetSimulationDimension()] : NULL)
{
  if (simulationModel==NULL)
    throw std::runtime_error("Model pointer may not be NULL.");

  std::stringstream name;
  name << "Euler";
  SetName(name.str());
}

void IntegratorEuler::SingleStep()
{
  double k1[dimension];
  model->Evaluate(state, time, k1);

  for (std::size_t i=0; i<dimension; ++i)
    state[i] += timeStep * k1[i];

  time += timeStep;
}

void IntegratorEuler::SetInitialState(double *initialState)
{
  for (unsigned i=0; i<dimension; ++i)
    state[i] = initialState[i];

  time = 0;
}

double* IntegratorEuler::GetState() const
{
  return state;
}