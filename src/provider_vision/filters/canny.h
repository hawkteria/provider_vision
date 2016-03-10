/**
 * \file	canny.h
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

#ifndef PROVIDER_VISION_FILTERS_CANNY_H_
#define PROVIDER_VISION_FILTERS_CANNY_H_

#include <provider_vision/filters/filter.h>
#include <memory>

namespace provider_vision {

class Canny : public Filter {
 public:
  //==========================================================================
  // T Y P E D E F   A N D   E N U M

  using Ptr = std::shared_ptr<Canny>;

  //============================================================================
  // P U B L I C   C / D T O R S

  explicit Canny(const GlobalParamHandler &globalParams)
      : Filter(globalParams),
        enable_("Enable", false, &parameters_),
        l2_gradiant_("l2_gradient", false, &parameters_),
        thresh_one_("thres_one", 100, 0, 255, &parameters_),
        thresh_two_("thres_two", 200, 0, 255, &parameters_),
        aperture_size_("Aperture_size", 3, 0, 20, &parameters_) {
    SetName("Canny");
  }

  virtual ~Canny() {}

  //============================================================================
  // P U B L I C   M E T H O D S

  virtual void Execute(cv::Mat &image) {
    if (enable_()) {
      if (image.channels() > 1) {
        cv::cvtColor(image, image, CV_BGR2GRAY);
      }
      cv::Canny(image, image, thresh_one_(), thresh_two_(),
                aperture_size_() * 2 + 1, l2_gradiant_());
    }
  }

 private:
  //============================================================================
  // P R I V A T E   M E M B E R S

  Parameter<bool> enable_, l2_gradiant_;
  RangedParameter<int> thresh_one_, thresh_two_, aperture_size_;
};

}  // namespace provider_vision

#endif  // PROVIDER_VISION_FILTERS_SOBEL_H_
