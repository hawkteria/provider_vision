//
// Created by parallels on 5/4/16.
//

#include "provider_vision/media/camera/gige_camera.h"
#include <ros/ros.h>
#include <string>



namespace provider_vision {

const std::string GigeCamera::CAM_TAG = "[GigE Camera]";

//==============================================================================
// C / D T O R S   S E C T I O N

//------------------------------------------------------------------------------
//
GigeCamera::GigeCamera(GEV_CAMERA_HANDLE *camera, const CameraConfiguration &config)
    : BaseCamera(config), gige_camera_(camera) { }

//------------------------------------------------------------------------------
//
GigeCamera::~GigeCamera() {
  GevAbortImageTransfer(gige_camera_);
  GevFreeImageTransfer(gige_camera_);
  GevCloseCamera(gige_camera_);
  GevApiInitialize();
  _CloseSocketAPI();
}

void GigeCamera::Open() {

  UINT16 status;

  if (IsOpened()) {
    throw std::logic_error("The media is already started");
  }

  std::lock_guard<std::mutex> guard(cam_access_);

  try {
    GEV_CAMERA_INFO *camera_info = GevGetCameraInfo(gige_camera_);
    status = GevOpenCameraByAddress(camera_info->ipAddr, GevExclusiveMode, gige_camera_);
    if (status != 0) {
      throw std::runtime_error(GevGetFormatString(status));
    }
  } catch (std::exception &e) {
    ROS_ERROR("%s", e.what());
    ROS_ERROR("Error opening GigE camera");
  }
}

//------------------------------------------------------------------------------
//
void GigeCamera::Close() {
  if (!IsOpened()) {
    throw std::logic_error("The media is not started");
  }

  std::lock_guard<std::mutex> guard(cam_access_);

  bool close_result = true;
  if (status_ == Status::STREAMING) {
    StopStreaming();
  }

  GEV_STATUS status = GevCloseCamera(gige_camera_);

  if (status != GEVLIB_OK) {
    close_result = false;
  }

  close_result ? status_ = Status::CLOSE : status_ = Status::ERROR;

}

//------------------------------------------------------------------------------
//
void GigeCamera::SetStreamingModeOn() {
  cam_access_.lock();
  GEV_STATUS status = GevStartImageTransfer(gige_camera_, -1);
  if (status == GEVLIB_ERROR_INVALID_HANDLE) {
    status_ = Status::ERROR;
    throw std::runtime_error("Invalid handle. Cannot set streaming mode on.");
  }
  else if (status == GEV_STATUS_BUSY) {
    status_ = Status::ERROR;
    throw std::runtime_error("Camera is busy. Cannot set streaming mode on.");
  }
  cam_access_.unlock();

  status_ = Status::STREAMING;
}

//------------------------------------------------------------------------------
//
void GigeCamera::SetStreamingModeOff() {
  std::lock_guard<std::mutex> guard(cam_access_);

  GEV_STATUS status = GevStopImageTransfer(gige_camera_);
  if (status != GEVLIB_OK) {
    status_ = Status::ERROR;
    throw std::runtime_error("Invalid handle. The camera could not be stopped.");
  }
}

//------------------------------------------------------------------------------
//
void GigeCamera::NextImage(cv::Mat &img) {
  GEV_BUFFER_OBJECT *frame = NULL;

  timer_access_.lock();
  acquisition_timer_.Sleep(3);
  acquisition_timer_.Start();
  timer_access_.unlock();

  GEV_STATUS status = GevWaitForNextImage(gige_camera_, &frame, 1000);

  cam_access_.unlock();
  timer_access_.lock();
  atlas::MilliTimer::Sleep(3);
  timer_access_.unlock();

  if (status != 0) {
    status_ = Status::ERROR;
    throw std::runtime_error("Cannot get next image.");
  }

  if (frame != NULL) {
    try {
      cv::Mat tmp = cv::Mat(frame->h, frame->w, CV_8UC3, frame->address);
      //TODO: check which color space the image is and convert it
      undistord_matrix_.CorrectInmage(tmp, img);
    } catch (cv::Exception &e) {
      status_ = Status::ERROR;
      throw;
    }
  }

  if (img.empty() || img.size().height == 0 || img.size().height == 0) {
    throw std::runtime_error(
        "The image is empty, there is a problem with the media");
  }

  // Calibrate(img);

}

//------------------------------------------------------------------------------
//
float GigeCamera::GetGainValue() const {
  std::lock_guard<std::mutex> guard(cam_access_);

  uint32_t value;
  int type;
  GEV_STATUS status = GevGetFeatureValue(gige_camera_, "Gain", &type, sizeof(value), &value);

  if (status != 0) {
    throw std::runtime_error("Cannot get the gain value on GigE Camera");
  }

  return static_cast<float>(value);
}

float GigeCamera::GetGammaValue() const {
  std::lock_guard<std::mutex> guard(cam_access_);

  uint32_t value;
  int type;
  GEV_STATUS status = GevGetFeatureValue(gige_camera_, "Gamma", &type, sizeof(value), &value);

  if (status != 0) {
    throw std::runtime_error("Cannot get the gamma value on GigE Camera");
  }

  return static_cast<float>(value);
}

float GigeCamera::GetExposureValue() const {
  std::lock_guard<std::mutex> guard(cam_access_);

  uint32_t value;
  int type;
  GEV_STATUS status = GevGetFeatureValue(gige_camera_, "Exposure", &type, sizeof(value), &value);

  if (status != 0) {
    throw std::runtime_error("Cannot get the exposure value on GigE Camera");
  }

  return static_cast<float>(value);
}

float GigeCamera::GetSaturationValue() const {
  std::lock_guard<std::mutex> guard(cam_access_);

  uint32_t value;
  int type;
  GEV_STATUS status = GevGetFeatureValue(gige_camera_, "Saturation", &type, sizeof(value), &value);

  if (status != 0) {
    throw std::runtime_error("Cannot get the saturation value on GigE Camera");
  }

  return static_cast<float>(value);
}

void GigeCamera::SetGainAuto() {

}

void GigeCamera::SetGainManual() {

}

void GigeCamera::SetGainValue(float value) {

}

void GigeCamera::SetGammaValue(float value) {

}

void GigeCamera::SetExposureValue(float value) {

}

void GigeCamera::SetSaturationValue(float value) {

}

void GigeCamera::SetShutterValue(float value) {

}

void GigeCamera::SetShutterAuto() {

}

void GigeCamera::SetShutterManual() {

}

float GigeCamera::GetShutterMode() const {

}

float GigeCamera::GetShutterValue() const {

}

void GigeCamera::SetFrameRateValue(float value) {

}

float GigeCamera::GetFrameRateValue() const {

}

void GigeCamera::SetWhiteBalanceAuto() {

}

void GigeCamera::SetWhiteBalanceManual() {

}

float GigeCamera::GetWhiteBalanceMode() const {

}

float GigeCamera::GetWhiteBalanceRed() const {

}

void GigeCamera::SetWhiteBalanceRedValue(float value) {

}

void GigeCamera::SetWhiteBalanceBlueValue(float value) {

}

float GigeCamera::GetWhiteBalanceBlue() const {

}

}