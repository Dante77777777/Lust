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
private:
    void setupUI();
    void testModel();
private:
    ControlWidget* controlWidget;
    ViewWidget* viewWidget;
    ParamWidget* paramWidget;
    LoadingDialog* loadingDialog;
    TcpClient* tcpClient;
};
#endif // MAINWINDOW_H
