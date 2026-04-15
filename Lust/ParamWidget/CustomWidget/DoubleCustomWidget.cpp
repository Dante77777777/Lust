#include "DoubleCustomWidget.h"

DoubleCustomWidget::DoubleCustomWidget(CameraParam param, const QModelIndex& index, QWidget* parent)
    : OneCustomWidget(param,index,parent)
    , spinbox(new QDoubleSpinBox(this))
{

}


void DoubleCustomWidget::setParam(CameraParam& param)
{
    disconnect(spinbox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),this,&DoubleCustomWidget::onValueChanged);

    OneCustomWidget::setParam(param);
    DoubleParam varParam = param_.getValue().value<DoubleParam>();
    spinbox->setValue(varParam.value);
    spinbox->setMaximum(varParam.max);
    spinbox->setMinimum(varParam.min);
    spinbox->setDecimals(3);
    spinbox->setSingleStep(0.100);

    connect(spinbox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),this,&DoubleCustomWidget::onValueChanged);
}

CameraParam DoubleCustomWidget::getParam()
{
    return OneCustomWidget::getParam();
}

void DoubleCustomWidget::onValueChanged(double value)
{
    DoubleParam varParam = param_.getValue().value<DoubleParam>();
    varParam.value = value;
    param_.setValue(QVariant::fromValue(varParam));
}

void DoubleCustomWidget::addEditLayout(QHBoxLayout* layout)
{
    layout->addWidget(spinbox);
}
