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


---

# 任务二：合成旋转视频的参数拟合

## 1. 项目简介
使用 OpenCV 读取 `resources/task2.mp4` 视频，对视频中的青色发光小球进行追踪，并计算其旋转角度。

## 2. 追踪结果与问题解决
在初始追踪中，因背景存在肉眼难以察觉的反光干扰，目标提取固定在135度。随后改用 HSV 颜色空间提取青色目标（H 范围 80-100），成功解决了反光干扰问题，实现了完整的目标追踪。

## 3. 数学拟合与结果
- 追踪视频：`result/task2_fit/tracking_overlay.mp4`
- 拟合对比图：`result/task2_fit/fit_comparison.png`
- 角速度图：`result/task2_fit/angular_velocity.png`
- 残差图：`result/task2_fit/residuals.png`
- 数据分析脚本：`src/task2_fit/fit_analysis.py`（使用 Python 的 scipy.optimize.curve_fit 进行正弦拟合）

## 4. 任务二运行方式
```bash
# 生成追踪视频与角度数据
./build/task2_fit

# 运行 Python 脚本生成图表
python3 src/task2_fit/fit_analysis.py