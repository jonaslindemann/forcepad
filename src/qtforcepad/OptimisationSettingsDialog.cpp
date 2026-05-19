#include "OptimisationSettingsDialog.h"
#include "QtPaintView.h"
#include "../common/FemGridSolver2.h"

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QRadioButton>
#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDialogButtonBox>

OptimisationSettingsDialog::OptimisationSettingsDialog(QtPaintView *view, QWidget *parent)
    : QDialog(parent), m_view(view)
{
    setWindowTitle("Optimisation Settings");
    setModal(true);
    setMinimumWidth(340);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(16, 16, 16, 16);

    auto makeSpinBox = [](double min, double max, int dec, double step) {
        auto *sb = new QDoubleSpinBox;
        sb->setRange(min, max);
        sb->setDecimals(dec);
        sb->setSingleStep(step);
        sb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        return sb;
    };

    // --- Stopping criteria ---
    auto *stopGroup = new QGroupBox("Stopping Criteria");
    auto *stopForm  = new QFormLayout(stopGroup);
    stopForm->setSpacing(8);

    m_volfrac  = makeSpinBox(0.01, 1.0,  2, 0.05);
    m_rmin     = makeSpinBox(1.0,  100.0, 1, 1.0);
    m_minChange = makeSpinBox(0.0001, 0.5, 4, 0.001);
    m_maxLoops  = new QSpinBox;
    m_maxLoops->setRange(1, 2000);
    m_maxLoops->setSingleStep(10);
    m_maxLoops->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    stopForm->addRow("Volume fraction:",  m_volfrac);
    stopForm->addRow("Filter r_min (px):", m_rmin);
    stopForm->addRow("Min change:",       m_minChange);
    stopForm->addRow("Max iterations:",   m_maxLoops);
    mainLayout->addWidget(stopGroup);

    // --- Filter type ---
    auto *filterGroup = new QGroupBox("Sensitivity Filter");
    auto *filterLayout = new QVBoxLayout(filterGroup);
    filterLayout->setSpacing(6);

    m_radioNoFilter = new QRadioButton("No filter");
    m_radioSigmund  = new QRadioButton("Ole Sigmund filter");
    m_radioSharp    = new QRadioButton("Sharp contouring filter");

    filterLayout->addWidget(m_radioNoFilter);
    filterLayout->addWidget(m_radioSigmund);
    filterLayout->addWidget(m_radioSharp);
    mainLayout->addWidget(filterGroup);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &OptimisationSettingsDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);

    loadFromView();
}

void OptimisationSettingsDialog::loadFromView()
{
    if (!m_view) return;
    m_volfrac->setValue(m_view->getOptVolumeFraction());
    m_rmin->setValue(m_view->getOptRmin());
    m_minChange->setValue(m_view->getOptMinChange());
    m_maxLoops->setValue(m_view->getOptMaxLoops());

    switch (m_view->getOptFilterType()) {
    case fp::FemGridSolver2::FT_NO_FILTER:       m_radioNoFilter->setChecked(true); break;
    case fp::FemGridSolver2::FT_OLE_SIGMUND:     m_radioSigmund->setChecked(true);  break;
    case fp::FemGridSolver2::FT_SHARP_CONTOURING: m_radioSharp->setChecked(true);   break;
    }
}

void OptimisationSettingsDialog::saveToView()
{
    if (!m_view) return;
    m_view->setOptVolumeFraction(m_volfrac->value());
    m_view->setOptRmin(m_rmin->value());
    m_view->setOptMinChange(m_minChange->value());
    m_view->setOptMaxLoops(m_maxLoops->value());

    fp::FemGridSolver2::TFilterType ft = fp::FemGridSolver2::FT_OLE_SIGMUND;
    if (m_radioNoFilter->isChecked()) ft = fp::FemGridSolver2::FT_NO_FILTER;
    else if (m_radioSharp->isChecked()) ft = fp::FemGridSolver2::FT_SHARP_CONTOURING;
    m_view->setOptFilterType(ft);
}

void OptimisationSettingsDialog::accept()
{
    saveToView();
    QDialog::accept();
}
