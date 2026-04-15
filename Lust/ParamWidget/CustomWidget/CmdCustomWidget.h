#ifndef CMDCUSTOMWIDGET_H
#define CMDCUSTOMWIDGET_H

#include "../OneCustomWidget.h"
#include <QPushButton>


class CmdCustomWidget : public OneCustomWidget
{
public:
    CmdCustomWidget(CameraParam param, const QModelIndex& index, QWidget* parent = nullptr);
    void setParam(CameraParam& param) override;
    CameraParam getParam() override;

public slots:
    void onCmdButtonClicked();

protected:
    void addEditLayout(QHBoxLayout* layout) override;

private:
    QPushButton* cmdButton;
};




























#endif
