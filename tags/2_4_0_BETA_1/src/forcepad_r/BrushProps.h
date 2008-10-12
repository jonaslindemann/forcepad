// generated by Fast Light User Interface Designer (fluid) version 1.0104

#ifndef BrushProps_h
#define BrushProps_h
#include <FL/Fl.H>
#define MR_OK 0
#define MR_CANCEL 1
#include <FL/Fl_Double_Window.H>
#include "Fl_HoverButton.h"
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Choice.H>

class CBrushProps {
  void* m_view;
  int m_modalResult;
public:
  CBrushProps();
  Fl_Double_Window *wndBrushProps;
  Fl_HoverButton *btnOk;
private:
  inline void cb_btnOk_i(Fl_HoverButton*, void*);
  static void cb_btnOk(Fl_HoverButton*, void*);
public:
  Fl_Value_Slider *sldBlendFactor;
private:
  inline void cb_sldBlendFactor_i(Fl_Value_Slider*, void*);
  static void cb_sldBlendFactor(Fl_Value_Slider*, void*);
public:
  Fl_Choice *mnuBrush;
private:
  inline void cb_mnuBrush_i(Fl_Choice*, void*);
  static void cb_mnuBrush(Fl_Choice*, void*);
  static Fl_Menu_Item menu_mnuBrush[];
public:
  static Fl_Menu_Item *mnuBrushR4;
  static Fl_Menu_Item *mnuBrushR8;
  static Fl_Menu_Item *mnuBrushR16;
  static Fl_Menu_Item *mnuBrushR32;
  static Fl_Menu_Item *mnuBrushR64;
  static Fl_Menu_Item *mnuBrushS4;
  static Fl_Menu_Item *mnuBrushS8;
  static Fl_Menu_Item *mnuBrushS16;
  static Fl_Menu_Item *mnuBrushS32;
  static Fl_Menu_Item *mnuBrushS64;
  ~CBrushProps();
  void show();
  void setPosition(int x, int y);
  void setSize(int width, int height);
  void hide();
  void setView(void* p);
};
#endif