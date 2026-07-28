cmake_minimum_required(VERSION 3.19)
project(Lust LANGUAGES CXX)

include(CTest)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(OpenCV 4.12.0 REQUIRED COMPONENTS core imgproc highgui)
qt_standard_project_setup()

if(BUILD_TESTING)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Test)
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
