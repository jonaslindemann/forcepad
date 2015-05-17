#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    ui->paintView->openModel();
}

void MainWindow::on_openModelButton_clicked()
{
    ui->paintView->openModel();
}

void MainWindow::on_tabWidget_currentChanged(int idx)
{
    qDebug() << "current changed " << idx;

    if (idx == 1)
        ui->paintView->setViewMode(CPaintView::VM_SKETCH);

    if (idx == 2)
        ui->paintView->setViewMode(CPaintView::VM_PHYSICS);

    if (idx == 3)
    {
        ui->paintView->setViewMode(CPaintView::VM_ACTION);
        ui->paintView->setOptLayer(false);
        ui->paintView->setEditMode(CPaintView::EM_DYNAMIC_FORCE_UPDATE);
        ui->paintView->setDrawDisplacements(false);
        ui->paintView->setStressType(CFemGrid2::ST_PRINCIPAL);
        ui->paintView->setDrawStress(true);
    }
}
