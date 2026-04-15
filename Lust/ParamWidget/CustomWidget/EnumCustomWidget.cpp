#include "EnumCustomWidget.h"

EnumCustomWidget::EnumCustomWidget(CameraParam param, const QModelIndex& index, QWidget* parent)
    : OneCustomWidget(param,index,parent)
    , combobox(new QComboBox(this))
{

}

void EnumCustomWidget::setParam(CameraParam& param)
{
    disconnect(combobox,&QComboBox::currentIndexChanged,this,&EnumCustomWidget::onValueChanged);

    OneCustomWidget::setParam(param);
    EnumParam varParam = param_.getValue().value<EnumParam>();

    QStringList comboList(varParam.availableValue.begin(),varParam.availableValue.end());
    combobox->addItems(comboList);
    combobox->setCurrentText(varParam.value);

    connect(combobox,&QComboBox::currentIndexChanged,this,&EnumCustomWidget::onValueChanged);
}

CameraParam EnumCustomWidget::getParam()
{
    return OneCustomWidget::getParam();
}

void EnumCustomWidget::onValueChanged(int value)
{
    EnumParam varParam = param_.getValue().value<EnumParam>();
    varParam.value = combobox->currentText();
    varParam.valueInt = value;
    param_.setValue(QVariant::fromValue(varParam));
}

void EnumCustomWidget::addEditLayout(QHBoxLayout* layout)
{
    layout->addWidget(combobox);
}
