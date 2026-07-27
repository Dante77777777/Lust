#ifndef VIEWWIDGETUI_H
#define VIEWWIDGETUI_H

#include <QObject>
#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "graphicsview.h"

class ViewWidgetUI
{
public:
    ViewWidgetUI(){}
    //整体
    QVBoxLayout* viewLayout;
    //图片
    QWidget* imageWidget;
    QVBoxLayout* imageLayout;
    QLabel* imageTitleLabel;
    GraphicsView* imageView;
    //底层
    QWidget* buttonWidget;
    QHBoxLayout* buttonLayout;
    QPushButton* grabbingButton;

    void setupUI(QWidget* parent)
    {
        parent->setObjectName("ViewWidget");
        viewLayout = new QVBoxLayout(parent);
        viewLayout->setContentsMargins(0, 0, 0, 0);
        viewLayout->setSpacing(8);

        imageWidget = new QWidget(parent);
        imageWidget->setObjectName("ImagePanel");
        imageLayout = new QVBoxLayout(imageWidget);
        imageLayout->setContentsMargins(0, 0, 0, 0);
        imageLayout->setSpacing(0);
        imageTitleLabel = new QLabel("实时图像", imageWidget);
        imageTitleLabel->setObjectName("ImagePanelTitle");
        imageView = new GraphicsView(imageWidget);
        imageView->setObjectName("ImageGraphicsView");

        buttonWidget = new QWidget(parent);
        buttonWidget->setObjectName("StreamControlBar");
        buttonLayout = new QHBoxLayout(buttonWidget);
        buttonLayout->setContentsMargins(10, 8, 10, 8);
        buttonLayout->setSpacing(8);
        grabbingButton = new QPushButton(buttonWidget);
        grabbingButton->setObjectName("PrimaryButton");

        grabbingButton->setText("开始拉流");

        setToolTip();

        imageWidget->setLayout(imageLayout);
        imageLayout->addWidget(imageTitleLabel);
        imageLayout->addWidget(imageView);

        buttonWidget->setLayout(buttonLayout);
        buttonLayout->addStretch();
        buttonLayout->addWidget(grabbingButton);
        grabbingButton->setMinimumWidth(150);
        buttonLayout->addStretch();
        viewLayout->addWidget(imageWidget);
        viewLayout->addWidget(buttonWidget);

        parent->setLayout(viewLayout);
        parent->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    }

    void setToolTip()
    {
#if QT_CONFIG(tooltip)
        grabbingButton->setToolTip("开始/暂停拉流");
#endif // QT_CONFIG(tooltip)
    }

};



#endif // VIEWWIDGETUI_H
