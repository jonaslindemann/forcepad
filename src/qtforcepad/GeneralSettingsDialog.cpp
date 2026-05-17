#include "GeneralSettingsDialog.h"
#include "QtPaintView.h"

#include <QSlider>
#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>

GeneralSettingsDialog::GeneralSettingsDialog(QtPaintView *view, QWidget *parent)
    : QDialog(parent), m_view(view)
{
    setWindowTitle("General Settings");
    setModal(true);
    setMinimumWidth(360);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(16, 16, 16, 16);

    auto makeSliderRow = [&](const QString &label, QSlider *&slider, QLabel *&display,
                              int min, int max, int step) {
        auto *group = new QVBoxLayout;
        auto *row   = new QHBoxLayout;
        row->addWidget(new QLabel(label));
        row->addStretch();
        display = new QLabel;
        display->setFixedWidth(36);
        display->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        row->addWidget(display);
        slider = new QSlider(Qt::Horizontal);
        slider->setRange(min, max);
        slider->setSingleStep(step);
        slider->setPageStep(step * 5);
        slider->setTickPosition(QSlider::TicksBelow);
        group->addLayout(row);
        group->addWidget(slider);
        return group;
    };

    auto *thicknessGroup = makeSliderRow("Line thickness (pixels):",
                                          m_lineThickness, m_lineThicknessDisplay,
                                          1, 20, 1);
    mainLayout->addLayout(thicknessGroup);

    auto *symbolGroup = makeSliderRow("Symbol length (pixels):",
                                       m_symbolLength, m_symbolLengthDisplay,
                                       10, 300, 5);
    mainLayout->addLayout(symbolGroup);

    mainLayout->addStretch();

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &GeneralSettingsDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);

    connect(m_lineThickness, &QSlider::valueChanged, this, &GeneralSettingsDialog::onLineThicknessChanged);
    connect(m_symbolLength,  &QSlider::valueChanged, this, &GeneralSettingsDialog::onSymbolLengthChanged);

    loadFromView();
}

void GeneralSettingsDialog::onLineThicknessChanged(int value)
{
    m_lineThicknessDisplay->setText(QString::number(value));
    if (m_view)
        m_view->setUiLineThickness(value);
}

void GeneralSettingsDialog::onSymbolLengthChanged(int value)
{
    m_symbolLengthDisplay->setText(QString::number(value));
    if (m_view)
        m_view->setUiSymbolLength(value);
}

void GeneralSettingsDialog::loadFromView()
{
    if (!m_view) return;
    const int lt = static_cast<int>(m_view->getUiLineThickness());
    const int sl = static_cast<int>(m_view->getUiSymbolLength());
    m_lineThickness->setValue(lt);
    m_symbolLength->setValue(sl);
    m_lineThicknessDisplay->setText(QString::number(lt));
    m_symbolLengthDisplay->setText(QString::number(sl));
}

void GeneralSettingsDialog::saveToView()
{
    if (!m_view) return;
    m_view->setUiLineThickness(m_lineThickness->value());
    m_view->setUiSymbolLength(m_symbolLength->value());
}

void GeneralSettingsDialog::accept()
{
    saveToView();
    QDialog::accept();
}
