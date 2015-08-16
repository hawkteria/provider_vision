/**
 * \file	AcquisitionLoop.cpp
 * \author	Jérémie St-Jules <jeremie.st.jules.prevost@gmail.com>
 * \date	12/10/2015
 * \copyright	Copyright (c) 2015 SONIA AUV ETS. All rights reserved.
 * Use of this source code is governed by the MIT license that can be
 * found in the LICENSE file.
 */

//==============================================================================
// I N C L U D E   F I L E S

#include <ros/ros.h>
#include <lib_atlas/sys/timer.h>
#include "server/acquisition_loop.h"
#include <lib_atlas/sys/fsinfo.h>

namespace vision_server {

//==============================================================================
// C O N S T R U C T O R / D E S T R U C T O R   S E C T I O N

//------------------------------------------------------------------------------
//
AcquisitionLoop::AcquisitionLoop(std::shared_ptr<Media> cam, int artificialFrameRateMs)
    : _media(cam),
      LOOP_TAG("[Acquisition Loop]"),
      _is_streaming(false),
      _artificialFrameRate(artificialFrameRateMs),
      // here 30 in case we forget to set it, so the cpu doesn't blow off.
      _frameRateMiliSec(1000 / 30),
      _image(),
      video_writer_(),
      is_recording_(false) {
  if (_media->HasArtificialFramerate() && _artificialFrameRate != 0)
    _frameRateMiliSec = 1000 / _artificialFrameRate;
}

//------------------------------------------------------------------------------
//
AcquisitionLoop::~AcquisitionLoop() {
  if (_is_streaming) {
    StopStreaming();
  }
  ROS_INFO_NAMED(LOOP_TAG, "Destroying AcquisitionLoop");
}

//==============================================================================
// M E T H O D   S E C T I O N

//------------------------------------------------------------------------------
//
void AcquisitionLoop::SetFramerate(int framePerSecond) {
  _frameRateMiliSec = 1000 / framePerSecond;
}

//------------------------------------------------------------------------------
//
bool AcquisitionLoop::StartStreaming() {
  ROS_INFO_NAMED(LOOP_TAG, "Starting streaming on camera %s",
                 _media->GetCameraID().GetName().c_str());

  // Start thread
  std::lock_guard<std::mutex> guard(_image_access);
  start();
  if (thread_.joinable()) {
    _is_streaming = true;
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
//
bool AcquisitionLoop::StopStreaming() {
  _is_streaming = false;

  // Send message on the line.
  ROS_INFO_NAMED(LOOP_TAG, "Stopping streaming on camera %s",
                 _media->GetCameraID().GetName().c_str());
  // Stop thread
  if (thread_.joinable()) {
    stop();
    return true;
  } else {
    ROS_WARN_NAMED(LOOP_TAG, "Thread is not alive");
  }
}

//------------------------------------------------------------------------------
//
bool AcquisitionLoop::StartRecording(const std::string &filename) {
  if (IsRecording()) {
    // ROS_INFO("[VISION_CLIENT] startVideoCapture not opened.");
    return false;
  }

  std::string filepath = filename;
  // If no filename was provided, set the default filepath
  if (filepath.empty()) {
    // TODO Thibaut Mattio: Save the video with the current timer.
    filepath = atlas::kLogPath + "save_video" + ".avi";
    ROS_INFO_NAMED(LOOP_TAG, "Starting video on %s", filepath.c_str());
  }

  // mjpg et divx fonctionnels aussi, HFYU est en lossless
  video_writer_ =
      cv::VideoWriter(filepath, CV_FOURCC('D', 'I', 'V', 'X'), 15.0,
                      cv::Size(_image.size().width, _image.size().height));

  if (!video_writer_.isOpened()) {
    ROS_ERROR_NAMED(LOOP_TAG, "Video writer was not opened!");
    return false;
  }
  is_recording_ = true;
  return true;
}

//------------------------------------------------------------------------------
//
bool AcquisitionLoop::StopRecording() {
  if (IsRecording()) {
    video_writer_.release();
    is_recording_ = false;
    return true;
  }
  ROS_INFO("[VISION_CLIENT] stopVideoCapture video is not running.");
  return false;
}

//------------------------------------------------------------------------------
//
void AcquisitionLoop::run() {
  bool acquival = false;
  bool must_set_record = false;

  // If the media is a real camera, start recording the feed.
  if (_media->IsRealCamera()) {
    must_set_record = true;
  }

  while (!stop_) {
    //_logger->LogInfo(LOOP_TAG, "Taking mutex for publishing");
    _image_access.lock();
    //_logger->LogInfo(LOOP_TAG, "Took mutex for publishing");
    acquival = _media->NextImage(_image);
    _image_access.unlock();
    //_logger->LogInfo(LOOP_TAG, "Releasing mutex for publishing");

    if (!acquival) {
      _image = cv::Mat::zeros(100, 100, CV_8UC3);
      ROS_ERROR_NAMED(LOOP_TAG, "Error on NextImage. Providing empty image %s",
                      _media->GetCameraID().GetName().c_str());
    } else {
      // This should not be the proper way to do this.
      // Actually the media, or even better the camera driver sould provide a
      // way
      // to know the size of the video feed they (are going to) send.
      // But for testing purpose we won't do such a modification.
      if (must_set_record) {
        // StartRecording();
        must_set_record = false;
      }

      if (IsRecording() && atlas::sys::percentage_used_physical_memory() < .8) {
        //        cv::Mat image_with_correct_format;
        //        cv::cvtColor(_image, image_with_correct_format, CV_BGR2RGB);
        //        video_writer_.write(image_with_correct_format);
        video_writer_.write(_image);
      }

      // Adding this here, because if the thread has been close,
      // but we have not pass through the while yet, we want to check that...
      if (IsStreaming()) {
        Notify();
      }
    }

    atlas::MilliTimer::sleep(_frameRateMiliSec);
  }

  if (IsRecording()) {
    StopRecording();
  }
}

//------------------------------------------------------------------------------
//
bool AcquisitionLoop::GetImage(cv::Mat &image) {
  bool retval = false;

  //_logger->LogInfo(LOOP_TAG, "Taking mutex for getting image");
  std::lock_guard<std::mutex> guard(_image_access);
  //_logger->LogInfo(LOOP_TAG, "Took mutex for getting image");
  if (_image.empty()) {
    image = cv::Mat::zeros(100, 100, CV_8UC3);
    ROS_ERROR_NAMED(LOOP_TAG, "Image is empty");
    return retval;
  }

  // Here we clone, since we want the acquisition loop to be the only
  // owner of the image. A copy would also give the memory address.
  try {
    image = _image.clone();
    retval = true;
  } catch (cv::Exception &e) {
    ROS_ERROR_NAMED(LOOP_TAG, "Exception in cloning (%s)", e.what());
    image = cv::Mat::zeros(100, 100, CV_8UC3);
    retval = false;
  };
  return retval;
}

//------------------------------------------------------------------------------
//
const CameraID AcquisitionLoop::GetMediaID() { return _media->GetCameraID(); }

//------------------------------------------------------------------------------
//
const STATUS AcquisitionLoop::GetMediaStatus() { return _media->getStatus(); }

}  // namespace vision_server
