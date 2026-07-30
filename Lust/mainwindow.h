#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeView>
#include <QSplitter>
#include "ControlWidget/ControlWidget.h"
#include "ViewWidget/viewwidget.h"
#include "ParamWidget/ParamWidget.h"
#include "LoadingDialog/LoadingDialog.h"
#include "Protocol/FrameProtocol.h"
#include "Protocol/InferenceResponseDispatcher.h"
#include "Transport/InferenceRequestTracker.h"
#include "Transport/ResponseStreamDecoder.h"
#include "Transport/TcpClient.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void connectDetectionServer(const QString& host, quint16 port);
    void disconnectDetectionServer();
private:
    void setupNetworkConnections();
    bool sendStreamControlPacket(
        FrameProtocol::MessageType messageType,
        quint64 sequence);
    void announceStreamStart();
    void announceStreamStop();
private:
    void setupUI();
    void testModel();
private:
    ControlWidget* controlWidget;
    ViewWidget* viewWidget;
    ParamWidget* paramWidget;
    LoadingDialog* loadingDialog;

    TcpClient* tcpClient;
    ResponseStreamDecoder* responseStreamDecoder;
    InferenceResponseDispatcher* responseDispatcher;
    InferenceRequestTracker* requestTracker;
    quint64 nextStreamSequence_ = 1;
    quint64 activeStreamSequence_ = 0;
    quint64 pendingStopRequestSequence_ = 0;
};
#endif // MAINWINDOW_H
