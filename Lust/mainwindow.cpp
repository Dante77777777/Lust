#include "mainwindow.h"

#include <QStatusBar>

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
    setWindowTitle(tr("Lust Vision"));
    setWindowIcon(QIcon(QStringLiteral(":/resource/camara.png")));
    resize(1650, 900);
    setMinimumSize(1180, 680);

    auto* centralWidget = new QWidget(this);
    centralWidget->setObjectName("CentralWidget");

    auto* centralLayout = new QHBoxLayout(centralWidget);
    centralLayout->setContentsMargins(10, 10, 10, 10);
    centralLayout->setSpacing(0);

    auto* splitter = new QSplitter(Qt::Horizontal, centralWidget);
    splitter->setObjectName("MainSplitter");
    splitter->setChildrenCollapsible(false);
    splitter->setHandleWidth(8);
    splitter->addWidget(controlWidget);
    splitter->addWidget(viewWidget);
    splitter->addWidget(paramWidget);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setStretchFactor(2, 0);
    splitter->setSizes({280, 1040, 300});

    controlWidget->setMinimumWidth(240);
    controlWidget->setMaximumWidth(360);
    paramWidget->setMinimumWidth(270);
    paramWidget->setMaximumWidth(400);

    centralLayout->addWidget(splitter);
    setCentralWidget(centralWidget);
    statusBar()->showMessage(tr("就绪"));
}

void MainWindow::testModel()
{

}
