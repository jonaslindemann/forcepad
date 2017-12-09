#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private Q_SLOTS:
    void on_actionOpen_triggered();
    void on_openModelButton_clicked();
    void on_tabWidget_currentChanged(int idx);

    void on_structBrushButton_clicked();
    void on_structEraseButton_clicked();
    void on_structRectButton_clicked();
    void on_structOvalButton_clicked();
    void on_structLineButton_clicked();

    void on_structSelectButton_clicked();
    void on_structCutButton_clicked();
    void on_structCopyButton_clicked();
    void on_structPasteButton_clicked();
    void on_structExpandButton_clicked();

    void on_color1Button_clicked();
    void on_color2Button_clicked();
    void on_color3Button_clicked();
    void on_color4Button_clicked();
    void on_color5Button_clicked();
    void on_color6Button_clicked();
    void on_color7Button_clicked();
    void on_color8Button_clicked();
    void on_color9Button_clicked();
    void on_color10Button_clicked();
    void on_color11Button_clicked();
    void on_color12Button_clicked();

    void on_structBrushCombo_currentIndexChanged(int idx);
    void on_structLineWidthCombo_currentIndexChanged(int idx);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
