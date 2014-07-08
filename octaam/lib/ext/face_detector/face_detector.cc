#include "face_detector.h"

#include <iostream>
#include <sstream>

#include "highgui.h"
#include "cv.h"

FaceDetector::FaceDetector(int size)
{

  /*** 0.9.5 ***
  CvHaarClassifierCascade* cascade;
  cascade = cvLoadHaarClassifierCascade( "<default_face_cascade>",
					 cvSize( size, size ));

  std::cout << cascade << std::endl;

  hid_cascade = cvCreateHidHaarClassifierCascade( cascade, 0, 0, 0, 1 );

  ***/

  // Now with 0.9.7 ...

  char cascade_name[] = "/opt/opencv-0.9.7/data/haarcascades/haarcascade_frontalface_default.xml";
  cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );

  //  std::cout << "CTor cascade=" << cascade << std::endl; 

  /*  cout << "# stages:" << cascade->count << endl;

  for (int i=0; i<cascade->count; i++) {
    cout << "# classifiers:" << cascade->stageClassifier[i].count << endl;

    CvSize is = { 64, 64 };
    IplImage* img = cvCreateImage(is, IPL_DEPTH_8U, 3);


    for (int j=0; j<cascade->stageClassifier[i].count; j++) {
      int s = cascade->stageClassifier[i].classifier[j].count;
      cout << "# features:" << s << endl;

      for (int k=0; k < s; k++) {
	CvHaarFeature* feature = &cascade->stageClassifier[i].classifier[j].haarFeature[k];
	if (feature->tilted)
	  cout << "45" << endl;
	else
	  cout << "0" << endl;

	for (int l=0; l < 3; l++) {
	  if (feature->rect[l].weight) {
	    cout << feature->rect[l].weight << endl;
	    int col;
	    if (feature->rect[l].weight < 0)
	      col = CV_RGB(0,0,0);
	    else {
	      int w = feature->rect[l].weight * 255 / 3;
	      col = CV_RGB(w,w,w);
	    }
	    CvPoint pt1, pt2;
	    pt1.x = feature->rect[l].r.x;
	    pt1.y = feature->rect[l].r.y;
	    pt2.x = pt1.x + feature->rect[l].r.width;
	    pt2.y = pt1.y + feature->rect[l].r.height;
	    cvRectangle(img, pt1, pt2, col, CV_FILLED);
	  }
	}
      }
    }
      stringstream str;
      
      str << "feature" << i << ".bmp";
      cvSaveImage(str.str().c_str(), img);
      cvReleaseImage(&img);
      }*/

  // 0.9.5
  // cvReleaseHaarClassifierCascade( &cascade );

  storage = cvCreateMemStorage(0);
}

FaceDetector::~FaceDetector()
{
  // 0.9.5
  //  cvReleaseHidHaarClassifierCascade( &hid_cascade );
  //std::cout << "DTOR" << std::endl;
  cvReleaseHaarClassifierCascade( &cascade );
}

bool FaceDetector::detect(IplImage* img, int& rx, int& ry, int& rw, int& rh)
{
    int scale = 2;
    IplImage* temp = cvCreateImage( cvSize(img->width/scale,img->height/scale), 8, 3 );
    int i;

    cvPyrDown( img, temp, CV_GAUSSIAN_5x5 );
    cvClearMemStorage( storage );


    //std::cout << "==" << cascade << std::endl;
    // 0.9.5
    // if( hid_cascade )
    if (cascade)
    {
      //std::cout << "Detection" << std::endl;
      // 0.9.5
      //      CvSeq* faces = cvHaarDetectObjects( temp, hid_cascade, storage,
      //                                            1.2, 2, CV_HAAR_DO_CANNY_PRUNING );
        
      CvSeq* faces = cvHaarDetectObjects( temp, cascade, storage,
					  1.1, 2, CV_HAAR_DO_CANNY_PRUNING );

	if (faces->total == 0)
	  {
	    cvReleaseImage( &temp );
	    return false;
	  }

        for( i = 0; i < (faces ? faces->total : 0); i++ )
        {
	  // 0.9.5
	  //	  CvRect* r = (CvRect*)cvGetSeqElem( faces, i, 0 );
	  CvRect* r = (CvRect*)cvGetSeqElem( faces, i);


	    //	    rect = *r;
	    rx = r->x * scale;
	    ry = r->y * scale;
	    rw = r->width * scale;
	    rh = r->height * scale;
        }
    }


    cvReleaseImage( &temp );

    return true;
}
