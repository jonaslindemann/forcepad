#include <QApplication>
#include <QSurfaceFormat>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setStencilBufferSize(8);
    fmt.setVersion(2, 1);
    fmt.setProfile(QSurfaceFormat::CompatibilityProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication app(argc, argv);
    app.setApplicationName("ForcePAD Qt");
    app.setOrganizationName("Structural Mechanics LTH");

    MainWindow w;
    w.show();

    return app.exec();
}
