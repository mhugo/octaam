#ifndef CAIRO_AREA
#define CAIRO_AREA

#include <gtkmm/drawingarea.h>
#include <glibmm/thread.h>
#include "shape.h"

class CairoArea : public Gtk::DrawingArea {

 public:
  CairoArea();
  virtual ~CairoArea();

  void loadImage(const Glib::ustring& fileName);
  void loadImage(Glib::RefPtr<Gdk::Pixbuf>);
  void setShape(const Shape2D& sh);
  void setTriangulation(const Triangulation& tri);

  // allocate a new shape and set it to the geometry transformed shape
  Shape2D* getTransformedShape() const;

  void draw(int x=0, int y=0, int width=0, int height=0);

 protected:

  bool on_expose_event(GdkEventExpose* event);
  bool on_motion_notify_event(GdkEventMotion* event);
  bool on_button_press_event(GdkEventButton* event);
  bool on_button_release_event(GdkEventButton* event);
  void on_realize();

  Cairo::RefPtr<Cairo::Context> context;
  Cairo::RefPtr<Cairo::ImageSurface> img_surf;

  Shape2D shape;
  Triangulation triangulation;

  bool has_shape;
  bool has_image;
  bool has_triangulation;

  double tX, tY, scale, angle;

  bool click1, click2, click3;
  double start_x, start_y;
  double old_x, old_y;

  Glib::Mutex* shapeLock;
};

#endif
