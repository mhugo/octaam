/* gtkmm example Copyright (C) 2002 gtkmm development team
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

#ifndef TEST_WINDOW
#define TEST_WINDOW

#include <string>
#include <gtkmm/button.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/window.h>
#include <gtkmm/scale.h>
#include <cairomm/context.h>
#include "cairoarea.h"
#include "normalizing_aam.h"
#include "simultaneous_aam.h"
#include "project_out_aam.h"

class MyTexture;

class TestWindow : public Gtk::Window
{

public:
  TestWindow();
  virtual ~TestWindow();

  void draw();
  void thread();

  void videoSeek(int);

  void onSlide();
  void updateSlide();
  void onReset();

protected:
  //Signal handlers:
  virtual void onStart();

  void on_realize();

  bool stopWork;
  Glib::Thread* workingThread;

  //Member widgets:
  Gtk::HScale scaler;
  Gtk::Button button, button2;
  Gtk::ScrolledWindow subwin;
  Gtk::VBox vbox;
  Gtk::HBox hbox;
  CairoArea area;
  //NormalizingAAM aam;
  //SimultaneousAAM aam;
  ProjectOutAAM aam;

  std::string fileName;
  Shape2D shape;

  ColumnVector tParams;
  Shape2D s0;
  MyTexture* imgTex;

  int currentFrame;

  Glib::Dispatcher draw_signal;
  Glib::Dispatcher update_slide_signal;

  bool slide_dont_update_video;
};

#endif
