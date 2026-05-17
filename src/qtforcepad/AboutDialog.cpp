#include "AboutDialog.h"
#include "forcepad_config.h"

#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QString>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("About ForcePAD");
    setModal(true);
    setFixedSize(402, 340);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Splash image
    auto *splashLabel = new QLabel;
    QPixmap splash(":/splash.png");
    if (!splash.isNull())
        splashLabel->setPixmap(splash);
    else
        splashLabel->setText("ForcePAD");
    splashLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(splashLabel);

    // Version info overlaid below image
    auto *infoArea = new QWidget;
    infoArea->setStyleSheet("background: #1a1a1a;");
    auto *infoLayout = new QVBoxLayout(infoArea);
    infoLayout->setContentsMargins(12, 8, 12, 8);
    infoLayout->setSpacing(4);

    QString versionText = FORCEPAD_VERSION_STRING;
    versionText += QString("\nCompiled: %1 %2")
                       .arg(__TIME__)
                       .arg(__DATE__);

    auto *versionLabel = new QLabel(versionText);
    versionLabel->setStyleSheet("color: #d4d4d4; font-size: 11px;");
    versionLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    infoLayout->addWidget(versionLabel);

    auto *btnRow = new QHBoxLayout;
    btnRow->addStretch();
    auto *closeBtn = new QPushButton("Close");
    closeBtn->setFixedWidth(80);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnRow->addWidget(closeBtn);
    infoLayout->addLayout(btnRow);

    mainLayout->addWidget(infoArea);
}
