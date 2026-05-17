#pragma once

#include <QDialog>

class QSlider;
class QLabel;
class QtPaintView;

class GeneralSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GeneralSettingsDialog(QtPaintView *view, QWidget *parent = nullptr);

private Q_SLOTS:
    void accept() override;
    void onLineThicknessChanged(int value);
    void onSymbolLengthChanged(int value);

private:
    void loadFromView();
    void saveToView();

    QtPaintView *m_view;

    QSlider *m_lineThickness{nullptr};
    QLabel  *m_lineThicknessDisplay{nullptr};
    QSlider *m_symbolLength{nullptr};
    QLabel  *m_symbolLengthDisplay{nullptr};
};
