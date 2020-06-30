/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2018, Case Western Reserve University
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of SRI International nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/* Author: Su Lu <sxl924@case.edu>
 * Description: A needle pose publisher for testing simple needle grasping.
 * This node is to publish updated needle pose.
 */

#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <gazebo_msgs/GetModelState.h>
#include <gazebo_msgs/SetModelState.h>

#include <cwru_davinci_grasp/davinci_simple_grasp_generator.h>

class DummyNeedleTracker
{
public :
  DummyNeedleTracker
  (
  const ros::NodeHandle &nh
  );

  bool perturbNeedlePose
  (
  double perturbRadian,
  const cwru_davinci_grasp::GraspInfo& selectedGrasp
  );

  bool publishNeedlePose();

private:
  void initialize();

  bool getNeedlePose();

private:
  ros::NodeHandle               m_NodeHandle;
  ros::ServiceClient            m_NeedlePoseClient;
  ros::ServiceClient            m_NeedlePoseModifier;
  ros::Publisher                m_NeedlePosePub;

  geometry_msgs::PoseStamped    m_StampedNeedlePose;
  geometry_msgs::Pose           m_PerturbedNeedlePose;
  gazebo_msgs::GetModelState    m_NeedleState;
  gazebo_msgs::SetModelState    m_PerturbedNeedleState;
};

DummyNeedleTracker::DummyNeedleTracker
(
const ros::NodeHandle &nh
) 
: m_NodeHandle(nh)
{
  initialize();
}

void DummyNeedleTracker::initialize
(
)
{
  m_NeedlePoseClient = m_NodeHandle.serviceClient<gazebo_msgs::GetModelState>("/gazebo/get_model_state");
  m_NeedlePoseModifier = m_NodeHandle.serviceClient<gazebo_msgs::SetModelState>("/gazebo/set_model_state");
  m_NeedlePosePub = m_NodeHandle.advertise<geometry_msgs::PoseStamped>("/updated_needle_pose", 1000);

  m_NeedleState.request.model_name = "needle_r";
  m_NeedleState.request.relative_entity_name = "davinci_endo_cam_l";

  m_PerturbedNeedleState.request.model_state.model_name = "needle_r";
  m_PerturbedNeedleState.request.model_state.reference_frame = "davinci_endo_cam_l";
}

bool DummyNeedleTracker::perturbNeedlePose
(
double perturbRadian,
const cwru_davinci_grasp::GraspInfo& selectedGrasp
)
{
  if (!m_NeedlePoseClient.call(m_NeedleState))
  {
    ROS_WARN("DummyNeedleTracker: Failed getting neeedle pose from gazebo");
    return false;
  }

  Eigen::Affine3d needlePose;
  tf::poseMsgToEigen(m_NeedleState.response.pose, needlePose);


  double needle_radius = 0.0125;  // TODO
  double radius = selectedGrasp.graspParamInfo.param_3;
  double x = needle_radius * cos(radius);
  double y = needle_radius * sin(radius);

  // Transform vector into world frame
  Eigen::Vector3d vecFromNeedleOriginToGraspPointWrtWorldFrame = needlePose.linear() * Eigen::Vector3d(x, y, 0.0);
  vecFromNeedleOriginToGraspPointWrtWorldFrame.normalize();

  // Transform point into world frame
  Eigen::Vector3d graspPointLocationWrtWorldFrame = needlePose * Eigen::Vector3d(x, y, 0.0);

  Eigen::Vector3d needleFrameZaxisProjectedOnWorldFrame = needlePose.rotation().col(2);

  Eigen::Vector3d perturbAxisOfRotWrtWorldFrame = vecFromNeedleOriginToGraspPointWrtWorldFrame.cross(needleFrameZaxisProjectedOnWorldFrame);

  Eigen::AngleAxisd perturbRotationMatWrtWorldFrame(perturbRadian, perturbAxisOfRotWrtWorldFrame);

  Eigen::Vector3d needleLocationWrtWorldFrame = needlePose.translation();
  // Eigen::Vector3d graspPointLocationWrtWorldFrame = needleLocationWrtWorldFrame + vecFromNeedleOriginToGraspPointWrtWorldFrame;

  Eigen::Affine3d TM1(Eigen::Affine3d::Identity());     TM1.translation() = graspPointLocationWrtWorldFrame;

  Eigen::Affine3d TM2(perturbRotationMatWrtWorldFrame);

  Eigen::Affine3d TM3(Eigen::Affine3d::Identity());     TM3.translation() = -graspPointLocationWrtWorldFrame;

  Eigen::Affine3d perturbedNeedlePose = TM1 * TM2 * TM3 * needlePose;

  tf::poseEigenToMsg(perturbedNeedlePose, m_PerturbedNeedleState.request.model_state.pose);
  if (!m_NeedlePoseModifier.call(m_PerturbedNeedleState))
  {
    ROS_WARN("DummyNeedleTracker: Failed setting perturbed neeedle pose to gazebo");
    return false;
  }

  return true;
}

bool DummyNeedleTracker::publishNeedlePose()
{
  if (!getNeedlePose())
    return false;

  m_NeedlePosePub.publish(m_StampedNeedlePose);
  return true;
}

bool DummyNeedleTracker::getNeedlePose
(
)
{
  if (!m_NeedlePoseClient.call(m_NeedleState))
  {
    ROS_WARN("DummyNeedleTracker: Failed getting neeedle pose from gazebo");
    return false;
  }

  m_StampedNeedlePose.header.frame_id = m_NeedleState.response.header.frame_id;
  m_StampedNeedlePose.header.stamp = ros::Time::now();
  m_StampedNeedlePose.pose = m_NeedleState.response.pose;
  return true;
}