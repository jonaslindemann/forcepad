#include "QtPaintView.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QTimer>
#include <QFontMetrics>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QLabel>
#include <QUrl>
#include <QDesktopServices>
#include <QApplication>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include "UiSettings.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

// ---------------------------------------------------------------------------
// Semi-transparent toast overlay shown on top of the GL canvas
// ---------------------------------------------------------------------------
class InfoOverlay : public QWidget
{
    QString  m_text;
    QTimer  *m_timer;

    static constexpr int kHPad   = 24;
    static constexpr int kVPad   = 12;
    static constexpr int kRadius = 10;
    static constexpr int kMargin = 28;
    static constexpr int kMaxW   = 440;

public:
    explicit InfoOverlay(QWidget *parent)
        : QWidget(parent), m_timer(new QTimer(this))
    {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_TranslucentBackground);
        setVisible(false);
        m_timer->setSingleShot(true);
        connect(m_timer, &QTimer::timeout, this, &QWidget::hide);
    }

    void showMessage(const QString &text, int msec)
    {
        m_text = text;
        reposition();
        setVisible(true);
        raise();
        update();
        m_timer->start(msec);
    }

    void reposition()
    {
        if (!parentWidget()) return;
        QFontMetrics fm(font());
        QRect tb = fm.boundingRect(QRect(0, 0, kMaxW - 2 * kHPad, 2000),
                                   Qt::TextWordWrap | Qt::AlignCenter, m_text);
        int w = tb.width()  + 2 * kHPad;
        int h = tb.height() + 2 * kVPad;
        int x = (parentWidget()->width()  - w) / 2;
        int y =  parentWidget()->height() - h - kMargin;
        setGeometry(x, y, w, h);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(20, 20, 20, 210));
        p.drawRoundedRect(rect(), kRadius, kRadius);
        p.setPen(QColor(255, 255, 255, 230));
        p.drawText(rect().adjusted(kHPad, kVPad, -kHPad, -kVPad),
                   Qt::TextWordWrap | Qt::AlignCenter, m_text);
    }
};

// ---------------------------------------------------------------------------

QtPaintView::QtPaintView(QWidget *parent)
    : QOpenGLWidget(parent), fp::PaintView(0, 0, 800, 600, "ForcePAD Qt")
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    doCreateCursors();
    m_infoOverlay = new InfoOverlay(this);
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
    fp::UiSettings::getInstance()->setDevicePixelRatio(doDevicePixelRatio());
    onInitContext();
}

void QtPaintView::paintGL()
{
    fp::UiSettings::getInstance()->setDevicePixelRatio(doDevicePixelRatio());

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
    fp::UiSettings::getInstance()->setDevicePixelRatio(doDevicePixelRatio());
    onInitContext();  // Recalculates drawing offset and scissor for the actual widget size
    if (m_infoOverlay && m_infoOverlay->isVisible())
        m_infoOverlay->reposition();
}

void QtPaintView::reinitGL()
{
    makeCurrent();
    fp::UiSettings::getInstance()->setDevicePixelRatio(doDevicePixelRatio());
    onInitContext();
    update();
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
            // Qt 6 does NOT automatically re-bind the widget FBO when
            // makeCurrent() is called outside of paintGL().  Without this
            // explicit bind, onClear()/onDraw() render to framebuffer 0
            // (meaningless in an offscreen-surface context) and glReadPixels
            // inside onRelease() reads back zeros instead of the drawn shape.
            QOpenGLContext::currentContext()->functions()->glBindFramebuffer(
                GL_FRAMEBUFFER, defaultFramebufferObject());
            onClear();
            // Mirror the glPixelTransfer state from paintGL() so glDrawPixels
            // inside onDraw() emits alpha=1.0 into the FBO.
            glPixelTransferf(GL_ALPHA_SCALE, 0.0f);
            glPixelTransferf(GL_ALPHA_BIAS,  1.0f);
            onDraw();   // m_leftMouseDown still true → line/rect/ellipse rendered
            glPixelTransferf(GL_ALPHA_SCALE, 1.0f);
            glPixelTransferf(GL_ALPHA_BIAS,  0.0f);
            glFinish(); // ensure GPU rendering is complete before glReadPixels
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
// fp::PaintView virtual UI bridge implementations
// ---------------------------------------------------------------------------

int QtPaintView::height()
{
    return QOpenGLWidget::height();
}

int QtPaintView::width()
{
    return QOpenGLWidget::width();
}

int QtPaintView::physicalWidth()
{
    return qRound(QOpenGLWidget::width() * devicePixelRatioF());
}

int QtPaintView::physicalHeight()
{
    return qRound(QOpenGLWidget::height() * devicePixelRatioF());
}

float QtPaintView::doDevicePixelRatio()
{
    return (float)devicePixelRatioF();
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
    stiffSpin->setRange(0, 255);
    stiffSpin->setValue(0);
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
    showInfoOverlay(QString::fromStdString(message));
    Q_EMIT infoMessage(QString::fromStdString(message));
}

void QtPaintView::showInfoOverlay(const QString &msg, int msec)
{
    m_infoOverlay->showMessage(msg, msec);
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
    QDesktopServices::openUrl(QUrl("https://jonaslindemann.github.io/forcepad/"));
}

void QtPaintView::doShowHelp()
{
    QDesktopServices::openUrl(QUrl("https://jonaslindemann.github.io/forcepad/"));
}
