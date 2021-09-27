#ifndef _PARTICLES
#define _PARTICLES

// Pack structure members
#pragma pack(push, 1)

struct ParticleState2D
{
  double positionX;
  double positionY;
  double velocityX;
  double velocityY;
};

struct ParticleNextState2D
{
  double velocityX;
  double velocityY;
  double accelerationX;
  double accelerationY;
};

struct ParticleState3D
{
  double positionX;
  double positionY;
  double positionZ;
  double velocityX;
  double velocityY;
  double velocityZ;
};

struct ParticleNextState3D
{
  double velocityX;
  double velocityY;
  double velocityZ;
  double accelerationX;
  double accelerationY;
  double accelerationZ;
};

struct ParticleParameters
{
  double mass;
  double radius;
};

#pragma pack(pop)

struct ParticleData2D
{
  ParticleData2D();
  ParticleData2D(ParticleState2D *state, ParticleParameters *parameters);
  ParticleData2D(const ParticleData2D &ref);
  ParticleData2D& operator=(const ParticleData2D &ref);

  void Reset();
  bool IsNull() const;

  ParticleState2D *particleState;
  ParticleParameters *particleParameters;
};

struct ParticleData3D
{
  ParticleData3D();
  ParticleData3D(ParticleState3D *state, ParticleParameters *parameters);
  ParticleData3D(const ParticleData3D &ref);
  ParticleData3D& operator=(const ParticleData3D &ref);

  void Reset();
  bool IsNull() const;

  ParticleState3D *particleState;
  ParticleParameters *particleParameters;
};

#endif