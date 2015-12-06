/**
 * \file	subtract_all_planes.h
 * \author	Jérémie St-Jules Prévôt <jeremie.st.jules.prevost@gmail.com>
 * \author  Pierluc Bédard <pierlucbed@gmail.com>
 *
 * \copyright Copyright (c) 2015 S.O.N.I.A. All rights reserved.
 *
 * \section LICENSE
 *
 * This file is part of S.O.N.I.A. software.
 *
 * S.O.N.I.A. software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * S.O.N.I.A. software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with S.O.N.I.A. software. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIB_VISION_FILTERS_SUBTRACT_ALL_PLANES_H_
#define LIB_VISION_FILTERS_SUBTRACT_ALL_PLANES_H_

#include <memory>
#include <lib_vision/filter.h>
#include <lib_vision/algorithm/general_function.h>

namespace lib_vision {

// Filter showing planes of different analysis (gray, _hsi, _bgr)
// No threshold
class SubtractAllPlanes : public Filter {
 public:
  //==========================================================================
  // T Y P E D E F   A N D   E N U M

  using Ptr = std::shared_ptr<SubtractAllPlanes>;

  //============================================================================
  // P U B L I C   C / D T O R S

  explicit SubtractAllPlanes(const GlobalParamHandler &globalParams)
      : Filter(globalParams),
        enable_("enable", false, &parameters_),
        _plane_one("Plane_1", 1, 0, 7, &parameters_,
                   "0=None, 1=Blue, 2=Green, 3=Red, 4=Hue, 5=Saturation, "
                   "6=Intensity, 7=Gray"),
        _plane_two("Plane_2", 1, 0, 7, &parameters_,
                   "0=None, 1=Blue, 2=Green, 3=Red, 4=Hue, 5=Saturation, "
                   "6=Intensity, 7=Gray"),
        _plane_three("Plane_3", 1, 0, 7, &parameters_,
                     "0=None, 1=Blue, 2=Green, 3=Red, 4=Hue, 5=Saturation, "
                     "6=Intensity, 7=Gray"),
        _invert_one("Invert_plane_1", false, &parameters_),
        _invert_two("Invert_plane_2", false, &parameters_),
        _invert_three("Invert_plane_3", false, &parameters_),
        _weight_one("Weight_Plane_1", 1.0, 0.0, 10.0, &parameters_),
        _weight_two("Weight_Plane_2", 1.0, 0.0, 10.0, &parameters_),
        _weight_three("Weight_Plane_3", 1.0, 0.0, 10.0, &parameters_),
        rows_(0),
        cols_(0) {
    SetName("SubtractAllPlanes");
  }

  virtual ~SubtractAllPlanes() {}

  //============================================================================
  // P U B L I C   M E T H O D S

  virtual void Execute(cv::Mat &image) {
    if (enable_()) {
      if (CV_MAT_CN(image.type()) != 3) {
        return;
      }

      rows_ = image.rows;
      cols_ = image.cols;
      // Set final matrices
      cv::Mat zero = cv::Mat::zeros(rows_, cols_, CV_8UC1);
      cv::Mat one = cv::Mat::zeros(rows_, cols_, CV_8UC1);
      cv::Mat two = cv::Mat::zeros(rows_, cols_, CV_8UC1);
      cv::Mat three = cv::Mat::zeros(rows_, cols_, CV_8UC1);
      cv::Mat final = cv::Mat::zeros(rows_, cols_, CV_8UC1);

      // Replace with new images

      _channel_vec = GetColorPlanes(image);

      // Set subtraction
      if (_plane_one() != 0)
        set_image(_plane_one() - 1, one, _weight_one(), _invert_one());

      if (_plane_two() != 0)
        set_image(_plane_two() - 1, two, _weight_two(), _invert_two());

      if (_plane_three() != 0)
        set_image(_plane_three() - 1, three, _weight_three(), _invert_three());

      cv::subtract(one, two, final);
      cv::subtract(final, three, final);

      final.copyTo(image);
    }
  }

 private:
  //============================================================================
  // P R I V A T E   M E T H O D S

  void set_image(const int choice, cv::Mat &out, const double weight,
                 const bool inverse) {
    cv::Mat two_five_five(rows_, cols_, CV_16SC1, cv::Scalar(255));
    cv::Mat one(rows_, cols_, CV_16SC1, cv::Scalar(1));

    // Thightly couple with parameter, but putting safety...
    int index = choice < 0 ? 0 : (choice > 6 ? 6 : choice);
    _channel_vec[index].copyTo(out);

    if (inverse) {
      InverseImage(out, out);
    }
    cv::multiply(out, one, out, weight, CV_8UC1);
  }

  //============================================================================
  // P R I V A T E   M E M B E R S

  Parameter<bool> enable_;
  RangedParameter<int> _plane_one, _plane_two, _plane_three;
  Parameter<bool> _invert_one, _invert_two, _invert_three;
  RangedParameter<double> _weight_one, _weight_two, _weight_three;

  // Color matrices
  std::vector<cv::Mat> _channel_vec;

  int rows_;
  int cols_;
};

}  // namespace lib_vision

#endif  // LIB_VISION_FILTERS_SUBTRACT_ALL_PLANES_H_
