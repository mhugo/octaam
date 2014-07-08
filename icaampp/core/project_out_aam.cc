#include <iostream>
#include "newmat/newmatio.h"
#include "imagick.h"
#include "project_out_aam.h"

ProjectOutAAM::ProjectOutAAM() : AAM() {
}

ProjectOutAAM::~ProjectOutAAM() {
}


void ProjectOutAAM::_readFile(FILE* fp) {

  AAM::_readFile(fp);

  short tmp1, tmp2;
  double tmpd;

  std::cout << "project-out readfile" << std::endl;

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

void ProjectOutAAM::precompute() {
  std::cout << "ProjectOut::precompute" << std::endl;
  // SDI precomputation
  int n_geoms = geoms.Ncols();
  int n_shapes = shapes.Ncols();
  int n_texs = textures.Ncols();

  for (int i=0; i < n_geoms+n_shapes; i++) {
    // SP = Schur product (element wise)
    Matrix A;
    A = SP(gradient_x, jac_x[i]) + SP(gradient_y, jac_y[i]);
    Matrix B = A;

    for (int j=0; j < n_texs; j++) {
      B = B - (A.AsRow() * textures.Column(j+1)).AsScalar() * textures.Column(j+1).AsMatrix(resY, resX);
    }

    sdi.push_back(B);

    char name[256];
    sprintf(name, "sdi%02d.png",i);
    writeTexture(Texture(sdi[i]), name);
  }

  int m = n_geoms + n_shapes;
  Matrix hessian(m,m);
  for (int i=0; i < m; i++) {
    for (int j=i; j < m; j++) {
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

AAM::IterateReturn ProjectOutAAM::iterate(const Shape2D& current_s, ColumnVector texParams, const Texture& projTex) {
  //  ColumnVector projTex_v = projTex.toVector();
  //  ColumnVector error_v = (projTex_v - meanTex);

  Real* error_w = new Real[resX*resY];

  uchar* projtex_data = projTex.getData();
  Real* meantex_data = meanTex.Store();
  Real* error_ptr = error_w;
  for (int y=0; y < resY; y++) {
    for (int x=0; x < resX; x++) {
      *error_ptr++ = *projtex_data++ - meantex_data[x*resY+y];
    }
  }

  //  writeTexture(Texture(error_v, 64,64), "error_v.png");

  int tex_modes = textures.Ncols();
  int geom_modes = geoms.Ncols();
  int shape_modes = shapes.Ncols();
  int m = geom_modes + shape_modes;

  ColumnVector dp(m);

  time.start();
  for (int i=0; i < m; i++) {
    Real sum = 0;
    Real* sdi_data = sdi[i].Store();
    //    Real* error_data = error_v.Store();
    Real* error_data = error_w;
    long nb = resX * resY;
    for (long j=0; j < nb; j++) {
	//	sum += sdi_data[x*resY+y] * error_data[x*resY+y];
	sum += *sdi_data++ * *error_data++;
    }
    //dp(i+1) = (sdi[i].t().AsRow() * error_v).AsScalar();
    dp(i+1) = sum;
  }
  delete error_w;
  time.stop();
  dp = invH * dp;


  ColumnVector delta_q = dp.Rows(1, geom_modes);
  ColumnVector delta_p = dp.Rows(geom_modes+1, geom_modes+shape_modes);

  // project out the appearance parameters,
  // i.e. project on the texture space
  // (will not be used in further step)

  // WARNING : this step must be done once converged
  /*  for (int i=1; i <= tex_modes; i++) {
    new_l(i) = (error_v.AsRow() * textures.Column(i)).AsScalar();
    }*/

  ColumnVector delta_s0 = - shapes * delta_p - geoms * delta_q;

  // warp composition
  Shape2D new_s;
  warpComposition(delta_s0, current_s, new_s);

  AAM::IterateReturn r;
  r.newShape = new_s;
  //r.error = (error_v.t() * error_v).AsScalar();
  r.newTextureParams = texParams;
  return r;
}
