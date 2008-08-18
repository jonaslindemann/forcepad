// generated by Fast Light User Interface Designer (fluid) version 1.0108

#ifndef LogWindow_h
#define LogWindow_h
#include <FL/Fl.H>
#ifdef WIN32
#include "editlog_stream.h"
#endif
#include "so_format.h"
#define so_show() CLogWindow::getInstance()->show();
#define so_print(a,b) CLogWindow::getInstance()->print(a,b);
#define so_println() CLogWindow::getInstance()->print("");
#define so_hide() CLogWindow::getInstance()->hide();
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Text_Display.H>

class CLogWindow {
#ifdef WIN32
  std::basic_streambuf<char>* m_oldBuf; 
  std::basic_editstreambuf<char> m_editStrBuf; 
#endif
  static CLogWindow* m_instance; 
  CLogWindow();
public:
  Fl_Double_Window *mainWindow;
  Fl_Text_Display *textDisplay;
  void hide();
  void show();
  static CLogWindow* getInstance();
  void print(const char* txt);
  void print(const char* context, const char* txt);
};
#endif