#ifndef CMCAMERAMETAINFO_H
#define CMCAMERAMETAINFO_H

#include <QString>

struct CMCameraMetaInfo
{
    QString Serial{};
    QString UserDefineID{};
    QString VenderName{};

    bool operator==(const CMCameraMetaInfo& info)
    {
        if(Serial == info.Serial)
        {
            return true;
        }
        return false;
    }
};

#endif
