#pragma once

#include <thread>
#include <tf/transform_listener.h>
#include <tf/tf.h>
#include <move_base_z_client_plugin/move_base_z_client_plugin.h>
#include <odom_tracker/odom_tracker.h>
#include <planner_switcher/planner_switcher.h>

namespace move_base_z_client
{
class CbNavigateForward : public smacc::SmaccClientBehavior
{
public:
    boost::optional<float> forwardDistance;

    // just a stub to show how to use parameterless constructor
    boost::optional<float> forwardSpeed;

    tf::TransformListener listener;

    move_base_z_client::ClMoveBaseZ *moveBaseClient_;

    CbNavigateForward(float forwardDistance)
    {
        this->forwardDistance = forwardDistance;
    }

    CbNavigateForward()
    {
    }

    virtual void onEntry() override
    {
        // straight motion distance
        double dist;

        if (!forwardDistance)
        {
            this->currentState->param("straight_motion_distance", dist, 3.5);
        }
        else
        {
            dist = *forwardDistance;
        }

        ROS_INFO_STREAM("Straight motion distance: " << dist);

        this->requiresClient(moveBaseClient_);
        auto odomTracker_ = this->getComponent<OdomTracker>();

        //this should work better with a coroutine and await
        ros::Rate rate(10.0);
        tf::StampedTransform currentPose;

        while (ros::ok())
        {
            try
            {
                listener.lookupTransform("/odom", "/base_link",
                                         ros::Time(0), currentPose);

                break;
            }
            catch (tf::TransformException ex)
            {
                ROS_ERROR("%s", ex.what());
                ros::Duration(1.0).sleep();
            }
        }

        tf::Transform forwardDeltaTransform;
        forwardDeltaTransform.setIdentity();
        forwardDeltaTransform.setOrigin(tf::Vector3(dist, 0, 0));

        tf::Transform targetPose = currentPose * forwardDeltaTransform;

        ClMoveBaseZ::Goal goal;
        goal.target_pose.header.frame_id = "/odom";
        goal.target_pose.header.stamp = ros::Time::now();
        tf::poseTFToMsg(targetPose, goal.target_pose.pose);

        ROS_INFO_STREAM("TARGET POSE FORWARD: " << goal.target_pose.pose);
        ros::Duration(10).sleep();
        this->odomTracker_->clearPath();

        geometry_msgs::PoseStamped currentPoseMsg;
        currentPoseMsg.header.frame_id = "/odom";
        currentPoseMsg.header.stamp = ros::Time::now();
        tf::poseTFToMsg(currentPose, currentPoseMsg.pose);
        this->odomTracker_->setStartPoint(currentPoseMsg);
        this->odomTracker_->setWorkingMode(WorkingMode::RECORD_PATH_FORWARD);

        moveBaseClient_->plannerSwitcher_->setForwardPlanner();

        moveBaseClient_->sendGoal(goal);
    }

    virtual void onExit() override
    {
        this->odomTracker_->setWorkingMode(WorkingMode::IDLE);
    }
};
} // namespace sm_dance_bot