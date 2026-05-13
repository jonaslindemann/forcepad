#include "MainWindow.h"

#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QAction>
#include <QKeySequence>
#include <QSurfaceFormat>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_paintView = new QtPaintView(this);
    setCentralWidget(m_paintView);

    m_paintView->setModeChangeEvent(this);
    m_paintView->setViewModeChangeEvent(this);
    m_paintView->setModelLoadedEvent(this);
    m_paintView->setNewModelEvent(this);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    setWindowTitle("ForcePAD Qt");
    resize(1024, 768);
}

// ---------------------------------------------------------------------------
// CPV event callbacks
// ---------------------------------------------------------------------------

void MainWindow::onModeChange(CPaintView::TEditMode /*oldMode*/, CPaintView::TEditMode newMode)
{
    const char *names[] = {
        "Brush", "Direct Brush", "Erase", "Direct Erase",
        "Force", "Constraint", "Constraint Vector", "Constraint Hinge",
        "Dynamic Force", "Rotate Constraint", "Line", "Rectangle",
        "Ellipse", "Flood Fill", "Erase Forces/BC", "Select",
        "Paste", "Result", "Ruler", "Arch"
    };
    int idx = static_cast<int>(newMode);
    if (idx >= 0 && idx < static_cast<int>(sizeof(names) / sizeof(names[0])))
        m_statusMode->setText(QString("Mode: %1").arg(names[idx]));
}

void MainWindow::onViewModeChange(CPaintView::TViewMode /*oldMode*/, CPaintView::TViewMode newMode)
{
    switch (newMode)
    {
        case CPaintView::VM_SKETCH:  m_statusModel->setText("View: Sketch");  break;
        case CPaintView::VM_PHYSICS: m_statusModel->setText("View: Physics"); break;
        case CPaintView::VM_ACTION:  m_statusModel->setText("View: Action");  break;
    }
}

void MainWindow::onModelLoaded()
{
    updateWindowTitle();
}

void MainWindow::onNewModel()
{
    updateWindowTitle();
}

void MainWindow::updateWindowTitle()
{
    setWindowTitle("ForcePAD Qt");
}

// ---------------------------------------------------------------------------
// Menu/toolbar slots
// ---------------------------------------------------------------------------

void MainWindow::fileNew()    { m_paintView->newModel(); }
void MainWindow::fileOpen()   { m_paintView->openModel(); }
void MainWindow::fileSave()   { m_paintView->saveModel(); }
void MainWindow::fileSaveAs() { m_paintView->saveModelAs(); }
void MainWindow::editUndo()   { m_paintView->undo(); }

void MainWindow::runCalculate()
{
    m_paintView->setViewMode(CPaintView::VM_PHYSICS);
    m_paintView->execute();
}

void MainWindow::runOptimise()
{
    m_paintView->setViewMode(CPaintView::VM_ACTION);
    m_paintView->executeOpt();
}

void MainWindow::setModeBrush()             { m_paintView->setEditMode(CPaintView::EM_BRUSH); }
void MainWindow::setModeErase()             { m_paintView->setEditMode(CPaintView::EM_ERASE); }
void MainWindow::setModeForce()             { m_paintView->setEditMode(CPaintView::EM_FORCE); }
void MainWindow::setModeConstraint()        { m_paintView->setEditMode(CPaintView::EM_CONSTRAINT); }
void MainWindow::setModeConstraintVector()  { m_paintView->setEditMode(CPaintView::EM_CONSTRAINT_VECTOR); }
void MainWindow::setModeConstraintHinge()   { m_paintView->setEditMode(CPaintView::EM_CONSTRAINT_HINGE); }
void MainWindow::setModeFloodFill()         { m_paintView->setEditMode(CPaintView::EM_FLOODFILL); }
void MainWindow::setModeRectangle()         { m_paintView->setEditMode(CPaintView::EM_RECTANGLE); }
void MainWindow::setModeEllipse()           { m_paintView->setEditMode(CPaintView::EM_ELLIPSE); }
void MainWindow::setModeLine()              { m_paintView->setEditMode(CPaintView::EM_LINE); }

void MainWindow::setViewSketch()  { m_paintView->setViewMode(CPaintView::VM_SKETCH); }
void MainWindow::setViewPhysics() { m_paintView->setViewMode(CPaintView::VM_PHYSICS); }
void MainWindow::setViewAction()  { m_paintView->setViewMode(CPaintView::VM_ACTION); }

// ---------------------------------------------------------------------------
// UI setup
// ---------------------------------------------------------------------------

void MainWindow::createActions()
{
    m_actNew     = new QAction("New",      this);  m_actNew->setShortcut(QKeySequence::New);
    m_actOpen    = new QAction("Open...",  this);  m_actOpen->setShortcut(QKeySequence::Open);
    m_actSave    = new QAction("Save",     this);  m_actSave->setShortcut(QKeySequence::Save);
    m_actSaveAs  = new QAction("Save As...", this);
    m_actQuit    = new QAction("Quit",     this);  m_actQuit->setShortcut(QKeySequence::Quit);

    m_actUndo      = new QAction("Undo",      this);  m_actUndo->setShortcut(QKeySequence::Undo);
    m_actCalculate = new QAction("Calculate", this);  m_actCalculate->setShortcut(Qt::Key_F5);
    m_actOptimise  = new QAction("Optimise",  this);  m_actOptimise->setShortcut(Qt::Key_F6);

    m_actBrush             = new QAction("Brush",              this);
    m_actErase             = new QAction("Erase",              this);
    m_actForce             = new QAction("Add Force",          this);
    m_actConstraint        = new QAction("Add Constraint XY",  this);
    m_actConstraintVector  = new QAction("Add Constraint Vec", this);
    m_actConstraintHinge   = new QAction("Add Hinge",          this);
    m_actFloodFill         = new QAction("Flood Fill",         this);
    m_actRectangle         = new QAction("Rectangle",          this);
    m_actEllipse           = new QAction("Ellipse",            this);
    m_actLine              = new QAction("Line",               this);

    m_actViewSketch  = new QAction("Sketch",  this);
    m_actViewPhysics = new QAction("Physics", this);
    m_actViewAction  = new QAction("Action",  this);

    connect(m_actNew,            &QAction::triggered, this, &MainWindow::fileNew);
    connect(m_actOpen,           &QAction::triggered, this, &MainWindow::fileOpen);
    connect(m_actSave,           &QAction::triggered, this, &MainWindow::fileSave);
    connect(m_actSaveAs,         &QAction::triggered, this, &MainWindow::fileSaveAs);
    connect(m_actQuit,           &QAction::triggered, this, &MainWindow::close);
    connect(m_actUndo,           &QAction::triggered, this, &MainWindow::editUndo);
    connect(m_actCalculate,      &QAction::triggered, this, &MainWindow::runCalculate);
    connect(m_actOptimise,       &QAction::triggered, this, &MainWindow::runOptimise);
    connect(m_actBrush,          &QAction::triggered, this, &MainWindow::setModeBrush);
    connect(m_actErase,          &QAction::triggered, this, &MainWindow::setModeErase);
    connect(m_actForce,          &QAction::triggered, this, &MainWindow::setModeForce);
    connect(m_actConstraint,     &QAction::triggered, this, &MainWindow::setModeConstraint);
    connect(m_actConstraintVector,&QAction::triggered,this, &MainWindow::setModeConstraintVector);
    connect(m_actConstraintHinge,&QAction::triggered, this, &MainWindow::setModeConstraintHinge);
    connect(m_actFloodFill,      &QAction::triggered, this, &MainWindow::setModeFloodFill);
    connect(m_actRectangle,      &QAction::triggered, this, &MainWindow::setModeRectangle);
    connect(m_actEllipse,        &QAction::triggered, this, &MainWindow::setModeEllipse);
    connect(m_actLine,           &QAction::triggered, this, &MainWindow::setModeLine);
    connect(m_actViewSketch,     &QAction::triggered, this, &MainWindow::setViewSketch);
    connect(m_actViewPhysics,    &QAction::triggered, this, &MainWindow::setViewPhysics);
    connect(m_actViewAction,     &QAction::triggered, this, &MainWindow::setViewAction);
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction(m_actNew);
    fileMenu->addAction(m_actOpen);
    fileMenu->addSeparator();
    fileMenu->addAction(m_actSave);
    fileMenu->addAction(m_actSaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(m_actQuit);

    QMenu *editMenu = menuBar()->addMenu("Edit");
    editMenu->addAction(m_actUndo);

    QMenu *runMenu = menuBar()->addMenu("Run");
    runMenu->addAction(m_actCalculate);
    runMenu->addAction(m_actOptimise);

    QMenu *viewMenu = menuBar()->addMenu("View");
    viewMenu->addAction(m_actViewSketch);
    viewMenu->addAction(m_actViewPhysics);
    viewMenu->addAction(m_actViewAction);
}

void MainWindow::createToolBars()
{
    QToolBar *fileTB = addToolBar("File");
    fileTB->addAction(m_actNew);
    fileTB->addAction(m_actOpen);
    fileTB->addAction(m_actSave);

    QToolBar *runTB = addToolBar("Run");
    runTB->addAction(m_actCalculate);
    runTB->addAction(m_actOptimise);

    QToolBar *modeTB = addToolBar("Draw Mode");
    modeTB->addAction(m_actBrush);
    modeTB->addAction(m_actErase);
    modeTB->addSeparator();
    modeTB->addAction(m_actForce);
    modeTB->addAction(m_actConstraint);
    modeTB->addAction(m_actConstraintVector);
    modeTB->addAction(m_actConstraintHinge);
    modeTB->addSeparator();
    modeTB->addAction(m_actFloodFill);
    modeTB->addAction(m_actRectangle);
    modeTB->addAction(m_actEllipse);
    modeTB->addAction(m_actLine);

    QToolBar *viewTB = addToolBar("View Mode");
    viewTB->addAction(m_actViewSketch);
    viewTB->addAction(m_actViewPhysics);
    viewTB->addAction(m_actViewAction);
}

void MainWindow::createStatusBar()
{
    m_statusMode  = new QLabel("Mode: Brush", this);
    m_statusModel = new QLabel("View: Sketch", this);
    statusBar()->addWidget(m_statusMode);
    statusBar()->addPermanentWidget(m_statusModel);
}
