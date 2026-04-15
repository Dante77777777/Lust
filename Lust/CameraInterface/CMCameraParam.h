#ifndef CMCAMERAPARAM_H
#define CMCAMERAPARAM_H

#include <QString>
#include <QVector>
#include <QVariant>

enum CMParamType
{
    UNKNOWN = 0,
    INT,
    DOUBLE,
    ENUM,
    BOOL,
    CMD,
    STRING
};

struct CameraParamMetaInfo
{
    QString group;
    QString name;
    CMParamType type{ UNKNOWN };
    QString relative_list;
    QString tips;
};

struct CMParam
{
    virtual ~CMParam() = default;
    virtual CMParam* clone() = 0;
};

struct IntParam : public CMParam
{
    int64_t value{};
    int64_t min{};
    int64_t max{};
    int64_t increment {};

    CMParam* clone() override
    {
        return new IntParam(*this);
    }
};

struct DoubleParam : public CMParam
{
    double value{};
    double min{};
    double max{};
    CMParam* clone() override
    {
        return new DoubleParam(*this);
    }
};

struct BoolParam : public CMParam
{
    bool value{};
    CMParam* clone() override
    {
        return new BoolParam(*this);
    }
};

struct StringParam : public CMParam
{
    QString value{};
    unsigned int nMaxLength {};
    CMParam* clone() override
    {
        return new StringParam(*this);
    }
};

struct EnumParam : public CMParam
{
    QString value{};
    QVector<QString> availableValue;
    int valueInt {};
    QVector<int> availableInt;

    CMParam* clone() override
    {
        return new EnumParam(*this);
    }
};

struct CmdParam : public CMParam
{
    CMParam* clone() override
    {
        return new CmdParam(*this);
    }
};


class CameraParam
{
public:
    CameraParam()
    {

    }

    CameraParam(CameraParamMetaInfo meta)
        : _meta(meta)
    {

    }

    CameraParam(const CameraParam& param)
    {
        _meta = param._meta;
        _value = param._value;
        _accessMode = param._accessMode;
    }

    QVariant getValue() const
    {
        return _value;
    }

    void setValue(const QVariant& value)
    {
        _value = value;
    }

    void reset(CameraParamMetaInfo meta)
    {
        _meta = meta;
    }

    const QString& name() const
    {
        return _meta.name;
    }

    const QString& group() const
    {
        return _meta.group;
    }

    const QString& relativeList() const
    {
        return _meta.relative_list;
    }

    CMParamType type() const
    {
        return _meta.type;
    }

    const QString& tips() const
    {
        return _meta.tips;
    }

    bool isValid()
    {
        return _accessMode.valid;
    }
    bool isReadable()
    {
        return _accessMode.readable;
    }
    bool isWriteable()
    {
        return _accessMode.writeable;
    }

    void setValid(bool valid)
    {
        _accessMode.valid = valid;
    }
    void setReadable(bool readable)
    {
        _accessMode.readable = readable;
    }
    void setWriteable(bool writeable)
    {
        _accessMode.writeable = writeable;
    }

    QString displayText() const
    {
        switch(type())
        {
        case STRING: {
            StringParam varParam = getValue().value<StringParam>();
            return varParam.value;
        }
        case CMD: {
            return "{Commond}";
        }
        case INT: {
            IntParam varParam = getValue().value<IntParam>();
            return QString::number(varParam.value);
        }
        case DOUBLE: {
            DoubleParam varParam = getValue().value<DoubleParam>();
            return QString::number(varParam.value);
        }
        case BOOL: {
            BoolParam varParam = getValue().value<BoolParam>();
            return varParam.value ? "True" : "False";
        }
        case ENUM: {
            EnumParam varParam = getValue().value<EnumParam>();
            return varParam.value;
        }
        default:
            break;
        }
        return QString("unknow");
    }
private:
    CameraParamMetaInfo _meta;
    QVariant _value;
    struct
    {
        bool valid : 1;
        bool readable : 1;
        bool writeable : 1;
    } _accessMode{};
};

#endif
























































