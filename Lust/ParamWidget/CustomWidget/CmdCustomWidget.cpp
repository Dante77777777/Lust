#include "CmdCustomWidget.h"

CmdCustomWidget::CmdCustomWidget(CameraParam param, const QModelIndex& index, QWidget* parent)
    : OneCustomWidget(param,index,parent)
    , cmdButton(new QPushButton(this))
{

}

void CmdCustomWidget::setParam(CameraParam& param)
{
    disconnect(cmdButton,&QPushButton::clicked,this,&CmdCustomWidget::onCmdButtonClicked);

    OneCustomWidget::setParam(param);
    cmdButton->setText(param_.displayText());

    connect(cmdButton,&QPushButton::clicked,this,&CmdCustomWidget::onCmdButtonClicked);
}

CameraParam CmdCustomWidget::getParam()
{
    return OneCustomWidget::getParam();
}

void CmdCustomWidget::onCmdButtonClicked()
{
    emit sigValueChanged(param_,index_);
}

void CmdCustomWidget::addEditLayout(QHBoxLayout* layout)
{
    layout->addWidget(cmdButton);
}
