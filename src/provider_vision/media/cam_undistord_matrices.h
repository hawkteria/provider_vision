/// \author	Pierluc Bédard <pierlucbed@gmail.com>
/// \author	Jérémie St-Jules Prévôt <jeremie.st.jules.prevost@gmail.com>
/// \author	Thibaut Mattio <thibaut.mattio@gmail.com>
/// \copyright Copyright (c) 2015 S.O.N.I.A. All rights reserved.
/// \section LICENSE
/// This file is part of S.O.N.I.A. software.
///
/// S.O.N.I.A. software is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// S.O.N.I.A. software is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with S.O.N.I.A. software. If not, see <http://www.gnu.org/licenses/>.

#ifndef PROVIDER_VISION_MEDIA_UNDISTORD_MATRICES_H_
#define PROVIDER_VISION_MEDIA_UNDISTORD_MATRICES_H_

#include <memory>
#include <opencv2/opencv.hpp>
#include <string>
#include "lib_atlas/macros.h"

namespace provider_vision {

/**
 * CameraID is a little class to handle the link between GUID and name.
 * We can then identify a camera by its name but the system uses its GUID.
 */
class CameraUndistordMatrices {
 public:
  //==========================================================================
  // T Y P E D E F   A N D   E N U M

  using Ptr = std::shared_ptr<CameraUndistordMatrices>;

  //==========================================================================
  // P U B L I C   C / D T O R S

  CameraUndistordMatrices();

  virtual ~CameraUndistordMatrices();

  //==========================================================================
  // P U B L I C   M E T H O D S

  void InitMatrices(const std::string &fullPath);

  void GetMatrices(cv::Mat &cameraMatrix, cv::Mat &distortionMatrix);

  bool IsCorrectionEnable();

  void CorrectInmage(const cv::Mat &in, cv::Mat &out) const;

 private:
  //==========================================================================
  // P R I V A T E   M E M B E R S

  cv::Mat camera_matrix_, distortion_matrix_;

  std::string xml_file_path_;

  bool matrices_founded_;
};

//==============================================================================
// I N L I N E   F U N C T I O N S   D E F I N I T I O N S

//------------------------------------------------------------------------------
//
inline void CameraUndistordMatrices::GetMatrices(cv::Mat &cameraMatrix,
                                                 cv::Mat &distortionMatrix) {
  camera_matrix_.copyTo(cameraMatrix);
  distortion_matrix_.copyTo(distortionMatrix);
}

//------------------------------------------------------------------------------
//
inline bool CameraUndistordMatrices::IsCorrectionEnable() {
  return matrices_founded_;
}

//------------------------------------------------------------------------------
//
inline void CameraUndistordMatrices::CorrectInmage(const cv::Mat &in,
                                                   cv::Mat &out) const {
  if (matrices_founded_) {
    cv::undistort(in, out, camera_matrix_, distortion_matrix_);
  } else {
    in.copyTo(out);
  }
}

}  // namespace provider_vision

#endif  // PROVIDER_VISION_MEDIA_UNDISTORD_MATRICES_H_
