#include <QApplication>
#include <QFileInfo>
#include <QStyleFactory>
#include <QSurfaceFormat>

#include "MainWindow.h"
#include "FPLog.h"

static QString startupModelPath(const QStringList &arguments)
{
    for (int i = 1; i < arguments.size(); ++i)
    {
        const QString &argument = arguments.at(i);
        if (argument.startsWith('-'))
            continue;

        QFileInfo fileInfo(argument);
        if (fileInfo.exists() && fileInfo.isFile())
            return fileInfo.absoluteFilePath();

        fp_warn("main", "Ignoring missing command-line model: {}", argument.toStdString());
        break;
    }

    return {};
}

int main(int argc, char *argv[])
{
    FPLog::init();
#ifdef NDEBUG
    FPLog::setLevel(spdlog::level::info);
#else
    FPLog::setLevel(spdlog::level::debug);
#endif

    // Must be set before QApplication is constructed
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    // Multisample antialiasing. This is the core-profile / WebGL-compatible
    // replacement for the old glEnable(GL_LINE_SMOOTH)/GL_POLYGON_SMOOTH, which
    // the rendering migration removed. QOpenGLWidget honours this by creating a
    // multisampled FBO and resolving it, so force/line/BC overlays stay smooth.
    fmt.setSamples(4);
#ifdef Q_OS_WASM
    // On WebAssembly the GL backend is WebGL 2, i.e. OpenGL ES 3.0. Qt cannot
    // satisfy a desktop *Core* profile request here, so asking for 3.3 Core
    // makes QOpenGLWidget fail to create a context (blank canvas). Request an
    // OpenGL ES 3.0 context instead; the renderer's shaders already emit a
    // "#version 300 es" header under a GLES context, so no shader changes are
    // needed. GLES uses NoProfile (Core/Compatibility are desktop-only).
    fmt.setRenderableType(QSurfaceFormat::OpenGLES);
    fmt.setVersion(3, 0);
    fmt.setProfile(QSurfaceFormat::NoProfile);
#else
    // 3.3 Core profile on desktop. The rendering migration removed every
    // fixed-function / immediate-mode call, so the app runs on a core profile -
    // the same feature level as WebGL 2 / OpenGL ES 3.0. (macOS supports
    // 3.3-4.1 Core.)
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
#endif
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication app(argc, argv);
    app.setApplicationName("ForcePAD Qt");
    app.setOrganizationName("Structural Mechanics LTH");

#ifdef Q_OS_WIN
    // Prefer the modern Windows 11 fluent style (qmodernwindowsstyle plugin).
    // QStyleFactory::keys() only lists styles whose plugin DLL is deployed.
    const QStringList styles = QStyleFactory::keys();
    if (styles.contains("windows11", Qt::CaseInsensitive))
        app.setStyle(QStyleFactory::create("windows11"));
    else if (styles.contains("windowsvista", Qt::CaseInsensitive))
        app.setStyle(QStyleFactory::create("windowsvista"));
    // else: Qt falls back to its built-in "Windows" style automatically
#endif

    MainWindow w;
    w.paintView()->setCommandLine(argc, argv);
    w.show();

    const QString modelPath = startupModelPath(app.arguments());
    if (!modelPath.isEmpty())
        w.paintView()->openModel(modelPath.toStdString());

    return app.exec();
}
