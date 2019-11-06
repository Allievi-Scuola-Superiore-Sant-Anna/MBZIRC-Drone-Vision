#pragma once

#include <map>
#include <string>

#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>

#include <actionlib/server/simple_action_server.h>

#include <distance_finder/GetDistanceAction.h>

namespace distance_finder {


typedef struct TargetParameters {
    std::string color;
    double radius;
} TargetParameters;


typedef struct CameraParameters {
    int resolution_width;
    int resolution_height; 
    double focal_len;
    double sensor_width;
    double sensor_height;
    double sensor_diag;
    double calib_dist;
    double calib_fov_width;
    double calib_fov_height;
    bool stereo;
} CameraParameters;


typedef struct PosError {
    uint32_t x_pix;
    uint32_t y_pix;
    double x_m;
    double y_m;
    double dist_m;
} PosError;


class DistanceFinder
{
    ros::NodeHandle nh_;
    actionlib::SimpleActionServer<distance_finder::GetDistanceAction> dist_act_srv_;
    std::map<std::string, CameraParameters> cam_params;
    TargetParameters fly_ball_params;
    TargetParameters gnd_ball_params;

    void initTargetParameters();
    void initCamParameters();
    void initDistanceActionServer();
    void getDistanceActionGoalCallback(const distance_finder::GetDistanceGoalConstPtr &dist_act_ptr);
    void getDistanceActionPreemptCallback();
    double findDistanceByProportion(std::string cam_name, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    PosError findPosErrorByProportion(std::string cam_name, uint32_t x, uint32_t y, uint32_t w, uint32_t h);


public:
    DistanceFinder(ros::NodeHandle nh);
    ~DistanceFinder();
};

}