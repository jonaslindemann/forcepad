#include "AboutDialog.h"
#include "forcepad_config.h"

#include <QCoreApplication>
#include <QFont>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QString>
#include <QStringList>

namespace {

QPixmap loadAboutLogo()
{
    const QStringList paths = {
        QCoreApplication::applicationDirPath() + "/icons/logo_1080.png",
        "icons/logo_1080.png",
        ":/icons/logo_1080.png",
        "images/logo_1080.png"
    };

    for (const QString &path : paths)
    {
        QPixmap logo(path);
        if (!logo.isNull())
            return logo;
    }

    return {};
}

}

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("About ForcePAD");
    setModal(true);
    setFixedSize(402, 302);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setStyleSheet(
        "QDialog { background: #000000; }"
        "QLabel { color: #f5f5f5; }"
        "QPushButton {"
        "  background: #1f1f1f;"
        "  color: #ffffff;"
        "  border: 1px solid #6b6b6b;"
        "  border-radius: 3px;"
        "  padding: 3px 8px;"
        "}"
        "QPushButton:hover { background: #2c2c2c; }"
        "QPushButton:pressed { background: #111111; }");

    auto *logoLabel = new QLabel(this);
    logoLabel->setGeometry(5, 10, 112, 112);
    logoLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    QPixmap logo = loadAboutLogo();
    if (!logo.isNull())
        logoLabel->setPixmap(logo.scaled(112, 112, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    else
        logoLabel->setText("ForcePAD");

    auto *creditsLabel = new QLabel(
        "Copyright (C) 2001-2026\n"
        "Division of Structural Mechanics\n"
        "Lund University\n\n"
        "Developed in cooperation with\n"
        "The School of Architecture\n"
        "Chalmers University of Technology",
        this);
    creditsLabel->setGeometry(180, 12, 206, 112);
    creditsLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    creditsLabel->setStyleSheet("color: #f5f5f5; font-size: 10px;");

    auto *titleLabel = new QLabel("ForcePAD", this);
    titleLabel->setGeometry(178, 238, 210, 48);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(34);
    titleFont.setWeight(QFont::Normal);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    titleLabel->setStyleSheet("color: #f5f5f5;");

    QString versionText = FORCEPAD_VERSION_STRING;
    versionText += QString("\nCompiled: %1 %2")
                       .arg(__TIME__)
                       .arg(__DATE__);

    auto *versionLabel = new QLabel(versionText, this);
    versionLabel->setGeometry(15, 143, 235, 52);
    versionLabel->setStyleSheet("color: #d4d4d4; font-size: 11px;");
    versionLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto *closeBtn = new QPushButton("Close", this);
    closeBtn->setGeometry(15, 265, 55, 25);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}
