#include "CameraParamDelegate.h"

CameraParamDelegate::CameraParamDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{

}

CameraParamDelegate::~CameraParamDelegate()
{

}

QWidget* CameraParamDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    auto createWidget = [](CameraParam& param, const QModelIndex& index, QWidget* parent) -> OneCustomWidget*
    {
        switch (param.type()) {
        case STRING:
            return new StringCustomWidget(param,index,parent);
            break;
        case CMD:
            return new CmdCustomWidget(param,index,parent);
            break;
        case BOOL:
            return new BoolCustomWidget(param,index,parent);
            break;
        case DOUBLE:
            return new DoubleCustomWidget(param,index,parent);
            break;
        case INT:
            return new IntCustomWidget(param,index,parent);
            break;
        case ENUM:
            return new EnumCustomWidget(param,index,parent);
            break;
        default:
            return nullptr;
            break;
        }
    };

    if(index.column() == CameraParamModel::ColType::VALUE)
    {
        const QVariant varParam = index.data(CameraParamModel::ParamRole);
        CameraParam paramData = varParam.value<CameraParam>();
        auto* oneCustomWidget = createWidget(paramData,index,parent);
        if(oneCustomWidget)
        {
            oneCustomWidget->InitWidget();
            connect(oneCustomWidget,&OneCustomWidget::sigValueChanged,this,&CameraParamDelegate::onValueChanged,Qt::UniqueConnection);
        }
        return oneCustomWidget;
    }
    return nullptr;
}

void CameraParamDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if(index.column() == CameraParamModel::ColType::VALUE)
    {
        OneCustomWidget* customWidget = qobject_cast<OneCustomWidget*>(editor);

        const QVariant varParam = index.data(CameraParamModel::ParamRole);
        CameraParam paramData = varParam.value<CameraParam>();

        customWidget->setParam(paramData);
    }
}

void CameraParamDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if(index.column() == CameraParamModel::ColType::VALUE)
    {
        OneCustomWidget* customWidget = qobject_cast<OneCustomWidget*>(editor);
        CameraParam paramData = customWidget->getParam();
        model->setData(index,QVariant::fromValue(paramData),CameraParamModel::ParamRole);
    }
}

void CameraParamDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    editor->setGeometry(option.rect);
}

void CameraParamDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->save();
    if(option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    QString text = index.data().toString();
    painter->drawText(option.rect,Qt::AlignLeft | Qt::AlignVCenter, text);

    QPen pen(QColor(220,220,220), 1, Qt::SolidLine);
    painter->setPen(pen);

    painter->drawLine(option.rect.topRight(),option.rect.bottomRight());
    painter->drawLine(option.rect.bottomLeft(),option.rect.bottomRight());

    painter->restore();
}

QSize CameraParamDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option,index);
    size.setHeight(20);
    return size;
}


void CameraParamDelegate::onValueChanged(const CameraParam& param, const QModelIndex& index)
{
    OneCustomWidget* customWidget = qobject_cast<OneCustomWidget*>(sender());
    if(customWidget)
    {
        if(index.isValid())
        {
            QAbstractItemModel* model = const_cast<QAbstractItemModel*>(index.model());
            if(model)
            {
                model->setData(index,QVariant::fromValue(param),CameraParamModel::ParamRole);
            }
        }
    }

}
