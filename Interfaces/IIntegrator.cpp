// Standard includes
#include <stdexcept>
#include <cassert>

// Project includes
#include "IIntegrator.h"

IIntegrator::IIntegrator(IModel *simulationModel, double dt) : model(simulationModel)
  ,timeStep(dt)
  ,time(0)
  ,dimension( (simulationModel) ? simulationModel->GetSimulationDimension() : 0)
{
  if (!simulationModel)
    throw std::runtime_error("Model pointer may not be NULL");

  if (dt<=0)
    throw std::runtime_error("Step size may not be negative or NULL.");
}

double IIntegrator::GetTimeStep() const
{
  return timeStep;
}

void IIntegrator::Reverse()
{
  timeStep *= -1;
}

void IIntegrator::SetTimeStep(double dt)
{
  timeStep = dt;
}

void IIntegrator::SetName(const std::string &integratorName)
{
  name = integratorName;
}

const std::string& IIntegrator::GetName() const
{
  return name;
}

double IIntegrator::GetTime() const
{
  return time;
}