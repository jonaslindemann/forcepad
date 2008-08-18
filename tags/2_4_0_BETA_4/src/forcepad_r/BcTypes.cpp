// generated by Fast Light User Interface Designer (fluid) version 1.0104

#include "BcTypes.h"
#include "PaintView.h"

inline void CBcTypes::cb_btnOk_i(Fl_HoverButton*, void*) {
  m_modalResult = MR_OK;
wnd->hide();
}
void CBcTypes::cb_btnOk(Fl_HoverButton* o, void* v) {
  ((CBcTypes*)(o->parent()->user_data()))->cb_btnOk_i(o,v);
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

inline void CBcTypes::cb_btnBcX_i(Fl_HoverButton*, void*) {
  if (m_view!=NULL)
{
	CPaintView* paintView = (CPaintView*) m_view;
	paintView->setConstraintType(CConstraint::CT_X);
	paintView->setEditMode(CPaintView::EM_CONSTRAINT);
}
wnd->hide();
}
void CBcTypes::cb_btnBcX(Fl_HoverButton* o, void* v) {
  ((CBcTypes*)(o->parent()->user_data()))->cb_btnBcX_i(o,v);
}

static const char *idata_bcx[] = {
"    36    36       96            2",
"`` c #7d7d79",
"`. c #302ca5",
"`# c #3030a5",
"`a c #3430a5",
"`b c #3030aa",
"`c c #3434aa",
"`d c #3834aa",
"`e c #3834ae",
"`f c #3838ae",
"`g c #3c3cb2",
"`h c #0000c2",
"`i c #442891",
"`j c #403cb6",
"`k c #4040b6",
"`l c #4444ba",
"`m c #4848be",
"`n c #6d6995",
"`o c #6161aa",
"`p c #7171b6",
"`q c #4848c2",
"`r c #4c48c2",
"`s c #4c4cc2",
"`t c #504cc6",
"`u c #7d7dc2",
"`v c #ba0c0c",
"`w c #ba1818",
"`x c #aa2828",
"`y c #ae3030",
"`z c #b22424",
"`A c #b22828",
"`B c #955d5d",
"`C c #816d69",
"`D c #817d79",
"`E c #817d7d",
"`F c #897171",
"`G c #956965",
"`H c #957571",
"`I c #ae7171",
"`J c #c20000",
"`K c #ce3c3c",
"`L c #c24848",
"`M c #c65550",
"`N c #de7d7d",
"`O c #81817d",
"`P c #85817d",
"`Q c #858181",
"`R c #858581",
"`S c #8d8985",
"`T c #8d8d89",
"`U c #91918d",
"`V c #95918d",
"`W c #959591",
"`X c #999591",
"`Y c #9d9591",
"`Z c #999995",
"`0 c #9d9995",
"`1 c #9d9d99",
"`2 c #a59995",
"`3 c #a19d99",
"`4 c #aa9d99",
"`5 c #b28581",
"`6 c #a5a19d",
"`7 c #baa19d",
"`8 c #a5a5a1",
".` c #aaa5a1",
".. c #aaaaa5",
".# c #aeaeaa",
".a c #b2aeaa",
".b c #baaeaa",
".c c #b2b2ae",
".d c #b6b2ae",
".e c #b6b6b2",
".f c #bab6b2",
".g c #babab2",
".h c #babab6",
".i c #bebab6",
".j c #bebeb6",
".k c #8181c6",
".l c #9995c2",
".m c #da8181",
".n c #da9d9d",
".o c #de9d9d",
".p c #c2beba",
".q c #e69d9d",
".r c #c2c2ba",
".s c #c2c2be",
".t c #c6c2be",
".u c #c6c6be",
".v c #cac6c2",
".w c #cacac2",
".x c #cecac6",
".y c #cecec6",
".z c #d2ceca",
".A c #d2d2ca",
".B c None",
".C c #ffffff",
".B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.B.B.B.A.A.A.A.B.B.B.B.B.B.A.A.A.A.B.B.B.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.B.k`t`s`u.x.x.z.A.B.B.k`t`s`u.x.x.z.A.B.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.B`t`h`h`m.j.s.x.z.B.B`t`h`h`m.j.s.x.z.B.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.A`s`h`h`k...d.s.x.A.A`s`h`h`k...d.s.x.A.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.A`s`h`h`f`X`8.h.w.z.z`s`h`h`f`X`8.h.w.A.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`c`S`1.e.v.z.z`r`h`h`c`S`1.e.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`R`Z.d.v.z.z`r`h`h`b`R`Z.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`Q`X.d.v.z.z`r`h`h`b`Q`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`Q`X.d.v.z.z`r`h`h`b`Q`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`Q`X.d.v.z.z`r`h`h`b`Q`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`Q`X.d.v.z.z`r`h`h`b`Q`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`Q`X.d.v.z.z`r`h`h`b`Q`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`Q`X.a.t.y.y`q`h`h`b`Q`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`P`W.#.p.v.v`m`h`h`#`Q`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`#`O`U`4`7.b.g`l`h`h`#`Q`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`#`D`5.n.m`M`I`g`h`h`#`P`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`#`F.o.C.q`K`A`d`h`h`#`O`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`#`G`N.q`N`K`v`i`h`h`.`O`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`#`C`L`K`K`J`x`a`h`h`.`O`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`#```B`z`w`y`H`e`h`h`#`O`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`#`E`T`Y`2`6.``j`h`h`#`P`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`O`V.`.f.i.i`l`h`h`#`Q`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`P`W.a.r.v.v`m`h`h`b`Q`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`Q`X.c.u.y.y`r`h`h`b`Q`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`Q`X.d.v.z.z`r`h`h`b`Q`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`Q`X.d.v.z.z`r`h`h`b`Q`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`Q`X.d.v.z.z`r`h`h`b`Q`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`Q`X.d.v.z.z`r`h`h`b`Q`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`Q`X.d.v.z.z`r`h`h`b`Q`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`R`X.d.v.z.z`r`h`h`b`R`X.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z`r`h`h`b`R`Z.d.v.z.z`r`h`h`b`R`Z.d.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.z.l`p`o`n`T`3.f.v.z.z.l`p`o`n`T`3.f.v.z.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.A.x.i.``0`0.`.i.x.A.A.x.i.``0`0.`.i.x.A.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.A.y.t.f.#.#.f.t.y.A.A.y.t.f.#.#.f.t.y.A.B.B.B.B.B.B",
".B.B.B.B.B.B.B.B.B.B.B.A.x.u.r.r.u.x.A.B.B.A.x.u.r.r.u.x.A.B.B.B.B.B.B.B"
};
static Fl_Pixmap image_bcx(idata_bcx);

inline void CBcTypes::cb_btnBcY_i(Fl_HoverButton*, void*) {
  if (m_view!=NULL)
{
	CPaintView* paintView = (CPaintView*) m_view;
	paintView->setConstraintType(CConstraint::CT_Y);
	paintView->setEditMode(CPaintView::EM_CONSTRAINT);
}
wnd->hide();
}
void CBcTypes::cb_btnBcY(Fl_HoverButton* o, void* v) {
  ((CBcTypes*)(o->parent()->user_data()))->cb_btnBcY_i(o,v);
}

static const char *idata_bcy[] = {
"    36    36      109            2",
"`` c #7d7179",
"`. c #0404be",
"`# c #0808be",
"`a c #1414b6",
"`b c #1410be",
"`c c #1814b6",
"`d c #1818b6",
"`e c #1c18ba",
"`f c #1c1cba",
"`g c #2020b6",
"`h c #2020be",
"`i c #0000c2",
"`j c #0808c2",
"`k c #1414c2",
"`l c #2c2cc2",
"`m c #696585",
"`n c #6d6989",
"`o c #6d6d8d",
"`p c #797981",
"`q c #7d7981",
"`r c #7d7d81",
"`s c #7d7d85",
"`t c #7d7191",
"`u c #7d7d9d",
"`v c #ba0c0c",
"`w c #ba1818",
"`x c #aa2828",
"`y c #aa2c28",
"`z c #b22828",
"`A c #ba2c2c",
"`B c #955d59",
"`C c #856161",
"`D c #85797d",
"`E c #897175",
"`F c #957975",
"`G c #c20000",
"`H c #ce3c3c",
"`I c #c24c4c",
"`J c #ce5555",
"`K c #de7d7d",
"`L c #85817d",
"`M c #89817d",
"`N c #da817d",
"`O c #858581",
"`P c #818189",
"`Q c #898985",
"`R c #8d8d89",
"`S c #818199",
"`T c #898991",
"`U c #918985",
"`V c #918d8d",
"`W c #91918d",
"`X c #95918d",
"`Y c #959591",
"`Z c #999591",
"`0 c #99959d",
"`1 c #999995",
"`2 c #9d9995",
"`3 c #9d9d99",
"`4 c #8d85a5",
"`5 c #8d8dae",
"`6 c #918dae",
"`7 c #9d99ba",
"`8 c #a19d99",
".` c #b68985",
".. c #b29995",
".# c #a19dbe",
".a c #a1a19d",
".b c #a5a5a1",
".c c #aaa5a1",
".d c #aaaaa5",
".e c #aeaaa5",
".f c #aeaaae",
".g c #aeaeaa",
".h c #b2b2ae",
".i c #b6b2ae",
".j c #b6b6b2",
".k c #bab6b2",
".l c #babab6",
".m c #bebab6",
".n c #bebabe",
".o c #a5a1c2",
".p c #aaa5c6",
".q c #aaaac6",
".r c #aeaaca",
".s c #aeaeca",
".t c #b2aeca",
".u c #c28585",
".v c #c69595",
".w c #de8181",
".x c #da9d99",
".y c #cab2ae",
".z c #c2beba",
".A c #e69d9d",
".B c #e2a19d",
".C c #c2c2ba",
".D c #c2c2be",
".E c #c6c2be",
".F c #c6c6be",
".G c #cac6c2",
".H c #cac6c6",
".I c #cecac6",
".J c #cecec6",
".K c #cececa",
".L c #cecece",
".M c #d2ceca",
".N c #d2d2ca",
".O c None",
".P c #ffffff",
".O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O",
".O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O",
".O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O",
".O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O",
".O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O",
".O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O",
".O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O",
".O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O",
".O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O",
".O.O.O.O.O.O.O.O.O.N.N.N.N.N.N.N.N.N.N.N.N.N.N.N.N.N.N.N.N.N.N.O.O.O.O.O",
".O.O.L.s.r.q.p.p.p.p.p.p.p.p.p.p.p.p.p.p.p.p.p.p.p.p.p.p.p.p.p.p.q.H.M.N",
".O.O.s`k`j`j`j`j`j`j`j`j`j`j`j`j`j`j`j`j`j`j`j`j`j`j`j`j`j`j`j`j`b.#.F.I",
".O.O.r`j`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`#`5.j.C",
".O.O.t`l`h`f`e`d`d`d`d`d`d`d`d`d`d`d`d`d`d`d`d`d`d`d`d`d`d`d`d`d`g`S.b.k",
".O.O.K.n.f`0`T`P`s`s`s`s`s`s`s`s`s`D`E```p`p`q`r`s`s`s`s`s`s`s`s`P`V`8.i",
".O.O.M.G.l.d`2`Y`X`X`W`W`W`W`W`W.`.x`N`I`B`O`Q`R`W`W`W`W`W`W`X`X`Y`2.d.l",
".O.O.N.I.E.l.i.g.e.e.e.e.e.e.e...B.P.A`H`z`U`Z.a.c.e.e.e.e.e.e.e.g.i.l.E",
".O.O.O.N.I.G.F.E.E.E.D.D.D.D.D.v.w.A`K`H`v`F`3.g.m.z.D.D.D.D.E.E.E.F.G.I",
".O.O.O.O.N.M.M.M.M.M.M.M.M.M.J.y`J`H`H`G`x`M`2.h.E.I.J.M.M.M.M.M.M.M.M.N",
".O.O.O.O.O.N.N.N.N.N.N.N.N.N.M.I.u`A`w`y`C`L`1.h.E.J.N.N.N.N.N.N.N.N.N.O",
".O.O.L.s.r.q.p.p.p.p.p.p.p.p.p.o`7`4`t`n`m`o`u`6.#.o.p.p.p.p.p.p.q.H.M.N",
".O.O.s`k`j`j`j`j`j`j`j`j`j`j`j`#`#`#`#`.`.`.`#`#`#`#`j`j`j`j`j`j`b.#.F.I",
".O.O.r`j`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`i`#`5.j.C",
".O.O.t`l`h`f`e`d`d`d`d`d`d`d`d`d`d`d`d`c`a`c`d`d`d`d`d`d`d`d`d`d`g`S.b.k",
".O.O.K.n.f`0`T`P`s`s`s`s`s`s`s`s`s`r`q`q`p`q`q`r`s`s`s`s`s`s`s`s`P`V`8.i",
".O.O.M.G.l.d`2`Y`X`X`W`W`W`W`W`W`W`W`W`W`W`W`W`W`W`W`W`W`W`W`X`X`Y`2.d.l",
".O.O.N.I.E.l.i.g.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.g.i.l.E",
".O.O.O.N.I.G.F.E.E.E.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.E.E.E.F.G.I",
".O.O.O.O.N.M.M.M.M.M.M.M.M.M.M.M.M.M.M.M.M.M.M.M.M.M.M.M.M.M.M.M.M.M.M.N",
".O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O",
".O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O",
".O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O",
".O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O",
".O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O",
".O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O",
".O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O.O"
};
static Fl_Pixmap image_bcy(idata_bcy);

inline void CBcTypes::cb_btnBcXY_i(Fl_HoverButton*, void*) {
  if (m_view!=NULL)
{
	CPaintView* paintView = (CPaintView*) m_view;
	paintView->setConstraintType(CConstraint::CT_XY);
	paintView->setEditMode(CPaintView::EM_CONSTRAINT);
}
wnd->hide();
}
void CBcTypes::cb_btnBcXY(Fl_HoverButton* o, void* v) {
  ((CBcTypes*)(o->parent()->user_data()))->cb_btnBcXY_i(o,v);
}

static const char *idata_bcxy[] = {
"    36    36      121            2",
"`` c #7d7d79",
"`. c #2c28a1",
"`# c #2c2ca1",
"`a c #2c2ca5",
"`b c #302ca5",
"`c c #3030a5",
"`d c #3030aa",
"`e c #3434aa",
"`f c #3838b2",
"`g c #3c38b2",
"`h c #3c3cb2",
"`i c #0000c2",
"`j c #504c95",
"`k c #505095",
"`l c #505099",
"`m c #555599",
"`n c #4040b6",
"`o c #4040ba",
"`p c #4440ba",
"`q c #4444ba",
"`r c #4844be",
"`s c #4848be",
"`t c #5d59a1",
"`u c #5d5da1",
"`v c #696991",
"`w c #6d6995",
"`x c #6d6d95",
"`y c #6565ae",
"`z c #6965ae",
"`A c #6969b2",
"`B c #6d6db2",
"`C c #716db6",
"`D c #7171b6",
"`E c #7571ba",
"`F c #7575ba",
"`G c #7975be",
"`H c #4848c2",
"`I c #4c48c2",
"`J c #4c4cc2",
"`K c #7d7dc2",
"`L c #ba0c0c",
"`M c #ba1818",
"`N c #aa2828",
"`O c #ae302c",
"`P c #b22c2c",
"`Q c #ba2c2c",
"`R c #817d79",
"`S c #916d69",
"`T c #917975",
"`U c #b27575",
"`V c #b67975",
"`W c #c20000",
"`X c #ce3c3c",
"`Y c #ca5050",
"`Z c #ca5555",
"`0 c #de7d7d",
"`1 c #817dc6",
"`2 c #85817d",
"`3 c #de817d",
"`4 c #858181",
"`5 c #858581",
"`6 c #898985",
"`7 c #8d8989",
"`8 c #8d8d89",
".` c #918d8d",
".. c #91918d",
".# c #959191",
".a c #959591",
".b c #999591",
".c c #9d9591",
".d c #999995",
".e c #9d9995",
".f c #9d9d99",
".g c #8585ae",
".h c #a18985",
".i c #a19d99",
".j c #b28181",
".k c #b69d99",
".l c #a1a19d",
".m c #a5a19d",
".n c #aea19d",
".o c #a5a5a1",
".p c #aaa5a1",
".q c #aaaaa5",
".r c #aeaaa5",
".s c #aeaeaa",
".t c #b2a5a1",
".u c #b2aeaa",
".v c #bea5a1",
".w c #baaeaa",
".x c #b2b2ae",
".y c #b6b2ae",
".z c #b6b6ae",
".A c #b6b6b2",
".B c #bab6b2",
".C c #babab6",
".D c #bebab6",
".E c #bebeb6",
".F c #bebeba",
".G c #9999c2",
".H c #9d99c2",
".I c #de8181",
".J c #c6bab6",
".K c #c2beba",
".L c #d2a1a1",
".M c #e69d9d",
".N c #e2a1a1",
".O c #e2a5a5",
".P c #c2c2ba",
".Q c #c2c2be",
".R c #c6c2be",
".S c #c6c6be",
".T c #cac6c2",
".U c #cacac2",
".V c #cecac2",
".W c #cecac6",
".X c #cecec6",
".Y c #d2ceca",
".Z c #d2d2ca",
".0 c None",
".1 c #ffffff",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.Z.Y.W.U.T.U.W.Y.Z.0.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.0`1`J`s`r`D.D.Q.W.Y.0.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Z`J`i`i`i`h.o.y.Q.W.Z.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Z`J`i`i`i`e...m.C.U.Z.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Y`I`i`i`i`a`4.e.z.T.Y.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Y`I`i`i`i`#``.b.y.T.Y.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Y`I`i`i`i`.``.a.y.T.Y.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Y`I`i`i`i`.``.a.x.T.Y.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Y`I`i`i`i`.``.a.y.T.Y.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Y`I`i`i`i`#`R.b.y.T.Y.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Y`I`i`i`i`b`5.f.A.T.Y.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Z.G`E`z`t`w.#.p.D.U.Z.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.Z.Z.Y.Y.Y.Y.Y.Y.Y.Z.Y.W.Q.y.p.l.p.y.Q.W.Y.Z.Y.Y.Y.Y.Y.Y.Y.Z.Z.0",
".0.0.Z.Y.W.U.T.T.T.T.T.T.T.U.W.V.J.v.t.s.y.D.S.W.W.U.T.T.T.T.T.T.T.U.W.Y",
".0.0`1`J`s`q`q`p`p`p`p`p`C.C.K.L.O.I`Z`U.s.B`G`H`s`q`q`p`p`p`p`p`C.C.Q.W",
".0.0`J`i`i`i`i`i`i`i`i`i`f.m.k.N.1.M`X`P.c.r`q`i`i`i`i`i`i`i`i`i`f.m.y.Q",
".0.0`J`i`i`i`i`i`i`i`i`i`d.`.j`3.M`0`X`L`T.f`n`i`i`i`i`i`i`i`i`i`d...o.D",
".0.0`J`i`i`i`i`i`i`i`i`i`b`6.h`Y`X`X`W`N`2.d`n`i`i`i`i`i`i`i`i`i`c`7.l.B",
".0.0.H`F`z`u`m`l`k`j`k`l`v.#.m`V`Q`M`O`S`8.i.g`B`y`t`m`l`k`j`k`l`v.a.p.D",
".0.0.X.R.B.q.l.i.f.f.f.i.l.q.y.C.w.n.e.d.i.s.C.D.y.q.l.i.f.f.f.i.l.q.B.R",
".0.0.Z.W.S.K.D.B.B.B.B.B.D.F.R.S.P.B.s.q.u.C.R.T.R.F.D.B.B.B.B.B.D.K.S.W",
".0.0.0.Z.Y.W.W.U.U.U.U.U.W.W.W`K`H`q`o`A.u.D.S.W.W.W.W.U.U.U.U.U.W.W.Y.Z",
".0.0.0.0.0.Z.Z.Z.Z.Z.Z.Z.Z.Z.Y`J`i`i`i`g.m.u.K.W.Y.Z.Z.Z.Z.Z.Z.Z.Z.Z.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Z`J`i`i`i`e.`.m.C.U.Z.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Y`I`i`i`i`a`4.e.z.T.Y.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Y`I`i`i`i`#``.b.y.T.Y.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Y`I`i`i`i`.``.a.y.T.Y.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Y`I`i`i`i`.``.a.x.T.Y.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Y`I`i`i`i`.``.a.y.T.Y.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Y`I`i`i`i`#`R.b.y.T.Y.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Y`I`i`i`i`b`5.f.A.T.Y.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Z.H`E`z`u`x.a.p.D.W.Z.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.Z.X.R.B.r.p.r.B.R.X.Z.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.Z.W.S.K.E.K.S.W.Z.0.0.0.0.0.0.0.0.0.0.0.0",
".0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.Z.Y.W.W.W.Y.Z.0.0.0.0.0.0.0.0.0.0.0.0.0"
};
static Fl_Pixmap image_bcxy(idata_bcxy);

CBcTypes::CBcTypes() {
  Fl_Window* w;
  { Fl_Window* o = wnd = new Fl_Window(144, 100, "Brush properties");
    w = o;
    o->box(FL_THIN_UP_BOX);
    o->user_data((void*)(this));
    { Fl_HoverButton* o = btnOk = new Fl_HoverButton(132, 3, 9, 42);
      o->box(FL_PLASTIC_UP_BOX);
      o->down_box(FL_PLASTIC_DOWN_BOX);
      o->color(49);
      o->selection_color(49);
      o->image(image_additional_close);
      o->labeltype(FL_NORMAL_LABEL);
      o->labelfont(0);
      o->labelsize(12);
      o->labelcolor(56);
      o->callback((Fl_Callback*)cb_btnOk);
      o->align(FL_ALIGN_CENTER);
      o->when(FL_WHEN_RELEASE);
    }
    { Fl_HoverButton* o = btnBcX = new Fl_HoverButton(3, 3, 42, 42);
      o->box(FL_PLASTIC_UP_BOX);
      o->down_box(FL_PLASTIC_DOWN_BOX);
      o->color(51);
      o->selection_color(51);
      o->image(image_bcx);
      o->labeltype(FL_NORMAL_LABEL);
      o->labelfont(0);
      o->labelsize(14);
      o->labelcolor(56);
      o->callback((Fl_Callback*)cb_btnBcX);
      o->align(FL_ALIGN_CENTER);
      o->when(FL_WHEN_RELEASE);
    }
    { Fl_HoverButton* o = btnBcY = new Fl_HoverButton(46, 3, 42, 42);
      o->box(FL_PLASTIC_UP_BOX);
      o->down_box(FL_PLASTIC_DOWN_BOX);
      o->color(51);
      o->selection_color(51);
      o->image(image_bcy);
      o->labeltype(FL_NORMAL_LABEL);
      o->labelfont(0);
      o->labelsize(14);
      o->labelcolor(56);
      o->callback((Fl_Callback*)cb_btnBcY);
      o->align(FL_ALIGN_CENTER);
      o->when(FL_WHEN_RELEASE);
    }
    { Fl_HoverButton* o = btnBcXY = new Fl_HoverButton(89, 3, 42, 42);
      o->box(FL_PLASTIC_UP_BOX);
      o->down_box(FL_PLASTIC_DOWN_BOX);
      o->color(51);
      o->selection_color(51);
      o->image(image_bcxy);
      o->labeltype(FL_NORMAL_LABEL);
      o->labelfont(0);
      o->labelsize(14);
      o->labelcolor(56);
      o->callback((Fl_Callback*)cb_btnBcXY);
      o->align(FL_ALIGN_CENTER);
      o->when(FL_WHEN_RELEASE);
    }
    o->set_non_modal();
    o->end();
  }
  m_view = NULL;
  m_view = NULL;
}

CBcTypes::~CBcTypes() {
  delete wnd;
}

void CBcTypes::show() {
  setSize(wnd->w(),btnOk->h()+4);
wnd->clear_border();
wnd->show();
}

void CBcTypes::setPosition(int x, int y) {
  wnd->position(x, y);
}

void CBcTypes::setSize(int width, int height) {
  //wnd->size_range(0, 0);
wnd->size(width, height);
}

void CBcTypes::setView(void* p) {
  m_view = p;
}

void CBcTypes::hide() {
  wnd->hide();
}