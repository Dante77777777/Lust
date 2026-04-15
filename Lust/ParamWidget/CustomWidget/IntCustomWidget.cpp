#include "IntCustomWidget.h"

IntCustomWidget::IntCustomWidget(CameraParam param, const QModelIndex& index, QWidget* parent)
    : OneCustomWidget(param,index,parent)
    , spinbox(new QSpinBox(this))
{

}

void IntCustomWidget::setParam(CameraParam& param)
{
    disconnect(spinbox, QOverload<int>::of(&QSpinBox::valueChanged),this,&IntCustomWidget::onValueChanged);

    OneCustomWidget::setParam(param);
    IntParam varParm = param_.getValue().value<IntParam>();

    spinbox->setMaximum(varParm.max);
    spinbox->setMinimum(varParm.min);
    spinbox->setSingleStep(varParm.increment);
    spinbox->setValue(varParm.value);

    connect(spinbox, QOverload<int>::of(&QSpinBox::valueChanged),this,&IntCustomWidget::onValueChanged);
}

CameraParam IntCustomWidget::getParam()
{
    return OneCustomWidget::getParam();
}

void IntCustomWidget::onValueChanged(int value)
{
    qDebug() << value;
    IntParam varParam = param_.getValue().value<IntParam>();
    varParam.value = value;
    param_.setValue(QVariant::fromValue(varParam));
}

void IntCustomWidget::addEditLayout(QHBoxLayout* layout)
{
    layout->addWidget(spinbox);
}
