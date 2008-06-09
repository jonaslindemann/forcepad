// generated by Fast Light User Interface Designer (fluid) version 1.0104

#ifndef StiffnessProps_h
#define StiffnessProps_h
#include <FL/Fl.H>
#define MR_OK 0
#define MR_CANCEL 1
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include "Fl_ColorBox.h"
#include <FL/Fl_Value_Slider.H>

class CStiffnessProps {
  void* m_view;
  int m_modalResult;
public:
  CStiffnessProps();
  Fl_Double_Window *wnd;
  Fl_Button *btnOk;
private:
  inline void cb_btnOk_i(Fl_Button*, void*);
  static void cb_btnOk(Fl_Button*, void*);
public:
  Fl_ColorBox *boxStiffnessColor;
  Fl_Value_Slider *sldStiffness;
private:
  inline void cb_sldStiffness_i(Fl_Value_Slider*, void*);
  static void cb_sldStiffness(Fl_Value_Slider*, void*);
public:
  ~CStiffnessProps();
  void show();
  void setPosition(int x, int y);
  void setSize(int width, int height);
  void setView(void* p);
  void hide();
};
#endif
