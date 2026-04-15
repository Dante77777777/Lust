#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QLabel>
#include <QWidget>
#include <QBoxLayout>
#include <QImage>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QMutex>
#include "imageitem.h"

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QWidget* parent = nullptr);
    ~GraphicsView();

    bool setupWidget();
    void clear();
    void setImage(const QImage& iamge);

protected:

    void wheelEvent(QWheelEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

public slots:

    void onZoom(double scaleFactor);
    void onCenter();

private:

    void fitFrame();
    void prepareBackGroundBoard(bool invertColor = false);

private:

    double zoomValue = 1;

    QGraphicsScene* GVscene;
    ImageItem* GVimageItem;
    QWidget* imageInfoWidget;
    QLabel* imageInfoLabel;
    QImage GVimage;
    QPixmap backGroundPixmap = QPixmap(36,36);
    //QMutex mutex;
};

#endif // GRAPHICSVIEW_H
