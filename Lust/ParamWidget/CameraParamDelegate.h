#ifndef CAMERAPARAMDELEGATE_H
#define CAMERAPARAMDELEGATE_H

#include "../CameraInterface/CMCameraParam.h"
#include "CameraParamModel.h"
#include "CustomWidget/BoolCustomWidget.h"
#include "CustomWidget/CmdCustomWidget.h"
#include "CustomWidget/DoubleCustomWidget.h"
#include "CustomWidget/EnumCustomWidget.h"
#include "CustomWidget/IntCustomWidget.h"
#include "CustomWidget/StringCustomWidget.h"
#include "OneCustomWidget.h"
#include "CameraParamItem.h"
#include <QModelIndex>
#include <QPainter>
#include <QWidget>
#include <QStyledItemDelegate>

class CameraParamDelegate : public QStyledItemDelegate
{
public:
    CameraParamDelegate(QObject* parent = Q_NULLPTR);
    virtual ~CameraParamDelegate();

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;

    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    // 更新编辑器样式
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    // 绘制样式
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    // 设置默认尺寸
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

protected slots:
    void onValueChanged(const CameraParam& param, const QModelIndex& index);
};






























#endif
