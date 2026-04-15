#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDialog>

class LoadingDialogUI
{
public:
    QPushButton* closeButton;
    QVBoxLayout* loadingDialogLayout;
    QLabel* label;

    void setupUI(QDialog* parent)
    {
        closeButton = new QPushButton();
        loadingDialogLayout = new QVBoxLayout(parent);
        label = new QLabel();

        closeButton->setText("Close");
        label->setText("This is a loading dialog...");

        loadingDialogLayout->addWidget(label);
        loadingDialogLayout->addWidget(closeButton);

        parent->resize(500,100);
        parent->setStyleSheet("background-color: white");
    }
};
