#include <iostream>
#include "newmat/newmatio.h"
#include "imagick.h"
#include "simultaneous_aam.h"

SimultaneousAAM::SimultaneousAAM() : AAM() {
}

SimultaneousAAM::~SimultaneousAAM() {
}


void SimultaneousAAM::_readFile(FILE* fp) {

  AAM::_readFile(fp);

  short tmp1, tmp2;
  double tmpd;

  std::cout << "simultaneous readfile" << std::endl;

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

void SimultaneousAAM::precompute() {
  std::cout << "Simultaneous::precompute" << std::endl;
  // SDI precomputation
  int n_geoms = geoms.Ncols();
  int n_shapes = shapes.Ncols();
  int n_texs = textures.Ncols();
  for (int i=0; i < n_texs+n_shapes+n_geoms; i++) {
    Matrix m(resY, resX);
    sdi.push_back(m);
  }
  for (int i=0; i < n_texs; i++) {
    Matrix m(resY, resX);
    for (int y=0; y < resY; y++) {
      for (int x=0; x < resX; x++) {
	m(y+1,x+1) = textures(x*resY+y+1, i+1);
      }
    }
    sdi[i+n_geoms+n_shapes] = m;
  }

  for (int i=0; i < n_geoms+n_shapes+n_texs; i++) {
    char name[256];
    sprintf(name, "sdi%02d.png",i);
    writeTexture(Texture(sdi[i]), name);
  }

}

//
// in :
//  * current shape
//  * current texture parameters
//  * projection of current image
// out :
//  * next parameters : shape and texture parameters

AAM::IterateReturn SimultaneousAAM::iterate(const Shape2D& current_s, ColumnVector texParams, const Texture& projTex) {
  ColumnVector projTex_v = projTex.toVector();
  ColumnVector appTex_v = textures * texParams + meanTex;

  //  std::cout << texParams << std::endl;

  //  writeTexture(Texture(projTex_v, 48,48), "proj1.png");
  //  writeTexture(Texture(appTex_v, 48,48), "app1.png");

  ColumnVector error_v = (projTex_v - appTex_v);

  //  writeTexture(Texture(error_v, 48,48), "err1.png");

  int tex_modes = textures.Ncols();
  int geom_modes = geoms.Ncols();
  int shape_modes = shapes.Ncols();
  Matrix AX = gradient_x;
  Matrix AY = gradient_y;
  //  std::cout << "AX ..." << std::endl;
  for (int i=0; i < tex_modes; i++) {
    AX = AX + texParams(i+1) * nabla_a_x[i];
    AY = AY + texParams(i+1) * nabla_a_y[i];
  }

  //  writeTexture(Texture(AX), "ax.png");

  //  std::cout << "SDI ..." << std::endl;
  for (int i=0; i < geom_modes+shape_modes; i++) {
    // SP = Schur product (element wise)
    sdi[i] = SP(AX, jac_x[i]) + SP(AY, jac_y[i]);

    //    char name[256];
    //sprintf(name, "sdi%02d.png",i);
    //writeTexture(Texture(sdi[i]), name);
  }

  //  std::cout << "Hessian ..." << std::endl;

  int m = geom_modes + shape_modes + tex_modes;
  Matrix hessian(m,m);
  for (int i=0; i < m; i++) {
    for (int j=i; j < m; j++) {
      hessian(i+1,j+1) = (sdi[i].AsRow() * sdi[j].AsColumn()).AsScalar();
      hessian(j+1,i+1) = hessian(i+1,j+1);
    }
  }

  Matrix invH = hessian.i();

  //  writeTexture(Texture(hessian), "hessian.png");


  ColumnVector dp(m);

  for (int i=0; i < m; i++) {
    dp(i+1) = (sdi[i].t().AsRow() * error_v).AsScalar();
  }
  dp = invH * dp;



  ColumnVector delta_q = dp.Rows(1, geom_modes);
  ColumnVector delta_p = dp.Rows(geom_modes+1, geom_modes+shape_modes);
  ColumnVector delta_l = dp.Rows(geom_modes+shape_modes+1, geom_modes+shape_modes+tex_modes);

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
