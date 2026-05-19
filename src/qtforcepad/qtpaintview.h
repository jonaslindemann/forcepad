#pragma once

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

#include "PaintView.h"

class InfoOverlay;

class QtPaintView : public QOpenGLWidget, public fp::PaintView {
    Q_OBJECT
public:
    explicit QtPaintView(QWidget *parent = nullptr);
    virtual ~QtPaintView();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void reinitGL();  // Re-runs onInitContext() after canvas resize, then repaints

    void showInfoOverlay(const QString &msg, int msec = 4000);

Q_SIGNALS:
    void editModeChanged(fp::PaintView::TEditMode newMode);
    void viewModeChanged(fp::PaintView::TViewMode newMode);
    void modelLoaded();
    void newModelCreated();
    void infoMessage(const QString &msg);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    // fp::PaintView virtual UI bridge
    int height() override;
    int width() override;
    int physicalWidth() override;
    int physicalHeight() override;
    float doDevicePixelRatio() override;
    void doRedraw() override;
    void doFlush() override;
    void doInvalidate() override;
    void doMakeCurrent() override;
    const std::string doSaveDialog(const std::string title, const std::string filter,
                                   const std::string defaultFilename) override;
    bool doNewModel(int &w, int &h, int &initialStiffness) override;
    void doInfoMessage(const std::string message) override;
    bool doAskYesNo(const std::string question) override;
    const std::string doOpenDialog(const std::string title, const std::string filter) override;
    void doCreateCursors() override;
    void doUpdateCursor(TEditMode mode) override;
    void doDeleteCursors() override;
    void doShowAbout() override;
    void doShowHelp() override;

private:
    InfoOverlay *m_infoOverlay{nullptr};
};
