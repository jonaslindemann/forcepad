#ifndef QTPAINTVIEW_H
#define QTPAINTVIEW_H


#include <QGLWidget>
#include <QMouseEvent>

#include "PaintView.h"

class QtPaintView : public QGLWidget, public CPaintView
{
    Q_OBJECT
public:
    explicit QtPaintView(QWidget *parent = 0);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QPoint lastPos;

    virtual int height();
    virtual int width();
    virtual void doRedraw();
    virtual void doFlush();
    virtual void doInvalidate();
    virtual void doMakeCurrent();
    virtual const std::string doSaveDialog(const string title, const string filter, const string defaultFilename);
    virtual bool doNewModel(int &width, int &height, int& initialStiffness);
    virtual void doInfoMessage(const string message);
    virtual bool doAskYesNo(const string question);
    virtual const std::string doOpenDialog(const string title, const string filter);

Q_SIGNALS:

public Q_SLOTS:

};

#endif // QTPAINTVIEW_H
