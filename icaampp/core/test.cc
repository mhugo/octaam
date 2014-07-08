
#include <iostream>

#include "newmat/newmat.h"
#include "newmat/newmatio.h"
#include "mytypes.h"
#include "texture.h"
#include "simultaneous_aam.h"
#include "shape.h"
#include "imagick.h"

int main() {

  Matrix m(5,5);

  m = 0.0;

  m.SubMatrix(1,1,3,5) = 2.0;

  std::cout << m << std::endl;

  SimultaneousAAM aam;

  aam.fromFile("../set/set.dat");

  const Texture& tex = aam.getTriangleMap();
  Magick::Image* img = textureToImage(tex);

  //  img->depth(8);
  std::cout << img->depth() << std::endl;

  img->write("tmap.bmp");


  const Triangulation& tri = aam.getTriangulation();
  Matrix* shapes = aam.getShapeVectors();

  Texture* psi_tex = new Texture(48,48);
  ColumnVector psi = aam.getMeanShape();
  Shape2D psi_shape;
  psi_shape.fromVector(psi);
  psi_shape.reshape(48,48);

  for (int i=0; i < tri.size(); i++) {
    Point2D p1 = psi_shape[tri[i].t1];
    Point2D p2 = psi_shape[tri[i].t2];
    Point2D p3 = psi_shape[tri[i].t3];
    ScanSegment left;
    ScanSegment right;

    scanTriangle(int(p1.x), int(p1.y),
		 int(p2.x), int(p2.y),
		 int(p3.x), int(p3.y),
		 left, right);

    for (int j=0; j < left.size(); j++) {
      int x1 = left[j].x;
      int x2 = right[j].x;
      int y = left[j].y;

      for (int k=0; k < x2-x1; k++) {
	psi_tex->setPixel(x1+k, y, i);
      }
    }
  }

  Magick::Image* img2 = textureToImage(*psi_tex);
  img2->write("tmap2.bmp");
  delete img2;
  delete psi_tex;

  delete img;

  return 0;
}
