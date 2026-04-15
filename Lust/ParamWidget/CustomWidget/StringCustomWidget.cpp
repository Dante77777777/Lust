#include "StringCustomWidget.h"

StringCustomWidget::StringCustomWidget(CameraParam param, const QModelIndex& index, QWidget* parent)
    : OneCustomWidget(param,index,parent)
    , lineEdit(new QLineEdit(this))
{

}


void StringCustomWidget::setParam(CameraParam& param)
{
    disconnect(lineEdit,&QLineEdit::editingFinished,this,&StringCustomWidget::onValueChanged);

    OneCustomWidget::setParam(param);
    StringParam varParam = param.getValue().value<StringParam>();
    lineEdit->setText(varParam.value);

    connect(lineEdit,&QLineEdit::editingFinished,this,&StringCustomWidget::onValueChanged);
}


CameraParam StringCustomWidget::getParam()
{
    return OneCustomWidget::getParam();
}


void StringCustomWidget::onValueChanged()
{

    StringParam varParam = param_.getValue().value<StringParam>();
    varParam.value = lineEdit->text();
    param_.setValue(QVariant::fromValue(varParam));
}

void StringCustomWidget::addEditLayout(QHBoxLayout* layout)
{
    layout->addWidget(lineEdit);
}
