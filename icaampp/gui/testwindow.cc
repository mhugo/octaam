
/* icaampp (c) 2007-2009 Hugo Mercier
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <iostream>
#include <gtkmm/filechooserdialog.h>
#include "imagick.h"
#include "testwindow.h"
#include "newmat/newmatio.h"
#include "timemeasure.h"

class MyTexture : public Texture
{
public:
  MyTexture () : Texture() {}

  void fromPixbuf(Glib::RefPtr<Gdk::Pixbuf> pixbuf) {
    if (is_mine && data)
      delete data;
    
    width = pixbuf->get_width();
    height = pixbuf->get_height();

    data = new uchar[width * height];

    int n_channels = pixbuf->get_n_channels();
    guint8* pixels = pixbuf->get_pixels();
    int row_stride = pixbuf->get_rowstride();
    for (int y=0; y < height; y++) {
      for (int x=0; x < width; x++) {
	guint8* p = pixels + y * row_stride + x * n_channels;
	data[y*width+x] = (p[0] + p[1] + p[2]) / 3;
      }
    }
    is_mine = true;
  }
};

TestWindow::TestWindow()
  : Gtk::Window(),
    button("Start"),
    button2("Reset shape"),
    stopWork(true),
    scaler(1,1000,1.0),
    slide_dont_update_video(false)
{
  if(!Glib::thread_supported()) Glib::thread_init();

  // Sets the border width of the window.
  set_border_width(1);
  set_size_request(640,600);

  
  button.signal_clicked().connect(sigc::mem_fun(*this,
						&TestWindow::onStart));
  button2.signal_clicked().connect(sigc::mem_fun(*this,
						 &TestWindow::onReset));

  scaler.signal_value_changed().connect(sigc::mem_fun(*this, &TestWindow::onSlide));

  subwin.add(area);

  vbox.pack_start(subwin, true, true);
  hbox.pack_start(button, false, true); 
  hbox.pack_start(button2, false, true); 
  hbox.pack_start(scaler, true, true); 
  vbox.pack_start(hbox, false, false);

  add(vbox);

  show_all();

  imgTex = new MyTexture();

  aam.fromFile((char*)"../set/set.dat");
  Matrix* shapes = aam.getShapeVectors();
  ColumnVector psi = aam.getMeanShape();

#define IMG_SIZE 512
  psi = psi * 400 * IMG_SIZE / 720;
  int i;
  for (i=0; i < 35; i++) {
    psi(i+1) += 375 * IMG_SIZE / 720;
    psi(i+36) += 250 * IMG_SIZE / 720;
  }


  shape.fromVector(psi);

  area.setShape(shape);

  Triangulation tri = aam.getTriangulation();
  area.setTriangulation(tri);

  currentFrame = 1;

  videoSeek(currentFrame);

  tParams.ReSize(aam.getNTextures());
  
  
  tParams = 0.0;
  s0 = shape;
  s0.reshape(aam.getXres(),aam.getYres());
}

void TestWindow::onSlide() {
  // seek in video

  if (!slide_dont_update_video) {
    currentFrame = int(scaler.get_value()); 
    videoSeek(currentFrame);
    draw_signal();
  } else {
    // unarm
    slide_dont_update_video = false;
  }
}

void TestWindow::updateSlide() {
  slide_dont_update_video = true;
  scaler.set_value(currentFrame);
}

void TestWindow::onReset() {
  ColumnVector psi = aam.getMeanShape();
  psi = psi * 400;

  shape.fromVector(psi);
  area.setShape(shape);

  draw_signal();
}

void TestWindow::draw() {
  //  std::cout << "draw()" << std::endl;
  area.draw();
}

void TestWindow::on_realize() {
  Gtk::Window::on_realize();
  std::cout << "on_realize()" << std::endl;

  draw_signal.connect(sigc::mem_fun(this, &TestWindow::draw));
  update_slide_signal.connect(sigc::mem_fun(this, &TestWindow::updateSlide));
}

TestWindow::~TestWindow()
{
  delete imgTex;
}

void TestWindow::videoSeek(int frame) {
  char name[256];
  sprintf(name, "../img/test_%d/%08d.jpg", IMG_SIZE, frame);
  //sprintf(name, "../img/test/%08d.jpg", frame);
  
  fileName = name;
  

  //  static bool first = true;
  //  if (first) {
  Glib::RefPtr<Gdk::Pixbuf> img_ptr = Gdk::Pixbuf::create_from_file(fileName);
  
  area.loadImage(img_ptr);
  
  imgTex->fromPixbuf(img_ptr);
  //    first = false;
  //  }
}


void TestWindow::thread() {
  Triangulation tri = aam.getTriangulation();
  const Texture& tmap = aam.getTriangleMap();
  

  TimeMeasure texture_time, frame_time, draw_time, seek_time, it_time;

  // video loop
  while (!stopWork) {

    frame_time.start();

    seek_time.start();
    videoSeek(currentFrame);
    seek_time.stop();
    //    draw_signal();

    AAM::IterateReturn ret;

#ifdef DISPLAY_ITERATION
    int nbIt = 0;
    while (!stopWork) {
      it_time.start();
      // ask the main thread to draw
      //      draw_signal();

      nbIt++;

      Shape2D* current_s = area.getTransformedShape();
      
      texture_time.start();
      Texture* ntex = current_s->textureWarp(s0, tri, tmap, *imgTex);
      texture_time.stop();

      //      writeTexture(*ntex, (char*)"proj0.png");
      
      ret = aam.iterate(*current_s, tParams, *ntex);
      delete ntex;
      delete current_s;
      
      //      std::cout << "E: " << r.error << std::endl;
      //if ((r.error < 1.0e5) || (nbIt >= 10)) {
      if (nbIt >= 10) {
	break;
      }
      
      area.setShape(ret.newShape);
      tParams = ret.newTextureParams;

      it_time.stop();
    }
#else
    Shape2D* current_s = area.getTransformedShape();
      
    for (int nbIt = 0; nbIt < 10; nbIt++) {
      it_time.start();
      // ask the main thread to draw
      //      draw_signal();

      texture_time.start();
      Texture* ntex = current_s->textureWarp(s0, tri, tmap, *imgTex);
      texture_time.stop();

      ret = aam.iterate(*current_s, tParams, *ntex);
      delete ntex;
      
      *current_s = ret.newShape;
      tParams = ret.newTextureParams;

      it_time.stop();
    }

    area.setShape(ret.newShape);
    delete current_s;
#endif

    draw_time.start();
    draw_signal();
    update_slide_signal();
    draw_time.stop();

    currentFrame++;
    double m_frame_time = frame_time.mean();
    double m_it_time = it_time.mean();
    double ips = 1.0 / m_it_time;
    double fps = 1.0 / m_frame_time;
    cout << "mean iteration time: \t\t" << it_time.mean() << " (" << ips << " ips)" << endl;
    cout << "mean frame time: \t\t" << m_frame_time << " (" << fps << " fps)" << endl;
    cout << endl;
    cout << "mean texture warping time: \t" << texture_time.mean() << endl;
    cout << "mean draw_signal time: \t\t" << draw_time.mean() << endl;
    cout << "mean seek time: \t\t" << seek_time.mean() << endl;
    cout << "mean compo time: \t\t" << aam.time.mean() << endl;

    frame_time.stop();
  }

  stopWork = true;
  button.set_label("Start");
}

void TestWindow::onStart()
{

  if (stopWork) {
    button.set_label("Stop");
    stopWork = false;
    workingThread = Glib::Thread::create(sigc::mem_fun(this,&TestWindow::thread), false);
  } else {
    stopWork = true;
  }

}

