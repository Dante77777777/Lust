#include "CameraParamModel.h"
#include <QDebug>
CameraParamModel::CameraParamModel(const QStringList& headers, QObject* parent)
    :QAbstractItemModel(parent)
    , modelHeaders(headers)
    , rootItem(new CameraParamItem(QVariant()))
{
}

CameraParamModel::~CameraParamModel()
{
    delete rootItem;
}

void CameraParamModel::addCameraParam(CameraParam& param)
{
    CameraParamItem* curGroupRoot = rootItem;
    QString curGroupName = param.group();
    auto it = groups.find(curGroupName);
    if(it != groups.end())
    {
        curGroupRoot = it.value();
    }
    else
    {
        auto newGroup = curGroupRoot->insertChild(curGroupRoot->childCount());
        CameraParam info = CameraParamMetaInfo {"root" , curGroupName, UNKNOWN, "", ""};
        newGroup->setData(QVariant::fromValue(info));
        groups[curGroupName] = newGroup;
        curGroupRoot = newGroup;
    }
    auto newChild = curGroupRoot->insertChild(curGroupRoot->childCount());
    newChild->setData(QVariant::fromValue(param));
}

void CameraParamModel::clear()
{
    groups.clear();
    delete rootItem;
    rootItem = new CameraParamItem(QVariant());
}

QVariant CameraParamModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
    {
        return QVariant();
    }

    CameraParamItem* item = getItem(index);
    QVariant varParam = item->getData();
    CameraParam param = varParam.value<CameraParam>();
    switch (role) {
    case ItemRoles::ParamRole:
        return varParam;
        break;
    case Qt::DisplayRole:

        if(index.column() == ColType::NAME)
        {
            return param.name();
        }
        else if(index.column() == ColType::VALUE)
        {
            if(groups.keys().contains(param.name()))
            {
                return "";
            }
            return param.displayText();
        }
        break;
    case ItemRoles::ParamDescriptionRole:
        return param.tips();
        break;
    default:
        break;
    }
    return QVariant();
}

QVariant CameraParamModel::headerData(int section, Qt::Orientation oritentation, int role) const
{
    if(oritentation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return modelHeaders.at(section);
    }
    return QVariant();
}

QModelIndex CameraParamModel::index(int row, int column, const QModelIndex& parent) const
{
    CameraParamItem* parentItem = getItem(parent);
    if(!parentItem)
    {
        return QModelIndex();
    }
    CameraParamItem* childItem = parentItem->child(row);
    if(childItem)
    {
        return createIndex(row,column,childItem);
    }
    return QModelIndex();
}

QModelIndex CameraParamModel::parent(const QModelIndex& index) const
{
    if(!index.isValid())
    {
        return QModelIndex();
    }
    CameraParamItem* childItem = getItem(index);
    CameraParamItem* parentItem = childItem ? childItem->getParent() : nullptr;

    if(!parentItem || parentItem == rootItem)
    {
        return QModelIndex();
    }
    return createIndex(parentItem->myPosition(),0,parentItem);
}

int CameraParamModel::rowCount(const QModelIndex& parent) const
{
    const CameraParamItem* parentItem = getItem(parent);
    return parentItem ? parentItem->childCount() : 0;
}

int CameraParamModel::columnCount(const QModelIndex& parant) const
{
    //Q_UNUSED(parent);
    return ColType::VALUE+1;
}

Qt::ItemFlags CameraParamModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    if(index.column() == ColType::NAME)
    {
        return Qt::ItemIsEnabled | QAbstractItemModel::flags(index);
    }

    auto varParam = data(index,ItemRoles::ParamRole);
    CameraParam paramData = varParam.value<CameraParam>();
    //qDebug() << paramData.name();
    if(paramData.isWriteable())
    {
        return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
    }
    else
    {
        return Qt::ItemIsEnabled | QAbstractItemModel::flags(index);
    }
}

bool CameraParamModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(!index.isValid())
    {
        return false;
    }
    CameraParamItem* item = getItem(index);

    if(role == ItemRoles::ParamRole)
    {
        if(item->setData(value))
        {
            emit dataChanged(index,index,{Qt::DisplayRole, Qt::EditRole});
            emit sigValueChanged(index);
            return true;
        }
    }
    return false;
}

CameraParamItem* CameraParamModel::getItem(const QModelIndex& index) const
{
    if(index.isValid())
    {
        CameraParamItem* item = static_cast<CameraParamItem*>(index.internalPointer());
        if(item)
        {
            return item;
        }
    }
    return rootItem;
}
