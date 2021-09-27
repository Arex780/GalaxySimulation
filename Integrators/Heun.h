#ifndef _HEUN
#define	_HEUN

#include "../Interfaces/IIntegrator.h"

class IntegratorHeun : public IIntegrator
{
public:

  IntegratorHeun(IModel *simulationModel, double dt);
  virtual void SingleStep();
  virtual void SetInitialState(double *initialState);
  virtual double* GetState() const;

private:

  double *state;
  double *temp;
  double *k1;
  double *k2;
};

#endif