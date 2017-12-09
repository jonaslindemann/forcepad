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

void MainWindow::on_structBrushCombo_currentIndexChanged(int idx)
{
    ui->paintView->setCurrentBrush(idx);
}

void MainWindow::on_structLineWidthCombo_currentIndexChanged(int idx)
{
    ui->paintView->setLineWidth(idx);
}

// |----|----|----|----|----|----|----|----|----|----|----|----|
//  1.0                                                    0.0

void MainWindow::on_color1Button_clicked()
{
    ui->paintView->setStiffness(1.0);
}

void MainWindow::on_color2Button_clicked()
{
    ui->paintView->setStiffness(0.9);
}

void MainWindow::on_color3Button_clicked()
{
    ui->paintView->setStiffness(0.8);
}

void MainWindow::on_color4Button_clicked()
{
    ui->paintView->setStiffness(0.7);
}

void MainWindow::on_color5Button_clicked()
{
    ui->paintView->setStiffness(0.6);
}

void MainWindow::on_color6Button_clicked()
{
    ui->paintView->setStiffness(0.5);
}

void MainWindow::on_color7Button_clicked()
{
    ui->paintView->setStiffness(0.4);
}

void MainWindow::on_color8Button_clicked()
{
    ui->paintView->setStiffness(0.3);
}

void MainWindow::on_color9Button_clicked()
{
    ui->paintView->setStiffness(0.2);
}

void MainWindow::on_color10Button_clicked()
{
    ui->paintView->setStiffness(0.1);
}

void MainWindow::on_color11Button_clicked()
{
    ui->paintView->setStiffness(0.0);
}

void MainWindow::on_color12Button_clicked()
{
    ui->paintView->setStiffness(0.0);
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
