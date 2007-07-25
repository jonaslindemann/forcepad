// generated by Fast Light User Interface Designer (fluid) version 1.0104

#include "DrawingProps.h"
#include "PaintView.h"

inline void CDrawingProps::cb_btnOk_i(Fl_HoverButton*, void*) {
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

inline void CDrawingProps::cb_sldBlendFactor_i(Fl_Value_Slider*, void*) {
  if (m_view!=NULL)
{
	CPaintView* paintView = (CPaintView*)m_view;
	paintView->setBlendFactor((int)sldBlendFactor->value());
};
}
void CDrawingProps::cb_sldBlendFactor(Fl_Value_Slider* o, void* v) {
  ((CDrawingProps*)(o->parent()->user_data()))->cb_sldBlendFactor_i(o,v);
}

inline void CDrawingProps::cb_sldLineWidth_i(Fl_Value_Slider*, void*) {
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
  Fl_Double_Window* w;
  { Fl_Double_Window* o = wndDrawingProps = new Fl_Double_Window(184, 73, "Drawing properties");
    w = o;
    o->box(FL_THIN_UP_BOX);
    o->user_data((void*)(this));
    { Fl_HoverButton* o = btnOk = new Fl_HoverButton(174, 1, 9, 42);
      o->box(FL_PLASTIC_UP_BOX);
      o->down_box(FL_PLASTIC_DOWN_BOX);
      o->color(FL_BACKGROUND_COLOR);
      o->selection_color(FL_BACKGROUND_COLOR);
      o->image(image_additional_close);
      o->labeltype(FL_NORMAL_LABEL);
      o->labelfont(0);
      o->labelsize(12);
      o->labelcolor(FL_BLACK);
      o->callback((Fl_Callback*)cb_btnOk);
      o->align(FL_ALIGN_CENTER);
      o->when(FL_WHEN_RELEASE);
    }
    { Fl_Value_Slider* o = sldBlendFactor = new Fl_Value_Slider(57, 4, 108, 17, "Blending");
      o->type(5);
      o->box(FL_PLASTIC_DOWN_BOX);
      o->labelsize(10);
      o->maximum(255);
      o->step(1);
      o->value(255);
      o->callback((Fl_Callback*)cb_sldBlendFactor);
      o->align(FL_ALIGN_LEFT);
    }
    { Fl_Value_Slider* o = sldLineWidth = new Fl_Value_Slider(57, 24, 108, 17, "Line width");
      o->type(5);
      o->box(FL_PLASTIC_DOWN_BOX);
      o->labelsize(10);
      o->minimum(1);
      o->maximum(128);
      o->step(1);
      o->value(4);
      o->callback((Fl_Callback*)cb_sldLineWidth);
      o->align(FL_ALIGN_LEFT);
    }
    o->set_non_modal();
    o->clear_border();
    o->end();
  }
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
