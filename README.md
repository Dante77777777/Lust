# Lust

Lust 是一个基于 Qt 和 OpenCV 的工业相机客户端，支持相机发现、连接、参数读写、实时取流与图像显示，并可通过 TCP 与推理服务通信。

## 相机支持

- **虚拟相机**：用于不连接实体相机时进行开发和调试。
- **海康机器人（Hikrobot）相机**：通过 MVS SDK 枚举设备、管理连接和取流，读取或设置相机参数，并将采集帧转换为 OpenCV 图像。

海康相机参数界面配置位于 `Lust/resource/HikCameraParam.json`，已随 Qt 资源文件打包。

## 环境要求

- Windows x64
- CMake 3.19 或更高版本
- Qt 6.5 或更高版本（Core、Widgets、Network；构建测试时还需要 Test）
- OpenCV 4.12.0（core、imgproc、highgui、imgcodecs）
- 海康机器人 MVS SDK x64（相机运行时及开发库）

仓库包含 MVS SDK 的头文件（`Lust/depends`）和导入库（`Lust/libs/MvCameraControl.lib`）。运行时 DLL 默认从以下目录查找并在构建后复制到程序目录：

```text
C:/Program Files (x86)/Common Files/MVS/Runtime/Win64_x64
```

如果 MVS 安装在其他位置，可在配置 CMake 时设置 `MV_CAMERA_RUNTIME_DIR`。

## 构建

在仓库根目录执行：

```powershell
cmake -S Lust -B build -DCMAKE_PREFIX_PATH="<Qt安装前缀>;<OpenCV安装前缀>"
cmake --build build --config Release
```

将 `<Qt安装前缀>` 和 `<OpenCV安装前缀>` 替换为本机对应版本的安装路径。若 MVS 运行时目录不是默认位置，可增加：

```powershell
-DMV_CAMERA_RUNTIME_DIR="<MVS运行时目录>"
```

例如：

```powershell
cmake -S Lust -B build `
  -DCMAKE_PREFIX_PATH="C:/Qt/6.5.3/msvc2019_64;C:/opencv/build" `
  -DMV_CAMERA_RUNTIME_DIR="C:/Program Files (x86)/Common Files/MVS/Runtime/Win64_x64"
cmake --build build --config Release
```

构建配置会检查 `MvCameraControl.dll` 是否存在；缺少运行时 DLL 时，CMake 配置会失败并提示实际查找路径。

## 目录说明

| 路径 | 内容 |
| --- | --- |
| `Lust/CameraInterface` | 相机接口、上下文、图像队列和参数定义 |
| `Lust/CameraFactory` | 虚拟相机与海康相机实现及工厂注册 |
| `Lust/ParamWidget` | 相机参数界面 |
| `Lust/ViewWidget` | 实时图像显示与采集流程 |
| `Lust/Transport`、`Lust/Protocol` | TCP 传输和推理协议处理 |
| `Lust/resource` | UI 配置与 Qt 资源 |
| `Lust/depends`、`Lust/libs` | MVS SDK 开发文件 |
