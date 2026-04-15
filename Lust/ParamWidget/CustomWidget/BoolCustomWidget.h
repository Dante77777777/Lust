#ifndef BOOLCUSTOMWIDGET_H
#define BOOLCUSTOMWIDGET_H

#include "../OneCustomWidget.h"
#include <QCheckBox>

class BoolCustomWidget : public OneCustomWidget
{
public:
    BoolCustomWidget(CameraParam param, const QModelIndex& index, QWidget* parent = nullptr);
    void setParam(CameraParam& param) override;
    CameraParam getParam() override;

public slots:
    void onValueChanged(bool checked);

protected:
    void addEditLayout(QHBoxLayout* layout) override;

private:
    QCheckBox* checkbox;

};

#endif
