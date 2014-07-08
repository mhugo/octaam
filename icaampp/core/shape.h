//
// (c) 2008 Hugo Mercier <hmercier31[at]gmail.com
//
// GNU GPL License 3
//

#ifndef _ICAAM_SHAPE_
#define _ICAAM_SHAPE_


#include <vector>
#include "newmat/newmat.h"
#include "texture.h"
#include "mytypes.h"

struct Triangle {
  int t1;
  int t2;
  int t3;
};

typedef std::vector<Triangle> Triangulation;


struct Point2D {
  real_t x,y;
};

struct IntPoint2D {
  int x,y;
};

class Shape2D : public std::vector<Point2D> {
 public:
  void fromVector(const ColumnVector& v);
  ReturnMatrix toVector() const;

  void reshape(double w, double h, double x = 0.0, double y = 0.0);

  Texture* computeTriangleMap(const Triangulation& tri, int w, int h);

  Texture* textureWarp(const Shape2D& dest_shape, const Triangulation& tri, const Texture& triangle_map, const Texture& image) const;

 private:
  
};


typedef std::vector<IntPoint2D> ScanSegment;

void scanLine(int x1, int y1, int x2, int y2, ScanSegment&);
void scanTriangle(int x1, int y1, int x2, int y2, int x3, int y3, ScanSegment& left, ScanSegment& right);

#endif
