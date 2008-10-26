// generated by Fast Light User Interface Designer (fluid) version 1.0300

#ifndef OptSettings_h
#define OptSettings_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Round_Button.H>

class COptSettings {
  void* m_paintView; 
public:
  COptSettings();
  Fl_Double_Window *mainWindow;
  Fl_Button *cancelButton;
private:
  void cb_cancelButton_i(Fl_Button*, void*);
  static void cb_cancelButton(Fl_Button*, void*);
public:
  Fl_Button *okButton;
private:
  void cb_okButton_i(Fl_Button*, void*);
  static void cb_okButton(Fl_Button*, void*);
public:
  Fl_Value_Slider *volFrac;
  Fl_Value_Slider *rmin;
  Fl_Value_Slider *minChange;
  Fl_Value_Slider *maxLoops;
  Fl_Round_Button *noFilter;
  Fl_Round_Button *sigmundFilter;
  Fl_Round_Button *backPedersenFilter;
  ~COptSettings();
  void show();
  void setView(void* view);
  void setData();
  void getData();
  void centerWindow(Fl_Window* window);
  void hide();
};
#endif
