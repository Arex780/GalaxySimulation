// Standard includes
#include <cstdio>
#include <cstring>
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <sstream>

// Project includes
#include "Quadtree.h"

// Static variables
double Quadtree::theta = 1.0;
std::vector<ParticleData2D> Quadtree::outsideParticles;
double Quadtree::gravitationalConstant = 0;
double Quadtree::softening = 0.01;

Quadtree::Quadtree(const Vector2D &min,
                       const Vector2D &max,
                       Quadtree *parent)
  :particleData()
  ,nodeMass(0)
  ,massCenter()
  ,minBoxPosition(min)
  ,maxBoxPosition(max)
  ,nodeCenter(min.x+(max.x-min.x)/2.0, min.y+(max.y-min.y)/2.0)
  ,parentNode(parent)
  ,nodeParticlesCount(0)
  ,maxDivided(false)
{
  quadNode[0] = quadNode[1] = quadNode[2] = quadNode[3] = NULL;
}

bool Quadtree::IsRoot() const
{
  return parentNode==NULL;
}

bool Quadtree::IsExternal() const
{
  return  quadNode[0]==NULL &&
          quadNode[1]==NULL &&
          quadNode[2]==NULL &&
          quadNode[3]==NULL;
}

bool Quadtree::IsDevided() const
{
  return maxDivided;
}

const Vector2D& Quadtree::GetMinimumDimension() const
{
  return minBoxPosition;
}

const Vector2D& Quadtree::GetMaximumDimension() const
{
  return maxBoxPosition;
}

const Vector2D& Quadtree::GetMassCenter() const
{
  return massCenter;
}

double Quadtree::GetTheta() const
{
  return theta;
}

void Quadtree::SetTheta(double newTheta)
{
  theta = newTheta;
}

int Quadtree::GetAllNodesParticles() const
{
  return nodeParticlesCount;
}

void Quadtree::ClearStatistics()
{
  if (!IsRoot())
    throw std::runtime_error("Only the root node may reset statistics data.");

  struct ResetSubdivideFlags
  {
    ResetSubdivideFlags(Quadtree *pRoot)
    {
      ResetFlag(pRoot);
    }

    void ResetFlag(Quadtree *pNode)
    {
      pNode->maxDivided = false;
      for (int i=0; i<4;++i)
      {
        if (pNode->quadNode[i])
          ResetFlag(pNode->quadNode[i]);
      }
    }
  } ResetFlagNow(this);
}

void Quadtree::Reset(const Vector2D &min,
                       const Vector2D &max)
{
  if (!IsRoot())
    throw std::runtime_error("Only the root node may reset the tree.");

  for (int i=0; i<4; ++i)
  {
    delete quadNode[i];
    quadNode[i] = NULL;
  }

  minBoxPosition = min;
  maxBoxPosition = max;
  nodeCenter = Vector2D(min.x + (max.x-min.x)/2.0,
                   min.y + (max.y-min.y)/2.0);
  nodeParticlesCount = 0;
  nodeMass = 0;
  massCenter = Vector2D(0, 0);

  outsideParticles.clear();
}

Quadtree::Quadrant Quadtree::GetQuadrant(double x, double y) const
{
  if (x<=nodeCenter.x && y<=nodeCenter.y)
  {
    return SW;
  }
  else if (x<=nodeCenter.x && y>=nodeCenter.y)
  {
    return NW;
  }
  else if (x>=nodeCenter.x && y>=nodeCenter.y)
  {
    return NE;
  }
  else if (x>=nodeCenter.x && y<=nodeCenter.y)
  {
    return SE;
  }
  else if (x>maxBoxPosition.x || y>maxBoxPosition.y || x<minBoxPosition.x || y<minBoxPosition.y)
  {
    std::stringstream ss;
    ss << "Can't determine quadrant!\n"
       << "particle  : " << "(" << x          << ", " << y          << ")\n"
       << "quadMin   : " << "(" << minBoxPosition.x    << ", " << minBoxPosition.y    << ")\n"
       << "quadMax   : " << "(" << maxBoxPosition.x    << ", " << maxBoxPosition.y    << ")\n"
       << "quadCenter: " << "(" << nodeCenter.x << ", " << nodeCenter.y << ")\n";
    throw std::runtime_error(ss.str().c_str());
  }
  else
  {
    throw std::runtime_error("Can't determine quadrant!");
  }
}

Quadtree* Quadtree::CreateQuadNode(Quadrant Quad)
{
  switch (Quad)
  {
  case SW: return new Quadtree(minBoxPosition, nodeCenter, this);
  case NW: return new Quadtree(Vector2D(minBoxPosition.x, nodeCenter.y),
                                 Vector2D(nodeCenter.x, maxBoxPosition.y),
                                 this);
  case NE: return new Quadtree(nodeCenter, maxBoxPosition, this);
  case SE: return new Quadtree(Vector2D(nodeCenter.x, minBoxPosition.y),
                                 Vector2D(maxBoxPosition.x, nodeCenter.y),
                                 this);
  default:
        {
          std::stringstream ss;
          ss << "Can't determine quadrant!\n";
          throw std::runtime_error(ss.str().c_str());
        }
  }
}

void Quadtree::ComputeMassDistribution()
{

  if (nodeParticlesCount==1)
  {
    ParticleState2D *state = particleData.particleState;
    ParticleParameters *parameters = particleData.particleParameters;
    assert(state);
    assert(parameters);

    nodeMass = parameters->mass;
    massCenter = Vector2D(state->positionX, state->positionY);
  }
  else
  {
    nodeMass = 0;
    massCenter = Vector2D(0, 0);

    for (int i=0; i<4; ++i)
    {
      if (quadNode[i])
      {
        quadNode[i]->ComputeMassDistribution();
        nodeMass += quadNode[i]->nodeMass;
        massCenter.x += quadNode[i]->massCenter.x * quadNode[i]->nodeMass;
        massCenter.y += quadNode[i]->massCenter.y * quadNode[i]->nodeMass;
      }
    }

    massCenter.x /= nodeMass;
    massCenter.y /= nodeMass;
  }
}

Vector2D Quadtree::CalculateAcceleration(const ParticleData2D &p1, const ParticleData2D &p2) const
{
  Vector2D acceleration;

  if (&p1==&p2)
    return acceleration;

  const double &x1(p1.particleState->positionX),
               &y1(p1.particleState->positionY);
  const double &x2(p2.particleState->positionX),
               &y2(p2.particleState->positionY),
               &m2(p2.particleParameters->mass);


  double r = sqrt( (x1 - x2) * (x1 - x2) +
                   (y1 - y2) * (y1 - y2) + softening);
  if (r>0)
  {
    double k = gravitationalConstant * m2 / (r*r*r);

    acceleration.x += k * (x2 - x1);
    acceleration.y += k * (y2 - y1);
  }
  else
  {
    acceleration.x = acceleration.y = 0;
  }

  return acceleration;
}

Vector2D Quadtree::CalculateForce(const ParticleData2D &p1) const
{
  // Calculate the force from the tree to the particle p1
  Vector2D acceleration = CalculateTreeForce(p1);

  // Calculate the force from particles not in the tree
  if (outsideParticles.size())
  {
    for (std::size_t i=0; i<outsideParticles.size(); ++i)
    {
      Vector2D buffer = CalculateAcceleration(p1, outsideParticles[i]);
      acceleration.x += buffer.x;
      acceleration.y += buffer.y;
    }
  }

  return acceleration;
}

Vector2D Quadtree::CalculateTreeForce(const ParticleData2D &p1) const
{
  Vector2D acceleration;

  double r(0), k(0), d(0);
  if (nodeParticlesCount==1)
  {
    acceleration = CalculateAcceleration(p1, particleData);
  }
  else
  {
    r = sqrt( (p1.particleState->positionX - massCenter.x) * (p1.particleState->positionX - massCenter.x) +
              (p1.particleState->positionY - massCenter.y) * (p1.particleState->positionY - massCenter.y) );
    d = maxBoxPosition.x - minBoxPosition.x;
    if (d/r <= theta)
    {
      maxDivided = false;
      k = gravitationalConstant * nodeMass / (r*r*r);
      acceleration.x = k * (massCenter.x - p1.particleState->positionX);
      acceleration.y = k * (massCenter.y - p1.particleState->positionY);
    }
    else
    {

      maxDivided = true;
      Vector2D buffer;
      for (int q=0; q<4; ++q)
      {
        if (quadNode[q])
        {
          buffer = quadNode[q]->CalculateTreeForce(p1);
          acceleration.x += buffer.x;
          acceleration.y += buffer.y;
        }
      }
    }
  }

  return acceleration;
}

void Quadtree::DumpNode(int quad, int level)
{
  for (int i=0; i<4;++i)
  {
    if (quadNode[i])
    {
      quadNode[i]->DumpNode(i, level+1);
    }
  }
}

void Quadtree::Insert(const ParticleData2D &newParticle, int level)
{
  const ParticleState2D &p1 = *(newParticle.particleState);
  if ( (p1.positionX < minBoxPosition.x || p1.positionX > maxBoxPosition.x) || (p1.positionY < minBoxPosition.y || p1.positionY > maxBoxPosition.y) )
  {
    std::stringstream ss;
    ss << "Particle position (" << p1.positionX << ", " << p1.positionY << ") "
       << "is outside tree node ("
       << "min.x=" << minBoxPosition.x << ", "
       << "max.x=" << maxBoxPosition.x << ", "
       << "min.y=" << minBoxPosition.y << ", "
       << "max.y=" << maxBoxPosition.y << ")";
    throw std::runtime_error(ss.str());
  }

  if (nodeParticlesCount>1)
  {
    Quadrant Quad = GetQuadrant(p1.positionX, p1.positionY);
    if (!quadNode[Quad])
      quadNode[Quad] = CreateQuadNode(Quad);

    quadNode[Quad]->Insert(newParticle, level+1);
  }
  else if (nodeParticlesCount==1)
  {
    assert(IsExternal() || IsRoot());

    const ParticleState2D &p2 = *(particleData.particleState);

    if ( (p1.positionX == p2.positionX) && (p1.positionY == p2.positionY) )
    {
      outsideParticles.push_back(newParticle);
    }
    else
    {
      Quadrant Quad = GetQuadrant(p2.positionX, p2.positionY);
      if (quadNode[Quad]==NULL)
        quadNode[Quad] = CreateQuadNode(Quad);
      quadNode[Quad]->Insert(particleData, level+1);
      particleData.Reset();

      Quad = GetQuadrant(p1.positionX, p1.positionY);
      if (!quadNode[Quad])
        quadNode[Quad] = CreateQuadNode(Quad);
      quadNode[Quad]->Insert(newParticle, level+1);
    }
  }
  else if (nodeParticlesCount==0)
  {
    particleData = newParticle;
  }

  nodeParticlesCount++;
}