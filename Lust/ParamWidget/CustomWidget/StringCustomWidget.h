#ifndef STRINGCUSTOMWIDGET_H
#define STRINGCUSTOMWIDGET_H

#include "../OneCustomWidget.h"
#include <QLineEdit>

class StringCustomWidget : public OneCustomWidget
{
public:
    StringCustomWidget(CameraParam param, const QModelIndex& index, QWidget* parent = nullptr);
    void setParam(CameraParam& param) override;
    CameraParam getParam() override;

public slots:
    void onValueChanged();

protected:
    void addEditLayout(QHBoxLayout* layout) override;

private:
    QLineEdit* lineEdit;
};

#endif
