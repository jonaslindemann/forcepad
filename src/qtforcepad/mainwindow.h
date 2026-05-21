#pragma once

#include <QMainWindow>
#include "QtPaintView.h"
#include "../common/FemGridSolver2.h"

class QAction;
class QToolBar;
class QLabel;
class QTabBar;
class QStackedWidget;
class QSlider;
class QDoubleSpinBox;
class QSpinBox;
class QComboBox;
class QPushButton;
class QButtonGroup;
class QProgressBar;
class QPlainTextEdit;
class QSplitter;

class MainWindow : public QMainWindow,
                   public fp::PVModeChangeEvent,
                   public fp::PVViewModeChangeEvent,
                   public fp::PVViewModeErrorEvent,
                   public fp::PVModelLoadedEvent,
                   public fp::PVNewModelEvent,
                   public GSStatusMessageEvent,
                   public GSContinueCalcEvent
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() = default;

    QtPaintView* paintView() const { return m_paintView; }

    // fp::PVModeChangeEvent
    void onModeChange(fp::PaintView::TEditMode oldMode, fp::PaintView::TEditMode newMode) override;

    // fp::PVViewModeChangeEvent
    void onViewModeChange(fp::PaintView::TViewMode oldMode, fp::PaintView::TViewMode newMode) override;

    // fp::PVViewModeErrorEvent
    void onViewModeError(fp::PaintView::TViewMode oldMode, fp::PaintView::TViewMode newMode) override;

    // fp::PVModelLoadedEvent
    void onModelLoaded() override;

    // fp::PVNewModelEvent
    void onNewModel() override;

    // GSStatusMessageEvent
    void onStatusMessage(const std::string &message, const int progress) override;

    // GSContinueCalcEvent
    bool onContinueCalc() override;

private Q_SLOTS:
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void editUndo();
    void editCopy();
    void editCut();
    void editPaste();
    void runCalculate();
    void runOptimise();
    void stopOptimise();
    void settingsCalc();
    void settingsGeneral();
    void helpDocumentation();
    void helpAbout();
    void toggleLogPanel(bool checked);

    // Mode tab switching
    void onModeTabChanged(int index);

    // Sketch tool buttons
    void setModeSelect();
    void setModeDrawBeam();
    void setModeRectangle();
    void setModeEllipse();
    void setModeFloodFill();
    void setModeLine();
    void setModeErase();

    // Physicks tool buttons
    void setModeForce();
    void setModePinSupport();
    void setModeRollerSupport();
    void setModeHinge();
    void setModeCut();
    void setModeEraseForcesConstraints();
    void toggleSelfLoad();

    // Action tool buttons
    void showPrincipalStress();
    void showStress();
    void showDisplacement();
    void showStructure();
    void setModeMoveLoad();
    void setModeRotateLoad();
    void toggleZoomMode();
    void expandToWindow();

    // Right panel property controls
    void onStiffnessChanged(int value);
    void onLineThicknessChanged(int value);
    void onForceMagnitudeChanged(double value);
    void onSizeChanged(int value);
    void onTransparencyChanged(int value);
    void onStepsChanged(int value);
    void onAutoScale();
    void onStressModeChanged(int mode);
    void onColorScaleChanged(int index);
    void onMisesMaxChanged(int value);
    void onDisplacementScaleChanged(int value);

    // Legacy view slots
    void setViewSketch();
    void setViewPhysics();
    void setViewAction();

private:
    void createActions();
    void createMenus();
    void createModeTabBar();
    void createCentralWidget();
    void createStatusBar();
    void applyStyleSheet();
    void updateWindowTitle();

    QWidget* createSketchToolPanel();
    QWidget* createPhysicksToolPanel();
    QWidget* createActionToolPanel();
    QWidget* createSketchPropsPanel();
    QWidget* createPhysicksPropsPanel();
    QWidget* createActionPropsPanel();
    QWidget* createPrincipalPropsWidget();
    QWidget* createMisesPropsWidget();
    QWidget* createDeflectionPropsWidget();
    void syncViewModeUi(fp::PaintView::TViewMode mode);
    void syncEditModeUi(fp::PaintView::TEditMode mode);
    void restoreActionSubmodes();
    void setVisButton(QToolButton *active);
    void setActionModeButton(QToolButton *active);

    QtPaintView *m_paintView{nullptr};

    // Mode tab bar
    QTabBar        *m_modeTabBar{nullptr};

    // Side panels
    QWidget        *m_leftPanel{nullptr};
    QWidget        *m_rightPanel{nullptr};
    QStackedWidget *m_leftStack{nullptr};
    QStackedWidget *m_rightStack{nullptr};

    // Per-mode button groups (for exclusive tool selection sync)
    QButtonGroup   *m_sketchBtnGroup{nullptr};
    QButtonGroup   *m_physicksBtnGroup{nullptr};

    // Action visualisation buttons (stored so slots can sync check state)
    QToolButton    *m_btnVisPrincipal{nullptr};
    QToolButton    *m_btnVisMises{nullptr};
    QToolButton    *m_btnVisDisp{nullptr};
    QToolButton    *m_btnVisStruct{nullptr};

    // Physics toggle buttons
    QToolButton    *m_btnSelfLoad{nullptr};

    // Action interaction mode buttons
    QToolButton    *m_btnMoveLoad{nullptr};
    QToolButton    *m_btnRotateLoad{nullptr};
    QToolButton    *m_btnZoom{nullptr};

    // File actions
    QAction *m_actNew{nullptr};
    QAction *m_actOpen{nullptr};
    QAction *m_actSave{nullptr};
    QAction *m_actSaveAs{nullptr};
    QAction *m_actQuit{nullptr};

    // Edit actions
    QAction *m_actUndo{nullptr};
    QAction *m_actCopy{nullptr};
    QAction *m_actCut{nullptr};
    QAction *m_actPaste{nullptr};

    // Run actions (keyboard shortcuts only — not in a menu)
    QAction *m_actCalculate{nullptr};
    QAction *m_actOptimise{nullptr};

    // Settings actions
    QAction *m_actSettingsCalc{nullptr};
    QAction *m_actSettingsGeneral{nullptr};

    // Help actions
    QAction *m_actHelpDocumentation{nullptr};
    QAction *m_actHelpAbout{nullptr};
    QAction *m_actShowLog{nullptr};

    // Sketch props
    QSlider        *m_stiffnessSlider{nullptr};
    QLabel         *m_stiffnessDisplay{nullptr};
    QSlider        *m_lineThicknessSlider{nullptr};
    QLabel         *m_lineThicknessDisplay{nullptr};
    fp::PaintView::TEditMode m_sketchEditMode{fp::PaintView::EM_DIRECT_BRUSH};
    fp::PaintView::TEditMode m_physicsEditMode{fp::PaintView::EM_SELECT_BOX};

    // Physicks props
    QDoubleSpinBox *m_forceMagnitudeSpinBox{nullptr};
    QComboBox      *m_forceDirectionCombo{nullptr};

    // Action props — stacked sub-panel
    QStackedWidget *m_actionPropsStack{nullptr};

    // Principal stress page
    QSlider      *m_sizeSlider{nullptr};
    QSlider      *m_transparencySlider{nullptr};
    QSlider      *m_stepsSlider{nullptr};
    QPushButton  *m_autoScaleBtn{nullptr};
    QButtonGroup *m_stressModeGroup{nullptr};

    // Mises page
    QButtonGroup *m_colorScaleGroup{nullptr};
    QSlider      *m_misesMaxSlider{nullptr};

    // Deflections page
    QSlider      *m_dispScaleSlider{nullptr};

    // Log panel
    QSplitter      *m_mainSplitter{nullptr};
    QPlainTextEdit *m_logView{nullptr};
    int             m_logPanelHeight{200};

    // Status bar labels
    QLabel *m_statusMode{nullptr};
    QLabel *m_statusModel{nullptr};

    // Optimisation progress widgets (all live in the status bar)
    QLabel       *m_optStatusLabel{nullptr};
    QProgressBar *m_optProgressBar{nullptr};
    QPushButton  *m_btnStopOpt{nullptr};

    // Optimisation stop flag (checked by onContinueCalc)
    bool m_continueCalc{true};
};
