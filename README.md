# RoboMaster 视觉第二次培训 - 任务一：图像处理

## 1. 项目简介
本项目完成了使用 OpenCV 对郁金香图片的处理任务，包括颜色空间转换、通道分离、滤波对比、颜色提取、形态学操作、轮廓查找、几何绘制及图像变换。

## 2. 环境信息
- OS: macOS (Apple Silicon)
- 编译器: AppleClang (C++17)
- 构建工具: CMake
- 依赖库: OpenCV 4.14.0（因为 Mac 上 OpenCV 5.0.0 存在模块兼容性问题，已降级至 4 版本）

## 3. 目录结构
- `src/`：源代码目录
- `resources/`：原图资源
- `result1/`：结果图输出
- `CMakeLists.txt`：构建脚本

## 4. 构建步骤
```bash
cmake -S . -B build
cmake --build build
```
## 5. 运行结果
执行 ./build/task1_image 后，终端输出“共找到 6 个有效轮廓”等信息。共提取出 6 个有效红花轮廓，按要求完成了绘制、旋转35度和左上角1/4裁剪。全部结果图保存在 result1/ 文件夹中。