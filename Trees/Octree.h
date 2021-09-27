#ifndef _OCTREE
#define _OCTREE

// Standard includes
#include <vector>

// Project includes
#include "../Structs/Vectors.h"
#include "../Structs/Particles.h"

class Octree
{
public:

  enum Octrant
  {
    UNE = 0,
    UNW,
    USW,
    USE,
    DNE,
    DNW,
    DSW,
    DSE,
    NONE
  };

  Octree(const Vector3D &min,
             const Vector3D &max,
             Octree *parent=nullptr);

  void Reset(const Vector3D &min,
             const Vector3D &max);

  bool IsRoot() const;
  bool IsExternal() const;
  bool IsDevided() const;

  void ClearStatistics();

  int GetAllNodesParticles() const;
  const Vector3D& GetMassCenter() const;
  const Vector3D& GetMinimumDimension() const;
  const Vector3D& GetMaximumDimension() const;

  double GetTheta() const;
  void SetTheta(double newTheta);

  void Insert(const ParticleData3D &newParticle, int level);

  Octrant GetOctrant(double x, double y, double z) const;
  Octree* CreateOctNode(Octrant Oct) ;

  void ComputeMassDistribution();

  Vector3D CalculateForce(const ParticleData3D &p) const;
  void DumpNode(int quad, int level);

public:

  Octree *octNode[8];

private:

  Vector3D CalculateAcceleration(const ParticleData3D &p1, const ParticleData3D &p2) const;
  Vector3D CalculateTreeForce(const ParticleData3D &p) const;

  ParticleData3D particleData;

  double nodeMass;     
  Vector3D massCenter;     
  Vector3D minBoxPosition; 
  Vector3D maxBoxPosition;       
  Vector3D nodeCenter;    
  Octree *parentNode;     
  int nodeParticlesCount;                
  mutable bool maxDivided;  

  static double theta;
  static std::vector<ParticleData3D> outsideParticles;
public:
  static double gravitationalConstant;

private:
  static double softening;
};

 #endif
