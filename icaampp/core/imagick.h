// interface to Magick++

#include <Magick++.h>
#include "texture.h"

ColorTexture* textureFromColorImage(const Magick::Image& img);
Texture* textureFromImage(const Magick::Image& img);
Magick::Image* textureToImage(const Texture& tex);
void writeTexture(const Texture& tex, const char* fileName);
