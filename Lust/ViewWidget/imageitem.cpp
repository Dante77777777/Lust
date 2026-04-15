#include "imageitem.h"
#include <QGraphicsSceneHoverEvent>

ImageItem::ImageItem(QWidget *parent)
    : QGraphicsPixmapItem(nullptr)
{
    setAcceptHoverEvents(true);
}


void ImageItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    QPointF point = event->pos();

    int x, y;
    int R,G,B;
    x = point.x();
    y = point.y();
    if(x < 0)
    {
        x = 0;
    }
    if(y < 0)
    {
        y = 0;
    }
    pixmap().toImage().pixelColor(x,y).getRgb(&R,&G,&B);
    QString info = QString("W:%1,H:%2 | X:%3,Y:%4 | R:%5,G:%6,B:%7")
                            .arg(QString::number(w))
                            .arg(QString::number(h))
                            .arg(QString::number(x))
                            .arg(QString::number(y))
                            .arg(QString::number(R))
                            .arg(QString::number(G))
                            .arg(QString::number(B));
    emit imageInfo(info);
}
