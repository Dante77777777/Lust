#include "HikCamera.h"
#include "opencv2/opencv.hpp"
#include <QDebug>

const QString HikCamera::VIRTUAL_CAMERA_VENDER = "Hikrobot";

bool IsColor(MvGvspPixelType enType)
{
    switch (enType) {
    case PixelType_Gvsp_BGR8_Packed:
    case PixelType_Gvsp_YUV422_Packed:
    case PixelType_Gvsp_YUV422_YUYV_Packed:
    case PixelType_Gvsp_BayerGR8:
    case PixelType_Gvsp_BayerRG8:
    case PixelType_Gvsp_BayerGB8:
    case PixelType_Gvsp_BayerBG8:
    case PixelType_Gvsp_BayerGB10:
    case PixelType_Gvsp_BayerGB10_Packed:
    case PixelType_Gvsp_BayerBG10:
    case PixelType_Gvsp_BayerBG10_Packed:
    case PixelType_Gvsp_BayerRG10:
    case PixelType_Gvsp_BayerRG10_Packed:
    case PixelType_Gvsp_BayerGR10:
    case PixelType_Gvsp_BayerGR10_Packed:
    case PixelType_Gvsp_BayerGB12:
    case PixelType_Gvsp_BayerGB12_Packed:
    case PixelType_Gvsp_BayerBG12:
    case PixelType_Gvsp_BayerBG12_Packed:
    case PixelType_Gvsp_BayerRG12:
    case PixelType_Gvsp_BayerRG12_Packed:
    case PixelType_Gvsp_BayerGR12:
    case PixelType_Gvsp_BayerGR12_Packed:
    case PixelType_Gvsp_BayerRBGG8:
    case PixelType_Gvsp_BayerGR16:
    case PixelType_Gvsp_BayerRG16:
    case PixelType_Gvsp_BayerGB16:
    case PixelType_Gvsp_BayerBG16:
        return true;
    default:
        return false;
    }
}

// 判断像素格式是否为黑白Mono
bool IsMono(MvGvspPixelType enType)
{
    switch (enType) {
    case PixelType_Gvsp_Mono8:
    case PixelType_Gvsp_Mono10:
    case PixelType_Gvsp_Mono10_Packed:
    case PixelType_Gvsp_Mono12:
    case PixelType_Gvsp_Mono12_Packed:
    case PixelType_Gvsp_Mono14:
    case PixelType_Gvsp_Mono16:
        return true;
    default:
        return false;
    }
}

bool HikConvert2Mat(void* handle, MV_FRAME_OUT_INFO_EX* pstImageInfo, unsigned char* pData, cv::Mat& dstImage)
{
    if(pstImageInfo == NULL || pData == NULL)
    {
        printf("NULL info or data.\n");
        return false;
    }
    MvGvspPixelType enDstPixelType = PixelType_Gvsp_Undefined;

    if(IsMono(pstImageInfo->enPixelType))
    {
        enDstPixelType = PixelType_Gvsp_Mono8;
    }
    else if(IsColor(pstImageInfo->enPixelType))
    {
        enDstPixelType = PixelType_Gvsp_RGB8_Packed;
    }

    if(enDstPixelType != PixelType_Gvsp_Undefined)
    {
        unsigned char* pConvertData = NULL;
        unsigned int nConvertDataSize = 0;
        int nChannelNum = 3;
        pConvertData = (unsigned char*)malloc(pstImageInfo->nWidth * pstImageInfo->nHeight * nChannelNum);
        if(pConvertData == NULL)
        {
            qDebug() << "malloc pConvertData fail!";
            return false;
        }

        nConvertDataSize = pstImageInfo->nWidth*pstImageInfo->nHeight*nChannelNum;

        MV_CC_PIXEL_CONVERT_PARAM stConvertParam = {0};

        stConvertParam.nWidth = pstImageInfo->nWidth;
        stConvertParam.nHeight = pstImageInfo->nHeight;
        stConvertParam.pSrcData = pData;
        stConvertParam.nSrcDataLen = pstImageInfo->nFrameLen;
        stConvertParam.enSrcPixelType = pstImageInfo->enPixelType;
        stConvertParam.enDstPixelType = enDstPixelType;
        stConvertParam.pDstBuffer = pConvertData;
        stConvertParam.nDstBufferSize = nConvertDataSize;

        auto nRet = MV_CC_ConvertPixelType(handle,&stConvertParam);
        if(nRet != MV_OK)
        {
            qDebug() << "Coonvert Pixel Type fail!";
            return false;
        }

        if(IsMono(pstImageInfo->enPixelType))
        {
            dstImage = cv::Mat(pstImageInfo->nHeight,pstImageInfo->nWidth,CV_8UC1,pConvertData);
        }
        else if(IsColor(pstImageInfo->enPixelType))
        {
            dstImage = cv::Mat(pstImageInfo->nHeight,pstImageInfo->nWidth,CV_8UC3,pConvertData);
        }
    }
    else
    {
        qDebug() << "Unsupported pixel format!";
        return false;
    }
    return true;
}

void __stdcall ImageCallBack(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser)
{
    if(!pFrameInfo || !pData)
    {
        return ;
    }
    HikCamera* pCamera = static_cast<HikCamera*>(pUser);
    if(!pCamera)
    {
        return ;
    }

    cv::Mat cvImage;
    HikConvert2Mat(pCamera->CameraHandle(),pFrameInfo,pData,cvImage);

    pCamera->getImageQueue().push(cvImage,100);
    qDebug() << "Get One Frame: Width:" << pFrameInfo->nWidth << " Height:" << pFrameInfo->nHeight;
}

HikCamera::HikCamera(const CMCameraMetaInfo& info) : CameraInterface(info)
{

}


HikCamera::~HikCamera()
{

}


uint32_t HikCamera::EnumCamera(QVector<CMCameraMetaInfo>& cameraInfos)
{
    // static const int sdkInitResult = MV_CC_Initialize();
    // if (sdkInitResult != MV_OK) {
    //     qWarning() << "MV_CC_Initialize failed, error=0x"
    //                << Qt::hex << static_cast<unsigned int>(sdkInitResult) << Qt::dec;
    //     return CAMERA_ACQUIRE_FAILED;
    // }

    // qDebug() << "Calling MV_CC_EnumDevices for Hikrobot cameras";
    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    auto nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE,&stDeviceList);
    if(MV_OK != nRet)
    {
        qWarning() << "MV_CC_EnumDevices failed, error=0x"
                   << Qt::hex << static_cast<unsigned int>(nRet) << Qt::dec;
        return CAMERA_ACQUIRE_FAILED;
    }

    qDebug() << "MV_CC_EnumDevices found" << stDeviceList.nDeviceNum << "Hikrobot device(s)";

    for(unsigned int i = 0; i < stDeviceList.nDeviceNum; i++)
    {
        MV_CC_DEVICE_INFO* cameraInfo = stDeviceList.pDeviceInfo[i];
        if(cameraInfo->nTLayerType == MV_GIGE_DEVICE)
        {
            CMCameraMetaInfo info;
            info.Serial = (char*)cameraInfo->SpecialInfo.stGigEInfo.chSerialNumber;
            info.UserDefineID = (char*)cameraInfo->SpecialInfo.stGigEInfo.chUserDefinedName;
            info.VenderName = (char*)cameraInfo->SpecialInfo.stGigEInfo.chManufacturerName;
            cameraInfos.push_back(info);
        }
        else if(cameraInfo->nTLayerType == MV_USB_DEVICE)
        {
            CMCameraMetaInfo info;
            info.Serial = (char*)cameraInfo->SpecialInfo.stUsb3VInfo.chSerialNumber;
            info.UserDefineID = (char*)cameraInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName;
            info.VenderName = (char*)cameraInfo->SpecialInfo.stUsb3VInfo.chManufacturerName;
            cameraInfos.push_back(info);
        }
    }
    return CHONGMING_OK;
}

uint32_t HikCamera::getParamList(QVector<CameraParam>& paramList)
{
    ParseUiJson* parser = ParseUiJson::getInstance();
    parser->loadFromFile(":/resource/HikCameraParam.json");
    QList<CameraParamMetaInfo> paramMetaInfos = parser->getParamList();
    for(CameraParamMetaInfo info : paramMetaInfos)
    {
        paramList.push_back(info);
    }
    return CHONGMING_OK;
}


bool HikCamera::isConnect()
{
    if(m_cameraHandle == NULL)
    {
        return false;
    }
    return MV_CC_IsDeviceConnected(m_cameraHandle);
}

bool HikCamera::isGrabbing()
{
    if(m_cameraHandle == NULL)
    {
        return false;
    }
    return isStartGrabbing;
}

uint32_t HikCamera::acquire()
{
    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList,0,sizeof(MV_CC_DEVICE_INFO_LIST));
    auto nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
    if(MV_OK != nRet)
    {
        return CAMERA_ACQUIRE_FAILED;
    }

    for(int i = 0; i < stDeviceList.nDeviceNum;i++)
    {
        MV_CC_DEVICE_INFO* cameraInfo = stDeviceList.pDeviceInfo[i];
        QString serial  = (char*)cameraInfo->SpecialInfo.stGigEInfo.chSerialNumber;
        if(serial == Serial())
        {
            m_pDeviceInfo = cameraInfo;
            break;
        }
    }

    if(m_pDeviceInfo == NULL)
    {
        return CAMERA_ACQUIRE_FAILED;
    }

    nRet = MV_CC_CreateHandle(&m_cameraHandle, m_pDeviceInfo);
    if(MV_OK != nRet)
    {
        return INVALID_CAMERA_HANDLE;
    }

    return CHONGMING_OK;
}


uint32_t HikCamera::release()
{
    MV_CC_DestroyHandle(m_cameraHandle);
    m_cameraHandle = NULL;

    m_pDeviceInfo = NULL;
    return CHONGMING_OK;
}


uint32_t HikCamera::connect()
{
    if(m_cameraHandle == NULL)
    {
        return INVALID_CAMERA_HANDLE;
    }

    if(MV_CC_IsDeviceAccessible(m_pDeviceInfo,MV_ACCESS_Exclusive) == false)
    {
        return DEVICE_NOT_ACCESSIBLE;
    }

    auto nRet = MV_CC_OpenDevice(m_cameraHandle);
    if(MV_OK != nRet)
    {
        return CONNECT_ERROR;
    }

    nRet = MV_CC_RegisterImageCallBackEx(m_cameraHandle,ImageCallBack,this);
    if(MV_OK != nRet)
    {
        return CONNECT_ERROR;
    }
    return CHONGMING_OK;

}


uint32_t HikCamera::disconnect()
{
    if(m_cameraHandle == NULL)
    {
        return INVALID_CAMERA_HANDLE;
    }

    auto nRet = MV_CC_RegisterImageCallBackEx(m_cameraHandle,NULL,this);

    if(nRet != MV_OK)
    {
        return DISCONNECT_ERROR;
    }

    nRet = MV_CC_CloseDevice(m_cameraHandle);
    if(nRet != MV_OK)
    {
        return DISCONNECT_ERROR;
    }
    return CHONGMING_OK;
}


uint32_t HikCamera::creatStream()
{
    if(m_cameraHandle == NULL)
    {
        return INVALID_CAMERA_HANDLE;
    }

    MV_CC_SetGrabStrategy(m_cameraHandle,MV_GRAB_STRATEGY::MV_GrabStrategy_OneByOne);
    MV_CC_SetImageNodeNum(m_cameraHandle,ImageQueueSize);
    return CHONGMING_OK;
}


uint32_t HikCamera::destroyStream()
{
    return CHONGMING_OK;
}


uint32_t HikCamera::startGrabbing()
{
    if(m_cameraHandle == NULL)
    {
        return INVALID_CAMERA_HANDLE;
    }

    auto nRet = MV_CC_StartGrabbing(m_cameraHandle);
    if(MV_OK != nRet)
    {
        return STARTGRAB_ERROR;
    }
    isStartGrabbing = true;
    return CHONGMING_OK;
}


uint32_t HikCamera::stopGrabbing()
{
    if(m_cameraHandle == NULL)
    {
        return INVALID_CAMERA_HANDLE;
    }

    auto nRet = MV_CC_StopGrabbing(m_cameraHandle);

    if(MV_OK != nRet)
    {
        return STOPGRAB_ERROR;
    }
    isStartGrabbing = false;
    return CHONGMING_OK;
}


uint32_t HikCamera::loadConfig(const QString path)
{
    if(m_cameraHandle == NULL)
    {
        return INVALID_CAMERA_HANDLE;
    }

    auto nRet = MV_CC_FeatureLoad(m_cameraHandle,path.toLocal8Bit().data());
    if(MV_OK != nRet)
    {
        return CAMERA_CONFIG_LOAD_FAILED;
    }
    return CHONGMING_OK;
}


uint32_t HikCamera::saveConfig(const QString path)
{
    if(m_cameraHandle == NULL)
    {
        return INVALID_CAMERA_HANDLE;
    }

    auto nRet = MV_CC_FeatureSave(m_cameraHandle,path.toLocal8Bit().data());

    if(nRet != MV_OK)
    {
        return CAMERA_CONFIG_SAVE_FAILED;
    }
    return CHONGMING_OK;
}


QString HikCamera::configFormat()
{
    return "mfs";
}


uint32_t HikCamera::readParam(CameraParam& param)
{
    getFeatureAccessMode(param);
    if(param.isReadable() == false)
    {
        return CHONGMING_OK;
    }

    if(param.type() == INT)
    {
        QString name = param.name();
        MVCC_INTVALUE value {};
        auto nRet = MV_CC_GetIntValue(m_cameraHandle,name.toLocal8Bit().data(),&value);
        if(nRet != MV_OK)
        {
            return WRITE_PARAM_FAILED;
        }

        IntParam varParam = param.getValue().value<IntParam>();
        varParam.value = value.nCurValue;
        varParam.increment = value.nInc;
        varParam.min = value.nMin;
        varParam.max = value.nMax;
        param.setValue(QVariant::fromValue(varParam));
    }
    else if(param.type() == DOUBLE)
    {
        QString name = param.name();
        MVCC_FLOATVALUE value{};
        auto nRet = MV_CC_GetFloatValue(m_cameraHandle,name.toLocal8Bit().data(),&value);
        if(nRet != MV_OK)
        {
            return WRITE_PARAM_FAILED;
        }

        DoubleParam varParam = param.getValue().value<DoubleParam>();

        varParam.value = value.fCurValue;
        varParam.min = value.fMin;
        varParam.max = value.fMax;
        param.setValue(QVariant::fromValue(varParam));
    }
    else if(param.type() == ENUM)
    {
        QString name = param.name();
        MVCC_ENUMVALUE value{};
        memset(&value,0,sizeof(MVCC_ENUMVALUE));

        auto nRet = MV_CC_GetEnumValue(m_cameraHandle,name.toLocal8Bit().data(),&value);
        if(nRet != MV_OK)
        {
            return WRITE_PARAM_FAILED;
        }

        MVCC_ENUMENTRY entryValue {};
        memset(&entryValue,0,sizeof(MVCC_ENUMENTRY));
        entryValue.nValue = value.nCurValue;
        MV_CC_GetEnumEntrySymbolic(m_cameraHandle,name.toLocal8Bit().data(),&entryValue);

        QVector<int> intlist {};
        QVector<QString> strlist {};
        for(unsigned int i = 0; i < value.nSupportedNum; i++)
        {
            int curInt = value.nSupportValue[i];
            intlist.push_back(curInt);

            MVCC_ENUMENTRY entryValue {};
            memset(&entryValue,0,sizeof(MVCC_ENUMENTRY));
            entryValue.nValue = curInt;
            MV_CC_GetEnumEntrySymbolic(m_cameraHandle,name.toLocal8Bit().data(),&entryValue);
            QString curStr = entryValue.chSymbolic;
            strlist.push_back(curStr);
        }

        EnumParam varParam = param.getValue().value<EnumParam>();
        varParam.value = entryValue.chSymbolic;
        varParam.valueInt = value.nCurValue;
        varParam.availableInt = intlist;
        varParam.availableValue = strlist;
        param.setValue(QVariant::fromValue(varParam));
    }
    else if(param.type() == BOOL)
    {
        QString name = param.name();
        bool bValue {};
        auto nRet = MV_CC_GetBoolValue(m_cameraHandle,name.toLocal8Bit().data(),&bValue);
        if(nRet != MV_OK)
        {
            return WRITE_PARAM_FAILED;
        }

        BoolParam varParam = param.getValue().value<BoolParam>();
        varParam.value = bValue;
        param.setValue(QVariant::fromValue(varParam));
    }
    else if(param.type() == CMD)
    {

    }
    else if(param.type() == STRING)
    {
        QString name = param.name();
        MVCC_STRINGVALUE value {};
        auto nRet = MV_CC_GetStringValue(m_cameraHandle,name.toLocal8Bit().data(),&value);

        if(nRet != MV_OK)
        {
            return WRITE_PARAM_FAILED;
        }

        StringParam varParam = param.getValue().value<StringParam>();
        varParam.value = value.chCurValue;
        varParam.nMaxLength = value.nMaxLength;
        param.setValue(QVariant::fromValue(varParam));
    }

    return CHONGMING_OK;
}


uint32_t HikCamera::writeParam(CameraParam& param)
{
    getFeatureAccessMode(param);
    if(param.isWriteable() == false)
    {
        return CHONGMING_OK;
    }

    if(param.type() == INT)
    {
        QString name = param.name();
        IntParam varValue = param.getValue().value<IntParam>();
        auto nRet = MV_CC_SetIntValue(m_cameraHandle,name.toLocal8Bit().data(),varValue.value);
        if(nRet != MV_OK)
        {
            return WRITE_PARAM_FAILED;
        }
    }
    else if(param.type() == DOUBLE)
    {
        QString name = param.name();
        DoubleParam varValue = param.getValue().value<DoubleParam>();
        float value = varValue.value;
        auto nRet = MV_CC_SetFloatValue(m_cameraHandle,name.toLocal8Bit().data(),value);
        if(nRet != MV_OK)
        {
            return WRITE_PARAM_FAILED;
        }
    }
    else if(param.type() == ENUM)
    {
        QString name = param.name();
        EnumParam varValue = param.getValue().value<EnumParam>();
        int value = varValue.valueInt;
        auto nRet = MV_CC_SetEnumValue(m_cameraHandle,name.toLocal8Bit().data(),value);
        if(nRet != MV_OK)
        {
            return WRITE_PARAM_FAILED;
        }
    }
    else if(param.type() == BOOL)
    {
        QString name = param.name();
        BoolParam varValue = param.getValue().value<BoolParam>();
        bool value = varValue.value;
        auto nRet = MV_CC_SetBoolValue(m_cameraHandle,name.toLocal8Bit().data(),value);
        if(nRet != MV_OK)
        {
            return WRITE_PARAM_FAILED;
        }
    }
    else if(param.type() == CMD)
    {
        QString name = param.name();
        auto nRet = MV_CC_SetCommandValue(m_cameraHandle,name.toLocal8Bit().data());
        if(nRet != MV_OK)
        {
            return WRITE_PARAM_FAILED;
        }
    }
    else if(param.type() == STRING)
    {
        QString name = param.name();
        StringParam varValue = param.getValue().value<StringParam>();
        QString value = varValue.value;
        auto nRet = MV_CC_SetStringValue(m_cameraHandle,name.toLocal8Bit().data(),value.toLocal8Bit().data());
        if(nRet != MV_OK)
        {
            return WRITE_PARAM_FAILED;
        }
    }
    return CHONGMING_OK;
}


uint32_t HikCamera::getImageLast(cv::Mat& image)
{
    auto ret = imageQueue.pop(image,100);
    if(ret != CHONGMING_OK)
    {
        return GETIMAGE_TIMEOUT;
    }
    return CHONGMING_OK;
}


uint32_t HikCamera::getFeatureAccessMode(CameraParam& param)
{
    QString name = param.name();
    MV_XML_AccessMode mode;
    MV_XML_GetNodeAccessMode(m_cameraHandle,name.toLocal8Bit().data(),&mode);
    if(mode == AM_NI || mode == AM_NA || mode == AM_Undefined)
    {
        param.setValid(false);
        param.setReadable(false);
        param.setWriteable(false);
    }
    else if(mode == AM_WO)
    {
        param.setValid(true);
        param.setReadable(false);
        param.setWriteable(true);
    }
    else if(mode == AM_RO)
    {
        param.setValid(true);
        param.setReadable(true);
        param.setWriteable(false);
    }
    else if(mode == AM_RW)
    {
        param.setValid(true);
        param.setReadable(true);
        param.setWriteable(true);
    }
    return CHONGMING_OK;
}
