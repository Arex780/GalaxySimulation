#ifndef _QUADTREE
#define _QUADTREE

// Standard includes
#include <vector>

// Project includes
#include "../Structs/Vectors.h"
#include "../Structs/Particles.h"

class Quadtree
{
public:

  enum Quadrant
  {
    NE = 0,
    NW,
    SW,
    SE,
    NONE
  };

  Quadtree(const Vector2D &min,
             const Vector2D &max,
             Quadtree *parent=nullptr);

  void Reset(const Vector2D &min,
             const Vector2D &max);

  bool IsRoot() const;
  bool IsExternal() const;
  bool IsDevided() const;

  void ClearStatistics();

  int GetAllNodesParticles() const;
  const Vector2D& GetMassCenter() const;
  const Vector2D& GetMinimumDimension() const;
  const Vector2D& GetMaximumDimension() const;

  double GetTheta() const;
  void SetTheta(double newTheta);

  void Insert(const ParticleData2D &newParticle, int level);

  Quadrant GetQuadrant(double x, double y) const;
  Quadtree* CreateQuadNode(Quadrant Quad) ;

  void ComputeMassDistribution();

  Vector2D CalculateForce(const ParticleData2D &p) const;
  void DumpNode(int quad, int level);

public:

  Quadtree *quadNode[4];

private:

  Vector2D CalculateAcceleration(const ParticleData2D &p1, const ParticleData2D &p2) const;
  Vector2D CalculateTreeForce(const ParticleData2D &p) const;

  ParticleData2D particleData;

  double nodeMass;    
  Vector2D massCenter;     
  Vector2D minBoxPosition;    
  Vector2D maxBoxPosition;        
  Vector2D nodeCenter;     
  Quadtree *parentNode;  
  int nodeParticlesCount;   
  mutable bool maxDivided;  

  static double theta;
  static std::vector<ParticleData2D> outsideParticles;

public:
  static double gravitationalConstant;

private:
  static double softening;
};

 #endif