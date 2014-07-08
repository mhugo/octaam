#ifndef _PROJECT_OUT_AAM_
#define _PROJECT_OUT_AAM_

#include <vector>
#include "aam.h"

#include "timemeasure.h"

class ProjectOutAAM : public AAM {
 public:
  ProjectOutAAM();
  virtual ~ProjectOutAAM();

  virtual void precompute();
  virtual AAM::IterateReturn iterate(const Shape2D&, ColumnVector texParams, const Texture&);

  // UGLY !!
  TimeMeasure time;

 protected:
  virtual void _readFile(FILE* fp);

  Matrix invH;
  Matrix gradient_x, gradient_y;

  std::vector<Matrix> jac_x;
  std::vector<Matrix> jac_y;

  std::vector<Matrix> nabla_a_x;
  std::vector<Matrix> nabla_a_y;

  std::vector<Matrix> sdi;
};

#endif
