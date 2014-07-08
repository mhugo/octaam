#ifndef _NORMALIZING_AAM_
#define _NORMALIZING_AAM_

#include <vector>
#include "aam.h"

class NormalizingAAM : public AAM {
 public:
  NormalizingAAM();
  virtual ~NormalizingAAM();

  virtual void precompute();
  virtual AAM::IterateReturn iterate(const Shape2D&, ColumnVector texParams, const Texture&);

 protected:
  virtual void _readFile(FILE* fp);

  Matrix gradient_x, gradient_y;
  Matrix invH;

  std::vector<Matrix> jac_x;
  std::vector<Matrix> jac_y;

  std::vector<Matrix> nabla_a_x;
  std::vector<Matrix> nabla_a_y;

  std::vector<Matrix> sdi;
};

#endif
