#ifndef CONTROLWIDGETUI_H
#define CONTROLWIDGETUI_H
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>


class ControlWidgetUI
{
public:
    QWidget* buttonWidget;
//整体为竖向布局
    QVBoxLayout* ControlWidgetLayout;
//横向布局四个按钮
    QHBoxLayout* buttonsLayout;
//控制面部顶层按钮
    //枚举相机按钮
    QPushButton* enumerationButton;
    //连接/断连按钮
    QPushButton* connectionButton;
    //保存配置按钮
    QPushButton* saveConfigButton;
    //载入配置按钮
    QPushButton* loadConfigButton;

//枚举出的相机视图
    QListWidget* enumerationCameras;

    void setupUI(QWidget* parent)
    {
        buttonWidget = new QWidget(parent);

        ControlWidgetLayout = new QVBoxLayout(parent);
        buttonsLayout = new QHBoxLayout(buttonWidget);

        enumerationButton = new QPushButton("枚举相机",parent);
        connectionButton = new QPushButton("连接/断开相机",parent);
        saveConfigButton = new QPushButton("保存配置",parent);
        loadConfigButton = new QPushButton("载入配置",parent);

        setToolTip();

        enumerationCameras = new QListWidget(parent);

        buttonsLayout->addWidget(enumerationButton);
        buttonsLayout->addWidget(connectionButton);
        buttonsLayout->addWidget(saveConfigButton);
        buttonsLayout->addWidget(loadConfigButton);
        buttonWidget->setLayout(buttonsLayout);
        buttonWidget->setStyleSheet(
            "background-color: lightgray;"
            "border: 1px solid gray;"
            );

        ControlWidgetLayout->addWidget(buttonWidget);
        ControlWidgetLayout->addWidget(enumerationCameras);

        parent->setLayout(ControlWidgetLayout);
        parent->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    }

    void setToolTip()
    {
#if QT_CONFIG(tooltip)
        enumerationButton->setToolTip("枚举相机按钮");
        connectionButton->setToolTip("连接/断开相机按钮");
        saveConfigButton->setToolTip("保存配置按钮");
        loadConfigButton->setToolTip("载入配置按钮");
#endif // QT_CONFIG(tooltip)
    }
};


































#endif // CONTROLWIDGETUI_H
