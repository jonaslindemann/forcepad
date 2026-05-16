#pragma once

#include <QMainWindow>
#include "QtPaintView.h"

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

class MainWindow : public QMainWindow,
                   public CPVModeChangeEvent,
                   public CPVViewModeChangeEvent,
                   public CPVModelLoadedEvent,
                   public CPVNewModelEvent
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() = default;

    QtPaintView* paintView() const { return m_paintView; }

    // CPVModeChangeEvent
    void onModeChange(CPaintView::TEditMode oldMode, CPaintView::TEditMode newMode) override;

    // CPVViewModeChangeEvent
    void onViewModeChange(CPaintView::TViewMode oldMode, CPaintView::TViewMode newMode) override;

    // CPVModelLoadedEvent
    void onModelLoaded() override;

    // CPVNewModelEvent
    void onNewModel() override;

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

    // Action tool buttons
    void showStress();
    void showDisplacement();
    void showStructure();

    // Right panel property controls
    void onStiffnessChanged(int value);
    void onThicknessChanged(double value);
    void onForceMagnitudeChanged(double value);
    void onSizeChanged(int value);
    void onTransparencyChanged(int value);
    void onStepsChanged(int value);
    void onAutoScale();

    // Legacy view slots (kept for View menu)
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

    // Run actions
    QAction *m_actCalculate{nullptr};
    QAction *m_actOptimise{nullptr};

    // View actions (menu only)
    QAction *m_actViewSketch{nullptr};
    QAction *m_actViewPhysics{nullptr};
    QAction *m_actViewAction{nullptr};

    // Sketch props
    QSlider        *m_stiffnessSlider{nullptr};
    QLabel         *m_stiffnessDisplay{nullptr};
    QDoubleSpinBox *m_thicknessSpinBox{nullptr};

    // Physicks props
    QDoubleSpinBox *m_forceMagnitudeSpinBox{nullptr};
    QComboBox      *m_forceDirectionCombo{nullptr};

    // Action props
    QSlider     *m_sizeSlider{nullptr};
    QSlider     *m_transparencySlider{nullptr};
    QSpinBox    *m_stepsSpinBox{nullptr};
    QPushButton *m_autoScaleBtn{nullptr};

    // Status bar labels
    QLabel *m_statusMode{nullptr};
    QLabel *m_statusModel{nullptr};
};
