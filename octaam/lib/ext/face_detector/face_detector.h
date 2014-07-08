#ifndef FACE_DETECTOR
#define FACE_DETECTOR

#include <cv.h>
#include <cvaux.h>

class FaceDetector
{
 public:
  FaceDetector(int);
  virtual ~FaceDetector();

  bool detect(IplImage*, int&, int&, int&, int&);

 private:
  CvMemStorage *storage;
  // 0.9.5  CvHidHaarClassifierCascade* hid_cascade;
  CvHaarClassifierCascade* cascade;
};

#endif
