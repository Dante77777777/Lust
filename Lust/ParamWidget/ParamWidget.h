#ifndef PARAMWIDGET_H
#define PARAMWIDGET_H
#include "ParamWidgetUI.h"
#include "CameraParamModel.h"
#include "CameraParamDelegate.h"
#include "../CameraInterface/CameraContext.h"
#include "../ControlWidget/ControlWidget.h"
#include "../Listener.h"
#include "QItemSelectionModel"
#include <QVector>
#include <QDebug>

class ParamWidget : public QWidget, Listener
{
    Q_OBJECT
public:
    ParamWidget(ControlWidget* controlWidget, QWidget* parent = nullptr);
    ~ParamWidget();
    void setupUI();

    void onRespondMessage(int message) override;
    void clearParamWidget();
    void writeCameraParam(const QModelIndex& index);
    void initParamList(QVector<CameraParam> params);

signals:
    void SigUpdateErrorInfo(QString info);

public slots:
    void onRefreshButtonClicked();
    void onUpdateSelecton(const QItemSelection& selected, const QItemSelection& deselected);

private:
    CameraParamModel* paramModel;
    CameraParamDelegate* paramDelegate;
    ControlWidget* controlWidget_;
    ParamWidgetUI* ui;
    QItemSelectionModel* selectionModel;
};


















#endif
