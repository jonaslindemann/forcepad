INCLUDES = -I$(top_srcdir)/src/ivf2d -I$(top_srcdir)/src/common
AM_CXXFLAGS = -DFORCEPAD_RIGID

bin_PROGRAMS = forcepad_r

forcepad_r_LDADD = -L/usr/X11R6/lib $(top_builddir)/src/common/libcommon.a $(top_builddir)/src/ivf2d/libivf2d.a -lnewmat -lcalfemcxx -lfltk_gl -lfltk -ljpeg -lpng -lz -lGLU -lGL -lX11 -lXext  -lm

forcepad_r_SOURCES = BcTypes.cpp \
	BrushProps.cpp \
	CalcProps.cpp \
	DrawingProps.cpp \
	DrawingTools.cpp \
	FemGridSolverCORBA.cpp \
	FemGridSolver.cpp \
	ImportProps.cpp \
	main.cpp \
	MainFrame.cpp \
	PaintView.cpp \
	SplashFrame.cpp \
	StiffnessProps.cpp \
	BcTypes.h \
	BrushProps.h \
	CalcProps.h \
	DrawingProps.h \
	DrawingTools.h \
	FemGridSolverCORBA.h \
	FemGridSolver.h \
	forcepad_config.h \
	ImportProps.h \
	MainFrame.h \
	PaintView.h \
	SplashFrame.h \
	StiffnessProps.h



EXTRA_DIST = brushes samples *.fl 
