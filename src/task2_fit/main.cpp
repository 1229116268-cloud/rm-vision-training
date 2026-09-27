#include <cmath>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

int main() {
  std::ofstream data_file("result2/task2_fit/angle_data.txt");
  // 1. 读取视频
  cv::VideoCapture cap("resources/task_2.mp4");
  if (!cap.isOpened()) {
    std::cout << "错误：无法打开视频！" << std::endl;
    return -1;
  }

  // 获取视频参数，用于生成输出视频
  int width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
  int height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
  double fps = cap.get(cv::CAP_PROP_FPS);
  int total_frames = cap.get(cv::CAP_PROP_FRAME_COUNT);
  std::cout << "视频尺寸: " << width << "x" << height
            << " 总帧数: " << total_frames << std::endl;

  // 已知常量：中心点坐标 (根据任务书) 和 实际物理半径 (200像素)
  cv::Point2f center(480, 360);
  // 如果你明确知道圆心在 (480, 360)，就改成 cv::Point2f center(480, 360);
  double R = 220.0; // 物理半径，单位像素

  // 2. 准备写入输出视频
  cv::VideoWriter writer("result2/task2_fit/tracking_overlay.mp4",
                         cv::VideoWriter::fourcc('m', 'p', '4', 'v'), fps,
                         cv::Size(width, height));

  cv::Mat frame;
  int frame_count = 0;

  std::cout << "开始处理视频..." << std::endl;

  while (true) {
    cap >> frame;
    if (frame.empty())
      break; // 视频读完了

    // --- 核心处理流程 ---

    // 1. 转HSV图
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
    cv::Mat mask;
    cv::inRange(hsv, cv::Scalar(80, 100, 150), cv::Scalar(100, 255, 255), mask);

    // 3. 找轮廓
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);

    if (!contours.empty()) {
      // 4. 找最大轮廓（排除噪点反光）
      int max_idx = 0;
      double max_area = 0;
      for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        cv::Moments m = cv::moments(contours[i]); // 提前算一下每个轮廓的质心
        if (m.m00 != 0) {
          int temp_cx = int(m.m10 / m.m00);
          int temp_cy = int(m.m01 / m.m00);
          cv::Point2f temp_center(temp_cx, temp_cy);
          double dist = cv::norm(temp_center - center); // 算距离

          // 距离必须在半径 150 到 250 像素之间，才算有效目标
          if (dist > 150 && dist < 250 && area > max_area) {
            max_area = area;
            max_idx = i;
          }
        }
      }

      // 5. 算质心坐标
      cv::Moments m = cv::moments(contours[max_idx]);
      if (m.m00 != 0) {
        int cx = int(m.m10 / m.m00);
        int cy = int(m.m01 / m.m00);

        // 6. 计算角度（逆时针旋转，用 atan2）
        double dx = cx - center.x;
        double dy = center.y - cy; // 图像坐标y轴向下，取反使得数学上逆时针为正
        double theta = std::atan2(dy, dx); // 弧度
        double angle_deg = theta * 180.0 / CV_PI;
        double current_time = frame_count / fps; // 当前时间（秒）
        data_file << current_time << " " << angle_deg << "\n"; // 写入文件

        // 7. 在图上画出来，方便做视频
        // 画圆心
        cv::circle(frame, center, 5, cv::Scalar(0, 255, 0), -1);
        // 画小球中心
        cv::circle(frame, cv::Point(cx, cy), 5, cv::Scalar(0, 0, 255), -1);
        // 画连接线
        cv::line(frame, center, cv::Point(cx, cy), cv::Scalar(255, 0, 0), 2);
        // 画外接圆（大圆）
        cv::circle(frame, center, (int)R, cv::Scalar(0, 255, 255), 1);

        // 把角度写在屏幕上
        std::string text = "Angle: " + std::to_string((int)angle_deg) + " deg";
        cv::putText(frame, text, cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX,
                    1.0, cv::Scalar(255, 255, 255), 2);
      }
    }

    // 写入输出视频
    writer.write(frame);
    frame_count++;

    // 打印进度（每50帧）
    if (frame_count % 50 == 0) {
      std::cout << "已处理 " << frame_count << " / " << total_frames << " 帧"
                << std::endl;
    }
  }

  cap.release();
  writer.release();
  data_file.close();
  std::cout << "任务二基础视频处理完成！结果保存在 "
               "result2/task2_fit/tracking_overlay.mp4"
            << std::endl;

  return 0;
}