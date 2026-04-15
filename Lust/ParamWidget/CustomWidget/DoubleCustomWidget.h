#ifndef DOUBLECUSTOMWIDGET_H
#define DOUBLECUSTOMWIDGET_H

#include "../OneCustomWidget.h"
#include <QDoubleSpinBox>

class DoubleCustomWidget : public OneCustomWidget
{
public:
    DoubleCustomWidget(CameraParam param, const QModelIndex& index, QWidget* parent = nullptr);
    void setParam(CameraParam& param) override;
    CameraParam getParam() override;

public slots:
    void onValueChanged(double value);

protected:
    void addEditLayout(QHBoxLayout* layout) override;

private:
    QDoubleSpinBox* spinbox;
};


#endif
