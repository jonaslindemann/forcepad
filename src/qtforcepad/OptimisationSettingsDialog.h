#pragma once

#include <QDialog>

class QDoubleSpinBox;
class QSpinBox;
class QRadioButton;
class QtPaintView;

class OptimisationSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit OptimisationSettingsDialog(QtPaintView *view, QWidget *parent = nullptr);

private Q_SLOTS:
    void accept() override;

private:
    void loadFromView();
    void saveToView();

    QtPaintView    *m_view;

    QDoubleSpinBox *m_volfrac{nullptr};
    QDoubleSpinBox *m_rmin{nullptr};
    QDoubleSpinBox *m_minChange{nullptr};
    QSpinBox       *m_maxLoops{nullptr};
    QRadioButton   *m_radioNoFilter{nullptr};
    QRadioButton   *m_radioSigmund{nullptr};
    QRadioButton   *m_radioSharp{nullptr};
};
