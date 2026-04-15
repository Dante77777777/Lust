#ifndef INTCUSTOMWIDGET_H
#define INTCUSTOMWIDGET_H
#include "../OneCustomWidget.h"
#include <QSpinBox>

class IntCustomWidget : public OneCustomWidget
{
public:
    IntCustomWidget(CameraParam param, const QModelIndex& index, QWidget* parant);
    void setParam(CameraParam& param) override;
    CameraParam getParam() override;

public slots:
    void onValueChanged(int value);

protected:
    void addEditLayout(QHBoxLayout* layout) override;

private:
    QSpinBox* spinbox;
};































#endif
