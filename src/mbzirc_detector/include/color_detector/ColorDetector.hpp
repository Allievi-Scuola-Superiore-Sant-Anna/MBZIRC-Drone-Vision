#pragma once

#include <string>

#include <opencv2/imgproc.hpp>

#include "BBox.hpp"


class ColorDetector {
    unsigned h_min;
    unsigned h_max;
    unsigned s_min;
    unsigned s_max;
    unsigned v_min;
    unsigned v_max;
    cv::Scalar lb1;
    cv::Scalar ub1;
    cv::Scalar lb2;
    cv::Scalar ub2;
    double min_area_pix;
    unsigned max_objects;

    void setColorThresholds(const std::vector<unsigned> &th); 
public:
    ColorDetector(std::vector<unsigned> th, double min_area_pix,
        unsigned max_objects);
    ~ColorDetector();
    std::vector<BBox> detect(const cv::Mat &img);
};
