/**
 * \file	CamCameraDC1394.cpp
 * \author	Jérémie St-Jules <jeremie.st.jules.prevost@gmail.com>
 * \date	18/10/2014
 * \copyright	Copyright (c) 2015 SONIA AUV ETS. All rights reserved.
 * Use of this source code is governed by the MIT license that can be
 * found in the LICENSE file.
 */

#ifndef PROVIDER_VISION_BaseMedia_H_
#define PROVIDER_VISION_BaseMedia_H_

#include <memory>
#include <opencv2/core/core.hpp>
#include <provider_vision/media/camera_configuration.h>
#include "provider_vision/utils/config.h"

namespace vision_server {

/**
 * Base class for anything that can provide an image to the system
 * implement basic functionality that is called through the system.
 */
class BaseMedia {
 public:
  //==========================================================================
  // T Y P E D E F   A N D   E N U M

  using Ptr = std::shared_ptr<BaseMedia>;

  enum class Status { OPEN, STREAMING, CLOSE, ERROR };

  //==========================================================================
  // P U B L I C   C / D T O R S

  explicit BaseMedia(const CameraConfiguration &config)
      : config_(config), status_(Status::CLOSE) {}

  virtual ~BaseMedia() = default;

  //==========================================================================
  // P U B L I C   M E T H O D S

  /**
   * Starts to get images
   */
  virtual void Start() = 0;

  /**
   * Stop getting images
   */
  virtual void Stop() = 0;

  /**
   * Gives the most recent image
   */
  virtual void NextImage(cv::Mat &image) = 0;

  //==========================================================================
  // G E T T E R S   A N D   S E T T E R S

  /**
   * Returns the current camera Status
   */
  virtual Status GetStatus() const;

  /**
   * Makes return true if it does not have a proper framerate
   * i.e. Images and video;
   */
  virtual bool HasArtificialFramerate() const;

  /**
   * Return the CameraID, the general identifier for a BaseMedia in the system.
   */
  virtual const CameraConfiguration &GetCameraConfiguration() const;

  const std::string &GetName() const;

  bool IsOpened() const;
  bool IsClosed() const;
  bool IsStreaming() const;

 protected:
  //==========================================================================
  // P R O T E C T E D   M E M B E R S

  CameraConfiguration config_;
  Status status_;
};

//==============================================================================
// I N L I N E   F U N C T I O N S   D E F I N I T I O N S

//------------------------------------------------------------------------------
//
inline BaseMedia::Status BaseMedia::GetStatus() const { return status_; };

//------------------------------------------------------------------------------
//
inline bool BaseMedia::HasArtificialFramerate() const { return true; }

//------------------------------------------------------------------------------
//
inline const CameraConfiguration &BaseMedia::GetCameraConfiguration() const {
  return config_;
}

//------------------------------------------------------------------------------
//
inline const std::string &BaseMedia::GetName() const {
  return config_.GetName();
}

//------------------------------------------------------------------------------
//
inline bool BaseMedia::IsOpened() const { return Status::OPEN == status_; }

//------------------------------------------------------------------------------
//
inline bool BaseMedia::IsClosed() const { return Status::CLOSE == status_; }

//------------------------------------------------------------------------------
//
inline bool BaseMedia::IsStreaming() const {
  return Status::STREAMING == status_;
}

}  // namespace vision_server
#endif  // PROVIDER_VISION_BaseMedia_H_
