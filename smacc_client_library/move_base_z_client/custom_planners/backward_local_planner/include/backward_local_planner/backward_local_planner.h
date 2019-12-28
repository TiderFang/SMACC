/*****************************************************************************************************************
 * ReelRobotix Inc. - Software License Agreement      Copyright (c) 2018
 * 	 Authors: Pablo Inigo Blasco, Brett Aldrich
 *
 ******************************************************************************************************************/
#pragma once
#include <dynamic_reconfigure/server.h>
#include <geometry_msgs/PoseStamped.h>
#include <nav_core/base_local_planner.h>
#include <backward_local_planner/BackwardLocalPlannerConfig.h>
#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <tf2_ros/buffer.h>

typedef double meter;
typedef double rad;

namespace move_base_z_client
{
namespace backward_local_planner
{
class BackwardLocalPlanner : public nav_core::BaseLocalPlanner
{

public:
    BackwardLocalPlanner();

    virtual ~BackwardLocalPlanner();

    /**
   * @brief  Given the current position, orientation, and velocity of the robot: compute velocity commands to send to the robot mobile base
   * @param cmd_vel Will be filled with the velocity command to be passed to the robot base
   * @return True if a valid velocity command was found, false otherwise
   */
    virtual bool computeVelocityCommands(geometry_msgs::Twist &cmd_vel) override;

    /**
   * @brief  Check if the goal pose has been achieved by the local planner
   * @return True if achieved, false otherwise
   */
    virtual bool isGoalReached() override;

    /**
   * @brief  Set the plan that the local planner is following
   * @param plan The plan to pass to the local planner
   * @return True if the plan was updated successfully, false otherwise
   */
    virtual bool setPlan(const std::vector<geometry_msgs::PoseStamped> &plan) override;

    /**
   * @brief  Constructs the local planner
   * @param name The name to give this instance of the local planner
   * @param tf A pointer to a transform listener
   * @param costmap_ros The cost map to use for assigning costs to local plans
   */
    void initialize(std::string name, tf::TransformListener *tf, costmap_2d::Costmap2DROS *costmapRos_);

    void initialize(std::string name, tf2_ros::Buffer *tf, costmap_2d::Costmap2DROS *costmapRos_);

    void initialize();

private:
    void reconfigCB(::backward_local_planner::BackwardLocalPlannerConfig &config, uint32_t level);

    // returns true for a pure spining motion request
    bool createCarrotGoal(const tf::Stamped<tf::Pose> &tfpose);

    void pureSpinningCmd(const tf::Stamped<tf::Pose> &tfpose, double vetta, double gamma, double alpha_error, double betta_error, double rho_error, geometry_msgs::Twist &cmd_vel);
    void defaultBackwardCmd(const tf::Stamped<tf::Pose> &tfpose, double vetta, double gamma, double alpha_error, geometry_msgs::Twist &cmd_vel);

    void publishGoalMarker(double x, double y, double phi);

    dynamic_reconfigure::Server<::backward_local_planner::BackwardLocalPlannerConfig> paramServer_;
    dynamic_reconfigure::Server<::backward_local_planner::BackwardLocalPlannerConfig>::CallbackType f;

    std::vector<geometry_msgs::PoseStamped> backwardsPlanPath_;
    costmap_2d::Costmap2DROS *costmapRos_;

    ros::Publisher goalMarkerPublisher_;

    double k_rho_;
    double k_alpha_;
    double k_betta_;

    bool goalReached_;
    bool initialPureSpinningStage_;
    bool pureSpinningMode_ = false;

    const double alpha_offset_ = M_PI;
    const double betta_offset_ = 0;

    double yaw_goal_tolerance_; // radians
    double xy_goal_tolerance_;  // meters

    meter carrot_distance_;
    rad carrot_angular_distance_;

    double max_linear_x_speed_;  //meters/sec
    double max_angular_z_speed_; // rads/sec

    // references the current point inside the backwardsPlanPath were the robot is located
    int currentPoseIndex_;
};
} // namespace backward_local_planner
} // namespace move_base_z_client