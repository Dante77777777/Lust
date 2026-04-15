#include "BoolCustomWidget.h"

BoolCustomWidget::BoolCustomWidget(CameraParam param, const QModelIndex& index, QWidget* parent)
    : OneCustomWidget(param,index,parent)
    , checkbox(new QCheckBox(this))
{
    checkbox->setStyleSheet(QString("QCheckBox{background-color: white;}"));
}

void BoolCustomWidget::setParam(CameraParam& param)
{
    disconnect(checkbox,&QCheckBox::toggled,this,&BoolCustomWidget::onValueChanged);

    OneCustomWidget::setParam(param);
    BoolParam varParam = param_.getValue().value<BoolParam>();
    checkbox->setChecked(varParam.value);
    checkbox->setText(param_.displayText());

    connect(checkbox,&QCheckBox::toggled,this,&BoolCustomWidget::onValueChanged);
}

CameraParam BoolCustomWidget::getParam()
{
    return OneCustomWidget::getParam();
}

void BoolCustomWidget::onValueChanged(bool checked)
{
    BoolParam varParam = getParam().getValue().value<BoolParam>();
    varParam.value = checked;
    param_.setValue(QVariant::fromValue(varParam));
}

void BoolCustomWidget::addEditLayout(QHBoxLayout* layout)
{
    layout->addWidget(checkbox);
}
