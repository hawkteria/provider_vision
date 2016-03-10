/**
 * \file	sobel.h
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

#ifndef PROVIDER_VISION_FILTERS_SOBEL_H_
#define PROVIDER_VISION_FILTERS_SOBEL_H_

#include <provider_vision/filters/filter.h>
#include <memory>

namespace provider_vision {

class Sobel : public Filter {
 public:
  //==========================================================================
  // T Y P E D E F   A N D   E N U M

  using Ptr = std::shared_ptr<Sobel>;

  //============================================================================
  // P U B L I C   C / D T O R S

  explicit Sobel(const GlobalParamHandler &globalParams)
      : Filter(globalParams),
        enable_("Enable", false, &parameters_),
        convert_to_uchar_("Convert_to_uchar", true, &parameters_),
        use_pixel_intensity_correction_("use_pixel_intensity_correction", false,
                                        &parameters_),
        delta_("Delta", 0, 0, 255, &parameters_),
        scale_("Scale", 1, 0, 255, &parameters_),
        power_pixel_correction_("pixel_correction_power", 1, -10, 10,
                                &parameters_),
        size_("Size", 2, 1, 20, &parameters_) {
    SetName("Sobel");
  }

  virtual ~Sobel() {}

  //============================================================================
  // P U B L I C   M E T H O D S

  virtual void Execute(cv::Mat &image) {
    if (enable_()) {
      if (image.channels() > 1) {
        cv::cvtColor(image, image, CV_BGR2GRAY);
      }
      cv::Mat sobelX, sobelY;
      int size = size_() * 2 + 1;
      cv::Sobel(image, sobelX, CV_32F, 1, 0, size, scale_(), delta_(),
                cv::BORDER_DEFAULT);
      cv::Sobel(image, sobelY, CV_32F, 0, 1, size, scale_(), delta_(),
                cv::BORDER_DEFAULT);

      cv::absdiff(sobelX, 0, sobelX);
      cv::absdiff(sobelY, 0, sobelY);
      cv::addWeighted(sobelX, 0.5, sobelY, 0.5, 0, image, CV_32F);

      if (use_pixel_intensity_correction_()) {
        for (int y = 0; y < image.rows; y++) {
          float *ptr = image.ptr<float>(y);
          for (int x = 0; x < image.cols; x++) {
            ptr[x] = pow(ptr[x], power_pixel_correction_());
          }
        }
      }

      if (convert_to_uchar_()) {
        cv::convertScaleAbs(image, image);
      }
    }
  }

 private:
  //============================================================================
  // P R I V A T E   M E M B E R S

  Parameter<bool> enable_, convert_to_uchar_, use_pixel_intensity_correction_;
  RangedParameter<double> delta_, scale_, power_pixel_correction_;
  RangedParameter<int> size_;
};

}  // namespace provider_vision

#endif  // PROVIDER_VISION_FILTERS_SOBEL_H_
