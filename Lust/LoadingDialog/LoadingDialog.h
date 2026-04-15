#ifndef LOADINGDIALOG_H
#define LOADINGDIALOG_H
#include <QWidget>
#include <QApplication>
#include "LoadingDialogUI.h"


class LoadingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoadingDialog(QWidget* parent = nullptr);
    ~LoadingDialog();

    // 显示loading窗
    static void Loading(QWidget* parent = nullptr);
    // 隐藏loading窗
    static void HideLoading();

public slots:
    void onCloseButtonCLicked();

private:
    LoadingDialogUI* ui;

};






























#endif
