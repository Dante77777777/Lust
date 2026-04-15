#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QSplitter>
#include <QTreeView>
#include <QTextBrowser>


class ParamWidgetUI
{
public:
    ParamWidgetUI(){}

    QVBoxLayout* paramLayout;

    QWidget* upperWidget;
    QVBoxLayout* upperLayout;
    QPushButton* refreshButton;
    QTreeView* paramTreeView;

    QSplitter* splitter;

    QTextBrowser* tipTextBrowser;

    void setupUI(QWidget* parent)
    {
        paramLayout = new QVBoxLayout(parent);

        upperWidget = new QWidget(parent);
        upperLayout = new QVBoxLayout(upperWidget);
        refreshButton = new QPushButton(upperWidget);
        refreshButton->setText("刷新");
        paramTreeView = new QTreeView(upperWidget);
        paramTreeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        splitter = new QSplitter(parent);

        tipTextBrowser = new QTextBrowser();
        tipTextBrowser->setReadOnly(true);
        tipTextBrowser->setText("this is a tipTextBrowser");

        upperLayout->addWidget(refreshButton);
        upperLayout->addWidget(paramTreeView);

        splitter->setOrientation(Qt::Vertical);
        splitter->addWidget(upperWidget);
        splitter->addWidget(tipTextBrowser);
        splitter->setStretchFactor(0, 8);
        splitter->setStretchFactor(1, 1);

        paramLayout->addWidget(splitter);

        parent->setLayout(paramLayout);
        parent->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    }
};
