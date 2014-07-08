#include <iostream>
#include "newmat/newmatio.h"
#include "aam.h"
#include "imagick.h"

void AAM::fromFile(const char* fileName) {
  FILE* fp = fopen(fileName, "r");
  _readFile(fp);
  fclose(fp);

  init();
  precompute();
}

void AAM::_readFile(FILE *fp) {
  std::cout << "aam readfile" << std::endl;

  short tmp1, tmp2;
  double tmpd;

  fread(&tmp1, sizeof(tmp1), 1, fp);
  fread(&tmp2, sizeof(tmp2), 1, fp);
  resX = tmp1;
  resY = tmp2;
  cout << "resX " << resX << endl;
  
  fread(&tmp1, sizeof(tmp1), 1, fp);
  fread(&tmp2, sizeof(tmp2), 1, fp);
  shapes.ReSize(tmp2, tmp1);
  int n_modes = tmp1;
  for (int j=1; j <= shapes.Ncols(); j++) {
    for (int i=1; i <= shapes.Nrows(); i++) {
      fread(&tmpd, sizeof(tmpd), 1, fp);
      shapes(i,j) = tmpd;
    }
  }

  meanShape = shapes.Column(1);
  shapes = shapes.Columns(2, shapes.Ncols());

  // v_geom
  geoms.ReSize(tmp2, 5);
  for (int j=1; j <= 5; j++) {
    for (int i=1; i <= geoms.Nrows(); i++) {
      fread(&tmpd, sizeof(tmpd), 1, fp);
      geoms(i,j) = tmpd;
    }
  }
  std::cout << "geoms" << std::endl;
  std::cout << geoms(4,5) << std::endl;

  // textures

  fread(&tmp1, sizeof(tmp1), 1, fp);
  fread(&tmp2, sizeof(tmp2), 1, fp);
  int n_tex_modes = tmp1;
  textures.ReSize(tmp2, tmp1);
  for (int j=1; j <= textures.Ncols(); j++) {
    for (int i=1; i <= textures.Nrows(); i++) {
      fread(&tmpd, sizeof(tmpd), 1, fp);
      textures(i,j) = tmpd;
    }
  }

  meanTex = textures.Column(1);
  std::cout << "s : " << textures.Ncols() << std::endl;
  textures = textures.Columns(2, textures.Ncols());
  std::cout << "s : " << textures.Ncols() << std::endl;

  
  fread(&tmp1, sizeof(tmp1), 1, fp);
  triangulation.resize(tmp1);
  char t1;
  for (int i=0; i < tmp1; i++) {
    fread(&t1, sizeof(char), 1, fp);
    triangulation[i].t1 = t1;
  }
  for (int i=0; i < tmp1; i++) {
    fread(&t1, sizeof(char), 1, fp);
    triangulation[i].t2 = t1;
  }
  for (int i=0; i < tmp1; i++) {
    fread(&t1, sizeof(char), 1, fp);
    triangulation[i].t3 = t1;
  }


  uchar* tmap = new uchar[resX*resY];
  for (int j=0; j < resX; j++) {
    for (int i=0; i < resY; i++) {
      fread(&t1, sizeof(char), 1, fp);
      tmap[i*resX+j] = t1;
    }
  }
  triangleMap.fromData(tmap, resX, resY, false, true);

}

void AAM::init() {

  //  computeGeometryVectors();


  // compute pseudo-inverse basis
  // TODO : Gram Schmidt

  iGeoms = (geoms.t() * geoms).i() * geoms.t();
  iShapes = shapes.t();
  iTextures = textures.t();



  // compute surrounding triangles

  int nPoints = meanShape.Nrows();
  for (int i=0; i < nPoints; i++) {
    // find triangles that includes this point
    int nTriangles = triangulation.size();

    std::vector<int> triList;
    for (int j=0; j < nTriangles; j++) {
      Triangle tr = triangulation[j];
      if ((tr.t1 == i) ||
	  (tr.t2 == i) ||
	  (tr.t3 == i)) {
	triList.push_back(j);
      }
    }
    surTriangles.push_back(triList);
  }
}

void AAM::computeGeometryVectors() {
  // compute geometry vectors from
  // the mean shape

  Matrix mean = meanShape;

  geoms.ReSize(mean.Nrows(), 5);

  int n = mean.Nrows() / 2;

  geoms.SubMatrix(1,n,1,1) = mean.Rows(1,n);
  geoms.SubMatrix(n+1,n*2,1,1) = 0.0;

  geoms.SubMatrix(1,n,2,2) = -mean.Rows(n+1,n*2);
  geoms.SubMatrix(n+1,n*2,2,2) = mean.Rows(1,n);

  geoms.SubMatrix(1,n,3,3) = 1.0;
  geoms.SubMatrix(n+1,n*2,3,3) = 0.0;

  geoms.SubMatrix(1,n,4,4) = 0.0;
  geoms.SubMatrix(n+1,n*2,4,4) = 1.0;
  
  geoms.SubMatrix(1,n,5,5) = 0.0;
  geoms.SubMatrix(n+1,n*2,5,5) = mean.Rows(n+1,n*2);

  // normalize
  for (int i=1; i <= 5; i++) {
    double n = geoms.Column(i).NormFrobenius();
    geoms.Column(i) = geoms.Column(i) / n;
  }

  std::cout << geoms << std::endl;
}


ReturnMatrix AAM::shapeGetShapeParams(const Shape2D& sh) {
  ColumnVector shape = sh.toVector();

  ColumnVector p = iShapes * (shape - meanShape);
  p.Release();
  return p;
}

ReturnMatrix AAM::textureGetParams(const Texture& tex) {
  ColumnVector text_mat = tex.toVector();

  ColumnVector p = iTextures * (text_mat - meanTex);
  p.Release();
  return p;
}

void AAM::precompute() {
  std::cout << "AAM::precompute" << std::endl;
}


//
// in :
//  * current shape
//  * current texture parameters
//  * projection of current image
// out :
//  * next parameters : shape and texture parameters


AAM::IterateReturn AAM::iterate(const Shape2D& current_s, ColumnVector texParams, const Texture& projTex) {

  // nothing to iterate in base class

  AAM::IterateReturn r;
  r.newShape = current_s;
  r.newTextureParams = texParams;
  r.error = 0.0;
  return r;
}

void AAM::warpComposition(ColumnVector& delta_s0, const Shape2D& current_s, Shape2D& new_s)
{
  Shape2D psi_p, delta;
  psi_p.fromVector(meanShape);
  psi_p.reshape(resX, resY);
  delta.fromVector(delta_s0);
  new_s = psi_p;

  for (int i=0; i < psi_p.size(); i++) {
    
    double tx = psi_p[i].x + delta[i].x;
    double ty = psi_p[i].y + delta[i].y;
    
    double vjx = 0.0;
    double vjy = 0.0;
    int nTri = surTriangles[i].size();
    for (int j=0; j < nTri; j++) {
      int trij = surTriangles[i][j];
      Point2D v0 = psi_p[triangulation[trij].t1];
      Point2D v1 = psi_p[triangulation[trij].t2];
      Point2D v2 = psi_p[triangulation[trij].t3];
      
      Point2D vt0 = current_s[triangulation[trij].t1];
      Point2D vt1 = current_s[triangulation[trij].t2];
      Point2D vt2 = current_s[triangulation[trij].t3];
      
      double denum = (v1.x - v0.x) * (v2.y - v0.y) - (v1.y - v0.y) * (v2.x - v0.x);
      
      double a = ((tx - v0.x) * (v2.y - v0.y) - (ty - v0.y) * (v2.x - v0.x)) / denum;
      double b = ((ty - v0.y) * (v1.x - v0.x) - (tx - v0.x) * (v1.y - v0.y)) / denum;
      
      vjx += vt0.x + a * (vt1.x - vt0.x) + b * (vt2.x - vt0.x);
      vjy += vt0.y + a * (vt1.y - vt0.y) + b * (vt2.y - vt0.y);
    }
    new_s[i].x = vjx / double(nTri);
    new_s[i].y = vjy / double(nTri);
  }

}
