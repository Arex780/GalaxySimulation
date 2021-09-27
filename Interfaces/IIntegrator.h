#ifndef _IINTEGRATOR
#define	_IINTEGRATOR

#include <memory>
#include "IModel.h"

class IIntegrator
{
public:
  
    IIntegrator(IModel *simulationModel, double dt);
    void SetTimeStep(double dt);
    double GetTimeStep() const;
    double GetTime() const;
    virtual void Reverse();
    virtual void SetInitialState(double *initialState) = 0;
    virtual void SingleStep() = 0;
    virtual double* GetState() const = 0;
    const std::string& GetName() const;

protected:

    void SetName(const std::string &integratorName);

    IModel *model;
    double timeStep;
    double time;
    const unsigned dimension;
    std::string name;

private:

    IIntegrator(const IIntegrator &ref);
    IIntegrator& operator=(const IIntegrator &ref);
};

#endif
