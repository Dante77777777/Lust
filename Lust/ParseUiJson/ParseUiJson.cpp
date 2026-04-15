#include "ParseUiJson.h"

const QString kGroup = "group";
const QString kParams = "params";
const QString kName = "name";
const QString kType = "type";
const QString kRelativeList = "relative_list";
const QString kTips = "tips";

const QString kInt = "INT";
const QString kDouble = "DOUBLE";
const QString kEnum = "ENUM";
const QString kBool = "BOOL";
const QString kCmd = "CMD";
const QString kString = "STRING";

ParseUiJson* ParseUiJson::getInstance()
{
    static ParseUiJson instance;
    return &instance;
}

ParseUiJson::ParseUiJson(QObject* parent)
    : QObject(parent)
    , valid(false)
{

}

ParseUiJson::~ParseUiJson()
{

}

bool ParseUiJson::loadFromFile(const QString& filepath)
{
    QFile file(filepath);
    qDebug("loadFromFile");
    if(!file.exists())
    {
        lastError = QString("文件不存在: %1").arg(filepath);
        valid = false;
        emit parseFinished(false,lastError);
        return false;
    }
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        lastError = QString("文件无法打开: %1").arg(filepath);
        valid = false;
        emit parseFinished(false,lastError);
        return false;
    }
    QByteArray jsonData = file.readAll();
    file.close();

    return loadFromByteArray(jsonData);
}

bool ParseUiJson::loadFromString(const QString& jsonString)
{
    return loadFromByteArray(jsonString.toUtf8());
}

bool ParseUiJson::loadFromByteArray(const QByteArray& jsonData)
{
    clear();
    bool success = parseJson(jsonData);
    emit parseFinished(success,lastError);
    return success;
}

bool ParseUiJson::parseJson(const QByteArray& jsonData)
{
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData,&parseError);

    if(parseError.error != QJsonParseError::NoError)
    {
        lastError = QString("JSON解析错误: %1 (位置: %2)").arg(parseError.errorString()).arg(parseError.offset);
        return false;
    }

    if(!jsonDoc.isArray())
    {
        lastError = QString("JSON根节点必须是数组");
        return false;
    }
    QJsonArray jsonArray = jsonDoc.array();
    int totalGroups = jsonArray.size();

    for(int i = 0; i < jsonArray.size(); ++i)
    {
        emit parseProgress(i,totalGroups);

        QJsonValue groupValue = jsonArray.at(i);
        if(!groupValue.isObject())
        {
            lastError = QString("第%1个元素不是对象格式").arg(i+1);
            return false;
        }

        QJsonObject groupObject = groupValue.toObject();
        if(!groupObject.contains(kGroup) || !groupObject[kGroup].isString())
        {
            lastError = QString("第%1个分组缺少group字段或group字段不是字符串").arg(i+1);
            return false;
        }

        QString groupName = groupObject[kGroup].toString();

        if(!groupObject.contains(kParams) || !groupObject[kParams].isArray())
        {
            lastError = QString("第%1个房租缺少param字段或param字段不是对象").arg(i+1);
            return false;
        }

        QJsonArray paramArray = groupObject[kParams].toArray();

        for(int j = 0; j < paramArray.size();++j)
        {
            QJsonValue paramValue = paramArray.at(j);
            if(!paramValue.isObject())
            {
                lastError = QString("分组'%1'的第%2个字段不是对象").arg(groupName).arg(j+1);
                return false;
            }

            QJsonObject paramObject = paramValue.toObject();

            if(!validateParamObject(paramObject,groupName,j))
            {
                return false;
            }

            CameraParamMetaInfo paramInfo;
            paramInfo.group = groupName;
            paramInfo.name = paramObject[kName].toString();
            paramInfo.relative_list = paramObject[kRelativeList].toString();
            paramInfo.tips = paramObject[kTips].toString();
            paramInfo.type = stringToParamType(paramObject[kType].toString());

            paramList.append(paramInfo);
        }
    }
    valid = true;
    return true;
}

bool ParseUiJson::validateParamObject(const QJsonObject& paramObj, const QString& groupName, int index)
{
    QStringList requiredFileds = {kName,kType,kTips};
    for(const auto& field : requiredFileds)
    {
        if(!paramObj.contains(field))
        {
            lastError = QString("分组'%1'的第%2个参数缺少字段%3").arg(groupName).arg(index).arg(field);
            return false;
        }
    }

    if(!paramObj[kName].isString())
    {
        lastError = QString("分组'%1'的第%2个参数的name字段必须是String类型").arg(groupName).arg(index);
        return false;
    }
    if(!paramObj[kTips].isString())
    {
        lastError = QString("分组'%1'的第%2个参数的tips字段必须是String类型").arg(groupName).arg(index);
        return false;
    }
    if(!paramObj[kType].isString())
    {
        lastError = QString("分组'%1'的第%2个参数的type字段必须是String类型").arg(groupName).arg(index);
        return false;
    }
    if(paramObj.contains(kRelativeList) && !paramObj[kRelativeList].isString())
    {
        lastError = QString("分组'%1'的第%2个参数的relative_list字段必须是String类型").arg(groupName).arg(index);
        return false;
    }
    return true;
}

CMParamType ParseUiJson::stringToParamType(const QString& paramType) const
{
    static QMap<QString, CMParamType> stringParamMap = { {kInt,INT}, {kDouble,DOUBLE}, {kEnum,ENUM}, {kCmd,CMD}, {kString,STRING}, {kBool,BOOL} };
    return stringParamMap.value(paramType,UNKNOWN);
}

QList<CameraParamMetaInfo> ParseUiJson::getParamList() const
{
    return paramList;
}

QList<CameraParamMetaInfo> ParseUiJson::getParamListByGroup(const QString& group) const
{
    QList<CameraParamMetaInfo> result;
    for(const auto& param : paramList)
    {
        if(param.group == group)
        {
            result.append(param);
        }
    }
    return result;
}


QStringList ParseUiJson::getAllGroups() const
{
    QStringList groups;
    for(const auto& param : paramList)
    {
        if(!groups.contains(param.group))
        {
            groups.contains(param.group);
        }
    }
    return groups;
}

void ParseUiJson::clear()
{
    paramList.clear();
    lastError.clear();
    valid = false;
}

