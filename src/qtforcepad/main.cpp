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
    fmt.setVersion(2, 1);
    fmt.setProfile(QSurfaceFormat::CompatibilityProfile);
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
