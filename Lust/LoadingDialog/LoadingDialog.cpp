#include "LoadingDialog.h"

LoadingDialog* s_LoadingWidget = Q_NULLPTR;

LoadingDialog::LoadingDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new LoadingDialogUI)
{
    ui->setupUI(this);
    setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    connect(ui->closeButton,&QPushButton::clicked,this,&LoadingDialog::onCloseButtonCLicked);
}

LoadingDialog::~LoadingDialog()
{
    delete ui;
}

void LoadingDialog::Loading(QWidget* parent)
{
    if(Q_NULLPTR == s_LoadingWidget)
    {
        s_LoadingWidget = new LoadingDialog(parent);
    }
    s_LoadingWidget->setModal(true);
    s_LoadingWidget->show();
    QApplication::processEvents();
}

void LoadingDialog::HideLoading()
{
    if(s_LoadingWidget)
    {
        s_LoadingWidget->close();
        delete s_LoadingWidget;
        s_LoadingWidget = Q_NULLPTR;
    }
}


void LoadingDialog::onCloseButtonCLicked()
{
    this->close();
}
