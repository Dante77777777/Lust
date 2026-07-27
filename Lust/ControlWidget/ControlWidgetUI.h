#ifndef CONTROLWIDGETUI_H
#define CONTROLWIDGETUI_H
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QListWidget>
#include <QLabel>


class ControlWidgetUI
{
public:
    QWidget* buttonWidget;
//整体为竖向布局
    QVBoxLayout* ControlWidgetLayout;
//两列布局四个按钮
    QGridLayout* buttonsLayout;
    QLabel* titleLabel;
    QLabel* operationLabel;
    QLabel* cameraListLabel;
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
        parent->setObjectName("ControlWidget");
        buttonWidget = new QWidget(parent);
        buttonWidget->setObjectName("PanelContent");

        ControlWidgetLayout = new QVBoxLayout(parent);
        ControlWidgetLayout->setContentsMargins(0, 0, 0, 0);
        ControlWidgetLayout->setSpacing(0);

        titleLabel = new QLabel("相机", parent);
        titleLabel->setObjectName("PanelHeader");

        operationLabel = new QLabel("设备操作", parent);
        operationLabel->setObjectName("SectionLabel");

        cameraListLabel = new QLabel("可用相机", parent);
        cameraListLabel->setObjectName("SectionLabel");

        buttonsLayout = new QGridLayout(buttonWidget);
        buttonsLayout->setContentsMargins(10, 8, 10, 10);
        buttonsLayout->setHorizontalSpacing(8);
        buttonsLayout->setVerticalSpacing(8);

        enumerationButton = new QPushButton("枚举相机",parent);
        enumerationButton->setObjectName("SecondaryButton");
        connectionButton = new QPushButton("连接相机",parent);
        connectionButton->setObjectName("PrimaryButton");
        saveConfigButton = new QPushButton("保存配置",parent);
        saveConfigButton->setObjectName("SecondaryButton");
        loadConfigButton = new QPushButton("载入配置",parent);
        loadConfigButton->setObjectName("SecondaryButton");

        setToolTip();

        enumerationCameras = new QListWidget(parent);
        enumerationCameras->setObjectName("CameraList");
        enumerationCameras->setAlternatingRowColors(true);
        enumerationCameras->setUniformItemSizes(true);

        buttonsLayout->addWidget(enumerationButton, 0, 0);
        buttonsLayout->addWidget(connectionButton, 0, 1);
        buttonsLayout->addWidget(saveConfigButton, 1, 0);
        buttonsLayout->addWidget(loadConfigButton, 1, 1);
        buttonsLayout->setColumnStretch(0, 1);
        buttonsLayout->setColumnStretch(1, 1);
        buttonWidget->setLayout(buttonsLayout);

        ControlWidgetLayout->addWidget(titleLabel);
        ControlWidgetLayout->addWidget(operationLabel);
        ControlWidgetLayout->addWidget(buttonWidget);
        ControlWidgetLayout->addWidget(cameraListLabel);
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
