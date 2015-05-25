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

void MainWindow::on_structBrushButton_clicked()
{
    ui->paintView->setEditMode(CPaintView::EM_DIRECT_BRUSH);
}

void MainWindow::on_structEraseButton_clicked()
{
    ui->paintView->setEditMode(CPaintView::EM_DIRECT_ERASE);
}

void MainWindow::on_structRectButton_clicked()
{
    ui->paintView->setEditMode(CPaintView::EM_RECTANGLE);
}

void MainWindow::on_structOvalButton_clicked()
{
    ui->paintView->setEditMode(CPaintView::EM_ELLIPSE);
}

void MainWindow::on_structLineButton_clicked()
{
    ui->paintView->setEditMode(CPaintView::EM_LINE);
}

void MainWindow::on_lineWidthSpin_valueChanged(int idx)
{
    ui->paintView->setLineWidth(idx);
}

void MainWindow::on_structSelectButton_clicked()
{
    ui->paintView->setEditMode(CPaintView::EM_SELECT_BOX);
}

void MainWindow::on_structCutButton_clicked()
{
    ui->paintView->cut();
}

void MainWindow::on_structCopyButton_clicked()
{
    ui->paintView->copy();
}

void MainWindow::on_structPasteButton_clicked()
{
    ui->paintView->setEditMode(CPaintView::EM_PASTE);
}

void MainWindow::on_structExpandButton_clicked()
{
    ui->paintView->expandImageToWindow();
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
