/**
 * \file	BuoySingle.h
 * \author  Jérémie St-Jules Prévôt <jeremie.st.jules.prevost@gmail.com>
 * \date	14/12/2014
 * \copyright	Copyright (c) 2015 SONIA AUV ETS. All rights reserved.
 * Use of this source code is governed by the MIT license that can be
 * found in the LICENSE file.
 */

#ifndef VISION_FILTER_BUOY_SINGLE_H_
#define VISION_FILTER_BUOY_SINGLE_H_

//==============================================================================
// I N C L U D E   F I L E S

#include <lib_vision/filter.h>
#include <lib_vision/algorithm/object_feature_factory.h>
#include <lib_vision/algorithm/object_full_data.h>
#include <lib_vision/algorithm/general_function.h>
#include <lib_vision/algorithm/target.h>
#include <lib_vision/algorithm/contour_list.h>
#include <math.h>

namespace vision_filter {

//==============================================================================
// C L A S S E S

class BuoySingle: public Filter {
 public:
  //============================================================================
  // C O N S T R U C T O R S   A N D   D E S T R U C T O R

  explicit BuoySingle(const GlobalParamHandler &globalParams)
      : Filter(globalParams),
        feat_factory_(1),
        _enable("Enable", false, parameters_),
        _debug_good_contour("Debug_contour", false, parameters_),
        _eliminate_same_x_targets("Eliminate_same_x", false, parameters_),
        _detect_red("Detect_red", false, parameters_),
        _color("Buoy", "red", parameters_),
        _min_area("Min_area", 200, 0, 10000, parameters_),
        _max_ratio("Max_ratio", 50, 0, 100, parameters_),
        _min_filled_percent("Min_percent", 50, 0, 100, parameters_),
        _max_x_difference_for_elimination("Min_x_difference", 50.0f, 0.0f,
                                          1000.0f, parameters_),
        _ratio_for_angle_check(
            "Ratio_for_angle_check", 50, 0, 100, parameters_,
            "When ratio smaller, will discard if vertical contour"),
        _admissible_horizontal_angle(
            "horizontal_max_angle", 50, 0, 90, parameters_,
            "When angle smaller, is consider vertical") {
    setName("BuoySingle");
  }

  virtual ~BuoySingle() { }

  //============================================================================
  // P U B L I C   M E T H O D S

  virtual void execute(cv::Mat &image) {
    if (_enable()) {
      cv::Mat in;
      // Copy the received image so we can see the objects.
      if (_debug_good_contour()) {
        // Case we receive a color or gray scale image.
        if (image.channels() == 1)
          cv::cvtColor(image, _outputImage, CV_GRAY2BGR);
        else
          image.copyTo(_outputImage);
      }

      if (image.channels() > 1) {
        cv::cvtColor(image, in, CV_BGR2GRAY);
      } else {
        image.copyTo(in);
      }
      // find contours in the image
      ContourList contours(image, ContourList::OUTER);

      // Filter contours
      ObjectFullData::FullObjectPtrVec objectVec;

      for (size_t j = 0; j < contours.size(); j++) {
        if (contours[j].size() <= 2) {
          continue;
        }

        ObjectFullData::Ptr object = std::make_shared<ObjectFullData>(
            global_params_.getOriginalImage(), image, contours[j]);

        // AREA
        if (_min_area() > object->GetArea()) {
          continue;
        }

        if (_debug_good_contour()) {
          contours[j].DrawContours(_outputImage, CV_RGB(255, 0, 0), 3);
        }
        feat_factory_.RatioFeature(object);
        // RATIO
        if (_max_ratio() > object->GetRatio()) {
          continue;
        }

        if (_debug_good_contour()) {
          contours[j].DrawContours(_outputImage, CV_RGB(255, 255, 0), 3);
        }

        // Angle of contour. If more rectangular, check to make sure it is
        // horizontal
        // not vertical, horizontal is caused by washed out buoy in the sun,
        // while
        // vertical is probably a pipe
        if (object->GetAngle() < _ratio_for_angle_check() &&
            fabs(object->GetAngle()) < _admissible_horizontal_angle()) {
          continue;
        }

        if (_debug_good_contour()) {
          contours[j].DrawContours(_outputImage, CV_RGB(255, 0, 255), 3);
        }

        feat_factory_.PercentFilledFeature(object);
        if (_min_filled_percent() > object->GetPercentFilled()) {
          continue;
        }

        objectVec.push_back(object);
      }

      // Gets the contour that is bigger
      std::sort(objectVec.begin(), objectVec.end(), AreaSorts);

      // Eliminate same target
      if (_eliminate_same_x_targets()) {
        EliminateSameXTarget(objectVec);
      }

      // Choose red if need be
      if (_detect_red() && objectVec.size() > 1) {
        ChooseMostRed(objectVec);
      }

      // Since we search only one buoy, get the biggest from sort function
      if (objectVec.size() != 0) {
        Target buoy;
        std::stringstream message;
        buoy.SetTarget(objectVec[0]);
        message << "buoy_" << _color() << ":" << buoy.OutputString();
        notify_str(message.str());

        if (_debug_good_contour()) {
          objectVec[0]->GetContourCopy().DrawContours(_outputImage,
                                                      CV_RGB(0, 255, 0),
                                                      5);
        }
      }

      if (_debug_good_contour()) {
        _outputImage.copyTo(image);
      }
    }
  }

 private:
  float getRadiusFromRectangle(ObjectFullData::Ptr &rectangle);

  bool IsSameX(ObjectFullData::Ptr a, ObjectFullData::Ptr b);

  // check if ref is higher than compared
  bool IsHigher(ObjectFullData::Ptr ref, ObjectFullData::Ptr compared);

  bool EliminateSameXTarget(ObjectFullData::FullObjectPtrVec &vec);

  void ChooseMostRed(ObjectFullData::FullObjectPtrVec &vec);

  cv::Mat _outputImage;

  // Params
  BooleanParameter _enable, _debug_good_contour, _eliminate_same_x_targets,
      _detect_red;
  StringParameter _color;
  DoubleParameter _min_area, _max_ratio, _min_filled_percent,
      _max_x_difference_for_elimination, _ratio_for_angle_check,
      _admissible_horizontal_angle;

  ObjectFeatureFactory feat_factory_;
};
//==============================================================================
//    INLINE FUNCTION
//------------------------------------------------------------------------------
//
inline float BuoySingle::getRadiusFromRectangle(ObjectFullData::Ptr &rectangle) {
  return (rectangle->GetRotatedRect().size.width / 2 +
      rectangle->GetRotatedRect().size.height / 2) / 2;
}

//------------------------------------------------------------------------------
//
inline bool BuoySingle::IsSameX(ObjectFullData::Ptr a, ObjectFullData::Ptr b) {
  double x_difference =
      static_cast<double>(a->GetCenter().x)
          - static_cast<double>(b->GetCenter().x);
  double abs_x_difference = fabs(x_difference);
  return abs_x_difference < _max_x_difference_for_elimination();
}

//------------------------------------------------------------------------------
//
// check if ref is higher than compared
inline bool BuoySingle::IsHigher(ObjectFullData::Ptr ref,
                                 ObjectFullData::Ptr compared) {
  return ref->GetCenter().y < compared->GetCenter().y;
}

//------------------------------------------------------------------------------
//
inline bool BuoySingle::EliminateSameXTarget(ObjectFullData::FullObjectPtrVec &vec) {
  std::vector<unsigned int> index_to_eliminate;
  // We should not have much target, so double loop is ok...
  for (unsigned int i = 0, size = vec.size(); i < size; i++) {
    for (unsigned int j = 0; j < size; j++) {
      if (i == j) {
        continue;
      }
      if (IsSameX(vec[i], vec[j])) {
        // If I is higher, eliminate it
        if (IsHigher(vec[i], vec[j])) {
          index_to_eliminate.push_back(i);
        }
      }
    }
  }
  // Erase from vector
  if (index_to_eliminate.size() > 0) {
    for (int i = index_to_eliminate.size() - 1; i >= 0; i--) {
      vec.erase(vec.begin() + i);
    }
  }
}

//------------------------------------------------------------------------------
//
inline void BuoySingle::ChooseMostRed(ObjectFullData::FullObjectPtrVec &vec) {
  ObjectFullData::Ptr a = vec[0], b = vec[1];
  cv::Mat original_img, hsv;
  cv::cvtColor(global_params_.getOriginalImage(), hsv, CV_BGR2HSV);
  cv::copyMakeBorder(hsv, hsv, 21, 21, 21, 21, cv::BORDER_CONSTANT);
  cv::Mat out;
  cv::cvtColor(a->GetBinaryImage(), out, CV_GRAY2BGR);
  cv::Point center_a = vec[0]->GetCenter(),
      center_b = vec[1]->GetCenter();

  cv::Mat roiA(hsv, cv::Rect(cv::Point(center_a.x - 5, (center_a.y)),
                             cv::Size(20, 20)));
  cv::Mat roiB(hsv, cv::Rect(cv::Point(center_b.x - 5, (center_b.y)),
                             cv::Size(20, 20)));

  cv::Scalar meanA = cv::mean(roiA);
  cv::Scalar meanB = cv::mean(roiB);

  // If a is more green, then it is yellow so we swap
  if (meanB[0] > meanA[0]) {
    std::swap(vec[0], vec[1]);
  }
}

}  // namespace vision_filter

#endif  // VISION_FILTER_BUOY_SINGLE_H_
