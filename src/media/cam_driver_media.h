/**
 * \file	CamDriverMedia.h
 * \author	Jérémie St-Jules <jeremie.st.jules.prevost@gmail.com>
 * \date	10/03/2015
 * \copyright	Copyright (c) 2015 SONIA AUV ETS. All rights reserved.
 * Use of this source code is governed by the MIT license that can be
 * found in the LICENSE file.
 */

#ifndef VISION_SERVER_CAM_DRIVER_MEDIA_H_
#define VISION_SERVER_CAM_DRIVER_MEDIA_H_

//==============================================================================
// I N C L U D E   F I L E S

#include <opencv2/opencv.hpp>
#include "media/media.h"
#include "media/cam_driver.h"
#include "media/cam_webcam.h"
#include "config.h"

#include "media/media_image.h"
#include "media/media_video.h"

namespace vision_server {

//==============================================================================
// C L A S S E S

/**
 * Class managing medias from a hard drive.
 *
 * Allow to load a video or a picture from a hard drive onto the vision server.
 * The creation of a static media is done during the execution in the method
 * StartCamera().
 * We Currently use _live_camera_list because the responsability of camera_list
 * is to list all the available camera. As they are videos, they are all and
 * always available.
 * Therefore, calling GetCameraList() will return the list of videos from
 * _live_camera_list.
 */
class CAMDriverMedia : public CAMDriver {
 public:
  const char *DRIVER_TAG;

  //==========================================================================
  // T Y P E D E F   A N D   E N U M

  enum MEDIA_TYPE { IMAGE, VIDEO, NONE };

  //==========================================================================
  // C O N S T R U C T O R S   A N D   D E S T R U C T O R

  CAMDriverMedia(const CAMConfig config);

  virtual ~CAMDriverMedia();

  //==========================================================================
  // P U B L I C   M E T H O D S

  void InitDriver() override;

  void CloseDriver() override;

  bool StartCamera(CameraID id) override;

  bool StopCamera(CameraID id) override;

  std::vector<CameraID> GetCameraList() override;

  bool IsMyCamera(const std::string &nameMedia) override;

  Media::Ptr GetActiveCamera(CameraID id) override;

  void SetFeature(FEATURE feat, CameraID id, float val) override;

  void GetFeature(FEATURE feat, CameraID id, float &val) override;

  /**
   * HTThread override
   * Is traditionally use to call the watchdog.
   */
  void ThreadFunc() override;

  bool WatchDogFunc() override;

  /**
   * IMPORTANT TO OVERRIDE since the driver look in camera_list,
   * not live_camera_list
   */
  CameraID GetIDFromName(const std::string &name) override;

 private:
  //==========================================================================
  // P R I V A T E   M E T H O D S

  /**
   * Copy all the data from _live_camera_list into _camera_list
   */
  void PopulateCameraList() override;

  /**
   * Return the type of a specific media passed in parameters.
   * The parameter can be either en image or a video
   */
  virtual MEDIA_TYPE GetMediaType(const std::string &nameMedia);
};

//==============================================================================
// I N L I N E   F U N C T I O N S   D E F I N I T I O N S

//------------------------------------------------------------------------------
//
inline CameraID CAMDriverMedia::GetIDFromName(const std::string &name) {
  if (IsMyCamera(name)) {
    return CameraID(name, 0);
  }
  return CameraID();
}

}  // namespace vision_server

#endif  // VISION_SERVER_CAM_DRIVER_MEDIA_H_
