/**
 * \file	ObjectFinder.h
 * \author  Jérémie St-Jules Prévôt <jeremie.st.jules.prevost@gmail.com>
 * \date	14/12/2014
 * \copyright	Copyright (c) 2015 SONIA AUV ETS. All rights reserved.
 * Use of this source code is governed by the MIT license that can be
 * found in the LICENSE file.
 */

#ifndef VISION_FILTER_HANDLE_DETECTOR_H_
#define VISION_FILTER_HANDLE_DETECTOR_H_

//==============================================================================
// I N C L U D E   F I L E S

#include <lib_vision/filter.h>
#include <lib_vision/algorithm/features.h>
#include <lib_vision/algorithm/general_function.h>
#include <lib_vision/algorithm/target.h>
#include <lib_vision/algorithm/object_full_data.h>
#include <lib_vision/algorithm/feature_factory.h>
#include <lib_vision/algorithm/time.h>

namespace vision_filter {

//==============================================================================
// C L A S S E S

class HandleDetector : public Filter {
 public:
  //============================================================================
  // C O N S T R U C T O R S   A N D   D E S T R U C T O R

  explicit HandleDetector(const GlobalParamHandler &globalParams)
      : Filter(globalParams),
        _enable("Enable", false, parameters_),
        _debug_contour("Debug_contour", false, parameters_),
        _look_for_rectangle("Look_for_Rectangle", false, parameters_),
        _disable_ratio("disable_ratio_check", false, parameters_),
        _disable_angle("disable_angle_check", false, parameters_),
        _id("ID", "buoy", parameters_),
        _spec_1("spec1", "red", parameters_),
        _spec_2("spec2", "blue", parameters_),
        _min_area("Min_area", 200, 0, 10000, parameters_),
        _targeted_ratio("Ratio_target", 0.5f, 0.0f, 1.0f, parameters_),
        _difference_from_target_ratio("Diff_from_ratio_target", 0.10f, 0.0f,
                                      1.0f, parameters_),
        _targeted_angle("angle_target", 0.0f, 0.0f, 90.0f, parameters_),
        _difference_from_target_angle("Diff_from_angle_target", 30.0f, 0.0f,
                                      90.0f, parameters_),
        _feature_factory(5) {
    setName("HandleDetector");
    _feature_factory.SetAllFeatureToCompute();
    // Little goodies for cvs
    // area_rank,length_rank,circularity,convexity,ratio,presence,percent_filled,hueMean,
  }

  virtual ~HandleDetector() {}

  //============================================================================
  // P U B L I C   M E T H O D S

  virtual void execute(cv::Mat &image) {
    if (_enable()) {
      if (_debug_contour()) {
        image.copyTo(_output_image);
        if (_output_image.channels() == 1) {
          cv::cvtColor(_output_image, _output_image, CV_GRAY2BGR);
        }
      }

      if (image.channels() != 1) cv::cvtColor(image, image, CV_BGR2GRAY);
      cv::Mat originalImage = global_params_.getOriginalImage();

      Time timer;
      timer.UpdateStartTime();

      contourList_t contours;
      retrieveAllContours(image, contours);
      ObjectFullData::FullObjectPtrVec objVec;
      for (int i = 0, size = contours.size(); i < size; i++) {
        std::shared_ptr<ObjectFullData> object =
            std::make_shared<ObjectFullData>(originalImage, image, contours[i]);
        if (object.get() == nullptr) {
          continue;
        }
        //
        // AREA
        //
        if (object->GetArea() < _min_area()) {
          continue;
        }
        if (_debug_contour()) {
          cv::drawContours(_output_image, contours, i, CV_RGB(255, 0, 0), 2);
        }

        //
        // RATIO
        //
        if (!_disable_ratio() && (fabs(object->GetRatio() - _targeted_ratio()) >
                                  fabs(_difference_from_target_ratio()))) {
          continue;
        }
        if (_debug_contour()) {
          cv::drawContours(_output_image, contours, i, CV_RGB(0, 0, 255), 2);
        }

        //
        // ANGLE
        //
        if (!_disable_angle() &&
            (fabs(object->GetRotatedRect().angle - _targeted_angle()) >
             fabs(_difference_from_target_angle()))) {
          continue;
        }

        //
        // RECTANGLE
        //
        if (_look_for_rectangle() && !IsRectangle(contours[i], 10)) {
          continue;
        }

        if (_debug_contour()) {
          cv::drawContours(_output_image, contours, i, CV_RGB(0, 255, 0), 2);
        }

        objVec.push_back(object);
      }

      std::sort(objVec.begin(), objVec.end(),
                [](std::shared_ptr<ObjectFullData> a,
                   std::shared_ptr<ObjectFullData> b)
                    -> bool { return a->GetArea() > b->GetArea(); });

      // Since we search only one buoy, get the biggest from sort function
      if (objVec.size() > 0) {
        Target target;
        std::shared_ptr<ObjectFullData> object = objVec[0];
        cv::Point center = object->GetCenter();
        setCameraOffset(&center, image.rows, image.cols);
        target.setTarget(center.x, center.y, object->GetLength(),
                         object->GetLength(), object->GetRotatedRect().angle);
        target.setSpecField_1(_spec_1());
        target.setSpecField_2(_spec_2());
        std::stringstream ss;
        ss << _id() << target.outputString();
        notify_str(ss.str().c_str());
        if (_debug_contour()) {
          cv::circle(_output_image, objVec[0]->GetCenter(), 3,
                     CV_RGB(0, 255, 0), 3);
        }
      }
      if (_debug_contour()) {
        _output_image.copyTo(image);
      }
    }
  }

 private:
  cv::Mat _output_image;
  // Params
  BooleanParameter _enable, _debug_contour, _look_for_rectangle, _disable_ratio,
      _disable_angle;
  StringParameter _id, _spec_1, _spec_2;
  DoubleParameter _min_area, _targeted_ratio, _difference_from_target_ratio,
      _targeted_angle, _difference_from_target_angle;

  FeatureFactory _feature_factory;
};

}  // namespace vision_filter

#endif  // VISION_FILTER_OBJECT_FINDER_H_
