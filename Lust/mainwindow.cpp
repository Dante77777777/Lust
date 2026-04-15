#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , controlWidget(new ControlWidget(this))
    , viewWidget(new ViewWidget(controlWidget,this))
    , paramWidget(new ParamWidget(controlWidget,this))
    , loadingDialog(new LoadingDialog(this))
{
    setupUI();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    resize(1650,800);
    QSplitter* splitter = new QSplitter();
    splitter->addWidget(controlWidget);
    splitter->addWidget(viewWidget);
    splitter->addWidget(paramWidget);
    splitter->setStretchFactor(0,2);
    splitter->setStretchFactor(1,3);
    splitter->setStretchFactor(2,5);
    this->setCentralWidget(splitter);
}

void MainWindow::testModel()
{

}
