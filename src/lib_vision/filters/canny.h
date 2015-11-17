/**
 * \file  Sobel.h
 * \author  Jérémie St-Jules Prévôt <jeremie.st.jules.prevost@gmail.com>
 * \date  14/12/2014
 * \copyright Copyright (c) 2015 SONIA AUV ETS. All rights reserved.
 * Use of this source code is governed by the MIT license that can be
 * found in the LICENSE file.
 */

#ifndef VISION_FILTER_CANNY_H_
#define VISION_FILTER_CANNY_H_

//==============================================================================
// I N C L U D E   F I L E S

#include <lib_vision/filter.h>

namespace lib_vision {

//==============================================================================
// C L A S S E S

class Canny : public Filter {
 public:
  //============================================================================
  // C O N S T R U C T O R S   A N D   D E S T R U C T O R

  explicit Canny(const GlobalParamHandler &globalParams)
      : Filter(globalParams),
        _enable("Enable", false, parameters_),
        _l2_gradiant("l2_gradient", false, parameters_),
        _thresh_one("thres_one", 100, 0, 255, parameters_),
        _thresh_two("thres_two", 200, 0, 255, parameters_),
        _aperture_size("Aperture_size", 3, 0, 20, parameters_) {
    setName("Canny");
  }

  virtual ~Canny() {}

  //============================================================================
  // P U B L I C   M E T H O D S

  virtual void execute(cv::Mat &image) {
    if (_enable()) {
      if (image.channels() > 1) {
        cv::cvtColor(image, image, CV_BGR2GRAY);
      }
      cv::Canny(image, image, _thresh_one(), _thresh_two(),
                _aperture_size() * 2 + 1, _l2_gradiant());
    }
  }

 private:
  // Params
  BooleanParameter _enable, _l2_gradiant;
  IntegerParameter _thresh_one, _thresh_two, _aperture_size;
};

}  // namespace lib_vision

#endif  // VISION_FILTER_SOBEL_H_
