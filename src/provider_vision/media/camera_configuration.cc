/**
 * \file	camera_configuration.cpp
 * \author	Jérémie St-Jules <jeremie.st.jules.prevost@gmail.com>
 * \author	Thibaut Mattio <thibaut.mattio@gmail.com>
 * \date	10/09/2015
 * \copyright	Copyright (c) 2015 SONIA AUV ETS. All rights reserved.
 * Use of this source code is governed by the MIT license that can be
 * found in the LICENSE file.
 */

#include <provider_vision/media/camera_configuration.h>
#include "provider_vision/utils/pugixml.h"

namespace provider_vision {

CameraConfiguration::CameraConfiguration(const ros::NodeHandle &nh,
                                         const std::string &name) ATLAS_NOEXCEPT
    : guid_(0),
      name_("Default Camera"),
      framerate_(0),
      gain_manual_(true),
      shutter_manual_(true),
      white_balance_manual_(true),
      gain_(350.0),
      shutter_(500.0),
      gamma_(0),
      exposure_(0),
      white_balance_blue_(511.0),
      white_balance_red_(412.0),
      nh_(nh) {
  DeserializeConfiguration(name);
}

//------------------------------------------------------------------------------
//
CameraConfiguration::CameraConfiguration(const CameraConfiguration &rhs)
    ATLAS_NOEXCEPT {
  guid_ = rhs.guid_;
  name_ = rhs.name_;
  framerate_ = rhs.framerate_;
  gain_manual_ = rhs.gain_manual_;
  shutter_manual_ = rhs.shutter_manual_;
  white_balance_manual_ = rhs.white_balance_manual_;
  gain_ = rhs.gain_;
  shutter_ = rhs.shutter_;
  gamma_ = rhs.gamma_;
  exposure_ = rhs.exposure_;
  white_balance_blue_ = rhs.white_balance_blue_;
  white_balance_red_ = rhs.white_balance_red_;
  nh_ = rhs.nh_;
}

//------------------------------------------------------------------------------
//
CameraConfiguration::CameraConfiguration(CameraConfiguration &&rhs)
    ATLAS_NOEXCEPT {
  guid_ = rhs.guid_;
  name_ = rhs.name_;
  framerate_ = rhs.framerate_;
  gain_manual_ = rhs.gain_manual_;
  shutter_manual_ = rhs.shutter_manual_;
  white_balance_manual_ = rhs.white_balance_manual_;
  gain_ = rhs.gain_;
  shutter_ = rhs.shutter_;
  gamma_ = rhs.gamma_;
  exposure_ = rhs.exposure_;
  white_balance_blue_ = rhs.white_balance_blue_;
  white_balance_red_ = rhs.white_balance_red_;
  nh_ = rhs.nh_;
}

CameraConfiguration::~CameraConfiguration() ATLAS_NOEXCEPT {}

//------------------------------------------------------------------------------
//
void CameraConfiguration::DeserializeConfiguration(const std::string &name)
    ATLAS_NOEXCEPT {
  FindParameter("/camera_parameters/" + name + "/guid", guid_);
  FindParameter("/camera_parameters/" + name + "/name", name_);
  FindParameter("/camera_parameters/" + name + "/framerate", framerate_);
  FindParameter("/camera_parameters/" + name + "/gain_manual", gain_manual_);
  FindParameter("/camera_parameters/" + name + "/shutter_manual",
                shutter_manual_);
  FindParameter("/camera_parameters/" + name + "/white_balance_manual",
                white_balance_manual_);
  FindParameter("/camera_parameters/" + name + "/gain", gain_);
  FindParameter("/camera_parameters/" + name + "/shutter", shutter_);
  FindParameter("/camera_parameters/" + name + "/gamma", gamma_);
  FindParameter("/camera_parameters/" + name + "/exposure", shutter_);
  FindParameter("/camera_parameters/" + name + "/white_balance_blue",
                white_balance_blue_);
  FindParameter("/camera_parameters/" + name + "/white_balance_red",
                white_balance_red_);
}

//------------------------------------------------------------------------------
//
template <typename Tp_>
void CameraConfiguration::FindParameter(const std::string &str,
                                        Tp_ &p) ATLAS_NOEXCEPT {
  if (nh_.hasParam("/provider_vision" + str)) {
    nh_.getParam("/provider_vision" + str, p);
  } else {
    ROS_WARN_STREAM("Did not find /provider_vision" + str
                    << ". Using default value instead.");
  }
}

}  // namespace provider_vision
