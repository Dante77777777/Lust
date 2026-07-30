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
    [[nodiscard]] bool isStreamActive() const noexcept;
signals:
    void SigUpdateErrorInfo(QString info);
    void sigPacketReady(quint64 sequence, const QByteArray& packet);
    void sigPacketError(const QString& message);
    void sigStreamStarted();
    void sigStreamStopped();
public slots:
    void onGrabbingButtonClicked();
    void stopAcquisitionFromServer();

private:
    bool startAcquisition();
    bool stopAcquisition();
    void setStreamActive(bool active);
    void setStartGrabbingButtonState(bool checked);
    void stopImageProcess();

private:
    AcquireImageProcess* imageProcessThread;
    ViewWidgetUI* ui;
    ControlWidget* controlWidget_;
    bool streamActive_ = false;
};

#endif // VIEWWIDGET_H
