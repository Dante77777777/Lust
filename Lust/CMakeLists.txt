cmake_minimum_required(VERSION 3.19)
project(Lust LANGUAGES CXX)

include(CTest)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)
find_package(OpenCV 4.12.0 REQUIRED COMPONENTS core imgproc highgui imgcodecs)
qt_standard_project_setup()

if(BUILD_TESTING)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Test Network)
endif()

qt_add_executable(Lust
    WIN32 MACOSX_BUNDLE
    ControlWidget/ControlWidget.h
    ControlWidget/ControlWidget.cpp
    ControlWidget/ControlWidgetUI.h

    LoadingDialog/LoadingDialog.h
    LoadingDialog/LoadingDialog.cpp
    LoadingDialog/LoadingDialogUI.h

    ParamWidget/ParamWidget.h
    ParamWidget/ParamWidget.cpp
    ParamWidget/CustomWidget/BoolCustomWidget.h
    ParamWidget/CustomWidget/BoolCustomWidget.cpp
    ParamWidget/OneCustomWidget.h
    ParamWidget/OneCustomWidget.cpp
    ParamWidget/CameraParamItem.h
    ParamWidget/CameraParamItem.cpp
    ParamWidget/CameraParamModel.h
    ParamWidget/CameraParamModel.cpp
    ParamWidget/CameraParamDelegate.h
    ParamWidget/CameraParamDelegate.cpp
    ParamWidget/ParamWidgetUI.h
    ParamWidget/CustomWidget/CmdCustomWidget.h
    ParamWidget/CustomWidget/CmdCustomWidget.cpp
    ParamWidget/CustomWidget/DoubleCustomWidget.h
    ParamWidget/CustomWidget/DoubleCustomWidget.cpp
    ParamWidget/CustomWidget/EnumCustomWidget.h
    ParamWidget/CustomWidget/EnumCustomWidget.cpp
    ParamWidget/CustomWidget/IntCustomWidget.h
    ParamWidget/CustomWidget/IntCustomWidget.cpp
    ParamWidget/CustomWidget/StringCustomWidget.h
    ParamWidget/CustomWidget/StringCustomWidget.cpp

    main.cpp
    mainwindow.h
    mainwindow.cpp
    Listener.h
    Listener.cpp

    ViewWidget/imageitem.h ViewWidget/imageitem.cpp
    ViewWidget/graphicsview.h ViewWidget/graphicsview.cpp
    ViewWidget/viewwidgetui.h
    ViewWidget/viewwidget.h ViewWidget/viewwidget.cpp
    ViewWidget/AcquireImageProcess.h
    ViewWidget/AcquireImageProcess.cpp

    CameraInterface/CMCameraParam.h
    CameraInterface/CameraError.h
    CameraInterface/CMCameraMetaInfo.h
    CameraInterface/CameraImageQueue.h
    CameraInterface/CameraImageQueue.cpp
    CameraInterface/CameraInterface.h
    CameraInterface/CameraContext.h
    CameraInterface/CameraContext.cpp

    CameraFactory/CameraFactory.h
    CameraFactory/CameraFactory.cpp
    CameraFactory/VirtualCamera.h
    CameraFactory/VirtualCamera.cpp
    CameraFactory/HikCamera.h
    CameraFactory/HikCamera.cpp

    Protocol/FrameProtocol.h
    Protocol/FrameProtocol.cpp
    Protocol/ImagePacketEncoder.h
    Protocol/ImagePacketEncoder.cpp
    Protocol/InferenceResponseCodec.h
    Protocol/InferenceResponseCodec.cpp
    Protocol/InferenceResponseDispatcher.h
    Protocol/InferenceResponseDispatcher.cpp

    Transport/TcpClient.h
    Transport/TcpClient.cpp
    Transport/ResponseStreamDecoder.h
    Transport/ResponseStreamDecoder.cpp

    Utils/ImageConver.h

    ParseUiJson/ParseUiJson.h
    ParseUiJson/ParseUiJson.cpp
    resource.qrc
)

set(RESOURCE resource.qrc)

qt_add_resources(APP_RESOURCE ${RESOURCE})
target_sources(Lust PRIVATE ${APP_RESOURCE})

target_link_libraries(Lust
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
)

target_include_directories(Lust PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(Lust PRIVATE ${OpenCV_LIBS})

if(BUILD_TESTING)
    qt_add_executable(CameraImageQueueTest
        tests/CameraImageQueueTest.cpp

        CameraInterface/CameraImageQueue.h
        CameraInterface/CameraImageQueue.cpp
        CameraInterface/CameraError.h
    )

    target_include_directories(CameraImageQueueTest
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${OpenCV_INCLUDE_DIRS}
    )

    target_link_libraries(CameraImageQueueTest
        PRIVATE
            Qt6::Core
            Qt6::Test
            ${OpenCV_LIBS}
    )

    add_test(
        NAME CameraImageQueueTest
        COMMAND CameraImageQueueTest
    )

    qt_add_executable(CameraContextImageTest
        tests/CameraContextImageTest.cpp

        CameraInterface/CameraContext.h
        CameraInterface/CameraContext.cpp
        CameraInterface/CameraInterface.h
        CameraInterface/CameraImageQueue.h
        CameraInterface/CameraImageQueue.cpp
        CameraInterface/CameraError.h
        CameraInterface/CMCameraMetaInfo.h
        CameraInterface/CMCameraParam.h

        CameraFactory/CameraFactory.h
        CameraFactory/CameraFactory.cpp
        CameraFactory/VirtualCamera.h
        CameraFactory/VirtualCamera.cpp

        ParseUiJson/ParseUiJson.h
        ParseUiJson/ParseUiJson.cpp

        Utils/ImageConver.h
    )

    target_include_directories(CameraContextImageTest
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${OpenCV_INCLUDE_DIRS}
    )

    target_link_libraries(CameraContextImageTest
        PRIVATE
            Qt6::Core
            Qt6::Widgets
            Qt6::Test
            ${OpenCV_LIBS}
    )

    add_test(
        NAME CameraContextImageTest
        COMMAND CameraContextImageTest
    )

    qt_add_executable(AcquireImageProcessTest
        tests/AcquireImageProcessTest.cpp

        ViewWidget/AcquireImageProcess.h
        ViewWidget/AcquireImageProcess.cpp

        CameraInterface/CameraContext.h
        CameraInterface/CameraContext.cpp
        CameraInterface/CameraInterface.h
        CameraInterface/CameraImageQueue.h
        CameraInterface/CameraImageQueue.cpp
        CameraInterface/CameraError.h
        CameraInterface/CMCameraMetaInfo.h
        CameraInterface/CMCameraParam.h

        CameraFactory/CameraFactory.h
        CameraFactory/CameraFactory.cpp
        CameraFactory/VirtualCamera.h
        CameraFactory/VirtualCamera.cpp

        ParseUiJson/ParseUiJson.h
        ParseUiJson/ParseUiJson.cpp

        Protocol/FrameProtocol.h
        Protocol/FrameProtocol.cpp
        Protocol/ImagePacketEncoder.h
        Protocol/ImagePacketEncoder.cpp

        Utils/ImageConver.h
    )

    target_include_directories(AcquireImageProcessTest
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${OpenCV_INCLUDE_DIRS}
    )

    target_link_libraries(AcquireImageProcessTest
        PRIVATE
            Qt6::Core
            Qt6::Widgets
            Qt6::Test
            ${OpenCV_LIBS}
    )

    add_test(
        NAME AcquireImageProcessTest
        COMMAND AcquireImageProcessTest
    )

    qt_add_executable(FrameProtocolTest
        tests/FrameProtocolTest.cpp

        Protocol/FrameProtocol.h
        Protocol/FrameProtocol.cpp
    )

    target_include_directories(FrameProtocolTest
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(FrameProtocolTest
        PRIVATE
            Qt6::Core
            Qt6::Test
    )

    add_test(
        NAME FrameProtocolTest
        COMMAND FrameProtocolTest
    )

    qt_add_executable(InferenceResponseCodecTest
        tests/InferenceResponseCodecTest.cpp

        Protocol/InferenceResponseCodec.h
        Protocol/InferenceResponseCodec.cpp
    )

    target_include_directories(InferenceResponseCodecTest
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(InferenceResponseCodecTest
        PRIVATE
            Qt6::Core
            Qt6::Test
    )

    add_test(
        NAME InferenceResponseCodecTest
        COMMAND InferenceResponseCodecTest
    )

    qt_add_executable(ImagePacketEncoderTest
        tests/ImagePacketEncoderTest.cpp

        Protocol/FrameProtocol.h
        Protocol/FrameProtocol.cpp
        Protocol/ImagePacketEncoder.h
        Protocol/ImagePacketEncoder.cpp
    )

    target_include_directories(ImagePacketEncoderTest
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${OpenCV_INCLUDE_DIRS}
    )

    target_link_libraries(ImagePacketEncoderTest
        PRIVATE
            Qt6::Core
            Qt6::Test
            ${OpenCV_LIBS}
    )

    add_test(
        NAME ImagePacketEncoderTest
        COMMAND ImagePacketEncoderTest
    )

    qt_add_executable(TcpClientTest
        tests/TcpClientTest.cpp

        Transport/TcpClient.h
        Transport/TcpClient.cpp

        Protocol/FrameProtocol.h
        Protocol/FrameProtocol.cpp
    )

    target_include_directories(TcpClientTest
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(TcpClientTest
        PRIVATE
            Qt6::Core
            Qt6::Network
            Qt6::Test
    )

    add_test(
        NAME TcpClientTest
        COMMAND TcpClientTest
    )

    qt_add_executable(ResponseStreamDecoderTest
        tests/ResponseStreamDecoderTest.cpp

        Transport/ResponseStreamDecoder.h
        Transport/ResponseStreamDecoder.cpp

        Protocol/FrameProtocol.h
        Protocol/FrameProtocol.cpp
    )

    target_include_directories(ResponseStreamDecoderTest
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(ResponseStreamDecoderTest
        PRIVATE
            Qt6::Core
            Qt6::Test
    )

    add_test(
        NAME ResponseStreamDecoderTest
        COMMAND ResponseStreamDecoderTest
    )

    qt_add_executable(InferenceResponseDispatcherTest
        tests/InferenceResponseDispatcherTest.cpp

        Protocol/InferenceResponseCodec.h
        Protocol/InferenceResponseCodec.cpp
        Protocol/InferenceResponseDispatcher.h
        Protocol/InferenceResponseDispatcher.cpp

        Protocol/FrameProtocol.h
        Protocol/FrameProtocol.cpp
        Transport/ResponseStreamDecoder.h
        Transport/ResponseStreamDecoder.cpp
    )

    target_include_directories(InferenceResponseDispatcherTest
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(InferenceResponseDispatcherTest
        PRIVATE
            Qt6::Core
            Qt6::Test
    )

    add_test(
        NAME InferenceResponseDispatcherTest
        COMMAND InferenceResponseDispatcherTest
    )

    qt_add_executable(TcpResponsePipelineTest
        tests/TcpResponsePipelineTest.cpp

        Transport/TcpClient.h
        Transport/TcpClient.cpp
        Transport/ResponseStreamDecoder.h
        Transport/ResponseStreamDecoder.cpp

        Protocol/FrameProtocol.h
        Protocol/FrameProtocol.cpp
        Protocol/InferenceResponseCodec.h
        Protocol/InferenceResponseCodec.cpp
        Protocol/InferenceResponseDispatcher.h
        Protocol/InferenceResponseDispatcher.cpp
    )

    target_include_directories(TcpResponsePipelineTest
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(TcpResponsePipelineTest
        PRIVATE
            Qt6::Core
            Qt6::Network
            Qt6::Test
    )

    add_test(
        NAME TcpResponsePipelineTest
        COMMAND TcpResponsePipelineTest
    )
endif()

include(GNUInstallDirs)

install(TARGETS Lust
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Lust
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
