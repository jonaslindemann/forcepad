// generated by Fast Light User Interface Designer (fluid) version 1.0108

#include "DrawingProps.h"
#include "PaintView.h"

void CDrawingProps::cb_btnOk_i(Fl_HoverButton*, void*) {
  m_modalResult = MR_OK;
wndDrawingProps->hide();
}
void CDrawingProps::cb_btnOk(Fl_HoverButton* o, void* v) {
  ((CDrawingProps*)(o->parent()->user_data()))->cb_btnOk_i(o,v);
}

#include <FL/Fl_Pixmap.H>
static const char *idata_additional_close[] = {
"     5    36       18            1",
"` c #656565",
". c #696969",
"# c #797975",
"a c #797979",
"b c #7d7d7d",
"c c #81817d",
"d c #898989",
"e c #959191",
"f c #a19d9d",
"g c #aeaeaa",
"h c #b2aeaa",
"i c #bab6b2",
"j c #c6c6c2",
"k c #cecec6",
"l c #cececa",
"m c #d2ceca",
"n c #d2d2ca",
"o c None",
"ooooo",
"ooooo",
"ooooo",
"ooooo",
"ooooo",
"ooooo",
"ooooo",
"ooooo",
"ooooo",
"ooooo",
"ooolo",
"oomfl",
"ooibl",
"omd#l",
"oh.#l",
"kc`#l",
"h``#l",
"h``#l",
"kc`#l",
"og`#l",
"okc#l",
"oogal",
"ookel",
"ooojn",
"ooooo",
"ooooo",
"ooooo",
"ooooo",
"ooooo",
"ooooo",
"ooooo",
"ooooo",
"ooooo",
"ooooo",
"ooooo",
"ooooo"
};
static Fl_Pixmap image_additional_close(idata_additional_close);

void CDrawingProps::cb_sldBlendFactor_i(Fl_Value_Slider*, void*) {
  if (m_view!=NULL)
{
	CPaintView* paintView = (CPaintView*)m_view;
	paintView->setBlendFactor((int)sldBlendFactor->value());
};
}
void CDrawingProps::cb_sldBlendFactor(Fl_Value_Slider* o, void* v) {
  ((CDrawingProps*)(o->parent()->user_data()))->cb_sldBlendFactor_i(o,v);
}

void CDrawingProps::cb_sldLineWidth_i(Fl_Value_Slider*, void*) {
  if (m_view!=NULL)
{
	CPaintView* paintView = (CPaintView*)m_view;
	paintView->setLineWidth((int)sldLineWidth->value());
};
}
void CDrawingProps::cb_sldLineWidth(Fl_Value_Slider* o, void* v) {
  ((CDrawingProps*)(o->parent()->user_data()))->cb_sldLineWidth_i(o,v);
}

CDrawingProps::CDrawingProps() {
  { wndDrawingProps = new Fl_Double_Window(184, 73, "Drawing properties");
    wndDrawingProps->box(FL_UP_BOX);
    wndDrawingProps->user_data((void*)(this));
    { btnOk = new Fl_HoverButton(174, 1, 9, 42);
      btnOk->box(FL_UP_BOX);
      btnOk->down_box(FL_DOWN_BOX);
      btnOk->color(FL_BACKGROUND_COLOR);
      btnOk->selection_color(FL_BACKGROUND_COLOR);
      btnOk->image(image_additional_close);
      btnOk->labeltype(FL_NORMAL_LABEL);
      btnOk->labelfont(0);
      btnOk->labelsize(12);
      btnOk->labelcolor(FL_FOREGROUND_COLOR);
      btnOk->callback((Fl_Callback*)cb_btnOk);
      btnOk->align(FL_ALIGN_CENTER);
      btnOk->when(FL_WHEN_RELEASE);
    } // Fl_HoverButton* btnOk
    { sldBlendFactor = new Fl_Value_Slider(57, 4, 108, 17, "Blending");
      sldBlendFactor->type(5);
      sldBlendFactor->labelsize(10);
      sldBlendFactor->maximum(255);
      sldBlendFactor->step(1);
      sldBlendFactor->value(255);
      sldBlendFactor->callback((Fl_Callback*)cb_sldBlendFactor);
      sldBlendFactor->align(FL_ALIGN_LEFT);
    } // Fl_Value_Slider* sldBlendFactor
    { sldLineWidth = new Fl_Value_Slider(57, 24, 108, 17, "Line width");
      sldLineWidth->type(5);
      sldLineWidth->labelsize(10);
      sldLineWidth->minimum(1);
      sldLineWidth->maximum(128);
      sldLineWidth->step(1);
      sldLineWidth->value(4);
      sldLineWidth->callback((Fl_Callback*)cb_sldLineWidth);
      sldLineWidth->align(FL_ALIGN_LEFT);
    } // Fl_Value_Slider* sldLineWidth
    wndDrawingProps->set_non_modal();
    wndDrawingProps->clear_border();
    wndDrawingProps->end();
  } // Fl_Double_Window* wndDrawingProps
}

CDrawingProps::~CDrawingProps() {
  delete wndDrawingProps;
}

void CDrawingProps::show() {
  setSize(wndDrawingProps->w(),btnOk->h()+2);

if (m_view!=NULL)
{
	CPaintView* paintView = (CPaintView*) m_view;
	sldBlendFactor->value(paintView->getBlendFactor());
	sldLineWidth->value(paintView->getLineWidth());
}

wndDrawingProps->show();
//while (wndDrawingProps->visible()) Fl::wait();
}

void CDrawingProps::setPosition(int x, int y) {
  wndDrawingProps->position(x, y);
}

void CDrawingProps::setSize(int width, int height) {
  wndDrawingProps->size_range(0, 0);
wndDrawingProps->size(width, height);
}

void CDrawingProps::setView(void* p) {
  m_view = p;
}

void CDrawingProps::hide() {
  wndDrawingProps->hide();
}
