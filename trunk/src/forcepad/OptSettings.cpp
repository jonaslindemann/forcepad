// generated by Fast Light User Interface Designer (fluid) version 1.0302

#include "OptSettings.h"
#include "FlPaintView.h"

void COptSettings::cb_cancelButton_i(Fl_Button*, void*) {
  mainWindow->hide();
}
void COptSettings::cb_cancelButton(Fl_Button* o, void* v) {
  ((COptSettings*)(o->parent()->user_data()))->cb_cancelButton_i(o,v);
}

void COptSettings::cb_okButton_i(Fl_Button*, void*) {
  this->getData();
mainWindow->hide();
}
void COptSettings::cb_okButton(Fl_Button* o, void* v) {
  ((COptSettings*)(o->parent()->user_data()))->cb_okButton_i(o,v);
}

COptSettings::COptSettings() {
  { mainWindow = new Fl_Double_Window(318, 291, "Optimisation Settings");
    mainWindow->color(FL_FOREGROUND_COLOR);
    mainWindow->labelcolor(FL_BACKGROUND2_COLOR);
    mainWindow->user_data((void*)(this));
    { cancelButton = new Fl_Button(230, 255, 70, 25, "Cancel");
      cancelButton->box(FL_FLAT_BOX);
      cancelButton->color(FL_DARK3);
      cancelButton->labelsize(11);
      cancelButton->callback((Fl_Callback*)cb_cancelButton);
    } // Fl_Button* cancelButton
    { okButton = new Fl_Button(154, 255, 70, 25, "OK");
      okButton->box(FL_FLAT_BOX);
      okButton->color(FL_DARK3);
      okButton->labelsize(11);
      okButton->callback((Fl_Callback*)cb_okButton);
    } // Fl_Button* okButton
    { volFrac = new Fl_Value_Slider(14, 25, 286, 20, "Volume fraction");
      volFrac->type(1);
      volFrac->box(FL_FLAT_BOX);
      volFrac->color((Fl_Color)37);
      volFrac->labelsize(11);
      volFrac->labelcolor(FL_BACKGROUND2_COLOR);
      volFrac->value(0.5);
      volFrac->textsize(11);
      volFrac->textcolor(FL_BACKGROUND2_COLOR);
      volFrac->align(Fl_Align(FL_ALIGN_TOP_LEFT));
    } // Fl_Value_Slider* volFrac
    { rmin = new Fl_Value_Slider(14, 65, 286, 20, "Filter r,min (elements)");
      rmin->type(1);
      rmin->box(FL_FLAT_BOX);
      rmin->color((Fl_Color)37);
      rmin->labelsize(11);
      rmin->labelcolor(FL_BACKGROUND2_COLOR);
      rmin->minimum(1);
      rmin->maximum(5);
      rmin->value(2.75);
      rmin->textsize(11);
      rmin->textcolor(FL_BACKGROUND2_COLOR);
      rmin->align(Fl_Align(FL_ALIGN_TOP_LEFT));
    } // Fl_Value_Slider* rmin
    { minChange = new Fl_Value_Slider(15, 105, 286, 20, "Min change stop criteria");
      minChange->type(1);
      minChange->box(FL_FLAT_BOX);
      minChange->color((Fl_Color)37);
      minChange->labelsize(11);
      minChange->labelcolor(FL_BACKGROUND2_COLOR);
      minChange->minimum(0.01);
      minChange->maximum(0.1);
      minChange->value(0.01);
      minChange->textsize(11);
      minChange->textcolor(FL_BACKGROUND2_COLOR);
      minChange->align(Fl_Align(FL_ALIGN_TOP_LEFT));
    } // Fl_Value_Slider* minChange
    { maxLoops = new Fl_Value_Slider(15, 147, 286, 20, "Max loops stop criteria");
      maxLoops->type(1);
      maxLoops->box(FL_FLAT_BOX);
      maxLoops->color((Fl_Color)37);
      maxLoops->labelsize(11);
      maxLoops->labelcolor(FL_BACKGROUND2_COLOR);
      maxLoops->minimum(1);
      maxLoops->maximum(1000);
      maxLoops->step(1);
      maxLoops->value(50);
      maxLoops->textsize(11);
      maxLoops->textcolor(FL_BACKGROUND2_COLOR);
      maxLoops->align(Fl_Align(FL_ALIGN_TOP_LEFT));
    } // Fl_Value_Slider* maxLoops
    { Fl_Group* o = new Fl_Group(15, 187, 285, 56, "Sensitivity filter");
      o->box(FL_FLAT_BOX);
      o->color((Fl_Color)37);
      o->labelsize(11);
      o->labelcolor(FL_BACKGROUND2_COLOR);
      o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      { noFilter = new Fl_Round_Button(22, 193, 64, 15, "No filter");
        noFilter->type(102);
        noFilter->down_box(FL_ROUND_DOWN_BOX);
        noFilter->labelsize(11);
        noFilter->labelcolor(FL_BACKGROUND2_COLOR);
      } // Fl_Round_Button* noFilter
      { sigmundFilter = new Fl_Round_Button(22, 208, 88, 15, "Ole Sigmund");
        sigmundFilter->type(102);
        sigmundFilter->down_box(FL_ROUND_DOWN_BOX);
        sigmundFilter->labelsize(11);
        sigmundFilter->labelcolor(FL_BACKGROUND2_COLOR);
      } // Fl_Round_Button* sigmundFilter
      { backPedersenFilter = new Fl_Round_Button(22, 223, 93, 15, "Sharp Contouring Filter");
        backPedersenFilter->type(102);
        backPedersenFilter->down_box(FL_ROUND_DOWN_BOX);
        backPedersenFilter->labelsize(11);
        backPedersenFilter->labelcolor(FL_BACKGROUND2_COLOR);
      } // Fl_Round_Button* backPedersenFilter
      o->end();
    } // Fl_Group* o
    mainWindow->set_modal();
    mainWindow->end();
  } // Fl_Double_Window* mainWindow
  m_paintView = NULL;
}

COptSettings::~COptSettings() {
  mainWindow->hide();
}

void COptSettings::show() {
  this->setData();
  mainWindow->show();
  while (mainWindow->visible()) Fl::wait();
}

void COptSettings::setView(void* view) {
  m_paintView = (CFlPaintView*)view;
}

void COptSettings::setData() {
  if (m_paintView!=NULL)
  {
  	CFlPaintView* view = (CFlPaintView*)m_paintView;
  	
  	volFrac->value(view->getOptVolumeFraction());
  	rmin->value(view->getOptRmin());
  	minChange->value(view->getOptMinChange());
  	maxLoops->value(view->getOptMaxLoops());
  	
  	if (view->getOptFilterType()==CFemGridSolver2::FT_NO_FILTER)
  		noFilter->value(1);
  	if (view->getOptFilterType()==CFemGridSolver2::FT_OLE_SIGMUND)
  		sigmundFilter->value(1);
  	if (view->getOptFilterType()==CFemGridSolver2::FT_SHARP_CONTOURING)
  		backPedersenFilter->value(1);
  
  }
}

void COptSettings::getData() {
  if (m_paintView!=NULL)
  {
  	CFlPaintView* view = (CFlPaintView*)m_paintView;
  	
  	view->setOptVolumeFraction(volFrac->value());
  	view->setOptRmin(rmin->value());
  	view->setOptMinChange(minChange->value());
  	view->setOptMaxLoops(maxLoops->value());
  	
  	if (noFilter->value()==1)
  		view->setOptFilterType(CFemGridSolver2::FT_NO_FILTER);
  	if (sigmundFilter->value()==1)
  		view->setOptFilterType(CFemGridSolver2::FT_OLE_SIGMUND);
  	if (backPedersenFilter->value()==1)
  		view->setOptFilterType(CFemGridSolver2::FT_SHARP_CONTOURING);
  	
  }
}

void COptSettings::centerWindow(Fl_Window* window) {
  mainWindow->position(window->x()+window->w()/2-mainWindow->w()/2, window->y()+window->h()/2-mainWindow->h()/2);
}

void COptSettings::hide() {
  mainWindow->hide();
}
