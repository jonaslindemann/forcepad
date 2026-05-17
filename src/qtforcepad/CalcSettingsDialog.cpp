#include "CalcSettingsDialog.h"
#include "QtPaintView.h"

#include <QTabWidget>
#include <QSlider>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>

CalcSettingsDialog::CalcSettingsDialog(QtPaintView *view, QWidget *parent)
    : QDialog(parent), m_view(view)
{
    setWindowTitle("Calculation Settings");
    setModal(true);
    setMinimumWidth(380);

    auto *mainLayout = new QVBoxLayout(this);
    auto *tabs = new QTabWidget;

    // --- Mesh tab ---
    auto *meshWidget = new QWidget;
    auto *meshLayout = new QFormLayout(meshWidget);
    meshLayout->setContentsMargins(12, 12, 12, 12);
    meshLayout->setSpacing(10);

    auto *sliderRow = new QWidget;
    auto *sliderH   = new QHBoxLayout(sliderRow);
    sliderH->setContentsMargins(0, 0, 0, 0);
    m_gridStep = new QSlider(Qt::Horizontal);
    m_gridStep->setRange(2, 16);
    m_gridStep->setSingleStep(1);
    m_gridStep->setTickPosition(QSlider::TicksBelow);
    m_gridStep->setTickInterval(1);
    sliderH->addWidget(new QLabel("Fine"));
    sliderH->addWidget(m_gridStep, 1);
    sliderH->addWidget(new QLabel("Coarse"));

    meshLayout->addRow("Grid step:", sliderRow);
    m_warnOnLarge = new QCheckBox("Warn on large models");
    meshLayout->addRow(m_warnOnLarge);
    tabs->addTab(meshWidget, "Mesh");

    // --- General tab ---
    auto *genWidget = new QWidget;
    auto *genLayout = new QFormLayout(genWidget);
    genLayout->setContentsMargins(12, 12, 12, 12);
    genLayout->setSpacing(8);

    auto makeSpinBox = [](double min, double max, int dec) {
        auto *sb = new QDoubleSpinBox;
        sb->setRange(min, max);
        sb->setDecimals(dec);
        sb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        return sb;
    };

    m_elasticModulus       = makeSpinBox(0, 1e12, 2);
    m_stiffnessScaleFactor = makeSpinBox(0, 1e12, 6);
    m_youngsModulus        = makeSpinBox(0, 1e12, 2);
    m_thickness            = makeSpinBox(0, 1e6,  4);
    m_elementThreshold     = makeSpinBox(0, 1,    6);

    genLayout->addRow("Elastic modulus:",       m_elasticModulus);
    genLayout->addRow("Stiffness scale factor:", m_stiffnessScaleFactor);
    genLayout->addRow("Young's modulus:",        m_youngsModulus);
    genLayout->addRow("Thickness:",             m_thickness);
    genLayout->addRow("Element threshold:",      m_elementThreshold);
    tabs->addTab(genWidget, "General");

    // --- Constraints tab ---
    auto *conWidget = new QWidget;
    auto *conLayout = new QFormLayout(conWidget);
    conLayout->setContentsMargins(12, 12, 12, 12);
    conLayout->setSpacing(8);

    m_constraintStiffnessScale = makeSpinBox(0, 1e12, 2);
    m_forceMagnitude           = makeSpinBox(0, 1e12, 2);
    m_weight                   = makeSpinBox(0, 1e12, 2);

    conLayout->addRow("Vector constraint stiffness scale:", m_constraintStiffnessScale);
    conLayout->addRow("Force magnitude (N):",               m_forceMagnitude);
    conLayout->addRow("Weight (N):",                        m_weight);
    tabs->addTab(conWidget, "Constraints");

    mainLayout->addWidget(tabs);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &CalcSettingsDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);

    loadFromView();
}

void CalcSettingsDialog::loadFromView()
{
    if (!m_view) return;
    m_elasticModulus->setValue(m_view->getElasticModulus());
    m_stiffnessScaleFactor->setValue(m_view->getStiffnessScaleFactor());
    m_youngsModulus->setValue(m_view->getYoungsModulus());
    m_thickness->setValue(m_view->getThickness());
    m_elementThreshold->setValue(m_view->getElementTreshold());
    m_constraintStiffnessScale->setValue(m_view->getConstraintStiffnessScale());
    m_forceMagnitude->setValue(m_view->getForceMagnitude());
    m_weight->setValue(m_view->getWeight());
    m_gridStep->setValue(m_view->getGridStride());
    m_warnOnLarge->setChecked(m_view->getWarnOnLargeModels());
}

void CalcSettingsDialog::saveToView()
{
    if (!m_view) return;
    m_view->setElasticModulus(m_elasticModulus->value());
    m_view->setStiffnessScaleFactor(m_stiffnessScaleFactor->value());
    m_view->setYoungsModulus(m_youngsModulus->value());
    m_view->setThickness(m_thickness->value());
    m_view->setElementTreshold(m_elementThreshold->value());
    m_view->setConstraintStiffnessScale(m_constraintStiffnessScale->value());
    m_view->setForceMagnitude(m_forceMagnitude->value());
    m_view->setWeight(m_weight->value());
    m_view->setGridStride(m_gridStep->value());
    m_view->setWarnOnLargeModels(m_warnOnLarge->isChecked());
}

void CalcSettingsDialog::accept()
{
    saveToView();
    QDialog::accept();
}
