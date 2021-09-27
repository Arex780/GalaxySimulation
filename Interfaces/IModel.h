#ifndef _IMODEL
#define	_IMODEL

#include <string>

class IModel
{
public:

    IModel(const std::string &modelName, unsigned dim=1);

    unsigned GetSimulationDimension() const;
    void SetSimulationDimension(unsigned dim) ;
    std::string GetName() const;
    virtual void Evaluate(double *state, double time, double *derivative) = 0;
    virtual double* GetInitialState() = 0;

private:
    unsigned dimension;
    std::string name;
};

#endif
