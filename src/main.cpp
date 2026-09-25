#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

int main() {
  // 1. 读取图片
  cv::Mat img = cv::imread("resources/task1_image.jpg");

  // 2. 检查是否读取成功
  if (img.empty()) {
    std::cout
        << "错误：无法读取图片，请确认 resources/task1_image.jpg 路径是否正确！"
        << std::endl;
    return -1;
  }

  std::cout << "图片读取成功！尺寸: " << img.cols << " x " << img.rows
            << std::endl;

  // 3. 先做最简单的：把原图保存到 result1 文件夹里，测试文件读写
  std::filesystem::create_directories("result1");
  cv::imwrite("result1/original.jpg", img);
  std::cout << "原图已保存到 result1/original.jpg" << std::endl;

  // ========== 新增部分：灰度化与通道分离 ==========
  cv::Mat gray, hsv, y_channel;
  std::vector<cv::Mat> channels;

  // 1. 转灰度图
  cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
  cv::imwrite("result1/gray.jpg", gray);

  // 2. 转 HSV 并分离通道
  cv::Mat img_blur;
  cv::GaussianBlur(img, img_blur, cv::Size(5, 5), 0); // 高斯模糊去噪
  cv::cvtColor(img_blur, hsv, cv::COLOR_BGR2HSV);
  cv::split(hsv, channels); // 分离出 H, S, V 三个通道
  cv::imwrite("result1/hsv_h.jpg", channels[0]); // H 通道
  cv::imwrite("result1/hsv_s.jpg", channels[1]); // S 通道
  cv::imwrite("result1/hsv_v.jpg", channels[2]); // V 通道

  // 3. 分离 Y 通道 (YCrCb 色彩空间)
  cv::Mat ycrcb;
  cv::cvtColor(img, ycrcb, cv::COLOR_BGR2YCrCb);
  cv::split(ycrcb, channels);
  cv::imwrite("result1/y_channel.jpg", channels[0]); // Y 通道

  std::cout << "灰度图、HSV 通道和 Y 通道均已分离并保存！" << std::endl;
  // ===============================================
  // cv::imshow("Test", img);
  // cv::waitKey(0);
  // ========== 新增：红、绿、黄颜色提取与形态学处理 ==========
  cv::Mat mask, result;
  // 定义一个 5x5 的矩形核，用于形态学去噪
  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));

  // 1. 提取红色 (红色在HSV中跨越0度和180度，需要合并两段)
  cv::Mat mask1, mask2, red_mask, raw_red_mask;
  cv::inRange(hsv, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), mask1);
  cv::inRange(hsv, cv::Scalar(160, 100, 100), cv::Scalar(180, 255, 255), mask2);
  red_mask = mask1 | mask2; // 合并两段红色掩码
  raw_red_mask = red_mask.clone(); // 复制一份原始红色掩码用于形态学对比
  cv::morphologyEx(red_mask, red_mask, cv::MORPH_OPEN, kernel); // 开运算去噪
  cv::morphologyEx(red_mask, red_mask, cv::MORPH_CLOSE, kernel); // 闭运算补洞
  cv::bitwise_and(img, img, result, red_mask); // 提取原图中的红色部分
  cv::imwrite("result1/red_mask.jpg", red_mask);
  cv::imwrite("result1/red_result.jpg", result);

  // 2. 提取绿色 (H范围约 35~85)
  cv::inRange(hsv, cv::Scalar(35, 50, 50), cv::Scalar(85, 255, 255), mask);
  cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
  cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);
  cv::bitwise_and(img, img, result, mask);
  cv::imwrite("result1/green_mask.jpg", mask);
  cv::imwrite("result1/green_result.jpg", result);

  // 3. 提取黄色 (H范围约 20~35)
  cv::inRange(hsv, cv::Scalar(20, 120, 120), cv::Scalar(35, 255, 255), mask);
  cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
  cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);
  cv::bitwise_and(img, img, result, mask);
  cv::imwrite("result1/yellow_mask.jpg", mask);
  cv::imwrite("result1/yellow_result.jpg", result);

  std::cout << "红、绿、黄三种颜色提取与形态学处理完成！" << std::endl;
  // =======================================================
  // ========== 新增：滤波对比 (任务点2) ==========
  cv::Mat meanImg, gaussianImg, medianImg;
  cv::blur(img, meanImg, cv::Size(5, 5)); // 均值滤波，核尺寸 5x5
  cv::GaussianBlur(img, gaussianImg, cv::Size(5, 5),
                   1.5); // 高斯滤波，核尺寸 5x5，标准差 1.5
  cv::medianBlur(img, medianImg, 5); // 中值滤波，核尺寸 5x5
  cv::imwrite("result1/filter_mean.jpg", meanImg);
  cv::imwrite("result1/filter_gaussian.jpg", gaussianImg);
  cv::imwrite("result1/filter_median.jpg", medianImg);

  // ========== 新增：形态学操作对比 (任务点4) ==========
  cv::Mat erode_img, dilate_img, open_img, close_img;
  // 注意这里用的是 raw_red_mask（纯净的原始红色掩膜）
  cv::erode(raw_red_mask, erode_img, kernel);                       // 腐蚀
  cv::dilate(raw_red_mask, dilate_img, kernel);                     // 膨胀
  cv::morphologyEx(raw_red_mask, open_img, cv::MORPH_OPEN, kernel); // 开运算
  cv::morphologyEx(raw_red_mask, close_img, cv::MORPH_CLOSE, kernel); // 闭运算
  cv::imwrite("result1/morph_erode.jpg", erode_img);
  cv::imwrite("result1/morph_dilate.jpg", dilate_img);
  cv::imwrite("result1/morph_open.jpg", open_img);
  cv::imwrite("result1/morph_close.jpg", close_img);

  std::cout << "滤波对比与形态学对比保存完成！" << std::endl;
  // ========== 新增：第4点 轮廓查找与外接矩形 ==========
  cv::Mat contour_img = img.clone(); // 拷贝一份原图用来画框，免得把原图弄脏
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;

  // 我们在刚才做好的 open_img（开运算后的红色掩膜）上找轮廓
  // 注意：findContours 会修改传入的图像，所以这里克隆一份
  cv::Mat temp_mask = open_img.clone();
  cv::findContours(temp_mask, contours, hierarchy, cv::RETR_EXTERNAL,
                   cv::CHAIN_APPROX_SIMPLE);

  int valid_contours = 0;
  for (size_t i = 0; i < contours.size(); i++) {
    double area = cv::contourArea(contours[i]);
    // 核心：按面积筛选，过滤掉叶子、噪点等小面积区域（比如面积 > 5000
    // 才算花朵）
    if (area < 5000)
      continue;

    valid_contours++;
    // 画出外接矩形
    cv::Rect rect = cv::boundingRect(contours[i]);
    cv::rectangle(contour_img, rect, cv::Scalar(255, 0, 0),
                  10); // 蓝色框，线宽10

    // 你也可以把面积数值写在图像上
    std::string text = std::to_string((int)area);
    cv::putText(contour_img, text, cv::Point(rect.x, rect.y - 10),
                cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 0, 255), 3);
  }
  cv::imwrite("result1/red_contours.jpg", contour_img);
  std::cout << "共找到 " << valid_contours << " 个有效轮廓！" << std::endl;

  // ========== 新增：第5点 绘制与变换 ==========
  cv::Mat draw_img = img.clone(); // 拿一份干净的原图来画画

  // 1. 画圆 (圆心设为中心点，半径为最小边长的1/4)
  cv::Point center(img.cols / 2, img.rows / 2);
  int radius = std::min(img.cols, img.rows) / 4;
  cv::circle(draw_img, center, radius, cv::Scalar(0, 255, 0), 10); // 绿色圆

  // 2. 画矩形 (左上角到中心)
  cv::rectangle(draw_img, cv::Point(0, 0), center, cv::Scalar(0, 255, 255),
                10); // 黄色矩形

  // 3. 写文字
  cv::putText(draw_img, "RoboMaster", cv::Point(100, 200),
              cv::FONT_HERSHEY_SIMPLEX, 5, cv::Scalar(255, 255, 255), 10);

  cv::imwrite("result1/drawn_shapes.jpg", draw_img);

  // 4. 绕图像中心旋转 35 度
  cv::Mat rot_mat =
      cv::getRotationMatrix2D(center, 35.0, 1.0); // 35度，缩放比例1.0
  cv::Mat rotated_img;
  cv::warpAffine(img, rotated_img, rot_mat, img.size()); // 保持原图大小进行旋转
  cv::imwrite("result1/rotated_35.jpg", rotated_img);

  // 5. 裁剪左上角 1/4 (即宽、高各取一半)
  cv::Rect roi_rect(0, 0, img.cols / 2, img.rows / 2);
  cv::Mat cropped_img = img(roi_rect).clone();
  cv::imwrite("result1/cropped_topleft.jpg", cropped_img);

  std::cout << "第5点绘制与变换操作完成！" << std::endl;
  // ===============================================
  return 0;
}