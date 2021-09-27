#include "IModel.h"

IModel::IModel(const std::string &sName, unsigned dim) : dimension(dim)
{}

unsigned IModel::GetSimulationDimension() const
{
  return dimension;
}

void IModel::SetSimulationDimension(unsigned dim)
{
  dimension = dim;
}

std::string IModel::GetName() const
{
  return name;
}
