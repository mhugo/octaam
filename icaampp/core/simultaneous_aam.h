#ifndef _SIMULTANEOUS_AAM_
#define _SIMULTANEOUS_AAM_

#include <vector>
#include "aam.h"

class SimultaneousAAM : public AAM {
 public:
  SimultaneousAAM();
  virtual ~SimultaneousAAM();

  virtual void precompute();
  virtual AAM::IterateReturn iterate(const Shape2D&, ColumnVector texParams, const Texture&);

 protected:
  virtual void _readFile(FILE* fp);

  Matrix gradient_x, gradient_y;

  std::vector<Matrix> jac_x;
  std::vector<Matrix> jac_y;

  std::vector<Matrix> nabla_a_x;
  std::vector<Matrix> nabla_a_y;

  std::vector<Matrix> sdi;
};

#endif
