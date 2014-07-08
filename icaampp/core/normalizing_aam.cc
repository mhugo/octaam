#include <iostream>
#include "newmat/newmatio.h"
#include "imagick.h"
#include "normalizing_aam.h"

using namespace std;

NormalizingAAM::NormalizingAAM() : AAM() {
}

NormalizingAAM::~NormalizingAAM() {
}


void NormalizingAAM::_readFile(FILE* fp) {

  AAM::_readFile(fp);

  short tmp1, tmp2;
  double tmpd;

  std::cout << "normalizing readfile" << std::endl;

  int n_geoms = geoms.Ncols();
  int n_shapes = shapes.Ncols();
  int n_texs = textures.Ncols();

  std::cout << "n_geoms : " << n_geoms << std::endl;
  std::cout << "n_shapes : " << n_shapes << std::endl;
  std::cout << "n_texs : " << n_texs << std::endl;

  // GX
  gradient_x.ReSize(resY, resX);
  gradient_y.ReSize(resY, resX);
  for (int j=1; j <= resX; j++) {
    for (int i=1; i <= resY; i++) {
      fread(&tmpd, sizeof(tmpd), 1, fp);
      gradient_x(i,j) = tmpd;
    }
  }  
  for (int j=1; j <= resX; j++) {
    for (int i=1; i <= resY; i++) {
      fread(&tmpd, sizeof(tmpd), 1, fp);
      gradient_y(i,j) = tmpd;
    }
  }  


  // jacobians
  int n = n_shapes + n_geoms;

  for (int i=0; i < n; i++) {
    Matrix m(resY, resX);
    for (int j=1; j <= resX; j++) {
      for (int k=1; k <= resY; k++) {
	fread(&tmpd, sizeof(tmpd), 1, fp);
	m(k,j) = tmpd;
      }
    }
    jac_x.push_back(m);
  }
  for (int i=0; i < n; i++) {
    Matrix m(resY, resX);
    for (int j=1; j <= resX; j++) {
      for (int k=1; k <= resY; k++) {
	fread(&tmpd, sizeof(tmpd), 1, fp);
	m(k,j) = tmpd;
      }
    }
    jac_y.push_back(m);
  }
  std::cout << jac_y[0](14,3) << std::endl;
  std::cout << jac_y[2](14,3) << std::endl;

  // nabla_a
  //

  int n_nabla = n_texs;

  for (int i=0; i < n_nabla; i++) {
    Matrix m(resY, resX);
    for (int j=1; j <= resX; j++) {
      for (int k=1; k <= resY; k++) {
	fread(&tmpd, sizeof(tmpd), 1, fp);
	m(k,j) = tmpd;
      }
    }
    nabla_a_x.push_back(m);

    for (int j=1; j <= resX; j++) {
      for (int k=1; k <= resY; k++) {
	fread(&tmpd, sizeof(tmpd), 1, fp);
	m(k,j) = tmpd;
      }
    }
    nabla_a_y.push_back(m);
  }


  for (int i=0; i < n_nabla; i++) {
    char name[256];
    sprintf(name, "nabla_x%02d.png",i);
    writeTexture(Texture(nabla_a_x[i]), name);
    sprintf(name, "nabla_y%02d.png",i);
    writeTexture(Texture(nabla_a_y[i]), name);
  }
  for (int i=0; i < n; i++) {
    char name[256];
    sprintf(name, "jac_x%02d.png",i);
    writeTexture(Texture(jac_x[i]), name);
    sprintf(name, "jac_y%02d.png",i);
    writeTexture(Texture(jac_y[i]), name);
  }
  writeTexture(Texture(gradient_x), "gx.png");
  writeTexture(Texture(gradient_y), "gy.png");

}

void NormalizingAAM::precompute() {
  std::cout << "Normalizing::precompute" << std::endl;
  // SDI precomputation
  int n_geoms = geoms.Ncols();
  int n_shapes = shapes.Ncols();
  int n_texs = textures.Ncols();
  for (int i=0; i < n_shapes+n_geoms; i++) {
    Matrix m(resY, resX);
    m = SP(gradient_x, jac_x[i]) + SP(gradient_y, jac_y[i]);
    sdi.push_back(m);

    char name[256];
    sprintf(name, "sdi%02d.png",i);
    writeTexture(Texture(sdi[i]), name);
  }

  int n = n_shapes + n_geoms;
  Matrix hessian(n,n);
  for (int i=0; i < n; i++) {
    for (int j=i; j < n; j++) {
      hessian(i+1,j+1) = (sdi[i].AsRow() * sdi[j].AsColumn()).AsScalar();
      hessian(j+1,i+1) = hessian(i+1,j+1);
    }
  }

  invH = hessian.i();

}

//
// in :
//  * current shape
//  * current texture parameters
//  * projection of current image
// out :
//  * next parameters : shape and texture parameters

AAM::IterateReturn NormalizingAAM::iterate(const Shape2D& current_s, ColumnVector texParams, const Texture& projTex) {

  ColumnVector projTex_v = projTex.toVector();
  ColumnVector appTex_v = textures * texParams + meanTex;

  //  writeTexture(Texture(projTex_v, 48,48), "proj1.png");
  //  writeTexture(Texture(appTex_v, 48,48), "app1.png");

  ColumnVector error_v = (projTex_v - appTex_v);

  ColumnVector delta_l = textures.t() * error_v;
  error_v = error_v - (textures * delta_l);

  //  cout << "delta_l" << endl;
  //  cout << delta_l << endl;

  //  writeTexture(Texture(error_v, 48,48), "err1.png");

  int tex_modes = textures.Ncols();
  int geom_modes = geoms.Ncols();
  int shape_modes = shapes.Ncols();

  int m = shape_modes + geom_modes;
  ColumnVector dp(m);

  for (int i=0; i < m; i++) {
    dp(i+1) = (sdi[i].t().AsRow() * error_v).AsScalar();
  }
  dp = invH * dp;

  ColumnVector delta_q = dp.Rows(1, geom_modes);
  ColumnVector delta_p = dp.Rows(geom_modes+1, geom_modes+shape_modes);

  //  cout << "delta_q" << endl;
  //  cout << delta_q << endl;

  ColumnVector delta_s0 = - shapes * delta_p - geoms * delta_q;
  //ColumnVector delta_s0 = - geoms * delta_q;

  // warp composition

  Shape2D new_s;
  warpComposition(delta_s0, current_s, new_s);

  AAM::IterateReturn r;
  r.newShape = new_s;
  r.error = (error_v.t() * error_v).AsScalar();
  r.newTextureParams = texParams + delta_l;
  return r;
}
