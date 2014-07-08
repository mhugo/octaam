//
// (c) 2008 Hugo Mercier <hmercier31[at]gmail.com
//
// GNU GPL License 3
//

#ifndef _ICAAM_TEXTURE_
#define _ICAAM_TEXTURE_

#include "newmat/newmat.h"
#include "mytypes.h"

class Texture {
 public:
  Texture();
  Texture(int w, int h);
  Texture(const ColumnVector&, int w, int h);
  Texture(const Matrix&);
  ~Texture();

  void fromData(uchar*, int w, int h, bool copy = true, bool is_mine = true);

  int getWidth() const { return width; }
  int getHeight() const { return height; }

  uchar getPixel(int x, int y) const { return data[y*width+x]; }
  void setPixel(int x, int y, uchar p) { data[y*width+x] = p; }
  uchar* getData() const { return data; }

  ReturnMatrix toMatrix() const;
  ReturnMatrix toVector() const;

 protected:
  uchar* data;
  int width;
  int height;

  // is data mine ?
  bool is_mine;
};

class ColorTexture {
 public:
  ColorTexture();
  ColorTexture(int w, int h);
  ~ColorTexture();

  Texture* getR() { return r_channel; }
  Texture* getG() { return g_channel; }
  Texture* getB() { return b_channel; }

  int getWidth() const { return r_channel->getWidth(); }
  int getHeight() const { return r_channel->getWidth(); }

 protected:
  Texture* r_channel;
  Texture* g_channel;
  Texture* b_channel;
};

// on external file
//Texture* fromImage()

#endif
