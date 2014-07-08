// interface to Magick++

#include "imagick.h"
#include "mytypes.h"

// namespace changed since ImageMagick 6.4.2
#define MagickLib MagickCore

ColorTexture* textureFromColorImage(const Magick::Image& img) {

  int w = img.columns();
  int h = img.rows();
  uchar* rpixels = new uchar[w*h];
  uchar* gpixels = new uchar[w*h];
  uchar* bpixels = new uchar[w*h];

  Magick::Image& img2 = const_cast<Magick::Image&>(img);

  img2.write(0,0,w,h,"R",MagickLib::CharPixel,rpixels);
  img2.write(0,0,w,h,"G",MagickLib::CharPixel,gpixels);
  img2.write(0,0,w,h,"B",MagickLib::CharPixel,bpixels);

  ColorTexture* tex = new ColorTexture();

  Texture* texR = tex->getR();
  Texture* texG = tex->getG();
  Texture* texB = tex->getB();

  // don't copy
  // transmit ownership
  texR->fromData(rpixels, w, h, false, true);
  texG->fromData(gpixels, w, h, false, true);
  texB->fromData(bpixels, w, h, false, true);

  return tex;
}

Texture* textureFromImage(const Magick::Image& img) {

  int w = img.columns();
  int h = img.rows();
  uchar* pixels = new uchar[w*h];
  uchar* rpixels = new uchar[w*h];
  uchar* gpixels = new uchar[w*h];
  uchar* bpixels = new uchar[w*h];

  const_cast<Magick::Image&>(img).write(0,0,w,h,"R",MagickLib::CharPixel,rpixels);
  const_cast<Magick::Image&>(img).write(0,0,w,h,"G",MagickLib::CharPixel,gpixels);
  const_cast<Magick::Image&>(img).write(0,0,w,h,"B",MagickLib::CharPixel,bpixels);
  for (int i = 0; i < w*h; i++) {
    pixels[i] = (rpixels[i] + gpixels[i] + bpixels[i]) / 3;
  }
  delete[] rpixels;
  delete[] gpixels;
  delete[] bpixels;

  Texture* tex = new Texture();

  // don't copy
  // transmit ownership
  tex->fromData(pixels, w, h, false, true);

  return tex;
}

Magick::Image* textureToImage(const Texture& tex) {
  int w = tex.getWidth();
  int h = tex.getHeight();

  uchar* pixels = new uchar[w*h*3];
  uchar* orig = tex.getData();

  for (int i=0; i < w*h; i++) {
    pixels[i*3] = orig[i];
    pixels[i*3+1] = orig[i];
    pixels[i*3+2] = orig[i];
  }


  Magick::Image* img = new Magick::Image(w,
					 h,
					 "RGB",
					 MagickLib::CharPixel,
					 pixels);
  delete[] pixels;

  return img;
}


void writeTexture(const Texture& tex, const char* fileName) {
  Magick::Image* img = textureToImage(tex);
  img->write(fileName);
  delete img;
}
