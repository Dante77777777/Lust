#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include "ControlWidgetUI.h"
#include "../LoadingDialog/LoadingDialog.h"
#include "../Listener.h"
#include "../CameraInterface/CMCameraMetaInfo.h"
#include "../CameraInterface/CameraContext.h"
#include <QWidget>
#include <QObject>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

class ControlWidget : public QWidget, Listener
{
    Q_OBJECT
public:
    explicit ControlWidget(QWidget* parent = nullptr);
    ~ControlWidget();

    void onRespondMessage(int message) override;

    CMCameraMetaInfo getCurrentCameraInfo();
    CMCameraMetaInfo getCameraInfo(int index);

    void setDetectionServerConnected(bool connected);
    void setDetectionServerBusy(
        bool busy,
        const QString& statusText
    );
signals:
    void SigUpdateErrorInfo(QString info);
    void detectionServerConnectRequested(
        const QString& host,
        quint16 port
    );
    void detectionServerDisconnectRequested();
private slots:
    void onEnumerationClicked();
    void onConnectionClicked();
    void onSaveConfigClicked();
    void onLoadConfigClicked();
    void onCamerasListCurrentRowChanged(int currentRow);
    void onDetectionServerButtonClicked();

private:
    ControlWidgetUI* ui;
    int lastCameraIndex;
    QVector<CMCameraMetaInfo> cameraMetaInfos;
    bool detectionServerConnected_ = false;
};

#endif //CONTROLWIDGET_H
