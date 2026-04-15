#ifndef PARSEUIJSON_H
#define PARSEUIJSON_H

#include "../CameraInterface/CMCameraParam.h"
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QList>
#include <QMutex>
#include <QObject>


class ParseUiJson : public QObject
{
    Q_OBJECT

public:
    static ParseUiJson* getInstance();
    // 从文件解析JSON
    bool loadFromFile(const QString& filePath);
    // 从字符串解析JSON
    bool loadFromString(const QString& jsonString);
    // 从QByteArray解析JSON
    bool loadFromByteArray(const QByteArray& jsonData);
    // 获取解析后的参数列表
    QList<CameraParamMetaInfo> getParamList() const;
    // 获取指定分组的参数列表
    QList<CameraParamMetaInfo> getParamListByGroup(const QString& group) const;
    // 获取所有分组名称
    QStringList getAllGroups() const;

    void clear();

    QString getLastError() const { return lastError; }

    bool isValid() const { return valid; }
signals:
    void parseFinished(bool success, const QString& error);

    void parseProgress(int current, int total);

private:
    explicit ParseUiJson(QObject* parent = nullptr);

    ~ParseUiJson();

    ParseUiJson(const ParseUiJson&) = delete;

    ParseUiJson& operator=(const ParseUiJson&) = delete;

    bool parseJson(const QByteArray& jsonData);

    CMParamType stringToParamType(const QString& typeStr) const;

    bool validateParamObject(const QJsonObject& paramObj, const QString& groupName, int index);

private:
    QString lastError;
    QList<CameraParamMetaInfo> paramList;
    bool valid;
};

#endif
