// 
// $Id$
//
// Main body implementation
//
// ten0k <hugo.mercier@laposte.net>
// Created on: <16-jui-2004 15:00:14 ten0k>
//
// Copyright (C) ten0k.  All rights reserved.
//

#include <octave/oct.h>
#include <iostream>
#include "face_detector.h"

#define IMAGE_ELEM(img, x, y) &((unsigned char*)((img)->imageData + (img)->widthStep*(y)))[(x)*((img)->nChannels)]


FaceDetector faceDetector(24);

DEFUN_DLD(fd_viola_jones, args, nargout,
	  "")
{
  using namespace std;
  octave_value_list retval;
  if (args.length() < 3) {
    error("Usage : fd_viola_jones(R,G,B)");
    return retval;

  }

  Matrix R = args(0).matrix_value();
  Matrix G = args(1).matrix_value();
  Matrix B = args(2).matrix_value();

  int width = R.cols();
  int height = R.rows();

  std::cout << width << "x" << height << std::endl;

  IplImage *temp = cvCreateImage(cvSize(width, height),
				 8,
				 3);

  for (int y=0; y < height; y++) {
    for (int x=0; x < width; x++) {
      unsigned char *p = IMAGE_ELEM(temp, x, y);
      p[0] = int(R(y,x));
      p[1] = int(G(y,x));
      p[2] = int(B(y,x));
    }
  }

  int rx, ry, rw, rh, x2, y2;
  bool ok = faceDetector.detect(temp, rx,ry,rw,rh);


  cvReleaseImage(&temp);


  if (ok) {
    x2 = rx + rw;
    y2 = ry + rh;
  } else {
    rx = -1;
    ry = -1;
    x2 = -1;
    y2 = -1;
  }
  retval.append(octave_value(rx));
  retval.append(octave_value(ry));
  retval.append(octave_value(x2));
  retval.append(octave_value(y2));
  return retval;
  
}

