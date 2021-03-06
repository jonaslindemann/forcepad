// generated by Fast Light User Interface Designer (fluid) version 1.0104

#ifndef SplashFrame_h
#define SplashFrame_h
#include <FL/Fl.H>
void cbHide(void* w);
/** Splash window class. */
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

class CSplashFrame {
  bool m_timer;
/** SplashFrame class constructor. */
public:
  CSplashFrame();
  Fl_Double_Window *wndSplash;
  Fl_Box *boxImage;
  Fl_Button *btnClose;
private:
  inline void cb_btnClose_i(Fl_Button*, void*);
  static void cb_btnClose(Fl_Button*, void*);
/** Show window and start timer. */
public:
  void show();
/** Center window on screen. */
  void center();
/** Enable/Disable timer for window closure. */
  void setTimer(bool flag);
/** Hide window. */
  void hide();
};
#endif
