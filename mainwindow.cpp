#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mu32DoNothing(1)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

