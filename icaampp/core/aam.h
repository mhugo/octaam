#ifndef _AAM_
#define _AAM_

#include <vector>
#include <stdio.h>
#include "newmat/newmat.h"
#include "texture.h"
#include "shape.h"

class AAM {
 public:
  AAM() {}
  virtual ~AAM() {};

  struct IterateReturn {
    Shape2D newShape;
    ColumnVector newTextureParams;
    double error;
  };

  virtual void fromFile(const char*);

  Matrix* getShapeVectors()      { return &shapes; }
  Matrix* getTextureVectors()    { return &textures; }
  Matrix* getGeometryVectors()   { return &geoms; }

  int getNGeoms() const          { return geoms.Ncols(); }
  int getNShapes() const         { return shapes.Ncols(); }
  int getNTextures() const       { return textures.Ncols(); }

  int getXres() const            { return resX;  }
  int getYres() const            { return resY;  }

  ColumnVector& getMeanShape()   { return meanShape; }
  ColumnVector& getMeanTexture() { return meanTex; }

  const Texture& getTriangleMap() const { return triangleMap; }

  const Triangulation& getTriangulation() const { return triangulation; };

  ReturnMatrix shapeGetGeomParams(const Shape2D&);
  ReturnMatrix shapeGetShapeParams(const Shape2D&);
  ReturnMatrix textureGetParams(const Texture& tex);


  virtual void precompute();
  virtual IterateReturn iterate(const Shape2D&, ColumnVector texParams, const Texture&);

 protected:

  void warpComposition(ColumnVector& delta_s0, const Shape2D& current_s, Shape2D& new_s);

  virtual void _readFile(FILE* fp);

  void init();

  void computeGeometryVectors();

  // s1 ... si
  Matrix shapes;
  // t1 ... ti
  Matrix textures;

  ColumnVector meanShape;
  ColumnVector meanTex;

  //
  Matrix geoms;

  // pseudo inverse basis
  Matrix iGeoms, iShapes, iTextures;

  // surrounding triangles
  std::vector<std::vector<int> > surTriangles;

  Triangulation triangulation;

  Texture triangleMap;

  // texture resolution
  int resX, resY;
};

#endif
