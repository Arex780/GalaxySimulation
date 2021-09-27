#ifndef _RK4
#define	_RK4

#include "../Interfaces/IIntegrator.h"

class IntegratorRK4 : public IIntegrator
{
public:

  IntegratorRK4(IModel *simulationModel, double dt);
  virtual void SingleStep();
  virtual void SetInitialState(double *initialState);
  virtual double* GetState() const;

private:

  double *state;
  double *temp;
  double *k1;
  double *k2;
  double *k3;
  double *k4;
};

#endif