#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>
class ImageItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit ImageItem(QWidget *parent = nullptr);
protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
signals:
    void imageInfo(QString info);
public:
    int w;
    int h;
};

#endif // IMAGEITEM_H
