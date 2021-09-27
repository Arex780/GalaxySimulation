// Standard includes
#include <cstdio>
#include <cstring>
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <sstream>

// Project includes
#include "Octree.h"

// Static variables
double Octree::theta = 0.5;
std::vector<ParticleData3D> Octree::outsideParticles;
double Octree::gravitationalConstant = 0;
double Octree::softening = 0.01; 

Octree::Octree(const Vector3D &min,
                       const Vector3D &max,
                       Octree *parent)
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
  octNode[0] = octNode[1] = octNode[2] = octNode[3] = octNode[4] = octNode[5] = octNode[6] = octNode[7] = NULL;
}

bool Octree::IsRoot() const
{
  return parentNode==NULL;
}

bool Octree::IsExternal() const
{
  return  octNode[0]==NULL &&
          octNode[1]==NULL &&
          octNode[2]==NULL &&
          octNode[3]==NULL &&
          octNode[4]==NULL &&
          octNode[5]==NULL &&
          octNode[6]==NULL &&
          octNode[7]==NULL;
}

bool Octree::IsDevided() const
{
  return maxDivided;
}

const Vector3D& Octree::GetMinimumDimension() const
{
  return minBoxPosition;
}

const Vector3D& Octree::GetMaximumDimension() const
{
  return maxBoxPosition;
}

const Vector3D& Octree::GetMassCenter() const
{
  return massCenter;
}

double Octree::GetTheta() const
{
  return theta;
}

void Octree::SetTheta(double newTheta)
{
  theta = newTheta;
}

int Octree::GetAllNodesParticles() const
{
  return nodeParticlesCount;
}

void Octree::ClearStatistics()
{
  if (!IsRoot())
    throw std::runtime_error("Only the root node may reset statistics data.");

  struct ResetSubdivideFlags
  {
    ResetSubdivideFlags(Octree *pRoot)
    {
      ResetFlag(pRoot);
    }

    void ResetFlag(Octree *pNode)
    {
      pNode->maxDivided = false;
      for (int i=0; i<8;++i)
      {
        if (pNode->octNode[i])
          ResetFlag(pNode->octNode[i]);
      }
    }
  } ResetFlagNow(this);
}

void Octree::Reset(const Vector3D &min,
                       const Vector3D &max)
{
  if (!IsRoot())
    throw std::runtime_error("Only the root node may reset the tree.");

  for (int i=0; i<8; ++i)
  {
    delete octNode[i];
    octNode[i] = NULL;
  }

  minBoxPosition = min;
  maxBoxPosition = max;
  nodeCenter = Vector3D(min.x + (max.x-min.x)/2.0,
                   min.y + (max.y-min.y)/2.0,
                   min.z + (max.z-min.z)/2.0);
  nodeParticlesCount = 0;
  nodeMass = 0;
  massCenter = Vector3D(0, 0, 0);

  outsideParticles.clear();
}

Octree::Octrant Octree::GetOctrant(double x, double y, double z) const
{
  if (x<=nodeCenter.x && y<=nodeCenter.y && z>=nodeCenter.z)
  {
    return USW;
  }
  else if (x<=nodeCenter.x && y>=nodeCenter.y && z>=nodeCenter.z)
  {
    return UNW;
  }
  else if (x>=nodeCenter.x && y>=nodeCenter.y && z>=nodeCenter.z)
  {
    return UNE;
  }
  else if (x>=nodeCenter.x && y<=nodeCenter.y && z>=nodeCenter.z)
  {
    return USE;
  }
  else if (x<=nodeCenter.x && y<=nodeCenter.y && z<=nodeCenter.z)
  {
    return DSW;
  }
  else if (x<=nodeCenter.x && y>=nodeCenter.y && z<=nodeCenter.z)
  {
    return DNW;
  }
  else if (x>=nodeCenter.x && y>=nodeCenter.y && z<=nodeCenter.z)
  {
    return DNE;
  }
  else if (x>=nodeCenter.x && y<=nodeCenter.y && z<=nodeCenter.z)
  {
    return DSE;
  }
  else if (x>maxBoxPosition.x || y>maxBoxPosition.y || z>maxBoxPosition.z || x<minBoxPosition.x || y<minBoxPosition.y || z<minBoxPosition.z)
  {
    std::stringstream ss;
    ss << "Can't determine octrant!\n"
       << "particle  : " << "(" << x          << ", " << y          << ", " << z          << ")\n"
       << "octMin   : " << "(" << minBoxPosition.x    << ", " << minBoxPosition.y    << ", " << minBoxPosition.z    << ")\n"
       << "octMax   : " << "(" << maxBoxPosition.x    << ", " << maxBoxPosition.y    << ", " << maxBoxPosition.z    << ")\n"
       << "octCenter: " << "(" << nodeCenter.x << ", " << nodeCenter.y << ", " << nodeCenter.z << ")\n";
    throw std::runtime_error(ss.str().c_str());
  }
  else
  {
    throw std::runtime_error("Can't determine octrant!");
  }
}

Octree* Octree::CreateOctNode(Octrant Oct)
{
  switch (Oct)
  {
  case USW: return new Octree(minBoxPosition, nodeCenter, this);
  case UNW: return new Octree(Vector3D(minBoxPosition.x, nodeCenter.y), Vector3D(nodeCenter.x, maxBoxPosition.y), this);
  case UNE: return new Octree(nodeCenter, maxBoxPosition, this);
  case USE: return new Octree(Vector3D(nodeCenter.x, minBoxPosition.y), Vector3D(maxBoxPosition.x, nodeCenter.y), this);
  case DSW: return new Octree(maxBoxPosition, nodeCenter, this);
  case DNW: return new Octree(Vector3D(minBoxPosition.x, nodeCenter.y), Vector3D(nodeCenter.x, maxBoxPosition.y), this);
  case DNE: return new Octree(nodeCenter, minBoxPosition, this);
  case DSE: return new Octree(Vector3D(nodeCenter.x, minBoxPosition.y), Vector3D(maxBoxPosition.x, nodeCenter.y), this);
  default:
        {
          std::stringstream ss;
          ss << "Can't determine octrant!\n";
          throw std::runtime_error(ss.str().c_str());
        }
  }
}

void Octree::ComputeMassDistribution()
{

  if (nodeParticlesCount==1)
  {
    ParticleState3D *state = particleData.particleState;
    ParticleParameters *parameters = particleData.particleParameters;
    assert(state);
    assert(parameters);

    nodeMass = parameters->mass;
    massCenter = Vector3D(state->positionX, state->positionY, state->positionZ);
  }
  else
  {
    nodeMass = 0;
    massCenter = Vector3D(0, 0, 0);

    for (int i=0; i<8; ++i)
    {
      if (octNode[i])
      {
        octNode[i]->ComputeMassDistribution();
        nodeMass += octNode[i]->nodeMass;
        massCenter.x += octNode[i]->massCenter.x * octNode[i]->nodeMass;
        massCenter.y += octNode[i]->massCenter.y * octNode[i]->nodeMass;
        massCenter.z += octNode[i]->massCenter.z * octNode[i]->nodeMass;
      }
    }

    massCenter.x /= nodeMass;
    massCenter.y /= nodeMass;
    massCenter.z /= nodeMass;
  }
}

Vector3D Octree::CalculateAcceleration(const ParticleData3D &p1, const ParticleData3D &p2) const
{
  Vector3D acceleration;

  if (&p1==&p2)
    return acceleration;

  const double &x1(p1.particleState->positionX),
               &y1(p1.particleState->positionY),
               &z1(p1.particleState->positionZ);
  const double &x2(p2.particleState->positionX),
               &y2(p2.particleState->positionY),
               &z2(p2.particleState->positionZ),
               &m2(p2.particleParameters->mass);

  double r = sqrt( (x1 - x2) * (x1 - x2) +
                   (y1 - y2) * (y1 - y2) + 
                   (z1 - z2) * (z1 - z2) + softening);

  if (r>0)
  {
    double k = gravitationalConstant * m2 / (r*r*r);

    acceleration.x += k * (x2 - x1);
    acceleration.y += k * (y2 - y1);
    acceleration.z += k * (z2 - z1);
  } 
  else
  {
    acceleration.x = acceleration.y = acceleration.z = 0;
  }

  return acceleration;
}

Vector3D Octree::CalculateForce(const ParticleData3D &p1) const
{
  // Calculate the force from the tree to the particle p1
  Vector3D acceleration = CalculateTreeForce(p1);

  // Calculate the force from particles not in the tree
  if (outsideParticles.size())
  {
    for (std::size_t i=0; i<outsideParticles.size(); ++i)
    {
      Vector3D buffer = CalculateAcceleration(p1, outsideParticles[i]);
      acceleration.x += buffer.x;
      acceleration.y += buffer.y;
      acceleration.z += buffer.z;
    }
  }

  return acceleration;
}

Vector3D Octree::CalculateTreeForce(const ParticleData3D &p1) const
{
  Vector3D acceleration;

  double r(0), k(0), d(0);
  if (nodeParticlesCount==1)
  {
    acceleration = CalculateAcceleration(p1, particleData);
  }
  else
  {
    r = sqrt( (p1.particleState->positionX - massCenter.x) * (p1.particleState->positionX - massCenter.x) +
              (p1.particleState->positionY - massCenter.y) * (p1.particleState->positionY - massCenter.y) +
              (p1.particleState->positionZ - massCenter.z) * (p1.particleState->positionZ - massCenter.z) );
    d = maxBoxPosition.x - minBoxPosition.x;
    if (d/r <= theta)
    {
      maxDivided = false;
      k = gravitationalConstant * nodeMass / (r*r*r);
      acceleration.x = k * (massCenter.x - p1.particleState->positionX);
      acceleration.y = k * (massCenter.y - p1.particleState->positionY);
      acceleration.z = k * (massCenter.z - p1.particleState->positionZ);
    }
    else
    {

      maxDivided = true;
      Vector3D buffer;
      for (int q=0; q<8; ++q)
      {
        if (octNode[q])
        {
          buffer = octNode[q]->CalculateTreeForce(p1);
          acceleration.x += buffer.x;
          acceleration.y += buffer.y;
          acceleration.z += buffer.z;
        } 
      } 
    }
  }

  return acceleration;
}

void Octree::DumpNode(int quad, int level)
{
  for (int i=0; i<8;++i)
  {
    if (octNode[i])
    {
      octNode[i]->DumpNode(i, level+1);
    }
  }
}

void Octree::Insert(const ParticleData3D &newParticle, int level)
{
  const ParticleState3D &p1 = *(newParticle.particleState);
  if ( (p1.positionX < minBoxPosition.x || p1.positionX > maxBoxPosition.x) || (p1.positionY < minBoxPosition.y || p1.positionY > maxBoxPosition.y) || (p1.positionZ < minBoxPosition.z || p1.positionZ > maxBoxPosition.z) )
  {
    std::stringstream ss;
    ss << "Particle position (" << p1.positionX << ", " << p1.positionY << ", " << p1.positionZ << ") "
       << "is outside tree node ("
       << "min.x=" << minBoxPosition.x << ", "
       << "max.x=" << maxBoxPosition.x << ", "
       << "min.y=" << minBoxPosition.y << ", "
       << "max.y=" << maxBoxPosition.y << ", "
       << "min.z=" << minBoxPosition.z << ", "
       << "max.z=" << maxBoxPosition.z << ")";
    throw std::runtime_error(ss.str());
  }

  if (nodeParticlesCount>1)
  {
    Octrant Oct = GetOctrant(p1.positionX, p1.positionY, p1.positionZ);
    if (!octNode[Oct])
      octNode[Oct] = CreateOctNode(Oct);

    octNode[Oct]->Insert(newParticle, level+1);
  }
  else if (nodeParticlesCount==1)
  {
    assert(IsExternal() || IsRoot());

    const ParticleState3D &p2 = *(particleData.particleState);

    if ( (p1.positionX == p2.positionX) && (p1.positionY == p2.positionY) && (p1.positionZ == p2.positionZ) )
    {
      outsideParticles.push_back(newParticle);
    }
    else
    {
      Octrant Oct = GetOctrant(p2.positionX, p2.positionY, p2.positionZ);
      if (octNode[Oct]==NULL)
        octNode[Oct] = CreateOctNode(Oct);
      octNode[Oct]->Insert(particleData, level+1);
      particleData.Reset();

      Oct = GetOctrant(p1.positionX, p1.positionY, p1.positionZ);
      if (!octNode[Oct])
        octNode[Oct] = CreateOctNode(Oct);
      octNode[Oct]->Insert(newParticle, level+1);
    }
  }
  else if (nodeParticlesCount==0)
  {
    particleData = newParticle;
  }

  nodeParticlesCount++;
}