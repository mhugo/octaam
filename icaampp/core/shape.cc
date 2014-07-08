#include <iostream>
#include <algorithm>
#include "shape.h"

void Shape2D::fromVector(const ColumnVector& v) {
  int n = v.Nrows();
  int m = n / 2;

  resize(m);

  for (int i=0; i < m; i++) {

    Point2D p;
    p.x = v(i+1);
    p.y = v(i+1+m);

    (*this)[i] = p;
  }
}

ReturnMatrix Shape2D::toVector() const {
  int n = size();

  ColumnVector v(n*2);

  for (int i=0; i < n; i++) {
    Point2D p = (*this)[i];
    v(i+1) = p.x;
    v(i+1+n) = p.y;
  }
  v.Release();
  return v;
}

Texture* Shape2D::textureWarp(const Shape2D& dest_shape, const Triangulation& tri, const Texture& triangle_map, const Texture& image) const {
  
  int pixel_w = triangle_map.getWidth();
  int pixel_h = triangle_map.getHeight();

  Texture* tex_out = new Texture(pixel_w, pixel_h);


  uchar * tmap_data = triangle_map.getData();
  uchar * texout_data = tex_out->getData();
  for (int y=0; y < pixel_h; y++) {
    for (int x=0; x < pixel_w; x++) {
      uchar t = *tmap_data++;
      
      // t == 0 => no triangle
      if (!t) {
	texout_data++;
	continue;
      }

      t = t - 1;

      Point2D v0 = dest_shape[tri[t].t1];
      Point2D v1 = dest_shape[tri[t].t2];
      Point2D v2 = dest_shape[tri[t].t3];

      Point2D w0 = (*this)[tri[t].t1];
      Point2D w1 = (*this)[tri[t].t2];
      Point2D w2 = (*this)[tri[t].t3];

      double denum = (v1.x - v0.x) * (v2.y - v0.y) - (v1.y - v0.y) * (v2.x - v0.x);

      double a1 = w0.x + ((v0.y*(v2.x - v0.x) - v0.x*(v2.y - v0.y)) * (w1.x - w0.x) + (v0.x * (v1.y - v0.y) - v0.y*(v1.x - v0.x)) * (w2.x - w0.x)) / denum;
      double a2 = ((v2.y - v0.y) * (w1.x - w0.x) - (v1.y - v0.y) * (w2.x - w0.x)) / denum;
      double a3 = ((v1.x - v0.x) * (w2.x - w0.x) - (v2.x - v0.x) * (w1.x - w0.x)) / denum;

      double a4 = w0.y + ((v0.y*(v2.x - v0.x) - v0.x*(v2.y - v0.y)) * (w1.y - w0.y) + (v0.x * (v1.y - v0.y) - v0.y*(v1.x - v0.x)) * (w2.y - w0.y)) / denum;
      double a5 = ((v2.y - v0.y) * (w1.y - w0.y) - (v1.y - v0.y) * (w2.y - w0.y)) / denum;
      double a6 = ((v1.x - v0.x) * (w2.y - w0.y) - (v2.x - v0.x) * (w1.y - w0.y)) / denum;

      int cx = int((a1 + a2 * x + a3 * y));
      int cy = int((a4 + a5 * x + a6 * y));

      static bool first = true;
      if (first) {
	cout << cx << "," << cy << endl;
	first = false;
      }

      if ((cx < 0) ||
	  (cx >= image.getWidth()) ||
	  (cy < 0) ||
	  (cy >= image.getHeight())) {
	*texout_data = 0;
      } else {
	*texout_data = image.getPixel(cx,cy);
      }
      texout_data++;
    }
  }

  return tex_out;
}

void Shape2D::reshape(double w, double h, double x, double y) {
  double xmin, xmax, ymin, ymax;

  xmin = xmax = operator[](0).x;
  ymin = ymax = operator[](0).y;
  for (int i=0; i < size(); i++) {
    Point2D p = operator[](i);
    if (p.x > xmax)
      xmax = p.x;
    if (p.x < xmin)
      xmin = p.x;
    if (p.y > ymax)
      ymax = p.y;
    if (p.y < ymin)
      ymin = p.y;
  }

  double current_w = xmax - xmin;
  double current_h = ymax - ymin;

  for (int i=0; i < size(); i++) {
    Point2D p = operator[](i);
    p.x = (p.x - xmin) / current_w * w + x;
    p.y = (p.y - ymin) / current_h * h + y;
    operator[](i) = p;
  }
}

// it doesn't assume the shape has been reshaped
// to  w x h
Texture* Shape2D::computeTriangleMap(const Triangulation& tri, int w, int h) {
  Texture* new_tex = new Texture(w,h);

  uchar* pixels = new uchar[w*h];

  for (int y=0; y < h ; y++) {
    for (int x=0; x < w; x++) {
      //
      //  TODOOO
      //
      pixels[y*w+x] = 1;
    }
  }
}

int round(double a) {
  if(a>0)
    return int(a + .5);
  else
    return int(a - .5);
}

void scanLine(int x1, int y1, int x2, int y2, ScanSegment& out)
{
  bool swap_xy = false;
  bool flip_y = false;

  // always scan convert from left to right.
  if(x1 > x2)
  {
    swap(x1, x2); swap(y1, y2);
  }
  // always scan convert from down to up.
  if( (flip_y = y1 > y2) )                  
  {
    y1 = -y1, y2 = -y2;
  }
  // and always scan convert a line with <= 45 deg to x-axis.
  if( (swap_xy = y2-y1 > x2-x1) )        
  {
    swap(x1, y1), swap(x2, y2);
  }


  bool 
    horizontal =  y1==y2,
    diagonal = y2-y1==x2-x1;

  double scale = 1.0 / (x2 - x1);

  int y = y1; 
  double yy = y1, y_step = (y2 - y1) * scale;
    
  for(int x = x1; x <= x2; ++x)
  {
    int X(x), Y(y);
    if(swap_xy) swap(X,Y);
    if(flip_y)  Y = -Y; 

    IntPoint2D p = {X, Y};
    out.push_back(p);

    if( ! horizontal )
    {
      if( diagonal ) y++;
      else y = int(round( yy += y_step ));
    }
  }
}

static bool cmpPoint(IntPoint2D p1, IntPoint2D p2) {
  if (p1.y == p2.y)
    return (p1.x < p2.x);
  return (p1.y < p2.y);
}

void scanTriangle(int x1, int y1, int x2, int y2, int x3, int y3, ScanSegment& left, ScanSegment& right) {
  ScanSegment scan;
  
  scanLine(x1, y1, x2, y2, scan);
  scanLine(x2, y2, x3, y3, scan);
  scanLine(x3, y3, x1, y1, scan);
  
  // sort by y value
  std::sort(scan.begin(), scan.end(), cmpPoint);

  std::vector<int> same_y;

  int current_y = -1;
  for (int i=0; i < scan.size();  i++) {
    int y = scan[i].y;

    if (current_y != y) {
      if (same_y.size()) {
	int n = same_y.size();
	int idx1 = same_y[0];
	int idx2 = same_y[n-1];
	left.push_back(scan[idx1]);
	right.push_back(scan[idx2]);
	same_y.clear();
      }
      current_y = y;
    }

    same_y.push_back(i);
  }
}
