#include "QtPaintView.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QLabel>
#include <QUrl>
#include <QDesktopServices>
#include <QApplication>

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

QtPaintView::QtPaintView(QWidget *parent)
    : QOpenGLWidget(parent), CPaintView(0, 0, 800, 600, "ForcePAD Qt")
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    doCreateCursors();
}

QtPaintView::~QtPaintView()
{
    doDeleteCursors();
}

QSize QtPaintView::minimumSizeHint() const
{
    return QSize(400, 300);
}

QSize QtPaintView::sizeHint() const
{
    return QSize(800, 600);
}

// ---------------------------------------------------------------------------
// QOpenGLWidget overrides
// ---------------------------------------------------------------------------

void QtPaintView::initializeGL()
{
    onInitContext();
}

void QtPaintView::paintGL()
{
    onClear();

    // m_drawing layer 0 stores alpha as a layer marker (0 = background,
    // 128 = undo region), not as opacity.  Qt composites the QOpenGLWidget
    // FBO over the window background using the FBO alpha channel, so any
    // pixel with alpha < 255 would bleed through and look gray.
    // glPixelTransfer forces every glDrawPixels call inside onDraw() to
    // emit alpha=1.0 regardless of the stored value.
    glPixelTransferf(GL_ALPHA_SCALE, 0.0f);
    glPixelTransferf(GL_ALPHA_BIAS,  1.0f);

    onDraw();

    glPixelTransferf(GL_ALPHA_SCALE, 1.0f);
    glPixelTransferf(GL_ALPHA_BIAS,  0.0f);
}

void QtPaintView::resizeGL(int w, int h)
{
    onInitContext();  // Recalculates drawing offset and scissor for the actual widget size
}

void QtPaintView::mousePressEvent(QMouseEvent *event)
{
    int x = event->pos().x();
    int y = event->pos().y();

    if (event->modifiers() & Qt::ShiftModifier)
    {
        int sx = x - m_drawingOffsetX;
        int sy = y - m_drawingOffsetY;
        m_femGrid->snapToGrid(sx, sy);
        x = sx + m_drawingOffsetX;
        y = sy + m_drawingOffsetY;
    }
    else if (m_snapToGrid)
    {
        int sx = x - m_drawingOffsetX;
        int sy = y - m_drawingOffsetY;
        m_femGrid->snapToGrid(sx, sy);
        x = sx + m_drawingOffsetX;
        y = sy + m_drawingOffsetY;
    }

    if (event->button() == Qt::LeftButton)
    {
        m_leftMouseDown = true;
        onPush(x, y);
    }
}

void QtPaintView::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->pos().x();
    int y = event->pos().y();

    if (event->modifiers() & Qt::ShiftModifier)
    {
        int sx = x - m_drawingOffsetX;
        int sy = y - m_drawingOffsetY;
        m_femGrid->snapToGrid(sx, sy);
        x = sx + m_drawingOffsetX;
        y = sy + m_drawingOffsetY;
    }
    else if (m_snapToGrid)
    {
        int sx = x - m_drawingOffsetX;
        int sy = y - m_drawingOffsetY;
        m_femGrid->snapToGrid(sx, sy);
        x = sx + m_drawingOffsetX;
        y = sy + m_drawingOffsetY;
    }

    if (m_leftMouseDown)
        onDrag(x, y);
    else
        onMove(x, y);
}

void QtPaintView::mouseReleaseEvent(QMouseEvent *event)
{
    int x = event->pos().x();
    int y = event->pos().y();

    if (event->button() == Qt::LeftButton)
    {
        if (!m_danglingRelease)
        {
            // Ensure the GL context is current and the FBO holds the final
            // draw state before onRelease() calls glReadPixels to commit the
            // geometry to the canvas image.  Qt does NOT make the context
            // current in mouse-event handlers, so we must do it explicitly.
            makeCurrent();
            onClear();
            onDraw();   // m_leftMouseDown still true → line/rect/ellipse rendered
            m_leftMouseDown = false;
            onRelease(x, y);
        }
        else
        {
            m_leftMouseDown = false;
            m_danglingRelease = false;
        }
        update();
    }
}

void QtPaintView::wheelEvent(QWheelEvent *event)
{
    QPoint delta = event->angleDelta();
    int dy = delta.y() / 120;
    int dx = delta.x() / 120;
    onMouseWheel(dx, -dy);
    event->accept();
}

void QtPaintView::keyPressEvent(QKeyEvent *event)
{
    QOpenGLWidget::keyPressEvent(event);
}

// ---------------------------------------------------------------------------
// CPaintView virtual UI bridge implementations
// ---------------------------------------------------------------------------

int QtPaintView::height()
{
    return QOpenGLWidget::height();
}

int QtPaintView::width()
{
    return QOpenGLWidget::width();
}

void QtPaintView::doRedraw()
{
    update();
}

void QtPaintView::doFlush()
{
    update();
}

void QtPaintView::doInvalidate()
{
    update();
}

void QtPaintView::doMakeCurrent()
{
    makeCurrent();
}

const std::string QtPaintView::doSaveDialog(const std::string title, const std::string filter,
                                             const std::string defaultFilename)
{
    QString qfilter = QString::fromStdString(filter);
    if (qfilter.isEmpty())
        qfilter = "ForcePAD Files (*.fp2);;All Files (*)";

    QString fname = QFileDialog::getSaveFileName(
        this,
        QString::fromStdString(title),
        QString::fromStdString(defaultFilename),
        qfilter
    );
    return fname.toStdString();
}

bool QtPaintView::doNewModel(int &w, int &h, int &initialStiffness)
{
    QDialog dlg(this);
    dlg.setWindowTitle("New Model");

    QFormLayout *form = new QFormLayout(&dlg);

    QSpinBox *wSpin = new QSpinBox(&dlg);
    wSpin->setRange(64, 4096);
    wSpin->setValue(640);
    form->addRow("Width:", wSpin);

    QSpinBox *hSpin = new QSpinBox(&dlg);
    hSpin->setRange(64, 4096);
    hSpin->setValue(480);
    form->addRow("Height:", hSpin);

    QSpinBox *stiffSpin = new QSpinBox(&dlg);
    stiffSpin->setRange(1, 255);
    stiffSpin->setValue(128);
    form->addRow("Initial stiffness:", stiffSpin);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form->addRow(buttons);

    QObject::connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted)
    {
        w = wSpin->value();
        h = hSpin->value();
        initialStiffness = stiffSpin->value();
        return true;
    }
    return false;
}

void QtPaintView::doInfoMessage(const std::string message)
{
    QMessageBox::information(this, "ForcePAD", QString::fromStdString(message));
}

bool QtPaintView::doAskYesNo(const std::string question)
{
    return QMessageBox::question(this, "ForcePAD", QString::fromStdString(question))
           == QMessageBox::Yes;
}

const std::string QtPaintView::doOpenDialog(const std::string title, const std::string filter)
{
    QString qfilter = QString::fromStdString(filter);
    if (qfilter.isEmpty())
        qfilter = "ForcePAD Files (*.fp2);;All Files (*)";

    QString fname = QFileDialog::getOpenFileName(
        this,
        QString::fromStdString(title),
        QString(),
        qfilter
    );
    return fname.toStdString();
}

void QtPaintView::doCreateCursors()
{
    // Qt uses standard cursors; custom cursor shapes can be added later
}

void QtPaintView::doUpdateCursor(TEditMode mode)
{
    switch (mode)
    {
        case EM_FORCE:
            setCursor(Qt::CrossCursor);
            break;
        case EM_CONSTRAINT:
        case EM_CONSTRAINT_VECTOR:
        case EM_CONSTRAINT_HINGE:
            setCursor(Qt::CrossCursor);
            break;
        case EM_ERASE:
        case EM_DIRECT_ERASE:
        case EM_ERASE_CONSTRAINTS_FORCES:
            setCursor(Qt::ForbiddenCursor);
            break;
        case EM_SELECT_BOX:
            setCursor(Qt::CrossCursor);
            break;
        default:
            setCursor(Qt::ArrowCursor);
            break;
    }
}

void QtPaintView::doDeleteCursors()
{
    // Nothing to clean up for Qt standard cursors
}

void QtPaintView::doShowAbout()
{
    QDesktopServices::openUrl(QUrl("https://structarch.org/forcepad/documentation/"));
}

void QtPaintView::doShowHelp()
{
    QDesktopServices::openUrl(QUrl("https://structarch.org/forcepad/documentation/"));
}
