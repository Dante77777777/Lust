#ifndef CAMERAPARAMITEM_H
#define CAMERAPARAMITEM_H

#include <QVector>
#include <QVariant>
class CameraParamItem
{
public:
    explicit CameraParamItem(const QVariant& data, CameraParamItem* parent = nullptr);
    ~CameraParamItem();
    CameraParamItem* child(int position);
    int childCount() const;
    QVariant getData() const;
    CameraParamItem* insertChild(int position);
    CameraParamItem* getParent() const;
    bool removeChild(int position);
    int myPosition();
    bool setData(const QVariant& data);

private:
    QVector<CameraParamItem*> children;
    QVariant itemData;
    CameraParamItem* parentItem;
};

































#endif
