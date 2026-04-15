#ifndef CAMERAPARAMMODEL_H
#define CAMERAPARAMMODEL_H

#include <QVariant>
#include <QModelIndex>
#include <QVector>
#include <QAbstractItemModel>
#include <QMap>
#include "CameraParamItem.h"
#include "../CameraInterface/CMCameraParam.h"

class CameraParamItem;
class CameraParam;

class CameraParamModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum ColType
    {
        NAME = 0,
        VALUE
    };

    enum ItemRoles
    {
        ParamRole = Qt::UserRole + 1,
        ParamDescriptionRole = Qt::UserRole + 2,
    };

    CameraParamModel(const QStringList& headers, QObject* parent = nullptr);
    ~CameraParamModel();

    void addCameraParam(CameraParam& param);
    void clear();

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation oritentation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parant = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

signals:
    void sigValueChanged(const QModelIndex& index);

protected:
    CameraParamItem* getItem(const QModelIndex& index) const;

private:
    CameraParamItem* rootItem;
    QStringList modelHeaders;
    QMap<QString, CameraParamItem*> groups;
};
































#endif
