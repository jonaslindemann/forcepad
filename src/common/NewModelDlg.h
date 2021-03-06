// generated by Fast Light User Interface Designer (fluid) version 1.0108

#ifndef NewModelDlg_h
#define NewModelDlg_h
#include <FL/Fl.H>
#define MR_OK 0
#define MR_CANCEL 1
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Slider.H>

class CNewModelDlg {
  int m_modalResult; 
public:
  CNewModelDlg();
  Fl_Double_Window *wndNewModel;
  Fl_Button *btnOk;
private:
  void cb_btnOk_i(Fl_Button*, void*);
  static void cb_btnOk(Fl_Button*, void*);
public:
  Fl_Button *btnCancel;
private:
  void cb_btnCancel_i(Fl_Button*, void*);
  static void cb_btnCancel(Fl_Button*, void*);
public:
  Fl_Group *grpNewModel;
  Fl_Value_Slider *sldImageWidth;
  Fl_Value_Slider *sldImageHeight;
  Fl_Value_Slider *sldInitialStiffness;
  ~CNewModelDlg();
  void show();
  int getModalResult();
  void setSize(int width, int height);
  void getSize(int &width, int &height);
  int getInitialStiffness();
  void centerWindow(Fl_Window* window);
};
#endif
