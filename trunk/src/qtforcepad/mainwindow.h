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

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
