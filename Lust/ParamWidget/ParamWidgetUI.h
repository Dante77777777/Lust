#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QSplitter>
#include <QTreeView>
#include <QTextBrowser>
#include <QLabel>


class ParamWidgetUI
{
public:
    ParamWidgetUI(){}

    QVBoxLayout* paramLayout;

    QWidget* upperWidget;
    QVBoxLayout* upperLayout;
    QLabel* titleLabel;
    QWidget* titleBar;
    QHBoxLayout* titleBarLayout;
    QPushButton* refreshButton;
    QTreeView* paramTreeView;

    QSplitter* splitter;

    QWidget* descriptionWidget;
    QVBoxLayout* descriptionLayout;
    QLabel* descriptionLabel;
    QTextBrowser* tipTextBrowser;

    void setupUI(QWidget* parent)
    {
        parent->setObjectName("ParamWidget");
        paramLayout = new QVBoxLayout(parent);
        paramLayout->setContentsMargins(0, 0, 0, 0);
        paramLayout->setSpacing(0);

        upperWidget = new QWidget(parent);
        upperWidget->setObjectName("PanelContent");
        upperLayout = new QVBoxLayout(upperWidget);
        upperLayout->setContentsMargins(10, 8, 10, 10);
        upperLayout->setSpacing(8);

        titleBar = new QWidget(upperWidget);
        titleBar->setObjectName("PanelTitleBar");
        titleBarLayout = new QHBoxLayout(titleBar);
        titleBarLayout->setContentsMargins(0, 0, 0, 0);
        titleBarLayout->setSpacing(8);
        titleLabel = new QLabel("相机参数", titleBar);
        titleLabel->setObjectName("PanelTitle");
        refreshButton = new QPushButton(upperWidget);
        refreshButton->setText("刷新");
        refreshButton->setObjectName("SecondaryButton");
        refreshButton->setMaximumWidth(72);
        paramTreeView = new QTreeView(upperWidget);
        paramTreeView->setObjectName("ParamTreeView");
        paramTreeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        paramTreeView->setAlternatingRowColors(true);
        paramTreeView->setUniformRowHeights(true);

        splitter = new QSplitter(parent);
        splitter->setObjectName("ParamSplitter");

        descriptionWidget = new QWidget(parent);
        descriptionWidget->setObjectName("DescriptionPanel");
        descriptionLayout = new QVBoxLayout(descriptionWidget);
        descriptionLayout->setContentsMargins(10, 8, 10, 10);
        descriptionLayout->setSpacing(6);
        descriptionLabel = new QLabel("参数说明", descriptionWidget);
        descriptionLabel->setObjectName("SectionTitle");
        tipTextBrowser = new QTextBrowser(descriptionWidget);
        tipTextBrowser->setObjectName("ParamDescription");
        tipTextBrowser->setReadOnly(true);
        tipTextBrowser->setPlaceholderText("选择一个参数以查看说明");

        titleBarLayout->addWidget(titleLabel);
        titleBarLayout->addStretch();
        titleBarLayout->addWidget(refreshButton);
        upperLayout->addWidget(titleBar);
        upperLayout->addWidget(paramTreeView);

        descriptionLayout->addWidget(descriptionLabel);
        descriptionLayout->addWidget(tipTextBrowser);

        splitter->setOrientation(Qt::Vertical);
        splitter->addWidget(upperWidget);
        splitter->addWidget(descriptionWidget);
        splitter->setStretchFactor(0, 4);
        splitter->setStretchFactor(1, 1);
        splitter->setSizes({650, 170});

        paramLayout->addWidget(splitter);

        parent->setLayout(paramLayout);
        parent->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    }
};
