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
signals:
    void SigUpdateErrorInfo(QString info);
private slots:
    void onEnumerationClicked();
    void onConnectionClicked();
    void onSaveConfigClicked();
    void onLoadConfigClicked();
    void onCamerasListCurrentRowChanged(int currentRow);

private:
    ControlWidgetUI* ui;
    int lastCameraIndex;
    QVector<CMCameraMetaInfo> cameraMetaInfos;
};

#endif //CONTROLWIDGET_H
