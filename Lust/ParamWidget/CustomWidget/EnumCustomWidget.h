#ifndef ENUMCUSTOMWIDGET_H
#define ENUMCUSTOMWIDGET_H

#include "../OneCustomWidget.h"
#include <QComboBox>

class EnumCustomWidget : public OneCustomWidget
{
public:
    EnumCustomWidget(CameraParam param, const QModelIndex& index, QWidget* parent = nullptr);
    void setParam(CameraParam& param) override;
    CameraParam getParam() override;

public slots:
    void onValueChanged(int value);

protected:
    void addEditLayout(QHBoxLayout* layout) override;

private:
    QComboBox* combobox;
};

#endif
