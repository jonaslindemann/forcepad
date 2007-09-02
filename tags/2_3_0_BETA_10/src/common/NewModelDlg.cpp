// generated by Fast Light User Interface Designer (fluid) version 1.0108

#include "NewModelDlg.h"

void CNewModelDlg::cb_btnOk_i(Fl_Button*, void*) {
  m_modalResult = MR_OK;
wndNewModel->hide();
}
void CNewModelDlg::cb_btnOk(Fl_Button* o, void* v) {
  ((CNewModelDlg*)(o->parent()->user_data()))->cb_btnOk_i(o,v);
}

void CNewModelDlg::cb_btnCancel_i(Fl_Button*, void*) {
  m_modalResult = MR_CANCEL;
wndNewModel->hide();
}
void CNewModelDlg::cb_btnCancel(Fl_Button* o, void* v) {
  ((CNewModelDlg*)(o->parent()->user_data()))->cb_btnCancel_i(o,v);
}

CNewModelDlg::CNewModelDlg() {
  { wndNewModel = new Fl_Double_Window(309, 142, "New model");
    wndNewModel->user_data((void*)(this));
    { btnOk = new Fl_Button(236, 6, 67, 25, "OK");
      btnOk->down_box(FL_DOWN_BOX);
      btnOk->labelsize(12);
      btnOk->callback((Fl_Callback*)cb_btnOk);
    } // Fl_Button* btnOk
    { btnCancel = new Fl_Button(236, 37, 67, 25, "Cancel");
      btnCancel->down_box(FL_DOWN_BOX);
      btnCancel->labelsize(12);
      btnCancel->callback((Fl_Callback*)cb_btnCancel);
    } // Fl_Button* btnCancel
    { grpNewModel = new Fl_Group(5, 6, 223, 129);
      grpNewModel->box(FL_DOWN_FRAME);
      { sldImageWidth = new Fl_Value_Slider(17, 28, 201, 20, "Image width (pixels)");
        sldImageWidth->type(1);
        sldImageWidth->labelsize(12);
        sldImageWidth->minimum(160);
        sldImageWidth->maximum(1280);
        sldImageWidth->step(20);
        sldImageWidth->value(640);
        sldImageWidth->align(FL_ALIGN_TOP_LEFT);
      } // Fl_Value_Slider* sldImageWidth
      { sldImageHeight = new Fl_Value_Slider(17, 68, 201, 20, "Image height (pixels)");
        sldImageHeight->type(1);
        sldImageHeight->labelsize(12);
        sldImageHeight->minimum(160);
        sldImageHeight->maximum(1280);
        sldImageHeight->step(20);
        sldImageHeight->value(480);
        sldImageHeight->align(FL_ALIGN_TOP_LEFT);
      } // Fl_Value_Slider* sldImageHeight
      { sldInitialStiffness = new Fl_Value_Slider(17, 107, 201, 20, "Initial stiffness (0 = no stiffness)");
        sldInitialStiffness->type(1);
        sldInitialStiffness->labelsize(12);
        sldInitialStiffness->maximum(255);
        sldInitialStiffness->step(1);
        sldInitialStiffness->align(FL_ALIGN_TOP_LEFT);
      } // Fl_Value_Slider* sldInitialStiffness
      grpNewModel->end();
    } // Fl_Group* grpNewModel
    wndNewModel->set_modal();
    wndNewModel->end();
  } // Fl_Double_Window* wndNewModel
}

CNewModelDlg::~CNewModelDlg() {
  wndNewModel->hide();
delete wndNewModel;
}

void CNewModelDlg::show() {
  wndNewModel->show();
while (wndNewModel->visible()) Fl::wait();
}

int CNewModelDlg::getModalResult() {
  return m_modalResult;
}

void CNewModelDlg::setSize(int width, int height) {
  sldImageWidth->value(width);
sldImageHeight->value(height);
}

void CNewModelDlg::getSize(int &width, int &height) {
  width = sldImageWidth->value();
height = sldImageHeight->value();
}

int CNewModelDlg::getInitialStiffness() {
  return sldInitialStiffness->value();
}

void CNewModelDlg::centerWindow(Fl_Window* window) {
  wndNewModel->position(window->x()+window->w()/2-wndNewModel->w()/2, window->y()+window->h()/2-wndNewModel->h()/2);
}
