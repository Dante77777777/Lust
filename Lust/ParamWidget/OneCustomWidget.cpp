#include "OneCustomWidget.h"

OneCustomWidget::OneCustomWidget(CameraParam param,const QModelIndex& index, QWidget* parent)
    : QWidget(parent)
    , index_(index)
    , param_(param)
{

}

void OneCustomWidget::InitWidget()
{
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    addEditLayout(layout);
    this->setLayout(layout);
}

void OneCustomWidget::setParam(CameraParam& param)
{
    param_ = param;
}

CameraParam OneCustomWidget::getParam()
{
    return param_;
}

void OneCustomWidget::addEditLayout(QHBoxLayout* layout)
{

}
