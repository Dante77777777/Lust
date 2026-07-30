#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QByteArray>
#include "viewwidgetui.h"
#include "../Listener.h"
#include "../CameraInterface/CameraContext.h"
#include "../ControlWidget/ControlWidget.h"
#include "AcquireImageProcess.h"

class ViewWidget : public QWidget, Listener
{
    Q_OBJECT
public:
    ViewWidget(ControlWidget* controlWidget, QWidget* parent = nullptr);
    ~ViewWidget();
    void onRespondMessage(int message) override;
    void setPacketPublishingEnabled(bool enabled);
signals:
    void SigUpdateErrorInfo(QString info);
    void sigPacketReady(quint64 sequence, const QByteArray& packet);
    void sigPacketError(const QString& message);
public slots:
    void onGrabbingButtonClicked();

private:
    void setStartGrabbingButtonState(bool checked);
    void stopImageProcess();

private:
    AcquireImageProcess* imageProcessThread;
    ViewWidgetUI* ui;
    ControlWidget* controlWidget_;
};

#endif // VIEWWIDGET_H
