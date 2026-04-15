#include "CameraParamItem.h"


CameraParamItem::CameraParamItem(const QVariant& data, CameraParamItem* parent)
    : itemData(data)
    , parentItem(parent)
{

}

CameraParamItem::~CameraParamItem()
{
    qDeleteAll(children);
}

CameraParamItem* CameraParamItem::child(int position)
{
    if(position < 0 || position >= children.size())
    {
        return nullptr;
    }
    return children.at(position);
}

int CameraParamItem::childCount() const
{
    return children.count();
}

QVariant CameraParamItem::getData() const
{
    return itemData;
}

CameraParamItem* CameraParamItem::insertChild(int position)
{
    if(position < 0 || position > children.size())
    {
        return nullptr;
    }
    CameraParamItem* child = new CameraParamItem(QVariant(),this);
    children.insert(position,child);
    return child;
}

CameraParamItem* CameraParamItem::getParent() const
{
    return parentItem;
}

bool CameraParamItem::removeChild(int position)
{
    if(position < 0 || position >= children.size())
    {
        return false;
    }
    delete children.takeAt(position);
    return true;
}

int CameraParamItem::myPosition()
{
    if(parentItem)
    {
        return parentItem->children.indexOf(const_cast<CameraParamItem*>(this));
    }
    return 0;
}

bool CameraParamItem::setData(const QVariant& data)
{
    itemData = data;
    return true;
}
