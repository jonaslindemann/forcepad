#pragma once

#include <QMainWindow>
#include "QtPaintView.h"

class QAction;
class QToolBar;
class QLabel;

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
    void runCalculate();
    void runOptimise();
    void setModeBrush();
    void setModeErase();
    void setModeForce();
    void setModeConstraint();
    void setModeConstraintVector();
    void setModeConstraintHinge();
    void setModeFloodFill();
    void setModeRectangle();
    void setModeEllipse();
    void setModeLine();
    void setViewSketch();
    void setViewPhysics();
    void setViewAction();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void updateWindowTitle();

    QtPaintView *m_paintView;

    // File actions
    QAction *m_actNew;
    QAction *m_actOpen;
    QAction *m_actSave;
    QAction *m_actSaveAs;
    QAction *m_actQuit;

    // Edit actions
    QAction *m_actUndo;

    // Run actions
    QAction *m_actCalculate;
    QAction *m_actOptimise;

    // Mode actions
    QAction *m_actBrush;
    QAction *m_actErase;
    QAction *m_actForce;
    QAction *m_actConstraint;
    QAction *m_actConstraintVector;
    QAction *m_actConstraintHinge;
    QAction *m_actFloodFill;
    QAction *m_actRectangle;
    QAction *m_actEllipse;
    QAction *m_actLine;

    // View actions
    QAction *m_actViewSketch;
    QAction *m_actViewPhysics;
    QAction *m_actViewAction;

    // Status bar labels
    QLabel *m_statusMode;
    QLabel *m_statusModel;
};
