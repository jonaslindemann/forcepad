#include "MainWindow.h"
#include "../common/FemGrid2.h"
#include "CalcSettingsDialog.h"
#include "GeneralSettingsDialog.h"
#include "AboutDialog.h"
#include "OptimisationSettingsDialog.h"

#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QAction>
#include <QKeySequence>
#include <QSurfaceFormat>
#include <QTabBar>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QFormLayout>
#include <QSizePolicy>
#include <QSplitter>
#include <QPainter>
#include <QLinearGradient>
#include <QIcon>
#include <QSize>
#include <QSvgRenderer>
#include <QProgressBar>
#include <QString>

// ---------------------------------------------------------------------------
// Small helper: stiffness gradient widget shown at the bottom of sketch panel
// ---------------------------------------------------------------------------
class GradientBar : public QWidget
{
public:
    explicit GradientBar(QWidget *parent = nullptr) : QWidget(parent)
    {
        setFixedWidth(32);
        setMinimumHeight(80);
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        QLinearGradient grad(0, 0, 0, height());
        grad.setColorAt(0.0, Qt::white);
        grad.setColorAt(1.0, QColor(0x1a, 0x1a, 0x1a));
        p.fillRect(rect(), grad);
        p.setPen(QColor(0x52, 0x52, 0x52));
        p.drawRect(rect().adjusted(0, 0, -1, -1));
    }
};

// ---------------------------------------------------------------------------
// Helper: horizontal separator line for dark toolbars
// ---------------------------------------------------------------------------
static QFrame* makeSeparator()
{
    auto *f = new QFrame;
    f->setFrameShape(QFrame::HLine);
    f->setFixedHeight(1);
    f->setStyleSheet("background: #3a3a3a; border: none;");
    return f;
}

// ---------------------------------------------------------------------------
// Custom button: owns its paintEvent so hover/checked states work on Windows
// regardless of the native style renderer.
// ---------------------------------------------------------------------------
class PanelButton : public QToolButton
{
public:
    explicit PanelButton(QWidget *parent = nullptr) : QToolButton(parent)
    {
        setAttribute(Qt::WA_Hover);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        const bool hov = underMouse();
        const bool on  = isChecked();

        QPainter p(this);

        if (on)
            p.fillRect(rect(), QColor("#2e2e2e"));
        else if (hov)
            p.fillRect(rect(), QColor("#2a2a2a"));

        if (on)
            p.fillRect(0, 0, 2, height(), QColor("#2563eb"));

        if (!icon().isNull()) {
            const QIcon::State st = on  ? QIcon::On     : QIcon::Off;
            const QIcon::Mode  mo = hov ? QIcon::Active : QIcon::Normal;
            const QRect r((width()  - iconSize().width())  / 2,
                          (height() - iconSize().height()) / 2,
                          iconSize().width(), iconSize().height());
            icon().paint(&p, r, Qt::AlignCenter, mo, st);
        }

        if (!text().isEmpty()) {
            p.setPen(on || hov ? QColor("#ffffff") : QColor("#a3a3a3"));
            p.setFont(font());
            p.drawText(rect(), Qt::AlignCenter, text());
        }
    }
};

// ---------------------------------------------------------------------------
// Brush size button: draws a filled circle scaled to the brush size
// ---------------------------------------------------------------------------
class BrushSizeButton : public QToolButton
{
    int m_dotR;
public:
    explicit BrushSizeButton(int dotRadius, QWidget *parent = nullptr)
        : QToolButton(parent), m_dotR(dotRadius)
    {
        setAttribute(Qt::WA_Hover);
        setCheckable(true);
        setFixedSize(44, 44);
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        const bool hov = underMouse();
        const bool on  = isChecked();
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(rect(), on ? QColor("#dbeafe") : (hov ? QColor("#f0f0f0") : QColor("#fafafa")));
        p.setPen(on ? QColor("#2563eb") : QColor("#d4d4d4"));
        p.drawRoundedRect(rect().adjusted(0, 0, -1, -1), 4, 4);
        p.setBrush(QColor("#1a1a1a"));
        p.setPen(Qt::NoPen);
        p.drawEllipse(rect().center(), m_dotR, m_dotR);
    }
};

// ---------------------------------------------------------------------------
// Helper: create a tool button for the left panel
// ---------------------------------------------------------------------------
static QToolButton* makeToolBtn(const QString &text, const QString &tooltip)
{
    auto *btn = new PanelButton;
    btn->setText(text);
    btn->setToolTip(tooltip);
    btn->setFixedSize(60, 60);
    btn->setCheckable(true);
    return btn;
}

// SVG variant — uses QSvgRenderer directly (no iconengines plugin needed).
// Provides two icon states: original colors when unchecked, white when checked
// (so the icon remains visible on the blue #2563eb checked background).
static QToolButton* makeSvgToolBtn(const QString &svgPath, const QString &tooltip)
{
    auto renderSvg = [&]() {
        QPixmap px(40, 40);
        px.fill(Qt::transparent);
        QSvgRenderer renderer(svgPath);
        if (renderer.isValid()) {
            QPainter p(&px);
            renderer.render(&p);
        }
        return px;
    };

    QPixmap normalPx = renderSvg();

    QPixmap onPx = normalPx.copy();
    QPainter tinter(&onPx);
    tinter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    tinter.fillRect(onPx.rect(), QColor("#e0e0e0"));
    tinter.end();

    QIcon icon;
    icon.addPixmap(normalPx, QIcon::Normal, QIcon::Off);
    icon.addPixmap(onPx,     QIcon::Normal, QIcon::On);

    auto *btn = new PanelButton;
    btn->setIcon(icon);
    btn->setIconSize(QSize(40, 40));
    btn->setToolTip(tooltip);
    btn->setFixedSize(50, 50);
    btn->setCheckable(true);
    return btn;
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_paintView = new QtPaintView(this);

    m_paintView->setModeChangeEvent(this);
    m_paintView->setViewModeChangeEvent(this);
    m_paintView->setModelLoadedEvent(this);
    m_paintView->setNewModelEvent(this);


    createActions();
    createMenus();
    createModeTabBar();
    createCentralWidget();
    createStatusBar();
    applyStyleSheet();

    setWindowTitle("ForcePAD");
    resize(1280, 800);
}

// ---------------------------------------------------------------------------
// CPV event callbacks
// ---------------------------------------------------------------------------

void MainWindow::onModeChange(fp::PaintView::TEditMode /*oldMode*/, fp::PaintView::TEditMode newMode)
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
        m_statusMode->setText(QString("  Tool: %1").arg(names[idx]));

    // Leaving paste mode re-enables copy/cut; entering select mode does too
    if (newMode != fp::PaintView::EM_PASTE) {
        m_actCopy->setEnabled(true);
        m_actCut->setEnabled(true);
    }
}

void MainWindow::onViewModeChange(fp::PaintView::TViewMode /*oldMode*/, fp::PaintView::TViewMode newMode)
{
    switch (newMode)
    {
        case fp::PaintView::VM_SKETCH:  m_statusModel->setText("View: Sketch  ");  break;
        case fp::PaintView::VM_PHYSICS: m_statusModel->setText("View: Physics  "); break;
        case fp::PaintView::VM_ACTION:  m_statusModel->setText("View: Action  ");  break;
    }
}

void MainWindow::onModelLoaded()
{
    updateWindowTitle();
    if (m_btnSelfLoad)
        m_btnSelfLoad->setChecked(m_paintView->getUseWeight());
}
void MainWindow::onNewModel()     { updateWindowTitle(); }

void MainWindow::updateWindowTitle()
{
    setWindowTitle("ForcePAD");
}

// ---------------------------------------------------------------------------
// Slots — file / edit / run
// ---------------------------------------------------------------------------

void MainWindow::fileNew()    { m_paintView->newModel(); }
void MainWindow::fileOpen()   { m_paintView->openModel(); }
void MainWindow::fileSave()   { m_paintView->saveModel(); }
void MainWindow::fileSaveAs() { m_paintView->saveModelAs(); }
void MainWindow::editUndo()  { m_paintView->undo(); }

void MainWindow::editCopy()
{
    m_paintView->copy();
    m_actPaste->setEnabled(true);
}

void MainWindow::editCut()
{
    m_paintView->cut();
    m_actPaste->setEnabled(true);
}

void MainWindow::editPaste()
{
    m_paintView->setEditMode(fp::PaintView::EM_PASTE);
    m_actCopy->setEnabled(false);
    m_actCut->setEnabled(false);
}

void MainWindow::runCalculate()
{
    m_paintView->setViewMode(fp::PaintView::VM_PHYSICS);
    m_paintView->execute();
}

void MainWindow::runOptimise()
{
    OptimisationSettingsDialog dlg(m_paintView, this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    m_paintView->setViewMode(fp::PaintView::VM_ACTION);
    m_paintView->setStatusMessageEvent(this);
    m_paintView->setContinueCalcEvent(this);

    m_continueCalc = true;
    m_optStatusLabel->setVisible(true);
    m_optProgressBar->setValue(0);
    m_optProgressBar->setVisible(true);
    m_btnStopOpt->setVisible(true);

    bool ok = m_paintView->executeOpt();

    m_btnStopOpt->setVisible(false);
    m_optProgressBar->setVisible(false);
    m_optStatusLabel->setText("");
    m_optStatusLabel->setVisible(false);

    m_paintView->setStatusMessageEvent(nullptr);
    m_paintView->setContinueCalcEvent(nullptr);

    if (!ok)
    {
        m_paintView->setViewMode(fp::PaintView::VM_PHYSICS);
        m_paintView->execute();
        showPrincipalStress();
    }
}

void MainWindow::stopOptimise()
{
    m_continueCalc = false;
}

void MainWindow::onStatusMessage(const std::string &message, const int progress)
{
    if (m_optStatusLabel)  m_optStatusLabel->setText(QString::fromStdString(message));
    if (m_optProgressBar)  m_optProgressBar->setValue(progress);
    m_paintView->update();
}

bool MainWindow::onContinueCalc()
{
    return m_continueCalc;
}

// ---------------------------------------------------------------------------
// Slots — mode tab
// ---------------------------------------------------------------------------

void MainWindow::onModeTabChanged(int index)
{
    m_leftStack->setCurrentIndex(index);
    m_rightStack->setCurrentIndex(index);
    switch (index)
    {
        case 0:
            m_paintView->setZoomResults(false);
            if (m_btnZoom) m_btnZoom->setChecked(false);
            m_paintView->setViewMode(fp::PaintView::VM_SKETCH);
            m_paintView->setEditMode(fp::PaintView::EM_DIRECT_BRUSH);
            break;
        case 1:
            m_paintView->setZoomResults(false);
            if (m_btnZoom) m_btnZoom->setChecked(false);
            m_paintView->setViewMode(fp::PaintView::VM_PHYSICS);
            m_paintView->setEditMode(fp::PaintView::EM_SELECT_BOX);
            break;
        case 2:
            m_paintView->setViewMode(fp::PaintView::VM_PHYSICS);
            m_paintView->execute();
            showPrincipalStress();
            setModeRotateLoad();
            break;
    }
}

// ---------------------------------------------------------------------------
// Slots — sketch tools
// ---------------------------------------------------------------------------

void MainWindow::setModeSelect()    { m_paintView->setEditMode(fp::PaintView::EM_SELECT_BOX); }
void MainWindow::setModeDrawBeam()  { m_paintView->setEditMode(fp::PaintView::EM_DIRECT_BRUSH); }
void MainWindow::setModeRectangle() { m_paintView->setEditMode(fp::PaintView::EM_RECTANGLE); }
void MainWindow::setModeEllipse()
{
    m_paintView->setEditMode(fp::PaintView::EM_ELLIPSE);
}
void MainWindow::setModeFloodFill() { m_paintView->setEditMode(fp::PaintView::EM_FLOODFILL); }
void MainWindow::setModeLine()      { m_paintView->setEditMode(fp::PaintView::EM_LINE); }
void MainWindow::setModeErase()     { m_paintView->setEditMode(fp::PaintView::EM_DIRECT_ERASE); }

// ---------------------------------------------------------------------------
// Slots — physicks tools
// ---------------------------------------------------------------------------

void MainWindow::setModeForce()         { m_paintView->setEditMode(fp::PaintView::EM_FORCE); }
void MainWindow::setModePinSupport()    { m_paintView->setEditMode(fp::PaintView::EM_CONSTRAINT); }
void MainWindow::setModeRollerSupport() { m_paintView->setEditMode(fp::PaintView::EM_CONSTRAINT_VECTOR); }
void MainWindow::setModeHinge()         { m_paintView->setEditMode(fp::PaintView::EM_CONSTRAINT_HINGE); }
void MainWindow::setModeCut()
{
    m_paintView->setEditMode(fp::PaintView::EM_ERASE_CONSTRAINTS_FORCES);
}

void MainWindow::setModeEraseForcesConstraints()
{
    m_paintView->setEditMode(fp::PaintView::EM_ERASE_CONSTRAINTS_FORCES);
}

void MainWindow::toggleSelfLoad()
{
    m_paintView->setUseWeight(m_btnSelfLoad->isChecked());
}

// ---------------------------------------------------------------------------
// Helpers — action visualisation
// ---------------------------------------------------------------------------

void MainWindow::setVisButton(QToolButton *active)
{
    for (auto *btn : {m_btnVisPrincipal, m_btnVisMises, m_btnVisDisp, m_btnVisStruct})
        if (btn) btn->setChecked(btn == active);
}

void MainWindow::setActionModeButton(QToolButton *active)
{
    for (auto *btn : {m_btnMoveLoad, m_btnRotateLoad})
        if (btn) btn->setChecked(btn == active);
}

// ---------------------------------------------------------------------------
// Slots — action tools
// ---------------------------------------------------------------------------

void MainWindow::showPrincipalStress()
{
    setVisButton(m_btnVisPrincipal);
    m_paintView->setVisualisationMode(fp::PaintView::VM_PRINCIPAL_STRESS);
    if (m_actionPropsStack)
        m_actionPropsStack->setCurrentIndex(0);
    m_paintView->update();
    m_paintView->repaint();
}

void MainWindow::showStress()
{
    setVisButton(m_btnVisMises);
    m_paintView->setVisualisationMode(fp::PaintView::VM_MISES_STRESS);
    if (m_actionPropsStack)
        m_actionPropsStack->setCurrentIndex(1);
    m_paintView->update();
    m_paintView->repaint();
}

void MainWindow::showDisplacement()
{
    setVisButton(m_btnVisDisp);
    m_paintView->setVisualisationMode(fp::PaintView::VM_DISPLACEMENTS);
    if (m_actionPropsStack)
        m_actionPropsStack->setCurrentIndex(2);
    m_paintView->update();
    m_paintView->repaint();
}

void MainWindow::showStructure()
{
    setVisButton(m_btnVisStruct);
    m_paintView->setVisualisationMode(fp::PaintView::VM_STRUCTURE);
    if (m_actionPropsStack)
        m_actionPropsStack->setCurrentIndex(0);
    m_paintView->update();
    m_paintView->repaint();
}

void MainWindow::setModeMoveLoad()
{
    setActionModeButton(m_btnMoveLoad);
    m_paintView->setEditMode(fp::PaintView::EM_DYNAMIC_FORCE_UPDATE);
    m_paintView->setMoveLoad(true);
}

void MainWindow::setModeRotateLoad()
{
    setActionModeButton(m_btnRotateLoad);
    m_paintView->setEditMode(fp::PaintView::EM_DYNAMIC_FORCE_UPDATE);
    m_paintView->setMoveLoad(false);
}

void MainWindow::toggleZoomMode()
{
    const bool on = m_btnZoom->isChecked();
    m_paintView->setZoomResults(on);
}

void MainWindow::expandToWindow()
{
    m_paintView->expandImageToWindow();
    m_paintView->reinitGL();
}

// ---------------------------------------------------------------------------
// Slots — right panel properties
// ---------------------------------------------------------------------------

void MainWindow::onStiffnessChanged(int value)
{
    m_paintView->setStiffness(value / 100.0);
}

void MainWindow::onThicknessChanged(double value)
{
    m_paintView->setThickness(value);
}

void MainWindow::onForceMagnitudeChanged(double value)
{
    m_paintView->setForceMagnitude(value);
}

void MainWindow::onSizeChanged(int value)
{
    m_paintView->setStressSize(value / 10.0);
}

void MainWindow::onTransparencyChanged(int value)
{
    m_paintView->setStressAlpha(1.0 - value / 100.0);
}

void MainWindow::onStepsChanged(int value)
{
    m_paintView->setStressStep(value);
}

void MainWindow::onAutoScale()
{
    m_paintView->setLockScaling(false);
    m_paintView->update();
}

void MainWindow::onStressModeChanged(int mode)
{
    m_paintView->setStressMode(static_cast<fp::FemGrid2::TStressMode>(mode));
    m_paintView->update();
}

void MainWindow::onColorScaleChanged(int index)
{
    m_paintView->setColorMap(index);
    m_paintView->update();
}

void MainWindow::onMisesMaxChanged(int value)
{
    m_paintView->setUpperMisesTreshold(value / 100.0);
    m_paintView->update();
}

void MainWindow::onDisplacementScaleChanged(int value)
{
    m_paintView->setDisplacementScale(value / 10.0);
    m_paintView->update();
}

// ---------------------------------------------------------------------------
// Legacy view slots
// ---------------------------------------------------------------------------

void MainWindow::setViewSketch()  { m_paintView->setViewMode(fp::PaintView::VM_SKETCH); }
void MainWindow::setViewPhysics() { m_paintView->setViewMode(fp::PaintView::VM_PHYSICS); }
void MainWindow::setViewAction()  { m_paintView->setViewMode(fp::PaintView::VM_ACTION); }

// ---------------------------------------------------------------------------
// Slots — Settings menu
// ---------------------------------------------------------------------------

void MainWindow::settingsCalc()
{
    CalcSettingsDialog dlg(m_paintView, this);
    dlg.exec();
}

void MainWindow::settingsGeneral()
{
    GeneralSettingsDialog dlg(m_paintView, this);
    dlg.exec();
}

// ---------------------------------------------------------------------------
// Slots — Help menu
// ---------------------------------------------------------------------------

void MainWindow::helpDocumentation()
{
    m_paintView->showAbout();
}

void MainWindow::helpAbout()
{
    AboutDialog dlg(this);
    dlg.exec();
}

// ---------------------------------------------------------------------------
// UI setup — actions
// ---------------------------------------------------------------------------

void MainWindow::createActions()
{
    m_actNew    = new QAction("New",       this);  m_actNew->setShortcut(QKeySequence::New);
    m_actOpen   = new QAction("Open...",   this);  m_actOpen->setShortcut(QKeySequence::Open);
    m_actSave   = new QAction("Save",      this);  m_actSave->setShortcut(QKeySequence::Save);
    m_actSaveAs = new QAction("Save As...", this);
    m_actQuit   = new QAction("Quit",      this);  m_actQuit->setShortcut(QKeySequence::Quit);

    m_actUndo      = new QAction("Undo",      this);  m_actUndo->setShortcut(QKeySequence::Undo);
    m_actCopy      = new QAction("Copy",      this);  m_actCopy->setShortcut(QKeySequence::Copy);
    m_actCut       = new QAction("Cut",       this);  m_actCut->setShortcut(QKeySequence::Cut);
    m_actPaste     = new QAction("Paste",     this);  m_actPaste->setShortcut(QKeySequence::Paste);
    m_actPaste->setEnabled(false);
    m_actCalculate = new QAction("Calculate", this);  m_actCalculate->setShortcut(Qt::Key_F5);
    m_actOptimise  = new QAction("Optimise",  this);  m_actOptimise->setShortcut(Qt::Key_F6);
    // Keep Calculate/Optimise actions alive for keyboard shortcuts even without a menu
    addAction(m_actCalculate);
    addAction(m_actOptimise);

    m_actSettingsCalc    = new QAction("Calculation...", this);
    m_actSettingsGeneral = new QAction("General...",     this);

    m_actHelpDocumentation = new QAction("Documentation", this);
    m_actHelpAbout         = new QAction("About",         this);

    connect(m_actNew,              &QAction::triggered, this, &MainWindow::fileNew);
    connect(m_actOpen,             &QAction::triggered, this, &MainWindow::fileOpen);
    connect(m_actSave,             &QAction::triggered, this, &MainWindow::fileSave);
    connect(m_actSaveAs,           &QAction::triggered, this, &MainWindow::fileSaveAs);
    connect(m_actQuit,             &QAction::triggered, this, &MainWindow::close);
    connect(m_actUndo,             &QAction::triggered, this, &MainWindow::editUndo);
    connect(m_actCopy,             &QAction::triggered, this, &MainWindow::editCopy);
    connect(m_actCut,              &QAction::triggered, this, &MainWindow::editCut);
    connect(m_actPaste,            &QAction::triggered, this, &MainWindow::editPaste);
    connect(m_actCalculate,        &QAction::triggered, this, &MainWindow::runCalculate);
    connect(m_actOptimise,         &QAction::triggered, this, &MainWindow::runOptimise);
    connect(m_actSettingsCalc,     &QAction::triggered, this, &MainWindow::settingsCalc);
    connect(m_actSettingsGeneral,  &QAction::triggered, this, &MainWindow::settingsGeneral);
    connect(m_actHelpDocumentation,&QAction::triggered, this, &MainWindow::helpDocumentation);
    connect(m_actHelpAbout,        &QAction::triggered, this, &MainWindow::helpAbout);
}

// ---------------------------------------------------------------------------
// UI setup — menus
// ---------------------------------------------------------------------------

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
    editMenu->addSeparator();
    editMenu->addAction(m_actCopy);
    editMenu->addAction(m_actCut);
    editMenu->addAction(m_actPaste);

    QMenu *settingsMenu = menuBar()->addMenu("Settings");
    settingsMenu->addAction(m_actSettingsCalc);
    settingsMenu->addAction(m_actSettingsGeneral);

    QMenu *helpMenu = menuBar()->addMenu("Help");
    helpMenu->addAction(m_actHelpDocumentation);
    helpMenu->addAction(m_actHelpAbout);
}

// ---------------------------------------------------------------------------
// UI setup — mode tab bar (in a non-movable toolbar at top)
// ---------------------------------------------------------------------------

void MainWindow::createModeTabBar()
{
    QToolBar *tabTB = addToolBar("Modes");
    tabTB->setMovable(false);
    tabTB->setFloatable(false);

    m_modeTabBar = new QTabBar;
    m_modeTabBar->addTab("  Sketch  ");
    m_modeTabBar->addTab("  Physics  ");
    m_modeTabBar->addTab("  Action  ");
    m_modeTabBar->setExpanding(false);

    tabTB->addWidget(m_modeTabBar);

    connect(m_modeTabBar, &QTabBar::currentChanged, this, &MainWindow::onModeTabChanged);
}

// ---------------------------------------------------------------------------
// UI setup — central widget (left panel + canvas + right panel)
// ---------------------------------------------------------------------------

void MainWindow::createCentralWidget()
{
    auto *central = new QWidget(this);
    auto *hbox = new QHBoxLayout(central);
    hbox->setContentsMargins(0, 0, 0, 0);
    hbox->setSpacing(0);

    // --- Left panel ---
    m_leftPanel = new QWidget;
    m_leftPanel->setObjectName("leftPanel");
    m_leftPanel->setFixedWidth(66);

    auto *leftVBox = new QVBoxLayout(m_leftPanel);
    leftVBox->setContentsMargins(0, 0, 0, 0);
    leftVBox->setSpacing(0);

    m_leftStack = new QStackedWidget;
    m_leftStack->addWidget(createSketchToolPanel());
    m_leftStack->addWidget(createPhysicksToolPanel());
    m_leftStack->addWidget(createActionToolPanel());
    leftVBox->addWidget(m_leftStack);

    hbox->addWidget(m_leftPanel);

    // --- Splitter: canvas | right panel ---
    auto *splitter = new QSplitter(Qt::Horizontal);
    splitter->setHandleWidth(4);
    splitter->setChildrenCollapsible(false);

    splitter->addWidget(m_paintView);

    // --- Right panel ---
    m_rightPanel = new QWidget;
    m_rightPanel->setObjectName("rightPanel");
    m_rightPanel->setMinimumWidth(160);

    auto *rightVBox = new QVBoxLayout(m_rightPanel);
    rightVBox->setContentsMargins(0, 0, 0, 0);
    rightVBox->setSpacing(0);

    auto *propsTitle = new QLabel("  Properties");
    propsTitle->setFixedHeight(36);
    propsTitle->setStyleSheet(
        "font-weight: bold; font-size: 12px; color: #171717;"
        "border-bottom: 1px solid #e5e5e5; background: white;");
    rightVBox->addWidget(propsTitle);

    m_rightStack = new QStackedWidget;
    m_rightStack->addWidget(createSketchPropsPanel());
    m_rightStack->addWidget(createPhysicksPropsPanel());
    m_rightStack->addWidget(createActionPropsPanel());
    rightVBox->addWidget(m_rightStack);
    rightVBox->addStretch();

    splitter->addWidget(m_rightPanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);
    splitter->setSizes({1024, 256});

    hbox->addWidget(splitter, 1);

    setCentralWidget(central);
}

// ---------------------------------------------------------------------------
// Left tool panels
// ---------------------------------------------------------------------------

QWidget* MainWindow::createSketchToolPanel()
{
    auto *panel = new QWidget;
    panel->setStyleSheet("background: transparent;");
    auto *vbox = new QVBoxLayout(panel);
    vbox->setContentsMargins(4, 4, 4, 4);
    vbox->setSpacing(4);
    vbox->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    m_sketchBtnGroup = new QButtonGroup(this);
    m_sketchBtnGroup->setExclusive(true);

    auto *btnSelect = makeSvgToolBtn("icons/select.svg", "Select (S)");
    auto *btnDraw = makeSvgToolBtn("icons/brush.svg", "Brush");
    auto *btnRect = makeSvgToolBtn("icons/rectangle.svg", "Rectangle");
    auto *btnEllipse = makeSvgToolBtn("icons/ellipse.svg", "Ellipse");
    auto *btnFill = makeSvgToolBtn("icons/flood_fill.svg", "Flood Fill");
    auto *btnLine = makeSvgToolBtn("icons/line.svg", "Line");
    auto *btnErase  = makeSvgToolBtn("icons/delete_force_bc.svg", "Erase");
    auto *btnExpand = makeSvgToolBtn("icons/expand_image.svg",    "Expand image to window");

    btnDraw->setChecked(true);
    btnExpand->setCheckable(false);

    for (auto *b : {btnSelect, btnDraw, btnRect, btnEllipse, btnFill, btnLine, btnErase})
        m_sketchBtnGroup->addButton(b);

    connect(btnSelect,  &QToolButton::clicked, this, &MainWindow::setModeSelect);
    connect(btnDraw,    &QToolButton::clicked, this, &MainWindow::setModeDrawBeam);
    connect(btnRect,    &QToolButton::clicked, this, &MainWindow::setModeRectangle);
    connect(btnEllipse, &QToolButton::clicked, this, &MainWindow::setModeEllipse);
    connect(btnFill,    &QToolButton::clicked, this, &MainWindow::setModeFloodFill);
    connect(btnLine,    &QToolButton::clicked, this, &MainWindow::setModeLine);
    connect(btnErase,   &QToolButton::clicked, this, &MainWindow::setModeErase);
    connect(btnExpand,  &QToolButton::clicked, this, &MainWindow::expandToWindow);

    vbox->addWidget(btnSelect);
    vbox->addWidget(btnDraw);
    vbox->addWidget(makeSeparator());
    vbox->addWidget(btnRect);
    vbox->addWidget(btnEllipse);
    vbox->addWidget(btnLine);
    vbox->addWidget(btnFill);
    vbox->addWidget(makeSeparator());
    vbox->addWidget(btnErase);
    vbox->addWidget(makeSeparator());
    vbox->addWidget(btnExpand);
    vbox->addStretch();

    return panel;
}

QWidget* MainWindow::createPhysicksToolPanel()
{
    auto *panel = new QWidget;
    panel->setStyleSheet("background: transparent;");
    auto *vbox = new QVBoxLayout(panel);
    vbox->setContentsMargins(4, 4, 4, 4);
    vbox->setSpacing(4);
    vbox->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    m_physicksBtnGroup = new QButtonGroup(this);
    m_physicksBtnGroup->setExclusive(true);

    auto *btnSelect       = makeSvgToolBtn("icons/select.svg", "Select (S)");
    auto *btnForce        = makeSvgToolBtn("icons/load.svg", "Add Force");
    auto *btnRoller       = makeSvgToolBtn("icons/bc.svg", "Roller Support");
    auto *btnDeleteForceBc = makeSvgToolBtn("icons/delete_force_bc.svg", "Delete forces/bc");

    for (auto *b : {btnSelect, btnForce, btnRoller, btnDeleteForceBc})
        m_physicksBtnGroup->addButton(b);

    // Self-load toggle — not part of the exclusive mode group
    m_btnSelfLoad = makeSvgToolBtn("icons/dist_load.svg", "Self-loading (weight)");

    connect(btnSelect,        &QToolButton::clicked, this, &MainWindow::setModeSelect);
    connect(btnForce,         &QToolButton::clicked, this, &MainWindow::setModeForce);
    connect(btnRoller,        &QToolButton::clicked, this, &MainWindow::setModeRollerSupport);
    connect(btnDeleteForceBc, &QToolButton::clicked, this, &MainWindow::setModeEraseForcesConstraints);
    connect(m_btnSelfLoad,    &QToolButton::clicked, this, &MainWindow::toggleSelfLoad);

    vbox->addWidget(btnSelect);
    vbox->addWidget(makeSeparator());
    vbox->addWidget(btnForce);
    vbox->addWidget(btnRoller);
    vbox->addWidget(makeSeparator());
    vbox->addWidget(btnDeleteForceBc);
    vbox->addWidget(makeSeparator());
    vbox->addWidget(m_btnSelfLoad);
    vbox->addStretch();

    return panel;
}

QWidget* MainWindow::createActionToolPanel()
{
    auto *panel = new QWidget;
    panel->setStyleSheet("background: transparent;");
    auto *vbox = new QVBoxLayout(panel);
    vbox->setContentsMargins(4, 4, 4, 4);
    vbox->setSpacing(4);
    vbox->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    m_btnVisPrincipal = makeSvgToolBtn("icons/principal.svg", "Show Principal Stresses");
    m_btnVisMises     = makeSvgToolBtn("icons/mises.svg",     "Show Mises Stress");
    m_btnVisDisp      = makeSvgToolBtn("icons/deflections.svg","Show Displacement");
    m_btnVisStruct    = makeSvgToolBtn("icons/structure.svg", "Show Structure");
    auto *btnOpt    = makeSvgToolBtn("icons/optimisation.svg", "Optimise (F6)");

    m_btnMoveLoad   = makeSvgToolBtn("icons/move_load.svg",   "Move Load");
    m_btnRotateLoad = makeSvgToolBtn("icons/rotate_load.svg", "Rotate Load");
    m_btnZoom       = makeSvgToolBtn("icons/zoom_mesh.svg",   "Zoom Mode (scroll wheel to zoom)");

    m_btnVisPrincipal->setChecked(true);
    m_btnRotateLoad->setChecked(true);
    btnOpt->setCheckable(false);

    connect(m_btnVisPrincipal, &QToolButton::clicked, this, &MainWindow::showPrincipalStress);
    connect(m_btnVisMises,     &QToolButton::clicked, this, &MainWindow::showStress);
    connect(m_btnVisDisp,      &QToolButton::clicked, this, &MainWindow::showDisplacement);
    connect(m_btnVisStruct,    &QToolButton::clicked, this, &MainWindow::showStructure);
    connect(m_btnMoveLoad,     &QToolButton::clicked, this, &MainWindow::setModeMoveLoad);
    connect(m_btnRotateLoad,   &QToolButton::clicked, this, &MainWindow::setModeRotateLoad);
    connect(m_btnZoom,         &QToolButton::clicked, this, &MainWindow::toggleZoomMode);
    connect(btnOpt,            &QToolButton::clicked, this, &MainWindow::runOptimise);

    vbox->addWidget(m_btnVisPrincipal);
    vbox->addWidget(m_btnVisMises);
    vbox->addWidget(m_btnVisDisp);
    vbox->addWidget(m_btnVisStruct);
    vbox->addWidget(makeSeparator());
    vbox->addWidget(m_btnMoveLoad);
    vbox->addWidget(m_btnRotateLoad);
    vbox->addWidget(m_btnZoom);
    vbox->addWidget(makeSeparator());
    vbox->addWidget(btnOpt);
    vbox->addStretch();

    return panel;
}

// ---------------------------------------------------------------------------
// Right property panels
// ---------------------------------------------------------------------------

QWidget* MainWindow::createSketchPropsPanel()
{
    auto *panel = new QWidget;
    auto *vbox = new QVBoxLayout(panel);
    vbox->setContentsMargins(12, 12, 12, 12);
    vbox->setSpacing(8);
    vbox->setAlignment(Qt::AlignTop);

    auto addSectionLabel = [&](const QString &txt) {
        auto *lbl = new QLabel(txt);
        lbl->setStyleSheet("color: #525252; font-size: 11px; font-weight: bold; margin-top: 4px;");
        vbox->addWidget(lbl);
    };

    // --- Stiffness ---
    addSectionLabel("Stiffness");

    m_stiffnessDisplay = new QLabel("100%");
    m_stiffnessDisplay->setAlignment(Qt::AlignCenter);
    m_stiffnessDisplay->setFixedHeight(36);
    m_stiffnessDisplay->setStyleSheet(
        "font-size: 22px; font-weight: bold; color: #171717;"
        "background: #f5f5f5; border: 1px solid #e5e5e5; border-radius: 4px;");
    vbox->addWidget(m_stiffnessDisplay);

    // Preset buttons: two rows of 5, 100→60 top, 50→10 bottom
    struct Preset { int pct; QColor bg; };
    static const Preset presets[] = {
        {100, {0xff,0xff,0xff}}, { 90, {0xe0,0xe0,0xe0}}, { 80, {0xc0,0xc0,0xc0}},
        { 70, {0xa0,0xa0,0xa0}}, { 60, {0x80,0x80,0x80}},
        { 50, {0x60,0x60,0x60}}, { 40, {0x48,0x48,0x48}}, { 30, {0x33,0x33,0x33}},
        { 20, {0x22,0x22,0x22}}, { 10, {0x1a,0x1a,0x1a}},
    };

    auto *stiffGroup = new QButtonGroup(this);
    stiffGroup->setExclusive(true);

    for (int row = 0; row < 2; row++) {
        auto *hbox = new QHBoxLayout;
        hbox->setSpacing(3);
        hbox->setContentsMargins(0, 0, 0, 0);
        for (int col = 0; col < 5; col++) {
            const auto &pr = presets[row * 5 + col];
            auto *btn = new QPushButton(QString::number(pr.pct) + "%");
            btn->setCheckable(true);
            btn->setFixedHeight(26);
            const bool dark = pr.bg.lightness() < 128;
            btn->setStyleSheet(QString(
                "QPushButton { background: %1; color: %2; border: 1px solid #bbb;"
                "  border-radius: 3px; font-size: 9px; padding: 0; }"
                "QPushButton:checked { border: 2px solid #2563eb; font-weight: bold; }"
                "QPushButton:hover:!checked { border: 1px solid #6b9fe4; }"
            ).arg(pr.bg.name(), dark ? "#fff" : "#111"));
            stiffGroup->addButton(btn);
            if (pr.pct == 100) btn->setChecked(true);
            const int pct = pr.pct;
            connect(btn, &QPushButton::clicked, this, [this, pct]() {
                m_stiffnessSlider->blockSignals(true);
                m_stiffnessSlider->setValue(pct);
                m_stiffnessSlider->blockSignals(false);
                m_stiffnessDisplay->setText(QString::number(pct) + "%");
                m_paintView->setStiffness(pct / 100.0);
            });
            hbox->addWidget(btn);
        }
        vbox->addLayout(hbox);
    }

    m_stiffnessSlider = new QSlider(Qt::Horizontal);
    m_stiffnessSlider->setRange(0, 100);
    m_stiffnessSlider->setValue(100);
    connect(m_stiffnessSlider, &QSlider::valueChanged, this, &MainWindow::onStiffnessChanged);
    connect(m_stiffnessSlider, &QSlider::valueChanged, this, [this](int v) {
        m_stiffnessDisplay->setText(QString::number(v) + "%");
    });
    vbox->addWidget(m_stiffnessSlider);

    vbox->addWidget(makeSeparator());

    // --- Brush Size ---
    addSectionLabel("Brush Size");

    auto *brushRow = new QHBoxLayout;
    brushRow->setSpacing(3);
    brushRow->setContentsMargins(0, 0, 0, 0);

    auto *brushGroup = new QButtonGroup(this);
    brushGroup->setExclusive(true);

    static const int dotRadii[] = {3, 6, 9, 12, 15};
    for (int i = 0; i < 5; i++) {
        auto *btn = new BrushSizeButton(dotRadii[i]);
        brushGroup->addButton(btn);
        if (i == 0) btn->setChecked(true);
        const int idx = i;
        connect(btn, &QToolButton::clicked, this, [this, idx]() {
            m_paintView->setCurrentBrush(idx);
            m_paintView->setBrushMagnification(idx + 1);
        });
        brushRow->addWidget(btn);
    }
    vbox->addLayout(brushRow);

    vbox->addWidget(makeSeparator());

    // --- Thickness ---
    addSectionLabel("Thickness");

    m_thicknessSpinBox = new QDoubleSpinBox;
    m_thicknessSpinBox->setRange(0.1, 100.0);
    m_thicknessSpinBox->setValue(1.0);
    m_thicknessSpinBox->setSingleStep(0.5);
    connect(m_thicknessSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::onThicknessChanged);
    vbox->addWidget(m_thicknessSpinBox);

    vbox->addStretch();
    return panel;
}

QWidget* MainWindow::createPhysicksPropsPanel()
{
    auto *panel = new QWidget;
    auto *form = new QFormLayout(panel);
    form->setContentsMargins(12, 12, 12, 12);
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignLeft);

    m_forceMagnitudeSpinBox = new QDoubleSpinBox;
    m_forceMagnitudeSpinBox->setRange(0.0, 1e9);
    m_forceMagnitudeSpinBox->setValue(1000.0);
    m_forceMagnitudeSpinBox->setSingleStep(100.0);
    m_forceMagnitudeSpinBox->setSuffix(" N");
    connect(m_forceMagnitudeSpinBox, &QDoubleSpinBox::valueChanged,
            this, &MainWindow::onForceMagnitudeChanged);
    form->addRow("Force", m_forceMagnitudeSpinBox);

    m_forceDirectionCombo = new QComboBox;
    m_forceDirectionCombo->addItems({"Down", "Up", "Left", "Right"});
    form->addRow("Direction", m_forceDirectionCombo);

    return panel;
}

QWidget* MainWindow::createActionPropsPanel()
{
    auto *container = new QWidget;
    auto *vbox = new QVBoxLayout(container);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing(0);

    m_actionPropsStack = new QStackedWidget;
    m_actionPropsStack->addWidget(createPrincipalPropsWidget());  // index 0
    m_actionPropsStack->addWidget(createMisesPropsWidget());      // index 1
    m_actionPropsStack->addWidget(createDeflectionPropsWidget()); // index 2

    vbox->addWidget(m_actionPropsStack);
    return container;
}

QWidget* MainWindow::createPrincipalPropsWidget()
{
    auto *panel = new QWidget;
    auto *vbox = new QVBoxLayout(panel);
    vbox->setContentsMargins(12, 12, 12, 12);
    vbox->setSpacing(8);
    vbox->setAlignment(Qt::AlignTop);

    auto addLabel = [&](const QString &txt) {
        auto *lbl = new QLabel(txt);
        lbl->setStyleSheet("color: #525252; font-size: 11px; font-weight: bold; margin-top: 4px;");
        vbox->addWidget(lbl);
    };

    // Helper: SVG icon button styled for the light right-panel background
    auto makePanelIconBtn = [](const QString &svgPath, const QString &tip) -> QToolButton* {
        QPixmap px(44, 44);
        px.fill(Qt::transparent);
        QSvgRenderer renderer(svgPath);
        if (renderer.isValid()) { QPainter p(&px); renderer.render(&p); }

        QPixmap tinted = px.copy();
        QPainter t(&tinted);
        t.setCompositionMode(QPainter::CompositionMode_SourceIn);
        t.fillRect(tinted.rect(), QColor("#2563eb"));
        t.end();

        QIcon icon;
        icon.addPixmap(px,     QIcon::Normal, QIcon::Off);
        icon.addPixmap(tinted, QIcon::Normal, QIcon::On);

        auto *btn = new QToolButton;
        btn->setIcon(icon);
        btn->setIconSize(QSize(44, 44));
        btn->setFixedHeight(44);
        btn->setCheckable(true);
        btn->setToolTip(tip);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn->setStyleSheet(
            "QToolButton { background: #f5f5f5; border: 1px solid #d4d4d4; border-radius: 4px; }"
            "QToolButton:checked { background: #dbeafe; border: 2px solid #2563eb; }"
            "QToolButton:hover:!checked { background: #e5e5e5; }");
        return btn;
    };

    // Size: range matches FLTK rolArrowSize (0–200), default 50; slider value / 10.0
    addLabel("Size");
    m_sizeSlider = new QSlider(Qt::Horizontal);
    m_sizeSlider->setRange(1, 2000);
    m_sizeSlider->setValue(500);
    connect(m_sizeSlider, &QSlider::valueChanged, this, &MainWindow::onSizeChanged);
    vbox->addWidget(m_sizeSlider);

    addLabel("Transparency");
    m_transparencySlider = new QSlider(Qt::Horizontal);
    m_transparencySlider->setRange(0, 100);
    m_transparencySlider->setValue(0);
    connect(m_transparencySlider, &QSlider::valueChanged, this, &MainWindow::onTransparencyChanged);
    vbox->addWidget(m_transparencySlider);

    // Step: FLTK maximum is 20
    addLabel("Step");
    m_stepsSlider = new QSlider(Qt::Horizontal);
    m_stepsSlider->setRange(1, 20);
    m_stepsSlider->setValue(1);
    connect(m_stepsSlider, &QSlider::valueChanged, this, &MainWindow::onStepsChanged);
    vbox->addWidget(m_stepsSlider);

    m_autoScaleBtn = new QPushButton("Auto Scale");
    m_autoScaleBtn->setObjectName("autoScaleBtn");
    connect(m_autoScaleBtn, &QPushButton::clicked, this, &MainWindow::onAutoScale);
    vbox->addWidget(m_autoScaleBtn);

    vbox->addWidget([]{
        auto *f = new QFrame; f->setFrameShape(QFrame::HLine);
        f->setFixedHeight(1);
        f->setStyleSheet("background: #e5e5e5; border: none;");
        return f;
    }());

    addLabel("Show");

    m_stressModeGroup = new QButtonGroup(this);
    m_stressModeGroup->setExclusive(true);

    auto *btnAll  = makePanelIconBtn("icons/compression_tension.svg", "Tensile & Compression");
    auto *btnTens = makePanelIconBtn("icons/tension.svg",              "Tensile only");
    auto *btnComp = makePanelIconBtn("icons/compression.svg",          "Compression only");

    m_stressModeGroup->addButton(btnAll,  fp::FemGrid2::SM_ALL);
    m_stressModeGroup->addButton(btnTens, fp::FemGrid2::SM_POSITIVE);
    m_stressModeGroup->addButton(btnComp, fp::FemGrid2::SM_NEGATIVE);
    btnAll->setChecked(true);

    connect(m_stressModeGroup, &QButtonGroup::idClicked, this, &MainWindow::onStressModeChanged);

    auto *iconRow = new QHBoxLayout;
    iconRow->setSpacing(4);
    iconRow->setContentsMargins(0, 0, 0, 0);
    iconRow->addWidget(btnAll);
    iconRow->addWidget(btnTens);
    iconRow->addWidget(btnComp);
    vbox->addLayout(iconRow);

    vbox->addStretch();
    return panel;
}

QWidget* MainWindow::createMisesPropsWidget()
{
    auto *panel = new QWidget;
    auto *vbox = new QVBoxLayout(panel);
    vbox->setContentsMargins(12, 12, 12, 12);
    vbox->setSpacing(8);
    vbox->setAlignment(Qt::AlignTop);

    auto addLabel = [&](const QString &txt) {
        auto *lbl = new QLabel(txt);
        lbl->setStyleSheet("color: #525252; font-size: 11px; font-weight: bold; margin-top: 4px;");
        vbox->addWidget(lbl);
    };

    addLabel("Color Scale");

    m_colorScaleGroup = new QButtonGroup(this);
    m_colorScaleGroup->setExclusive(true);

    auto makePanelIconBtn2 = [](const QString &svgPath, const QString &tip) -> QToolButton* {
        QPixmap px(44, 44);
        px.fill(Qt::transparent);
        QSvgRenderer renderer(svgPath);
        if (renderer.isValid()) { QPainter p(&px); renderer.render(&p); }

        QPixmap tinted = px.copy();
        QPainter t(&tinted);
        t.setCompositionMode(QPainter::CompositionMode_SourceIn);
        t.fillRect(tinted.rect(), QColor("#2563eb"));
        t.end();

        QIcon icon;
        icon.addPixmap(px,     QIcon::Normal, QIcon::Off);
        icon.addPixmap(tinted, QIcon::Normal, QIcon::On);

        auto *btn = new QToolButton;
        btn->setIcon(icon);
        btn->setIconSize(QSize(44, 44));
        btn->setFixedHeight(44);
        btn->setCheckable(true);
        btn->setToolTip(tip);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn->setStyleSheet(
            "QToolButton { background: #f5f5f5; border: 1px solid #d4d4d4; border-radius: 4px; }"
            "QToolButton:checked { background: #dbeafe; border: 2px solid #2563eb; }"
            "QToolButton:hover:!checked { background: #e5e5e5; }");
        return btn;
    };

    auto *btnSpectrum = makePanelIconBtn2("icons/rainbow.svg", "Spectrum color scale");
    btnSpectrum->setChecked(true);
    auto *btnHeat     = makePanelIconBtn2("icons/heat.svg",    "Heat color scale");

    m_colorScaleGroup->addButton(btnSpectrum, 1);
    m_colorScaleGroup->addButton(btnHeat,     2);

    auto *colorRow = new QHBoxLayout;
    colorRow->setSpacing(4);
    colorRow->setContentsMargins(0, 0, 0, 0);
    colorRow->addWidget(btnSpectrum);
    colorRow->addWidget(btnHeat);
    vbox->addLayout(colorRow);

    connect(m_colorScaleGroup, &QButtonGroup::idClicked, this, &MainWindow::onColorScaleChanged);

    addLabel("Max Value");
    m_misesMaxSlider = new QSlider(Qt::Horizontal);
    m_misesMaxSlider->setRange(1, 200);
    m_misesMaxSlider->setValue(100);
    connect(m_misesMaxSlider, &QSlider::valueChanged, this, &MainWindow::onMisesMaxChanged);
    vbox->addWidget(m_misesMaxSlider);

    vbox->addStretch();
    return panel;
}

QWidget* MainWindow::createDeflectionPropsWidget()
{
    auto *panel = new QWidget;
    auto *vbox = new QVBoxLayout(panel);
    vbox->setContentsMargins(12, 12, 12, 12);
    vbox->setSpacing(8);
    vbox->setAlignment(Qt::AlignTop);

    auto *lbl = new QLabel("Scale");
    lbl->setStyleSheet("color: #525252; font-size: 11px; font-weight: bold; margin-top: 4px;");
    vbox->addWidget(lbl);

    m_dispScaleSlider = new QSlider(Qt::Horizontal);
    m_dispScaleSlider->setRange(1, 1000);
    m_dispScaleSlider->setValue(1);
    connect(m_dispScaleSlider, &QSlider::valueChanged, this, &MainWindow::onDisplacementScaleChanged);
    vbox->addWidget(m_dispScaleSlider);

    vbox->addStretch();
    return panel;
}

// ---------------------------------------------------------------------------
// UI setup — status bar
// ---------------------------------------------------------------------------

void MainWindow::createStatusBar()
{
    m_statusMode  = new QLabel("  Tool: Draw Beam");
    m_statusModel = new QLabel("View: Sketch  ");
    statusBar()->addWidget(m_statusMode);

    // Fixed-width label so changing text does not shift the progress bar.
    m_optStatusLabel = new QLabel;
    m_optStatusLabel->setFixedWidth(220);
    m_optStatusLabel->setVisible(false);
    statusBar()->addWidget(m_optStatusLabel);

    m_optProgressBar = new QProgressBar;
    m_optProgressBar->setRange(0, 100);
    m_optProgressBar->setFixedWidth(140);
    m_optProgressBar->setFixedHeight(13);
    m_optProgressBar->setTextVisible(false);
    m_optProgressBar->setVisible(false);
    m_optProgressBar->setStyleSheet(
        "QProgressBar { background: #404040; border-radius: 3px; }"
        "QProgressBar::chunk { background: #2563eb; border-radius: 3px; }");
    statusBar()->addWidget(m_optProgressBar);

    // Stop button — lives in the status bar so it is always reachable.
    m_btnStopOpt = new QPushButton("Stop");
    m_btnStopOpt->setFixedHeight(20);
    m_btnStopOpt->setVisible(false);
    m_btnStopOpt->setStyleSheet(
        "QPushButton { background: #dc2626; color: white; border: none;"
        "  border-radius: 3px; padding: 0 10px; font-size: 11px; }"
        "QPushButton:hover { background: #ef4444; }"
        "QPushButton:pressed { background: #b91c1c; }");
    connect(m_btnStopOpt, &QPushButton::clicked, this, &MainWindow::stopOptimise);
    statusBar()->addWidget(m_btnStopOpt);

    statusBar()->addPermanentWidget(m_statusModel);
}

// ---------------------------------------------------------------------------
// Stylesheet
// ---------------------------------------------------------------------------

void MainWindow::applyStyleSheet()
{
    // Menu bar — explicit colors to survive Windows dark mode
    menuBar()->setStyleSheet(
        "QMenuBar { background: #ffffff; color: #171717; border-bottom: 1px solid #e5e5e5; }"
        "QMenuBar::item { color: #171717; padding: 6px 10px; }"
        "QMenuBar::item:selected { background: #f0f0f0; color: #171717; }"
        "QMenu { background: #ffffff; color: #171717; border: 1px solid #d4d4d4; }"
        "QMenu::item:selected { background: #e5e5e5; color: #171717; }");

    // Mode tab toolbar + tooltips (applied at MainWindow level so QToolTip inherits)
    setStyleSheet(
        "QToolBar { background: #f5f5f5; border: none; border-bottom: 1px solid #e5e5e5; padding: 0; }"
        "QTabBar::tab { background: #f5f5f5; border: none; border-bottom: 2px solid transparent;"
        "  padding: 10px 20px; font-size: 13px; color: #737373; }"
        "QTabBar::tab:selected { color: #171717; border-bottom: 2px solid #2563eb; }"
        "QTabBar::tab:hover { color: #171717; background: #ebebeb; }"
        "QToolTip { background: #1e293b; color: #f1f5f9; border: 1px solid #334155;"
        "  padding: 4px 8px; border-radius: 4px; font-size: 12px; }");

    // Left dark panel + tool buttons
    m_leftPanel->setStyleSheet("#leftPanel { background: #1a1a1a; }");

    // Splitter handle
    setStyleSheet(styleSheet() +
        "QSplitter::handle { background: #d4d4d4; }"
        "QSplitter::handle:hover { background: #2563eb; }");

    // Right panel
    m_rightPanel->setStyleSheet(
        "#rightPanel { background: #ffffff; border-left: 1px solid #d4d4d4; }"
        "QLabel { color: #737373; font-size: 11px; }"
        "QSlider::groove:horizontal { height: 4px; background: #e5e5e5; border-radius: 2px; }"
        "QSlider::handle:horizontal { width: 14px; height: 14px; margin: -5px 0;"
        "  background: #2563eb; border-radius: 7px; }"
        "QSlider::sub-page:horizontal { background: #2563eb; border-radius: 2px; }"
        "#autoScaleBtn { background: #2563eb; color: white; border: none;"
        "  border-radius: 4px; padding: 6px; font-size: 12px; }"
        "#autoScaleBtn:hover { background: #3b82f6; }"
        "#autoScaleBtn:pressed { background: #1d4ed8; }");

    // Status bar
    statusBar()->setStyleSheet(
        "QStatusBar { background: #262626; color: #ffffff; font-size: 11px; }"
        "QStatusBar QLabel { color: #ffffff; }");
}
