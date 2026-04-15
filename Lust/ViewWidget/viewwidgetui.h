#ifndef VIEWWIDGETUI_H
#define VIEWWIDGETUI_H

#include <QObject>
#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>
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
    GraphicsView* imageView;
    //底层
    QWidget* buttonWidget;
    QHBoxLayout* buttonLayout;
    QPushButton* grabbingButton;

    void setupUI(QWidget* parent)
    {
        viewLayout = new QVBoxLayout(parent);

        imageWidget = new QWidget(parent);
        imageLayout = new QVBoxLayout(imageWidget);
        imageView = new GraphicsView(imageWidget);

        buttonWidget = new QWidget(parent);
        buttonLayout = new QHBoxLayout(buttonWidget);
        grabbingButton = new QPushButton(buttonWidget);

        grabbingButton->setText("开始拉流");

        setToolTip();

        imageWidget->setLayout(imageLayout);
        imageLayout->addWidget(imageView);

        buttonWidget->setLayout(buttonLayout);
        buttonLayout->addWidget(grabbingButton);
        buttonWidget->setStyleSheet(
            "background-color: lightgray;"
            "border: 1px solid gray;"
            );
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
