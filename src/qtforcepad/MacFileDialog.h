#pragma once

#include <QByteArray>
#include <QString>

class QWidget;

namespace fp::macos {

struct PickedFile
{
    QString path;
    QString displayName;
    QByteArray content;
    QByteArray bookmark;
    QString error;
    bool cancelled{false};
};

struct SavedFile
{
    QString path;
    QByteArray bookmark;
    QString error;
    bool cancelled{false};
};

PickedFile pickFile(QWidget *parent, const QString &title, const QString &nameFilter);
SavedFile saveFile(QWidget *parent, const QString &title, const QString &defaultName,
                   const QString &nameFilter, const QByteArray &content);
bool writeFile(const QString &path, const QByteArray &bookmark,
               const QByteArray &content, QString *errorMessage);

} // namespace fp::macos
