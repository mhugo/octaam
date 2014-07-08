#include <iostream>
#include <gdkmm/pixbuf.h>
#include <gdkmm/general.h>
#include "cairoarea.h"

CairoArea::CairoArea() :
  Gtk::DrawingArea(),
  has_image(false),
  has_shape(false),
  has_triangulation(false),
  scale(1.0),
  tX(0),
  tY(0),
  angle(0.0),
  click1(false),
  click2(false),
  click3(false)
{
  if(!Glib::thread_supported()) Glib::thread_init();
  shapeLock = new Glib::Mutex;
}

CairoArea::~CairoArea() {
  delete shapeLock;
}

void CairoArea::on_realize() {
  Gtk::DrawingArea::on_realize();

  add_events(Gdk::POINTER_MOTION_MASK |
	     Gdk::BUTTON_PRESS_MASK |
	     Gdk::BUTTON_RELEASE_MASK);
}

void CairoArea::draw(int x, int y, int width, int height) {
  context = get_window()->create_cairo_context();

  if (width)
    {
      // clip to the area indicated by the expose event so that we only
      // redraw the portion of the window that needs to be redrawn
      context->rectangle(x, y,
			 width, height);
      context->clip();
    }

  // draw background
  if (has_image) {
      context->set_source(img_surf, 0.0, 0.0);
  } else {
    context->set_source_rgb(0.0, 0.3, 0.9);
  }
  context->paint();


  if (has_shape) {
    shapeLock->lock();

    context->set_source_rgb(1.0, 1.0, 1.0);
    context->save();

    for (int i=0; i < shape.size(); i++) {
      double x = shape[i].x;
      double y = shape[i].y;

      double xp = x * cos(angle) - y * sin(angle);
      double yp = x * sin(angle) + y * cos(angle);

      x = xp * scale + tX;
      y = yp * scale + tY;

      context->arc(x,y, 2.0, 0.0, 2*M_PI);
      context->close_path();
      context->fill();
    }

    //
    if (has_triangulation) {
      for (int i=0; i < triangulation.size(); i++) {
	int t1 = triangulation[i].t1;
	int t2 = triangulation[i].t2;
	int t3 = triangulation[i].t3;

	int idx;
	context->set_line_width(0.5);
	for (int j=0; j < 4; j++) {
	  switch (j) {
	  case 0: idx = t1; break;
	  case 1: idx = t2; break;
	  case 2: idx = t3; break;
	  case 3: idx = t1; break;
	  }

	  double x = shape[idx].x;
	  double y = shape[idx].y;

	  double xp = x * cos(angle) - y * sin(angle);
	  double yp = x * sin(angle) + y * cos(angle);
	  
	  x = xp * scale + tX;
	  y = yp * scale + tY;

	  if (j==0)
	    context->move_to(x,y);
	  else
	    context->line_to(x,y);

	}
	context->close_path();
	context->stroke();
      }
    }
    context->restore();
    
    shapeLock->unlock();
  }
}

bool CairoArea::on_expose_event(GdkEventExpose* event) {
  if (event)
    draw(event->area.x, event->area.y, event->area.width, event->area.height);
  else
    draw();
  return true;
}

void CairoArea::loadImage(const Glib::ustring& fileName) {
  Glib::RefPtr<Gdk::Pixbuf> img_ptr = Gdk::Pixbuf::create_from_file(fileName);
  this->loadImage(img_ptr);
}


void CairoArea::loadImage(Glib::RefPtr<Gdk::Pixbuf> img_ptr) {
  // Detect transparent colors for loaded image
  Cairo::Format format = Cairo::FORMAT_RGB24;
  if (img_ptr->get_has_alpha())
    {
      format = Cairo::FORMAT_ARGB32;
    }
  
  // Create a new ImageSurface
  img_surf = Cairo::ImageSurface::create  (format, img_ptr->get_width(), img_ptr->get_height());

  // resize window to the image size
  set_size_request(img_ptr->get_width(), img_ptr->get_height());

  Cairo::RefPtr<Cairo::Context> image_context_ptr = Cairo::Context::create (img_surf);
  
  // Draw the image on the new Context
  Gdk::Cairo::set_source_pixbuf (image_context_ptr, img_ptr, 0.0, 0.0);
  image_context_ptr->paint();

  has_image = true;
}



void CairoArea::setShape(const Shape2D& sh) {
  shapeLock->lock();

  shape = sh;
  scale = 1.0;
  angle = 0.0;
  has_shape = true;

  // recenter
  double sX = 0.0;
  double sY = 0.0;
  int n = sh.size();
  for (int i=0; i < n; i++) {
    sX += sh[i].x;
    sY += sh[i].y;
  }
  tX = sX / n;
  tY = sY / n;
  for (int i=0; i < n; i++) {
    shape[i].x -= tX;
    shape[i].y -= tY;
  }

  shapeLock->unlock();

  queue_draw();
}

void CairoArea::setTriangulation(const Triangulation& tri) {
  triangulation = tri;
  has_triangulation = true;
}

bool CairoArea::on_motion_notify_event(GdkEventMotion* event) {
  double dx = event->x - old_x;
  double dy = event->y - old_y;

  double dx1 = old_x - tX;
  double dy1 = old_y - tY;
  double dx2 = event->x - tX;
  double dy2 = event->y - tY;

  if (click1) {
    // translation
    tX += dx;
    tY += dy;
    queue_draw();
  }

  if (click2) {
    // scale modification
    double l1 = sqrt(dx1*dx1 + dy1*dy1);
    double l2 = sqrt(dx2*dx2 + dy2*dy2);
    scale *= l2 / l1;
    queue_draw();
  }

  if (click3) {
    // rotation
    double a1 = atan2(dy1, dx1);
    double a2 = atan2(dy2, dx2);

    angle += a2 - a1;
    queue_draw();
  }

  old_x = event->x;
  old_y = event->y;  

  return true;
}

bool CairoArea::on_button_press_event(GdkEventButton* event) {
  if (event->button == 1) {
    click1 = true;
  }
  if (event->button == 2) {
    click2 = true;
  }
  if (event->button == 3) {
    click3 = true;
  }
  start_x = event->x;
  start_y = event->y;
  return true;
}

bool CairoArea::on_button_release_event(GdkEventButton* event) {
  if (event->button == 1) {
    click1 = false;
  }
  if (event->button == 2) {
    click2 = false;
  }
  if (event->button == 3) {
    click3 = false;
  }
  return true;
}

Shape2D* CairoArea::getTransformedShape() const {
  // copy initial shape
  Shape2D* new_shape = new Shape2D(shape);

  for (int i=0 ; i < shape.size() ; i++) {
    Point2D p = (*new_shape)[i];
 
    double xp = p.x * cos(angle) - p.y * sin(angle);
    double yp = p.x * sin(angle) + p.y * cos(angle);
	  
    p.x = xp * scale + tX;
    p.y = yp * scale + tY;
    (*new_shape)[i] = p;
  }

  return new_shape;
}
