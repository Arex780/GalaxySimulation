#ifndef _EULER
#define _EULER

#include <string>
#include "../Interfaces/IIntegrator.h"

class IntegratorEuler : public IIntegrator
{
public:

  IntegratorEuler(IModel *simulationModel, double dt);
  virtual void SingleStep();
  virtual void SetInitialState(double *initialState);
  virtual double* GetState() const;

private:

  double *state;
};

#endif