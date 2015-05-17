#include "qtpaintview.h"

#include <QFileDialog>
#include <QApplication>
#include <QtDebug>

QtPaintView::QtPaintView(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent), CPaintView()
{
    this->setCommandLine(QApplication::arguments());
    QApplication::instance()->installEventFilter(this);
}

void QtPaintView::initializeGL()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    onInitContext();
}

void QtPaintView::resizeGL(int width, int height)
{
    glViewport(0,0,width*this->devicePixelRatio(),height*this->devicePixelRatio());
    onInitContext();
}

void QtPaintView::paintGL()
{
    // Clear screen

    if (m_runOnce)
    {
        m_runOnce = false;
    }

    onClear();
    glPushMatrix();
    onDraw();
    glPopMatrix();
}

int QtPaintView::height()
{
    return QGLWidget::height()*this->devicePixelRatio();
}

int QtPaintView::width()
{
    return QGLWidget::width()*this->devicePixelRatio();
}

void QtPaintView::doRedraw()
{
    this->repaint();
}

void QtPaintView::doFlush()
{
    this->repaint();
}

void QtPaintView::doInvalidate()
{
    this->repaint();
}

void QtPaintView::doMakeCurrent()
{
    this->makeCurrent();
}

const std::string QtPaintView::doSaveDialog(const string title, const string filter, const string defaultFilename)
{
    return "";
}

bool QtPaintView::doNewModel(int &width, int &height, int& initialStiffness)
{
    return false;
}

void QtPaintView::doInfoMessage(const string message)
{

}

bool QtPaintView::doAskYesNo(const string question)
{
    return false;
}

const std::string QtPaintView::doOpenDialog(const string title, const string filter)
{
    QString filename = QFileDialog::getOpenFileName(this, QString::fromStdString(title), "", "*.fp2");
    return filename.toStdString();
}

QSize QtPaintView::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize QtPaintView::sizeHint() const
{
    return QSize(400, 400);
}

/*
//
// Overridden FLTK handle method for capturing
// events.
//

int x = Fl::event_x();
int y = Fl::event_y();

int sx, sy;

// If the SHIFT button is down coordinates are snapped
// to a grid

if ((Fl::event_state()&FL_SHIFT)>0)
{
    sx = x - m_drawingOffsetX;
    sy = y - m_drawingOffsetY;
    m_femGrid->snapToGrid(sx, sy);
    x = sx + m_drawingOffsetX;
    y = sy + m_drawingOffsetY;
}
else
{
    if (m_snapToGrid)
    {
        sx = x - m_drawingOffsetX;
        sy = y - m_drawingOffsetY;
        m_femGrid->snapToGrid(sx, sy);
        x = sx + m_drawingOffsetX;
        y = sy + m_drawingOffsetY;
    }
}

switch (event) {
case FL_PUSH:
    m_leftMouseDown = true;
    onPush(x, y);
    return 1;
    break;
case FL_DRAG:
    onDrag(x, y);
    return 1;
case FL_RELEASE:
    if (!m_danglingRelease)
        onRelease(x, y);
    else
        m_danglingRelease = false;
    return 1;
    break;
case FL_MOVE:
    onMove(x, y);
    return 1;
    break;
case FL_ENTER:
    updateCursor();
    return 1;
    break;
case FL_LEAVE:
#ifndef __APPLE__
    fl_cursor( FL_CURSOR_DEFAULT );
#endif
    return 1;
    break;
case FL_MOUSEWHEEL:
    onMouseWheel(Fl::event_dx(), Fl::event_dy());
    return 1;
    break;
default:
    return Fl_Gl_Window::handle(event);
}
return Fl_Gl_Window::handle(event);
*/

bool QtPaintView::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        if (mouseEvent->buttons() & Qt::LeftButton)
            CPaintView::onDrag(mouseEvent->x()*this->devicePixelRatio(), mouseEvent->y()*this->devicePixelRatio());
        else
            CPaintView::onMove(mouseEvent->x()*this->devicePixelRatio(), mouseEvent->y()*this->devicePixelRatio());

        //CPaintView::onMove(mouseEvent->x()*this->devicePixelRatio(), mouseEvent->y()*this->devicePixelRatio());
    }
    return false;
}

void QtPaintView::mousePressEvent(QMouseEvent *event)
{
    CPaintView::m_leftMouseDown = true;
    this->onPush(event->x()*this->devicePixelRatio(), event->y()*this->devicePixelRatio());
}

void QtPaintView::mouseReleaseEvent(QMouseEvent *event)
{
    if (!CPaintView::m_danglingRelease)
        CPaintView::onRelease(event->x()*this->devicePixelRatio(), event->y()*this->devicePixelRatio());
    else
        m_danglingRelease = false;
}

void QtPaintView::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "mouseMoveEvent()";
    /*
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {

    }
    else if (event->buttons() & Qt::RightButton)
    {

    }
    lastPos = event->pos();
    */
}
