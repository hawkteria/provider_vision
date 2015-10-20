/**
 * \file	MediaManager.h
 * \author  Thibaut Mattio <thibaut.mattio@gmail.com>
 * \author	Jérémie St-Jules <jeremie.st.jules.prevost@gmail.com>
 * \date	12/10/2015
 * \copyright	Copyright (c) 2015 SONIA AUV ETS. All rights reserved.
 * Use of this source code is governed by the MIT license that can be
 * found in the LICENSE file.
 */

#ifndef PROVIDER_VISION_CAMERA_MANAGER_H_
#define PROVIDER_VISION_CAMERA_MANAGER_H_

#include <memory>
#include <vector>
#include <string>
#include <provider_vision/media/camera/base_media.h>
#include "provider_vision/media/camera/base_camera.h"
#include "provider_vision/media/context/base_context.h"
#include "provider_vision/media/media_streamer.h"

namespace vision_server {

class MediaManager {
 public:
  //==========================================================================
  // T Y P E D E F   A N D   E N U M

  using Ptr = std::shared_ptr<MediaManager>;

  //==========================================================================
  // P U B L I C   C / D T O R S

  MediaManager() noexcept;

  ~MediaManager() noexcept;

  //==========================================================================
  // P U B L I C   M E T H O D S

  /**
   * Start the acquisition of the images on the given media.
   *
   * By default, the media streamer will not be streaming, if the stream flag
   * is set to true, it will start a thread and notify all observers whenever
   * there is an image. See MediaStreamer class for more informations.
   */
  MediaStreamer::Ptr StartMedia(const std::string &media_name,
                                bool stream = false);

  void StopMedia(const std::string &media) noexcept;

  std::vector<std::string> GetAllMediasName() const noexcept;

  /**
   * If the media is a camera, set the feature to a specific value.
   *
   * This will try to convert the media to a camera. If this work, calls the
   * method to set a feature on it. If it does not, this throws an exception.
   *
   * \param media_name The name of the media to set the feature to.
   * \param feature The feature to change the value of.
   * \param value The value to set on the given feature.
   */
  void SetCameraFeature(const std::string &media_name,
                        const std::string &feature, float value);

  /**
   * If the media is a camera, get the value of the given feature.
   *
   * This will try to convert the media to a camera. If this work, calls the
   * method to ge the value of a feature on it. If it does not, this throws
   * an exception.
   *
   * \param media_name The name of the media to set the feature to.
   * \param feature The feature to change the value of.
   */
  float GetCameraFeature(const std::string &media_name,
                         const std::string &feature);

 private:
  //==========================================================================
  // P R I V A T E   M E T H O D S

  BaseMedia::Ptr GetMedia(const std::string &name) const noexcept;

  MediaStreamer::Ptr GetMediaStreamer(const std::string &name);

  bool IsMediaStreamerExist(const std::string &name);

  BaseContext::Ptr GetContextFromMedia(const std::string &name) const;

  BaseCamera::Feature GetFeatureFromName(const std::string &name) const;

  void AddMediaStreamer(MediaStreamer::Ptr media_streamer);

  void RemoveMediaStreamer(const std::string &name);

  //==========================================================================
  // P R I V A T E   M E M B E R S

  std::vector<BaseContext::Ptr> contexts_;

  std::vector<MediaStreamer::Ptr> media_streamers_;
};

//-----------------------------------------------------------------------------
//
inline MediaStreamer::Ptr MediaManager::GetMediaStreamer(
    const std::string &name) {
  MediaStreamer::Ptr media_ptr(nullptr);

  for (const auto &elem : media_streamers_) {
    if (elem->GetMediaName().compare(name) == 0) {
      media_ptr = elem;
    }
  }
  return media_ptr;
}

//-----------------------------------------------------------------------------
//
inline void MediaManager::AddMediaStreamer(MediaStreamer::Ptr media_streamer) {
  media_streamers_.push_back(media_streamer);
}

//-----------------------------------------------------------------------------
//
inline void MediaManager::RemoveMediaStreamer(const std::string &name) {
  for (auto elem = media_streamers_.begin(); elem != media_streamers_.end();
       elem++) {
    if ((*elem)->GetMediaName().compare(name) == 0) {
      media_streamers_.erase(elem);
      return;
    }
  }
}

//-------------------------------------------------------------------------
//
inline bool MediaManager::IsMediaStreamerExist(const std::string &name) {
  for (const auto &elem : media_streamers_) {
    if (name.compare(elem->GetMediaName()) == 0) {
      return true;
    }
  }
  return false;
}

}  // namespace vision_server

#endif  // PROVIDER_VISION_CAMERA_MANAGER_H_
