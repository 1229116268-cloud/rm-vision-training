#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <filesystem>

int main() {
    // 1. 读取图片
    cv::Mat img = cv::imread("resources/task1_image.jpg");
    if (img.empty()) {
        std::cout << "错误：无法读取图片！" << std::endl;
        return -1;
    }
    std::cout << "图片读取成功！尺寸: " << img.cols << " x " << img.rows << std::endl;

    // 2. 灰度化与通道分离
    cv::Mat gray, hsv, y_channel;
    std::vector<cv::Mat> channels;

    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::imwrite("result1/gray.jpg", gray);

    cv::Mat img_blur;
    cv::GaussianBlur(img, img_blur, cv::Size(5, 5), 0); 
    cv::cvtColor(img_blur, hsv, cv::COLOR_BGR2HSV);
    cv::split(hsv, channels);
    cv::imwrite("result1/hsv_h.jpg", channels[0]);
    cv::imwrite("result1/hsv_s.jpg", channels[1]);
    cv::imwrite("result1/hsv_v.jpg", channels[2]);

    cv::Mat ycrcb;
    cv::cvtColor(img, ycrcb, cv::COLOR_BGR2YCrCb);
    cv::split(ycrcb, channels);
    cv::imwrite("result1/y_channel.jpg", channels[0]);

    // 3. 红、绿、黄颜色提取与形态学处理
    cv::Mat mask, result, mask1, mask2, red_mask, raw_red_mask;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));

    // 红色提取
    cv::inRange(hsv, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), mask1);
    cv::inRange(hsv, cv::Scalar(160, 100, 100), cv::Scalar(180, 255, 255), mask2);
    red_mask = mask1 | mask2;
    raw_red_mask = red_mask.clone();
    cv::morphologyEx(red_mask, red_mask, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(red_mask, red_mask, cv::MORPH_CLOSE, kernel);
    cv::bitwise_and(img, img, result, red_mask);
    cv::imwrite("result1/red_mask.jpg", red_mask);
    cv::imwrite("result1/red_result.jpg", result);

    // 绿色提取
    cv::inRange(hsv, cv::Scalar(35, 50, 50), cv::Scalar(85, 255, 255), mask);
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);
    cv::bitwise_and(img, img, result, mask);
    cv::imwrite("result1/green_mask.jpg", mask);
    cv::imwrite("result1/green_result.jpg", result);

    // 黄色提取
    cv::inRange(hsv, cv::Scalar(20, 120, 120), cv::Scalar(35, 255, 255), mask);
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);
    cv::bitwise_and(img, img, result, mask);
    cv::imwrite("result1/yellow_mask.jpg", mask);
    cv::imwrite("result1/yellow_result.jpg", result);

    // 4. 滤波对比
    cv::Mat meanImg, gaussianImg, medianImg;
    cv::blur(img, meanImg, cv::Size(5, 5));
    cv::GaussianBlur(img, gaussianImg, cv::Size(5, 5), 1.5);
    cv::medianBlur(img, medianImg, 5);
    cv::imwrite("result1/filter_mean.jpg", meanImg);
    cv::imwrite("result1/filter_gaussian.jpg", gaussianImg);
    cv::imwrite("result1/filter_median.jpg", medianImg);

    // 5. 形态学操作对比
    cv::Mat erode_img, dilate_img, open_img, close_img;
    cv::erode(raw_red_mask, erode_img, kernel);
    cv::dilate(raw_red_mask, dilate_img, kernel);
    cv::morphologyEx(raw_red_mask, open_img, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(raw_red_mask, close_img, cv::MORPH_CLOSE, kernel);
    cv::imwrite("result1/morph_erode.jpg", erode_img);
    cv::imwrite("result1/morph_dilate.jpg", dilate_img);
    cv::imwrite("result1/morph_open.jpg", open_img);
    cv::imwrite("result1/morph_close.jpg", close_img);

    // 6. 轮廓查找与外接矩形
    cv::Mat contour_img = img.clone();
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::Mat temp_mask = open_img.clone(); 
    cv::findContours(temp_mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    int valid_contours = 0;
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area < 5000) continue; 
        valid_contours++;
        cv::Rect rect = cv::boundingRect(contours[i]);
        cv::rectangle(contour_img, rect, cv::Scalar(255, 0, 0), 10);
        std::string text = std::to_string((int)area);
        cv::putText(contour_img, text, cv::Point(rect.x, rect.y - 10), 
                    cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 0, 255), 3);
    }
    cv::imwrite("result1/red_contours.jpg", contour_img);
    std::cout << "共找到 " << valid_contours << " 个有效轮廓！" << std::endl;

       // 7. 绘制与变换
    cv::Mat draw_img = img.clone();
    cv::Point center(img.cols / 2, img.rows / 2);
    int radius = std::min(img.cols, img.rows) / 4;
    cv::circle(draw_img, center, radius, cv::Scalar(0, 255, 0), 10);
    cv::rectangle(draw_img, cv::Point(0, 0), center, cv::Scalar(0, 255, 255), 10);
    cv::putText(draw_img, "RoboMaster", cv::Point(100, 200), 
                cv::FONT_HERSHEY_SIMPLEX, 5, cv::Scalar(255, 255, 255), 10);
    cv::imwrite("result1/drawn_shapes.jpg", draw_img);

    cv::Mat rot_mat = cv::getRotationMatrix2D(center, 35.0, 1.0);
    cv::Mat rotated_img;
    cv::warpAffine(img, rotated_img, rot_mat, img.size());
    cv::imwrite("result1/rotated_35.jpg", rotated_img);

    cv::Rect roi_rect(0, 0, img.cols / 2, img.rows / 2);
    cv::Mat cropped_img = img(roi_rect).clone();
    cv::imwrite("result1/cropped_topleft.jpg", cropped_img);

    std::cout << "第5点绘制与变换操作完成！" << std::endl;
    return 0;
}