#include <string.h>
#include "texture.h"

Texture::Texture() {
  is_mine = false;
  data = NULL;
  width = 0;
  height = 0;
}

Texture::Texture(int w, int h) {
  data = new uchar[w*h];
  memset(data, 0, w*h);
  is_mine = true;
  width = w;
  height = h;
}

Texture::Texture(const ColumnVector& tex, int w, int h) {
  data = new uchar[w*h];
  is_mine = true;
  width = w;
  height = h;

  double min = tex.Minimum();
  double max = tex.Maximum();

  for (int y=0; y < h; y++) {
    for (int x=0; x < w; x++) {
      data[y*width+x] = uchar((tex(x*height+y+1) - min) / (max - min) * 255);
    }
  }
}

Texture::Texture(const Matrix& tex) {
  int w = tex.Nrows();
  int h = tex.Ncols();
  data = new uchar[w*h];
  is_mine = true;
  width = w;
  height = h;

  double min = tex.Minimum();
  double max = tex.Maximum();

  for (int y=0; y < h; y++) {
    for (int x=0; x < w; x++) {
      data[y*width+x] = uchar((tex(y+1,x+1) - min) / (max - min) * 255);
    }
  }
}

Texture::~Texture() {
  if (is_mine)
    delete data;
}

void Texture::fromData(uchar* data, int w, int h, bool copy, bool is_mine) {
  if (this->is_mine && this->data)
    delete this->data;
  if (copy) {
    this->data = new uchar[w*h];
    memcpy(this->data, data, w*h);
  } else {
    this->data = data;
  }
  this->is_mine = is_mine;

  width = w;
  height = h;
}

ReturnMatrix Texture::toMatrix() const {
  Matrix new_mat(height, width);

  for (int i=0; i < height; i++) {
    for (int j=0; j < width; j++) {
      new_mat(i+1,j+1) = data[i*width+j];
    }
  }
  new_mat.Release();
  return new_mat;
}

ReturnMatrix Texture::toVector() const {
  ColumnVector new_mat(height* width);

  for (int i=0; i < height; i++) {
    for (int j=0; j < width; j++) {
      new_mat(j*height+i+1) = data[i*width+j];
    }
  }
  new_mat.Release();
  return new_mat;
}

ColorTexture::ColorTexture(int w, int h) {
  r_channel = new Texture(w,h);
  g_channel = new Texture(w,h);
  b_channel = new Texture(w,h);
}

ColorTexture::ColorTexture() {
  r_channel = new Texture();
  g_channel = new Texture();
  b_channel = new Texture();
}

ColorTexture::~ColorTexture() {
  delete r_channel;
  delete g_channel;
  delete b_channel;
}
