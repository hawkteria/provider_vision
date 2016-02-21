/**
 * \file	base_camera.cpp
 * \author	Jérémie St-Jules <jeremie.st.jules.prevost@gmail.com>
 * \author	Thibaut Mattio <thibaut.mattio@gmail.com>
 * \date	19/05/2015
 * \copyright	Copyright (c) 2015 SONIA AUV ETS. All rights reserved.
 * Use of this source code is governed by the MIT license that can be
 * found in the LICENSE file.
 */

#include <ros/ros.h>
#include "provider_vision/media/camera/base_camera.h"

namespace provider_vision {

//==============================================================================
// C / D T O R S   S E C T I O N

//------------------------------------------------------------------------------
//
BaseCamera::BaseCamera(const CameraConfiguration &configuration)
    : BaseMedia(configuration) {
  undistord_matrix_.InitMatrices(config_.GetUndistortionMatricePath());
}

//------------------------------------------------------------------------------
//
BaseCamera::~BaseCamera() {}

//==============================================================================
// M E T H O D   S E C T I O N

//------------------------------------------------------------------------------
//
void BaseCamera::SetFeature(const Feature &feat, float value) {
  std::stringstream ss;
  ss << std::hex << config_.GetGUID() << " " << GetName();

  switch (feat) {
    case Feature::SHUTTER:
      SetShutterValue(value);
      break;
    case Feature::SHUTTER_AUTO:

      if (value > 0) {
        SetShutterAuto();
      } else {
        //          //ROS_INFO_NAMED(ss.str(), "Setting shutter to manual");
        SetShutterMan();
      }
      break;
    case Feature::GAIN_AUTO:

      if (value > 0) {
        //          //ROS_INFO_NAMED(ss.str(), "Setting gain to auto");
        SetGainAuto();
      } else {
        //          //ROS_INFO_NAMED(ss.str(), "Setting gain to manual");
        SetGainMan();
      }
      break;
    case Feature::GAIN:
      ////ROS_INFO_NAMED(ss.str(), "Setting gain to manual %f", value);
      SetGainValue(value);
      break;
    case Feature::FRAMERATE:
      ////ROS_INFO_NAMED(sEXPOSUREs.str(), "Setting framerate to %f", value);
      SetFrameRateValue(value);
    case Feature::WHITE_BALANCE_AUTO:
      if (value > 0) {
        ////ROS_INFO_NAMED(ss.str(), "Setting auto white balance to auto");
        SetWhiteBalanceAuto();

      } else {
        ////ROS_INFO_NAMED(ss.str(), "Setting auto white balance to manual");
        SetWhiteBalanceMan();
      }
      break;
    case Feature::WHITE_BALANCE_BLUE:
      ////ROS_INFO_NAMED(ss.str(), "Setting blue value to %f", value);
      SetWhiteBalanceBlueValue(value);
      break;
    case Feature::WHITE_BALANCE_RED:
      ////ROS_INFO_NAMED(ss.str(), "Setting red value to %f", value);
      SetWhiteBalanceRedValue(value);

      break;
    case Feature::EXPOSURE:
      SetExposureValue(value);
      break;
    case Feature::GAMMA:
      SetGammaValue(value);
      break;
    case Feature::SATURATION:
      SetSaturationValue(value);
      break;
    case Feature::ERROR_FEATURE:
    default:
      break;
  }

}

//------------------------------------------------------------------------------
//
float BaseCamera::GetFeature(const Feature &feat) const {

  try {
    switch (feat) {
      case Feature::SHUTTER:
        return GetShutterValue();
      case Feature::SHUTTER_AUTO:
        return GetShutterMode();
      case Feature::FRAMERATE:
        return GetFrameRateValue();
      case Feature::WHITE_BALANCE_AUTO:
        return GetWhiteBalanceMode();
      case Feature::WHITE_BALANCE_BLUE:
        return GetWhiteBalanceBlue();
      case Feature::WHITE_BALANCE_RED:
        return GetWhiteBalanceRed();
      case Feature::ERROR_FEATURE:
        break;
      case Feature::GAIN:
        return GetGainValue();
      case Feature::GAMMA:
        return GetGammaValue();
      case Feature::EXPOSURE:
        return GetExposureValue();
      case Feature::SATURATION:
        return GetSaturationValue();
      default:
        break;
    }
  } catch (const std::runtime_error &e) {
    ROS_ERROR(e.what());
  }
}

//------------------------------------------------------------------------------
//
double BaseCamera::UpdatePID(const std::shared_ptr<SPid> &pid, double error, double position) ATLAS_NOEXCEPT
{
  double pTerm, dTerm, iTerm;
  pTerm = pid->pGain * error;
  // calculate the proportional term
  // calculate the integral state with appropriate limiting
  pid->iState += error;
  if (pid->iState > pid->iMax) pid->iState = pid->iMax;
  else if (pid->iState < pid->iMin) pid->iState = pid->iMin;
  iTerm = pid->iGain * pid->iState; // calculate the integral term
  dTerm = pid->dGain * (pid->dState - position);
  pid->dState = position;
  return pTerm + dTerm + iTerm;
}

}  // namespace provider_vision
