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
#include <QOpenGLExtraFunctions>

#include "UiSettings.h"
#include "Renderer2D.h"
#include "StreamTexture.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

// ---------------------------------------------------------------------------
// Phase 0 smoke test for the modern-GL abstraction (Renderer2D / StreamTexture).
// Enabled by setting the environment variable FORCEPAD_R2D_SMOKETEST; when set,
// paintGL() draws this test scene instead of the normal view. It exercises the
// shader program, the batched VBO path (gradient quad + lines) and the
// streaming-texture path (a checkerboard), proving the abstraction works on the
// live GL context before any real renderer is ported to it.
// ---------------------------------------------------------------------------
static void renderR2DSmokeTest(int w, int h, int pw, int ph)
{
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if (ctx == nullptr)
        return;
    QOpenGLExtraFunctions *f = ctx->extraFunctions();

    f->glViewport(0, 0, pw, ph);
    f->glDisable(GL_SCISSOR_TEST);
    f->glClearColor(0.12f, 0.12f, 0.15f, 1.0f);
    f->glClear(GL_COLOR_BUFFER_BIT);

    ivf2d::Renderer2D &r = ivf2d::Renderer2D::instance();
    r.setOrtho(0.0f, (float)w, 0.0f, (float)h);
    r.loadIdentity();

    // Gradient quad (batched VBO + flat shader, GL_QUADS emulation).
    r.beginQuads();
    r.color(0.90f, 0.20f, 0.20f); r.vertex(w * 0.08f, h * 0.10f);
    r.color(0.20f, 0.90f, 0.20f); r.vertex(w * 0.48f, h * 0.10f);
    r.color(0.20f, 0.20f, 0.90f); r.vertex(w * 0.48f, h * 0.52f);
    r.color(0.90f, 0.90f, 0.20f); r.vertex(w * 0.08f, h * 0.52f);
    r.end();

    // Streaming texture: a 16x16 RGBA checkerboard uploaded once.
    static ivf2d::StreamTexture s_checker;
    static bool s_built = false;
    if (!s_built)
    {
        const int n = 16;
        unsigned char pixels[16 * 16 * 4];
        for (int y = 0; y < n; ++y)
            for (int x = 0; x < n; ++x)
            {
                const bool on = ((x / 2) + (y / 2)) % 2 == 0;
                unsigned char *p = &pixels[(y * n + x) * 4];
                p[0] = on ? 230 : 40;
                p[1] = on ? 180 : 40;
                p[2] = on ? 60  : 90;
                p[3] = 255;
            }
        s_checker.update(pixels, n, n, GL_RGBA);
        s_built = true;
    }
    r.setBlend(false);
    s_checker.draw(w * 0.55f, h * 0.10f, w * 0.37f, h * 0.42f);

    // Batched lines.
    r.beginLines(2.0f);
    r.color(1.0f, 1.0f, 1.0f);
    r.vertex(w * 0.08f, h * 0.62f); r.vertex(w * 0.92f, h * 0.62f);
    r.vertex(w * 0.08f, h * 0.70f); r.vertex(w * 0.92f, h * 0.70f);
    r.end();
}

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

    static const bool s_smokeTest = qEnvironmentVariableIsSet("FORCEPAD_R2D_SMOKETEST");
    if (s_smokeTest)
    {
        renderR2DSmokeTest(width(), height(), physicalWidth(), physicalHeight());
        return;
    }

    // The drawing image's alpha channel encodes layer markers, not opacity.
    // Renderer2D forces opaque output for the canvas blit (setForceOpaque), so
    // no alpha bleeds into the composited FBO and the old glPixelTransfer
    // alpha-forcing hack is no longer needed.
    onClear();
    onDraw();

    // Leave the scissor test disabled so Qt's multisample-FBO resolve blit
    // (glBlitFramebuffer, which honours GL_SCISSOR_TEST) copies the whole widget
    // and not just the drawing area - otherwise the surround renders black.
    ivf2d::Renderer2D::instance().setScissorEnabled(false);
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

unsigned int QtPaintView::ensureCaptureFramebuffer(int width, int height)
{
    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();
    if (width <= 0 || height <= 0)
        return 0;

    if (m_captureFbo != 0 && (width != m_captureW || height != m_captureH))
    {
        f->glDeleteFramebuffers(1, &m_captureFbo);
        f->glDeleteTextures(1, &m_captureTex);
        m_captureFbo = 0;
        m_captureTex = 0;
    }

    if (m_captureFbo == 0)
    {
        f->glGenTextures(1, &m_captureTex);
        f->glBindTexture(GL_TEXTURE_2D, m_captureTex);
        f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        f->glBindTexture(GL_TEXTURE_2D, 0);

        f->glGenFramebuffers(1, &m_captureFbo);
        f->glBindFramebuffer(GL_FRAMEBUFFER, m_captureFbo);
        f->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_captureTex, 0);
        f->glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

        m_captureW = width;
        m_captureH = height;
    }
    return m_captureFbo;
}

void QtPaintView::mouseReleaseEvent(QMouseEvent *event)
{
    int x = event->pos().x();
    int y = event->pos().y();

    if (event->button() == Qt::LeftButton)
    {
        if (!m_danglingRelease)
        {
            // Ensure the GL context is current before committing geometry.  Qt
            // does NOT make the context current in mouse-event handlers.
            makeCurrent();
            QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();

            // The shape (line/rect/ellipse/arch) is committed to the drawing
            // image by rendering it and reading it back with glReadPixels
            // (onRelease). glReadPixels is invalid on the widget's *multisample*
            // FBO, so render this capture pass into a single-sample FBO instead
            // (MSAA is irrelevant for the captured pixel raster). Fall back to
            // the widget FBO if the capture FBO can't be created.
            unsigned int captureFbo = ensureCaptureFramebuffer(physicalWidth(), physicalHeight());
            f->glBindFramebuffer(GL_FRAMEBUFFER, captureFbo != 0 ? captureFbo : defaultFramebufferObject());

            onClear();
            onDraw();   // m_leftMouseDown still true → line/rect/ellipse rendered
            f->glFinish(); // ensure GPU work is complete before glReadPixels
            m_leftMouseDown = false;
            onRelease(x, y);

            f->glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
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
