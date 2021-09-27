#ifndef _NBODY
#define	_NBODY

// Library includes
#include <jsoncpp/json/json.h>

// Project includes
#include "../Interfaces/IModel.h"
#include "../Structs/Vectors.h"
#include "../Trees/Quadtree.h"
#include "../Structs/Particles.h"

class NBody : public IModel
{
public:

    NBody(Json::Value config);
    void SingleGalaxy();
    void GalaxyCollision();
    virtual void Evaluate(double *state, double time, double *deriv);
    virtual double* GetInitialState();
    Quadtree* GetTree();
    const ParticleParameters* GetParticleParameters() const;
    int GetTotalParticles() const;
    Vector3D GetMassCenter() const;
    double GetTheta() const;
    void SetTheta(double theta);

private:

    void BuiltTree(const ParticleData2D &p);
    void GetOrbitalVelocity(const ParticleData2D &p1, const ParticleData2D &p2);
    void SimulationSettings(int num);

    ParticleState2D *particleState;
    ParticleParameters *particleParameters;
    Json::Value configuration;
    Quadtree quadtree;
    Vector2D cornerNW;
    Vector2D cornerSE;
    Vector2D massCenter;
    double areaOfInterest;
    const double year;
    const double massSun;
    const double pc;
    const double gravitationalConstant;
    const double g;
    int particles;
};

#endif

