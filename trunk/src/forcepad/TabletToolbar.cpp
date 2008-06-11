// generated by Fast Light User Interface Designer (fluid) version 1.0109

#include "TabletToolbar.h"
#include "PaintView.h"

void CTabletToolbar::cb_btnOpen_i(Fl_Button*, void*) {
  if (m_paintView!=NULL)
{
	CPaintView* view = (CPaintView*)m_paintView;
	view->openModel();
};
}
void CTabletToolbar::cb_btnOpen(Fl_Button* o, void* v) {
  ((CTabletToolbar*)(o->parent()->user_data()))->cb_btnOpen_i(o,v);
}

#include <FL/Fl_Pixmap.H>
static const char *idata_open[] = {
"    36    36      128            2",
"`` c #818175",
"`. c #818179",
"`# c #8d8975",
"`a c #918d79",
"`b c #919171",
"`c c #959579",
"`d c #a5aa48",
"`e c #aaaa48",
"`f c #a5a555",
"`g c #a5a559",
"`h c #aaaa50",
"`i c #aeae59",
"`j c #aeb250",
"`k c #b2b250",
"`l c #b2b259",
"`m c #b6ba5d",
"`n c #baba5d",
"`o c #aaaa61",
"`p c #aaaa69",
"`q c #aeae7d",
"`r c #b2b261",
"`s c #b6ba65",
"`t c #baba61",
"`u c #bebe69",
"`v c #b2b275",
"`w c #b2b279",
"`x c #baba71",
"`y c #bebe7d",
"`z c #bec265",
"`A c #bec271",
"`B c #c2c265",
"`C c #c2c269",
"`D c #c2c271",
"`E c #c6c679",
"`F c #caca71",
"`G c #caca79",
"`H c #ced279",
"`I c #d2d279",
"`J c #e6ea7d",
"`K c #858181",
"`L c #898581",
"`M c #898981",
"`N c #8d8989",
"`O c #918d89",
"`P c #959585",
"`Q c #91918d",
"`R c #99958d",
"`S c #9d998d",
"`T c #959191",
"`U c #999591",
"`V c #999995",
"`W c #9d9d99",
"`X c #a19d99",
"`Y c #aaaa8d",
"`Z c #a1a19d",
"`0 c #aaa59d",
"`1 c #aeaa95",
"`2 c #b2b281",
"`3 c #bab68d",
"`4 c #baba81",
"`5 c #baba89",
"`6 c #b6b699",
"`7 c #bab691",
"`8 c #baba91",
".` c #a5a5a1",
".. c #aaa5a1",
".# c #aaaaa1",
".a c #aeaeaa",
".b c #b2aeaa",
".c c #b6b2a5",
".d c #b2b2ae",
".e c #babaa1",
".f c #bebaaa",
".g c #b6b6b2",
".h c #bab6b2",
".i c #babab2",
".j c #bebeba",
".k c #c2beb2",
".l c #c2beba",
".m c #c6c681",
".n c #caca81",
".o c #caca89",
".p c #d2d281",
".q c #d2d289",
".r c #dade85",
".s c #d6d695",
".t c #d2d299",
".u c #dada99",
".v c #c6c2aa",
".w c #cecea1",
".x c #c2c2b2",
".y c #c2c2ba",
".z c #cac6be",
".A c #d2d6a1",
".B c #dadaaa",
".C c #dedeb2",
".D c #dee281",
".E c #e6e681",
".F c #e2e289",
".G c #e6ea81",
".H c #eaea81",
".I c #eaea89",
".J c #e6e691",
".K c #eaea91",
".L c #eaea99",
".M c #eef28d",
".N c #f2f291",
".O c #f2f299",
".P c #e2e2ba",
".Q c #f6f6a1",
".R c #f6faaa",
".S c #cac6c2",
".T c #cacac2",
".U c #d2ceca",
".V c None",
".W c #e6e6ca",
".X c #eeeeda",
".Y c #eef2de",
".Z c #f2f2de",
".0 c #f2f2e2",
".1 c #f6f6ee",
".2 c #fafaf2",
".3 c #fffffa",
".4 c #898979",
".5 c #8d8979",
".6 c #8d8d79",
".7 c #817d75",
".8 c #000000",
".V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V",
".V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.U.T.T.x`2`o`6.i.j.y.T.U.V.V.V.V.V.V",
".V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.U.T.T.x`2`h`e`e`o`0.b.i.y.T.U.V.V.V.V.V",
".V.V.V.V.V.V.V.V.V.U.U.U.V.U.U.T.S.k`2`h`e`e`e`r.t.e`W.#.h.y.T.U.V.V.V.V",
".V.V.V.V.V.V.U.T.T.T.T.T.T.T.S.f`w`h`h`e`e`r.A.1.3.1`S`V.#.i.y.T.T.U.V.V",
".V.V.V.V.V.T.T.e`w.f.i.j.l.e`v`h`h`e`e`r.A.1.3.3.3.3`8`M`W.#.i.y.S.T.U.V",
".V.V.V.U.T.x`v`e`d`o.#`1`p`h`h`e`e`r.A.1.3.3.3.3.3.3.W`b`Y.u.u.d.i.y.T.U",
".V.V.U.z`8`h`e`d`e`e`g`f`h`e`e`l.A.1.3.3.3.3.3.3.3.3.P`D.L.O.K`0.b.i.S.T",
".V.U.v`p`e`d`d`e`e`e`d`e`e`k.o.0.3.3.3.3.3.3.3.3.X.o`H.K.O.O.J`R`Z.d.y.T",
".V`8`h`e`d`e`e`e`e`e`e`k.o.Z.2.3.3.3.3.3.3.3.X.o`I.K.O.O.O.N.J`M`V.#.j.T",
".U`5`e`e`e`e`e`e`e`k.o.Z.2.3.3.3.3.3.3.3.Z.o`I.K.O.O.O.O.N.N.F`.`Q...i.S",
".U.x`h`e`e`e`e`k.o.Y.2.3.3.3.3.3.3.3.0.t.p.L.O.O.O.O.O.N.N.N.F`.`O...i.S",
".U.S`p`e`e`e.o.Y.2.3.3.3.3.3.3.3.0.t.p.L.Q.O.O.O.O.O.N.N.N.N.F.7`N.`.i.S",
".U.T`7`h`e`s.2.3.3.3.3.3.3.3.1.A.p.L.Q.Q.O.O.O.O.O.N.N.N.N.N.F.7`N.`.i.S",
".V.T.k`i`e`l.0.3.3.3.3.3.1.B.q.L.Q.Q.Q.O.O.O.O.O.N.N.N.N.N.M.F```N`Z.i.S",
".V.U.S`w`j`h.P.3.3.3.1.B.q.L.Q.Q.Q.Q.O.O.O.O.O.N.N.N.N.N.M.M.I.6`N`Z.h.S",
".V.U.T.e`k`k.m.3.3.3.s.L.R.Q.Q.Q.Q.Q.O.O.O.O.N.N.N.N.N.M.M.I.I`#`M`Z.h.S",
".V.V.T.y`r`k`t.2.3.3.u.R.Q.Q.Q.Q.Q.O.O.O.O.O.N.N.N.N.M.M.I.I.H`#`M`Z.h.S",
".V.V.U.S`3`l`k.W.3.3.q.Q.Q.Q.Q.Q.O.O.O.O.O.N.N.N.N.M.M.I.I.I.H`#`M`Z.h.S",
".V.V.V.T.f`l`k.t.3.3.n.Q.Q.Q.Q.O.O.O.O.O.N.N.N.N.M.M.I.I.I.H.G`#`M`Z.h.S",
".V.V.V.T.y`v`l`A.2.2`F.Q.Q.Q.O.O.O.O.O.N.N.N.N.M.M.I.I.I.H.H.G`#`M`Z.h.S",
".V.V.V.U.S`8`l`m.X.2`G.Q.Q.O.O.O.O.O.N.N.N.N.N.M.I.I.I.H.H.H.E`#`N`Z.h.S",
".V.V.V.V.T.x`r`m.C.2`D.O.O.O.O.O.O.N.N.N.N.N.M.I.I.I.I.H.H.H`J`a`O...i.S",
".V.V.V.V.U.S`4`t`D.t`C.O.O.O.O.O.N.N.N.N.N.M.M.I.I.I.H.H.G`J`E`M`V.#.j.T",
".V.V.V.V.U.T.e`t`n`t`B.O.O.O.O.N.N.N.N.N.M.M.I.I.I.H.H.E`G`c`L`Q`Z.d.y.T",
".V.V.V.V.V.T.y`x`t`t`C.O.O.O.N.N.N.N.N.M.M.I.I.I.H.G`G`c`.`M`U`Z.d.l.S.U",
".V.V.V.V.V.U.S`8`t`t`C.K.O.N.N.N.N.N.M.M.I.I.H.E`G`c`.`M`T`X.#.d.l.S.T.V",
".V.V.V.V.V.V.T.k`u`t`B.K.O.N.N.N.N.M.M.I.I.E`y.6`.`M`T`X.#.d.i.y.S.T.V.V",
".V.V.V.V.V.V.T.y`4`z`C.I.N.N.N.N.M.I.I.F`y.6`.`N`T`X.#.d.i.y.S.T.U.V.V.V",
".V.V.V.V.V.V.U.T.e`B`C.I.N.N.N.I.I.F`y.6`.`N`U`Z.#.d.i.y.S.T.U.V.V.V.V.V",
".V.V.V.V.V.V.V.T.x`D`C.F.N.K.I.F`q.5`K`N`U`Z.#.d.i.y.S.T.U.V.V.V.V.V.V.V",
".V.V.V.V.V.V.V.U.S.n`C.D.K.F`2.4`K`N`V`Z.#.d.i.y.S.T.U.V.V.V.V.V.V.V.V.V",
".V.V.V.V.V.V.V.U.T.w`F.r`5`P`M`O`V`Z.a.g.j.y.S.T.U.V.V.V.V.V.V.V.V.V.V.V",
".V.V.V.V.V.V.V.V.T.z.v.c`Z`X`X.`.a.h.j.y.T.T.U.V.V.V.V.V.V.V.V.V.V.V.V.V",
".V.V.V.V.V.V.V.V.U.T.y.i.d.d.d.h.l.y.T.T.U.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V",
".V.V.V.V.V.V.V.V.V.U.T.S.y.y.y.S.T.T.U.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V.V"
};
static Fl_Pixmap image_open(idata_open);

void CTabletToolbar::cb_btnNew_i(Fl_Button*, void*) {
  if (m_paintView!=NULL)
{
	CPaintView* view = (CPaintView*)m_paintView;
	view->newModel();
};
}
void CTabletToolbar::cb_btnNew(Fl_Button* o, void* v) {
  ((CTabletToolbar*)(o->parent()->user_data()))->cb_btnNew_i(o,v);
}

static const char *idata_new[] = {
"    36    36       28            1",
"` c #7d7d7d",
". c #818181",
"# c #858585",
"a c #898989",
"b c #8d8d8d",
"c c #919191",
"d c #959595",
"e c #999999",
"f c #9d9d9d",
"g c #a1a1a1",
"h c #a5a5a5",
"i c #aaaaaa",
"j c #aeaeae",
"k c #b2b2b2",
"l c #b6b6b6",
"m c #bababa",
"n c #bebebe",
"o c #c2c2c2",
"p c #c6c6c6",
"q c #cacaca",
"r c #cecece",
"s c None",
"t c #dedede",
"u c #e2e2e2",
"v c #e6e6e6",
"w c #eaeaea",
"x c #f6f6f6",
"y c #fafafa",
"ssssssssssssssssssssssssssssssssssss",
"ssssssssssssssssssssssssssssssssssss",
"sssssssrrrqqqqqqqqqqqqqqrrrsssssssss",
"ssssstvvvuuuuuuuuuuuuuupoopqrsssssss",
"sssssvyyyyyyyyyyyyyyyyyvplmoqrssssss",
"ssssrvyyyyyyyyyyyyyyyyyvvpilnqrsssss",
"ssssrvyyyyyyyyyyyyyyyyyvvvpgknprssss",
"ssssruyyyyyyyyyyyyyyyyyvvvvpgknqrsss",
"ssssquyyyyyyyyyyyyyyyyyvvvvvpgloqsss",
"ssssquyyyyyyyyyyyyyyyyyywwwwwdimprss",
"ssssquyyyyyyyyyyyyyyyyyyyyyypafkorss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp.djnqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp`dinqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp`dinqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp`dinqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp`djnqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp`djnqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp`djnqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp`djnqss",
"ssssquyyyyxxyyyyyyyyyyyyyyyyp`djnqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp`djnqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp`djnqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp`djnqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp`djnqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp`djnqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp`djnqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp`djnqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp`djnqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp.djnqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp.djnqss",
"ssssquyyyyyyyyyyyyyyyyyyyyyyp#ejoqss",
"ssssrrroljihhhhhhhhhhhhhhhhifbgkorss",
"ssssrpmifca################acfimprss",
"ssssrqoligfffeeeeeeeeeeeefffgiloqrss",
"sssssrqomllkkkkkkkkkkkkkkkkllmoqrsss",
"ssssssrqppooooooooooooooooooppqrssss"
};
static Fl_Pixmap image_new(idata_new);

void CTabletToolbar::cb_btnSaveAs_i(Fl_Button*, void*) {
  if (m_paintView!=NULL)
{
	CPaintView* view = (CPaintView*)m_paintView;
	view->saveModelAs();
};
}
void CTabletToolbar::cb_btnSaveAs(Fl_Button* o, void* v) {
  ((CTabletToolbar*)(o->parent()->user_data()))->cb_btnSaveAs_i(o,v);
}

static const char *idata_save[] = {
"    36    36      109            2",
"`` c #000000",
"`. c #040404",
"`# c #080808",
"`a c #0c0c0c",
"`b c #141414",
"`c c #181818",
"`d c #1c1c1c",
"`e c #201c1c",
"`f c #202020",
"`g c #282824",
"`h c #282828",
"`i c #2c2c2c",
"`j c #343434",
"`k c #383838",
"`l c #3c3c3c",
"`m c #403c3c",
"`n c #404040",
"`o c #444040",
"`p c #444440",
"`q c #444444",
"`r c #484844",
"`s c #4c4c4c",
"`t c #50504c",
"`u c #505050",
"`v c #555550",
"`w c #595555",
"`x c #595959",
"`y c #61615d",
"`z c #616161",
"`A c #656561",
"`B c #696565",
"`C c #696965",
"`D c #696969",
"`E c #6d6d6d",
"`F c #717171",
"`G c #757571",
"`H c #797975",
"`I c #7d7975",
"`J c #81817d",
"`K c #858181",
"`L c #858581",
"`M c #858585",
"`N c #898585",
"`O c #898985",
"`P c #8d8985",
"`Q c #8d8989",
"`R c #8d8d89",
"`S c #8d8d8d",
"`T c #918d89",
"`U c #919191",
"`V c #959191",
"`W c #959591",
"`X c #959595",
"`Y c #999591",
"`Z c #999995",
"`0 c #9d9995",
"`1 c #999999",
"`2 c #9d9d99",
"`3 c #9d9d9d",
"`4 c #a19d99",
"`5 c #a1a19d",
"`6 c #a5a19d",
"`7 c #a5a5a1",
"`8 c #a5a5a5",
".` c #aaa5a1",
".. c #aeaaa5",
".# c #aaaaaa",
".a c #aeaeae",
".b c #b2aeaa",
".c c #b2b2ae",
".d c #b6b2ae",
".e c #b6b6ae",
".f c #b2b2b2",
".g c #b6b6b2",
".h c #b6b6b6",
".i c #bab6b2",
".j c #babab2",
".k c #babab6",
".l c #bebab6",
".m c #bebeb6",
".n c #bababa",
".o c #bebeba",
".p c #bebebe",
".q c #c2beba",
".r c #c2c2ba",
".s c #c2c2be",
".t c #c6c2be",
".u c #c6c6be",
".v c #c2c2c2",
".w c #c6c6c2",
".x c #cac6c2",
".y c #cacac2",
".z c #cecac6",
".A c #cecec6",
".B c #d2ceca",
".C c #d2d2ca",
".D c None",
".E c #ceceff",
".F c #d2d2ff",
".G c #dadaff",
".H c #e6e6ff",
".I c #eaeaff",
".J c #eeeeff",
".K c #f6f6f6",
".L c #f2f2ff",
".M c #f6f6ff",
".N c #fafafa",
".O c #fafaff",
".P c #ffffff",
".D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D",
".D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D.D",
".D.D.D.D.D.D.D.C.C.C.C.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.B.C.C.C.C.D.D.D.D",
".D.D.D.D.D.C.C.B.z.z.z.y.y.y.y.y.x.x.x.x.x.x.x.x.y.y.y.y.z.z.z.A.B.C.D.D",
".D.D.D.D.B`I`m`l`E.p.v.v.v.v.v.v.v.v.v.v.v.v.v.v.v.v`8`n`k`w.l.x.z.B.C.D",
".D.D.D.D.w`f`q`h.a.N.N.N.N.N.N.N.N.N.N.N.N.N.N.N.N.N.K`i```.`V.l.t.z.B.C",
".D.D.D.C.t`u.p`3.f.N.L.G.G.G.G.G.G.G.G.G.G.G.G.G.G.O.K`i```.`K...k.t.z.C",
".D.D.D.C.r`g`s`j.f.N.M.H.H.H.H.H.H.H.H.H.H.H.H.H.I.O.K`i```.`G`4.b.q.y.B",
".D.D.D.B.q`b````.f.N.P.P.P.P.P.P.P.P.P.P.P.P.P.P.P.P.K`i```.`C`V.`.l.x.B",
".D.D.D.B.q`b````.f.N.O.M.M.M.M.M.M.M.M.M.M.M.M.M.M.P.K`i```.`A`R`6.i.x.B",
".D.D.D.B.q`b````.f.N.J.E.E.E.E.E.E.E.E.E.E.E.E.E.F.M.K`i```.`y`P`6.i.x.B",
".D.D.D.B.q`b````.f.N.P.P.P.P.P.P.P.P.P.P.P.P.P.P.P.P.K`i```.`y`O`5.i.u.B",
".D.D.D.B.o`b````.f.N.P.P.P.P.P.P.P.P.P.P.P.P.P.P.P.P.K`i```.`y`O`5.i.u.B",
".D.D.D.B.o`b````.f.N.L.G.G.G.G.G.G.G.G.G.G.G.G.G.G.O.K`i```.`y`O`5.i.u.B",
".D.D.D.B.o`b````.f.N.M.H.H.H.H.H.H.H.H.H.H.H.H.H.I.O.K`i```.`y`O`5.i.u.B",
".D.D.D.B.o`b````.f.N.P.P.P.P.P.P.P.P.P.P.P.P.P.P.P.P.K`i```.`y`O`5.i.u.B",
".D.D.D.B.o`b````.a.N.K.K.K.K.K.K.K.K.K.K.K.K.K.K.K.N.K`i```.`y`O`5.i.u.B",
".D.D.D.B.o`b`````x.a.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f.f`1`b```.`y`O`5.i.u.B",
".D.D.D.B.o`b```````````````````````````````````````````````.`y`O`5.i.u.B",
".D.D.D.B.o`b```````````#`f`f`f`f`f`f`f`f`f`f`f`f`c`````````.`y`O`5.i.u.B",
".D.D.D.B.o`b```````````D.n.h`1`X`1.h.n.n.n.n.n.n.a`c```````.`y`O`5.i.u.B",
".D.D.D.B.o`b```````````M.p`M`#`#`#`M.v.v.v.v.v.v.n`f```````.`y`O`5.i.u.B",
".D.D.D.B.q`b```````````M.p`F`.```.`F.v.v.v.v.v.v.n`f```````.`y`O`5.i.u.B",
".D.D.D.B.q`b```````````M.p`F`.```.`F.v.v.v.v.v.v.n`f```````.`y`O`5.i.u.B",
".D.D.D.B.q`b```````````F.n`F`.```.`F.v.v.v.v.v.v.n`f```````.`y`O`5.i.u.B",
".D.D.D.B.q`e```````````s.n`F`.```.`F.v.v.v.v.v.v.n`f```````.`y`O`5.i.u.B",
".D.D.D.B.z`P`a`````````s.n`F`.`.`.`F.v.v.v.v.v.v.n`f```````.`y`O`5.i.u.B",
".D.D.D.C.z.r`J`a```````s.n`1`n`k`n`1.v.v.v.v.v.v.n`f```````.`z`P`6.i.x.B",
".D.D.D.C.A.u.i`H`a`.`#`j`S`U`X`X`X`3.#.#`8`8`8`8`3`d`.`.`.`.`B`T`7.i.x.B",
".D.D.D.D.B.z.q.d`J`v`s`r`q`o`o`o`o`p`r`r`r`r`r`r`r`n`o`o`p`t`J`Y...l.x.B",
".D.D.D.D.C.A.x.o.c`6`Y`T`O`L`L`L`L`K`K`K`K`K`K`K`K`K`L`L`N`Q`W`6.d.q.z.B",
".D.D.D.D.D.C.A.x.q.g..`6`4`2`0`0`0`0`0`0`Z`Z`Z`Z`Z`0`0`0`2`5.`.b.l.x.A.C",
".D.D.D.D.D.D.C.A.x.t.m.i.e.d.d.c.b.b.b.b.b.b.b.b.b.b.b.c.d.g.j.q.u.z.B.C",
".D.D.D.D.D.D.D.C.B.z.x.x.t.t.t.t.s.s.s.s.s.s.s.s.s.s.s.s.t.t.x.y.z.B.C.D",
".D.D.D.D.D.D.D.D.C.C.B.B.A.z.z.z.z.z.z.z.z.z.z.z.z.z.z.z.z.A.B.B.C.C.D.D",
".D.D.D.D.D.D.D.D.D.D.D.D.C.C.C.C.C.C.C.C.C.C.C.C.C.C.C.C.C.C.C.D.D.D.D.D"
};
static Fl_Pixmap image_save(idata_save);

void CTabletToolbar::cb_btnUndo_i(Fl_Button*, void*) {
  if (m_paintView!=NULL)
{
	CPaintView* view = (CPaintView*)m_paintView;
	view->undo();
};
}
void CTabletToolbar::cb_btnUndo(Fl_Button* o, void* v) {
  ((CTabletToolbar*)(o->parent()->user_data()))->cb_btnUndo_i(o,v);
}

static const char *idata_undo[] = {
"    36    36      256            2",
"`` c None",
"`. c #ebe8d7",
"`# c #eae7d6",
"`a c #e9e6d5",
"`b c #e7e4d4",
"`c c #e6e4d3",
"`d c #b5b2c3",
"`e c #e7e4d5",
"`f c #a4a2bc",
"`g c #6564a4",
"`h c #2b2a8f",
"`i c #000080",
"`j c #070782",
"`k c #29288d",
"`l c #52519b",
"`m c #afaeba",
"`n c #dfdccc",
"`o c #e3e0d0",
"`p c #0b0b84",
"`q c #9f9db9",
"`r c #e6e3d2",
"`s c #20208b",
"`t c #2c2c8d",
"`u c #908fab",
"`v c #d9d7c7",
"`w c #e0ddcd",
"`x c #0a0a83",
"`y c #a4a2b4",
"`z c #d3d1c3",
"`A c #5c5b9e",
"`B c #3d3c90",
"`C c #bfbdb8",
"`D c #e2dfcf",
"`E c #cfcdcc",
"`F c #090982",
"`G c #33328d",
"`H c #0c0c81",
"`I c #2d2c81",
"`J c #34337f",
"`K c #33327e",
"`L c #30307e",
"`M c #17177f",
"`N c #1c1c86",
"`O c #b8b6b0",
"`P c #d3d0c1",
"`Q c #cbc8ca",
"`R c #2f2e83",
"`S c #706e80",
"`T c #86847b",
"`U c #85847a",
"`V c #89877d",
"`W c #8a887e",
"`X c #69687e",
"`Y c #2a2a7f",
"`Z c #282786",
"`0 c #b8b5ac",
"`1 c #d0cebf",
"`2 c #adabbe",
"`3 c #020280",
"`4 c #53527e",
"`5 c #838278",
"`6 c #929086",
"`7 c #9a988d",
"`8 c #a09e93",
".` c #a4a296",
".. c #a19f93",
".# c #9b9a8e",
".a c #6a6985",
".b c #0b0b80",
".c c #55548c",
".d c #bcb9ac",
".e c #d2cfc0",
".f c #e8e5d5",
".g c #a9a7bc",
".h c #06067f",
".i c #5c5b72",
".j c #7f7d74",
".k c #8b897f",
".l c #9c9a8f",
".m c #adab9e",
".n c #b9b7a9",
".o c #c1beb0",
".p c #c3c1b3",
".q c #c4c2b4",
".r c #bab8aa",
".s c #b0aea1",
".t c #898891",
".u c #0c0b81",
".v c #050480",
".w c #939095",
".x c #c0bdaf",
".y c #d6d3c4",
".z c #e4e1d0",
".A c #8f8db2",
".B c #05057f",
".C c #6a6879",
".D c #97958a",
".E c #c5c3b4",
".F c #dbd9c9",
".G c #dcd9ca",
".H c #dad8c8",
".I c #ccc9ba",
".J c #bfbcae",
".K c #696892",
".L c #2a2a82",
".M c #c8c5b7",
".N c #8786b0",
".O c #0c0c7f",
".P c #85838e",
".Q c #d1cfbf",
".R c #e5e2d1",
".S c #272687",
".T c #82808c",
".U c #b7b5a8",
".V c #8e8db2",
".W c #343490",
".X c #44438e",
".Y c #383783",
".Z c #302f7c",
".0 c #393876",
".1 c #3b3a75",
".2 c #3d3d78",
".3 c #51507e",
".4 c #72718c",
".5 c #e1dece",
".6 c #d8d5c5",
".7 c #9795a8",
".8 c #5e5e82",
"#` c #a8a69a",
"#. c #d4d1c2",
"## c #bbb9ab",
"#a c #a2a094",
"#b c #8f8e83",
"#c c #88867d",
"#d c #8d8b81",
"#e c #959388",
"#f c #a6a498",
"#g c #cecbc0",
"#h c #45447e",
"#i c #c9c6b8",
"#j c #acaa9e",
"#k c #a8a599",
"#l c #a9a79b",
"#m c #afada0",
"#n c #b8b6a9",
"#o c #c6c4b5",
"#p c #cac7c4",
"#q c #4c4b7d",
"#r c #969489",
"#s c #b4b1a4",
"#t c #d9d6c6",
"#u c #cfcdbe",
"#v c #c7c4b6",
"#w c #d7d4c5",
"#x c #9c9ab7",
"#y c #56557d",
"#z c #ceccbd",
"#A c #dedbcb",
"#B c #5d5ba1",
"#C c #76747c",
"#D c #1c1c8a",
"#E c #191981",
"#F c #8281af",
"#G c #454480",
"#H c #87857c",
"#I c #d5d2c3",
"#J c #6564a5",
"#K c #6b697e",
"#L c #c9c6c9",
"#M c #2b2b8f",
"#N c #4f4d81",
"#O c #918f85",
"#P c #cbc8ba",
"#Q c #e4e1d4",
"#R c #7472aa",
"#S c #070783",
"#T c #403f83",
"#U c #9d9b90",
"#V c #b0aec1",
"#W c #24248d",
"#X c #080781",
"#Y c #565582",
"#Z c #98968b",
"#0 c #dddaca",
"#1 c #c3c1c8",
"#2 c #1f1f83",
"#3 c #7a7881",
"#4 c #474699",
"#5 c #080881",
"#6 c #4f4e84",
"#7 c #88867e",
"#8 c #8e8c82",
"a` c #c0bec2",
"a. c #060682",
"a# c #3e3d8c",
"aa c #8b898c",
"ab c #939187",
"ac c #b5b3a6",
"ad c #b5b3b6",
"ae c #99978c",
"af c #9e9c91",
"ag c #b6b4a7",
"ah c #000000",
"ai c #000000",
"aj c #000000",
"ak c #000000",
"al c #000000",
"am c #000000",
"an c #000000",
"ao c #000000",
"ap c #000000",
"aq c #000000",
"ar c #000000",
"as c #000000",
"at c #000000",
"au c #000000",
"av c #000000",
"aw c #000000",
"ax c #000000",
"ay c #000000",
"az c #000000",
"aA c #000000",
"aB c #000000",
"aC c #000000",
"aD c #000000",
"aE c #000000",
"aF c #000000",
"aG c #000000",
"aH c #000000",
"aI c #000000",
"aJ c #000000",
"aK c #000000",
"aL c #000000",
"aM c #000000",
"aN c #000000",
"aO c #000000",
"aP c #000000",
"aQ c #000000",
"aR c #000000",
"aS c #000000",
"aT c #000000",
"aU c #000000",
"aV c #000000",
"aW c #000000",
"aX c #000000",
"aY c #000000",
"aZ c #000000",
"a0 c #000000",
"a1 c #000000",
"a2 c #000000",
"a3 c #000000",
"a4 c #000000",
"a5 c #000000",
"a6 c #000000",
"a7 c #000000",
"a8 c #000000",
"````````````````````````````````````````````````````````````````````````",
"````````````````````````````````````````````````````````````````````````",
"````````````````````````````````````````````````````````````````````````",
"````````````````````````````````````````````````````````````````````````",
"````````````````````````````````````````````````````````````````````````",
"`````````````````````````````````.`.`.`.`.`.`.`.````````````````````````",
"`````````````````.`````````.`.`#`a`b`b`c`c`b`b`a`#`.`.``````````````````",
"`````````````d`#`a`#`#`.`e`f`g`h`i`i`i`j`k`l`m`n`o`c`a`#`.``````````````",
"```````````.`p`q`o`o`r`q`s`i`i`i`i`i`i`i`i`i`i`t`u`v`w`r`a`.````````````",
"```````````#`i`x`y`z`A`i`i`i`i`i`i`i`i`i`i`i`i`i`i`B`C`v`D`b`#``````````",
"```````````E`i`i`F`G`i`i`i`i`i`H`I`J`K`L`M`i`i`i`i`i`N`O`P`n`c`#````````",
"`````````.`Q`i`i`i`i`i`i`i`R`S`T`U`T`V`W`W`X`Y`i`i`i`i`Z`0`1`n`b`.``````",
"`````````.`2`i`i`i`i`i`3`4`5`5`W`6`7`8.`.`...#.a.b`i`i`i.c.d.e`w.f`.````",
"`````````..g`i`i`i`i`i.h.i.j.k.l.m.n.o.p.q.o.r.s.t.u`i`i.v.w.x.y.z`#````",
"`````````#.A`i`i`i`i`i`i.B.C.D.s.E`P`v.F.G.H.y.I.J.K`i`i`i.L.m.M.F`c`.``",
"`````````#.N`i`i`i`i`i`i`i.O.P.n.Q`n`r`b`b`c.R`n.y.E.S`i`i`i.T.U.Q`D`a``",
"`````````#.V.W.X.Y.Z.0.1.2.3.4.n.e.5.f`.`.`.`#.f`o.6.7`i`i`i.8#`.E.F`b`.",
"`````````#.z#.###a#b`V#c#d#e#f##`1`w.f`.```````.`a`o#g`i`i`i#h.l##.y`r`.",
"`````````.`r.H#i.U#j#k#k#l#m#n#o.y`D`a`.`````````.`b#p`i`i`i#q#r#s`1`o`#",
"`````````..f`D#t#u#i#v#v#i.I#u#w`n`r`#`````````````##x`i`i`i#y`6.s#z`D`#",
"```````````..f.R.5#A#A#A`n`n.5.z`c`a`.`````````````##B`i`i`i#C`6.s#z`D`#",
"`````````````.`#`a.f.f.f`a`a`a`#`.`.```````````````a#D`i`i#E`T#e#s`1`o`#",
"`````````````````.`.`.`.`.```````````````````````.#F`i`i`i#G#H`7.n#I.R`.",
"`````````````````````````````````````````````.`e#J`i`i`i`i#K.k#a.o`v`c`.",
"```````````````````````````````````````````.#L#M`i`i`i`i#N`T#O#j#P`n`a`.",
"```````````````````````````````````````.#Q#R#S`i`i`i`i#T#H`W#U.r#..z`#``",
"``````````````````````````````````````#V#W`i`i`i`i#X#Y#H`V#Z.s#i#0`b`.``",
"````````````````````````````````````#1`i`i`i`i`i#2#3#H`W#Z.m.q.6.z`#````",
"`````````````````````````````````````a#4`i`i#5#6#7#H#8.l.s.q#w`o`a`.````",
"```````````````````````````````````.`aa`a.a#aa#d.kab.`ac#i.6`o`a`.``````",
"`````````````````````````````````````a`wadac..aeaf.m.J#u.F.z`a`.````````",
"`````````````````````````````````````#.z#w.Eag#s##.M.y`w`c`#`.``````````",
"`````````````````````````````````````.`b.5#w#u#z#.#0.z.f`.``````````````",
"```````````````````````````````````````#`b.z`w`w.z`b`#`.````````````````",
"`````````````````````````````````````````.`#`a`a`#`.````````````````````",
"`````````````````````````````````````````````.`.````````````````````````"
};
static Fl_Pixmap image_undo(idata_undo);

static const char *idata_calc[] = {
"    36    36      128            2",
"`` c #717d6d",
"`. c #797975",
"`# c #04ba04",
"`a c #1cae1c",
"`b c #10ba0c",
"`c c #3c9d38",
"`d c #20ae1c",
"`e c #20b21c",
"`f c #24aa24",
"`g c #24ba24",
"`h c #34a534",
"`i c #34b234",
"`j c #04c204",
"`k c #24c220",
"`l c #409124",
"`m c #40993c",
"`n c #40a53c",
"`o c #489944",
"`p c #509548",
"`q c #559550",
"`r c #44a540",
"`s c #50a54c",
"`t c #698165",
"`u c #6d9969",
"`v c #71816d",
"`w c #798575",
"`x c #799575",
"`y c #61b25d",
"`z c #69a165",
"`A c #71aa6d",
"`B c #79aa75",
"`C c #75ba71",
"`D c #6dc669",
"`E c #9d3c38",
"`F c #ae201c",
"`G c #aa2828",
"`H c #a13838",
"`I c #b63030",
"`J c #a1403c",
"`K c #895d59",
"`L c #994444",
"`M c #915555",
"`N c #89615d",
"`O c #856561",
"`P c #85716d",
"`Q c #817575",
"`R c #916d69",
"`S c #957975",
"`T c #ae4444",
"`U c #a15050",
"`V c #b65955",
"`W c #b6615d",
"`X c #a17979",
"`Y c #b66561",
"`Z c #ba7975",
"`0 c #c20404",
"`1 c #c21010",
"`2 c #c2201c",
"`3 c #c62828",
"`4 c #c23c38",
"`5 c #ca615d",
"`6 c #c67571",
"`7 c #81857d",
"`8 c #81c67d",
".` c #8d9985",
".. c #918d89",
".# c #91918d",
".a c #999591",
".b c #89a581",
".c c #8db285",
".d c #91a18d",
".e c #99aa91",
".f c #99b695",
".g c #aa8581",
".h c #a59995",
".i c #be918d",
".j c #b69d99",
".k c #a5a19d",
".l c #a5b69d",
".m c #aaaaa1",
".n c #aabea5",
".o c #b2aaa5",
".p c #b2b2ae",
".q c #babab2",
".r c #91c28d",
".s c #9dca95",
".t c #a5caa1",
".u c #bec2b6",
".v c #c28985",
".w c #c2918d",
".x c #ca9995",
".y c #cea19d",
".z c #caaeaa",
".A c #c2bab6",
".B c #d2aeaa",
".C c #d2b2ae",
".D c #d2bab6",
".E c #c2c6ba",
".F c #cacac2",
".G c #d2cac6",
".H c None",
".I c #b21010",
".J c #b61814",
".K c #b21818",
".L c #ba1010",
".M c #ba1818",
".N c #858181",
".O c #858981",
".P c #898581",
".Q c #898985",
".R c #8d8d89",
".S c #55b255",
".T c #55ba50",
".U c #59b255",
".V c #5db659",
".W c #59ba55",
".X c #10b610",
".Y c #10be10",
".Z c #18b218",
".0 c #18be18",
".1 c #b22020",
".2 c #b22c2c",
".3 c #be2020",
".4 c #ba2824",
".5 c #be0000",
".6 c #ba0808",
".7 c #618959",
".8 c #000000",
".H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H",
".H.H.H.H.H.H.H.H.H.B.B.B.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H",
".H.H.H.H.H.H.H.G.v`0`0`0.v.E.F.F.G.G.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H",
".H.H.H.G.x.y.F.F`6`0`0`0`Y.p.q.v.x.F.F.G.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H",
".H.H.G`6`0`1.i.A`V`0`0`0`V.k`W`0`1.v.E.F.F.G.H.H.H.H.H.H.H.H.H.H.H.H.H.H",
".H.H.x`0`0`0`1.4`0`0`0`0`0.1`0`0`0`1.j.q.E.F.H.H.H.H.H.H.H.H.H.H.H.H.H.H",
".H.H.y`1`0`0`0`0`0`0`0`0`0`0`0`0`0.M.h.o.u.F.G.H.H.H.H.H.H.H.H.H.H.H.H.H",
".H.H.F.i`1`0`0`0`0.L.I.6`0`0`0`0.J`S.a.k.q.E.F.H.H.H.H.H.H.H.H.H.H.H.H.H",
".H.H.F.q.3`0`0`0`L`P`Q`P`G`0`0.6`E`7...h.o.u.F.G.H.H.H.H.H.H.H.H.H.H.H.H",
".G`5`4`I`0`0`0`E`.`.`7.Q`7`G`0`0.6`G`G`T.k.q.F.G.H.H.H.H.H.H.H.H.H.H.H.H",
".B`0`0`0`0`0`0`N`.`7...a.a`X`0`0`0`0`0`0.g.p.u.F.G.H.H.H.H.H.H.H.H.H.H.H",
".B`0`0`0`0`0`0`N`7...a.k.m.g`0`0`0`0`0`0`X.m.q.F.G.H.H.H.H.H.H.H.H.H.H.H",
".D`3.M`1`0`0`0`K`7.#.k.p.q`Z`0`0`0`0`0.L`S.k.p.E.F.G.H.H.H.H.H.H.H.H.H.H",
".H.G.o`X`0`0`0.K`Q.a.m.q.i`1`0`0.K`K`N`P...h.o.u.F.s.t.E.F.G.G.H.H.H.H.H",
".H.H.p.k.1`0`0`0`F`U`W`W.M`0`0`0`E`.`.`w`s`z.m.q.r`j`j`k.r.E.F.G.H.H.H.H",
".H.H.q`W`1`0`0`0`0`0`0`0`0`0`0`0.6`M`p`b`#`b.e.p.W`j`j`j.W.q.u.F.F.H.H.H",
".H.H.i`0`0`0`0`0`0`0`0`0`0`0`0`0`0.6`l`#`j`#`i`y.Y`j`j`#`A.k.p.q.F.F.G.H",
".H.H.z`2`0`0.1`L.I`0`0`0.K`L.I`0`0`G.7`#`j`j`#`#`#`j`j`#`u.`.S`g.f.E.F.G",
".H.H.F.i`2.4`S`7`L`0`0`0`L`.`K.I`F`P`w`a`#`j`j`j`j`j`j`#`b.X`#`j`i.p.u.F",
".H.H.F.E.o.k.a..`U`0`0`0`L`.`7`p`a`h`o`#`j`j`#`b`b`#`#`#`#`#`j`j`#.c.p.u",
".H.H.G.F.u.p.m.k`X`I`I`G`R.Q.Q.Z`#`#`#`#`#`#`f`t``.7`h`#`j`j`#`#`d.`.m.q",
".H.H.H.F.F.u.q.p.o.m.k.a.#.a.b`b`j`j`j`j`#`c`.`.`7`7`w.Z`#`j`#`o`7...k.p",
".H.H.H.H.G.F.F.E.u.q.p.m.k.k.e.0`#`#`#`j`#`t`.`7...#.a`z`#`j`#`t`Q...k.p",
".H.H.H.H.H.G.G.F.F.F.u.q.q.q.q.l`B`i`#`#`b```7...a.k.m`B`#`j`#`a`o`x.h.p",
".H.H.H.H.H.H.H.H.G.G.F.F.F.E.E.u.p`A`#`j`#.7`7.#.k.p.q.T`#`j`j`#`#.Z.a.o",
".H.H.H.H.H.H.H.H.H.H.H.G.G.G.F.u`C`b`#`j`#`a`7.a.m.p.f.0`#`j`j`j`j`a.a.m",
".H.H.H.H.H.H.H.H.H.H.H.H.H.H.G`D`j`j`j`j`j`#`e`r`B`y`b`#`j`#`b`#`#`n.a.m",
".H.H.H.H.H.H.H.H.H.H.H.H.H.H.G`8`j`j`j`#`#`j`#`#`#`#`j`j`#`f```t`t`7.a.m",
".H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.E`g`#`b`n`a`#`j`j`j`j`j`j`#`c`.`.`7...h.o",
".H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.F.n.c.d..`p`#`j`#`#`#`#`j`j`#```7...h.m.q",
".H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.F.E.q.m.a`i`#`j`#`c```a`#`j`#`q...a.m.q.E",
".H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.G.F.u.q.m`g`#`#`#.7`..7`b`b`o`7.#.k.q.E.F",
".H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.G.F.E.q.f.U`i`n`7.P.Q.O.Q...#.k.o.u.F.G",
".H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.G.F.F.u.p.k.a.a.a.a.h.k.k.k.o.q.E.F.H",
".H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.G.F.u.p.o.m.m.o.p.p.p.q.q.E.F.G.H",
".H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.H.G.F.E.q.q.q.u.E.E.E.E.F.F.G.H.H"
};
static Fl_Pixmap image_calc(idata_calc);

CTabletToolbar::CTabletToolbar() {
  { mainWindow = new Fl_Double_Window(232, 49, "Tablet Toolbar");
    mainWindow->user_data((void*)(this));
    { btnOpen = new Fl_Button(50, 3, 42, 42);
      btnOpen->image(image_open);
      btnOpen->callback((Fl_Callback*)cb_btnOpen);
    } // Fl_Button* btnOpen
    { btnNew = new Fl_Button(5, 3, 42, 42);
      btnNew->image(image_new);
      btnNew->callback((Fl_Callback*)cb_btnNew);
    } // Fl_Button* btnNew
    { btnSaveAs = new Fl_Button(95, 3, 42, 42);
      btnSaveAs->image(image_save);
      btnSaveAs->callback((Fl_Callback*)cb_btnSaveAs);
    } // Fl_Button* btnSaveAs
    { btnUndo = new Fl_Button(140, 3, 42, 42);
      btnUndo->image(image_undo);
      btnUndo->callback((Fl_Callback*)cb_btnUndo);
    } // Fl_Button* btnUndo
    { btnSettings = new Fl_Button(185, 3, 42, 42);
      btnSettings->image(image_calc);
    } // Fl_Button* btnSettings
    mainWindow->set_non_modal();
    mainWindow->end();
  } // Fl_Double_Window* mainWindow
  m_paintView = NULL;
}

void CTabletToolbar::show() {
  mainWindow->show();
}

void CTabletToolbar::setView(void* view) {
  m_paintView = view;
}

void CTabletToolbar::centerWindow(Fl_Window* window) {
  mainWindow->position(window->x()+window->w()/2-mainWindow->w()/2, window->y()+window->h()/2-mainWindow->h()/2);
}
