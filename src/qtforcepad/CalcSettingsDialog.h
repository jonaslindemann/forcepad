#pragma once

#include <QDialog>

class QSlider;
class QCheckBox;
class QDoubleSpinBox;
class QtPaintView;

class CalcSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CalcSettingsDialog(QtPaintView *view, QWidget *parent = nullptr);

private Q_SLOTS:
    void accept() override;

private:
    void loadFromView();
    void saveToView();

    QtPaintView *m_view;

    // Mesh tab
    QSlider   *m_gridStep{nullptr};
    QCheckBox *m_warnOnLarge{nullptr};

    // General tab
    QDoubleSpinBox *m_elasticModulus{nullptr};
    QDoubleSpinBox *m_stiffnessScaleFactor{nullptr};
    QDoubleSpinBox *m_youngsModulus{nullptr};
    QDoubleSpinBox *m_thickness{nullptr};
    QDoubleSpinBox *m_elementThreshold{nullptr};

    // Constraints tab
    QDoubleSpinBox *m_constraintStiffnessScale{nullptr};
    QDoubleSpinBox *m_forceMagnitude{nullptr};
    QDoubleSpinBox *m_weight{nullptr};
};
