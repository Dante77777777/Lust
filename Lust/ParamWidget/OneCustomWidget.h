#ifndef ONECUSTOMWIDGET_H
#define ONECUSTOMWIDGET_H

#include <QLayout>
#include <QModelIndex>
#include <QWidget>
#include "../CameraInterface/CMCameraParam.h"

class OneCustomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OneCustomWidget(CameraParam param, const QModelIndex& index, QWidget* parent = nullptr);
    void InitWidget();

    virtual void setParam(CameraParam& param);
    virtual CameraParam getParam();

signals:
    void sigValueChanged(const CameraParam& param, const QModelIndex& index);

protected:
    virtual void addEditLayout(QHBoxLayout* layout);

protected:
    CameraParam param_;
    QModelIndex index_;

};

#endif
