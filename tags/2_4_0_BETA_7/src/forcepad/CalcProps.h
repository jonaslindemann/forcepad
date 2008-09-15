// generated by Fast Light User Interface Designer (fluid) version 1.0105

#ifndef CalcProps_h
#define CalcProps_h
#include <FL/Fl.H>
#define MR_OK 0
#define MR_CANCEL 1
#include <FL/Fl_Double_Window.H>
#include "Fl_HoverButton.h"
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Light_Button.H>

class CCalcProps {
  void* m_view;
  int m_modalResult;
public:
  CCalcProps();
  Fl_Double_Window *wndCalcProps;
  Fl_HoverButton *btnOk;
private:
  inline void cb_btnOk_i(Fl_HoverButton*, void*);
  static void cb_btnOk(Fl_HoverButton*, void*);
public:
  Fl_Value_Slider *sldGridSize;
private:
  inline void cb_sldGridSize_i(Fl_Value_Slider*, void*);
  static void cb_sldGridSize(Fl_Value_Slider*, void*);
public:
  Fl_Light_Button *chkUseWeight;
private:
  inline void cb_chkUseWeight_i(Fl_Light_Button*, void*);
  static void cb_chkUseWeight(Fl_Light_Button*, void*);
public:
  ~CCalcProps();
  void show();
  void setPosition(int x, int y);
  void setSize(int width, int height);
  void setView(void* p);
  void hide();
};
#endif
