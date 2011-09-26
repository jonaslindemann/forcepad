// generated by Fast Light User Interface Designer (fluid) version 1.0300

#ifndef TabletToolbar_kiosk_h
#define TabletToolbar_kiosk_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include "Fl_HoverButton.h"

class CTabletToolbar {
  void* m_paintView; 
public:
  CTabletToolbar();
  Fl_Double_Window *mainWindow;
  Fl_HoverButton *btnNew;
private:
  void cb_btnNew_i(Fl_HoverButton*, void*);
  static void cb_btnNew(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnUndo;
private:
  void cb_btnUndo_i(Fl_HoverButton*, void*);
  static void cb_btnUndo(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnSettings;
private:
  void cb_btnSettings_i(Fl_HoverButton*, void*);
  static void cb_btnSettings(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnEx1;
private:
  void cb_btnEx1_i(Fl_HoverButton*, void*);
  static void cb_btnEx1(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnEx2;
private:
  void cb_btnEx2_i(Fl_HoverButton*, void*);
  static void cb_btnEx2(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnEx3;
private:
  void cb_btnEx3_i(Fl_HoverButton*, void*);
  static void cb_btnEx3(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnEx4;
private:
  void cb_btnEx4_i(Fl_HoverButton*, void*);
  static void cb_btnEx4(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnEx5;
private:
  void cb_btnEx5_i(Fl_HoverButton*, void*);
  static void cb_btnEx5(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnEx6;
private:
  void cb_btnEx6_i(Fl_HoverButton*, void*);
  static void cb_btnEx6(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnEx7;
private:
  void cb_btnEx7_i(Fl_HoverButton*, void*);
  static void cb_btnEx7(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnEx8;
private:
  void cb_btnEx8_i(Fl_HoverButton*, void*);
  static void cb_btnEx8(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnEx9;
private:
  void cb_btnEx9_i(Fl_HoverButton*, void*);
  static void cb_btnEx9(Fl_HoverButton*, void*);
public:
  void show();
  void setView(void* view);
  void centerWindow(Fl_Window* window);
  void placeWindow(Fl_Window* window);
  void hide();
};
#endif