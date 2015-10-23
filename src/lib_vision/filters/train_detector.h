/**
 * \file	TrainDetector.h
 * \author  Jérémie St-Jules Prévôt <jeremie.st.jules.prevost@gmail.com>
 * \date	14/12/2014
 * \copyright	Copyright (c) 2015 SONIA AUV ETS. All rights reserved.
 * Use of this source code is governed by the MIT license that can be
 * found in the LICENSE file.
 */

#ifndef VISION_FILTER_TRAIN_DETECTOR_H_
#define VISION_FILTER_TRAIN_DETECTOR_H_

//==============================================================================
// I N C L U D E   F I L E S

#include <lib_vision/filter.h>
#include <lib_vision/algorithm/features.h>
#include <lib_vision/algorithm/general_function.h>
#include <lib_vision/algorithm/target.h>
#include <lib_vision/algorithm/object_full_data.h>
#include <lib_vision/algorithm/performance_evaluator.h>
#include <lib_vision/algorithm/ai_trainer.h>

namespace vision_filter {

//==============================================================================
// C L A S S E S

class TrainDetector : public Filter {
 public:
  //==========================================================================
  // N E S T E D   C L A S S   D E F I N I T I O N

  class ObjectPair {
   public:
    //==========================================================================
    // C O N S T R U C T O R S   A N D   D E S T R U C T O R

    explicit ObjectPair(ObjectFullData::Ptr object1, ObjectFullData::Ptr object2,
                        ObjectFeatureFactory &featFactory)
        : _object1(object1),
          _object2(object2),
          _convexity_mean(0) {
      assert(object1.get() != nullptr);
      assert(object2.get() != nullptr);

      featFactory.ConvexityFeature(object1);
      featFactory.ConvexityFeature(object2);
      _convexity_mean = _object1->GetConvexity();
      _convexity_mean += _object2->GetConvexity();
      _convexity_mean /= 2.0f;
    }

    ~ObjectPair() {}

    static inline bool ConvexitySort(const ObjectPair &a, const ObjectPair &b) {
      return a._convexity_mean > b._convexity_mean;
    }

    ObjectFullData::Ptr _object1, _object2;
    float _convexity_mean;
  };

  //============================================================================
  // C O N S T R U C T O R S   A N D   D E S T R U C T O R

  explicit TrainDetector(const GlobalParamHandler &globalParams)
      : Filter(globalParams),
        _enable("Enable", false, parameters_),
        _debug_contour("Debug_contour", false, parameters_),
        _pair_distance_maximum("Pair_distance_maximum", 200, 0, 10000,
                               parameters_),
        _min_area("Min_area", 200, 0, 10000, parameters_),

        _feat_factory(3) {
    setName("TrainDetector");
  }

  virtual ~TrainDetector() {}

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

      PerformanceEvaluator timer;
      timer.UpdateStartTime();

      contourList_t contours;
      retrieveAllContours(image, contours);
      ObjectFullData::FullObjectPtrVec objVec;
      for (int i = 0, size = contours.size(); i < size; i++) {
        ObjectFullData::Ptr object =
            std::make_shared<ObjectFullData>(originalImage, image, contours[i]);
        if (object.get() == nullptr) {
          continue;
        }

        if (object->GetArea() < _min_area()) {
          continue;
        }

        if (_debug_contour()) {
          cv::drawContours(_output_image, contours, i, CV_RGB(255, 0, 0), 2);
        }

        objVec.push_back(object);
      }

      std::vector<ObjectPair> pairs;
      // Iterate through object to find pair between objects
      for (int i = 0, size = objVec.size(); i < size; i++) {
        ObjectFullData::Ptr currentObj = objVec[i];
        if (currentObj.get() == nullptr) continue;
        for (int j = 0; j < size; j++) {
          if (j == i || objVec[j].get() == nullptr) continue;

          // If they are near enough, add it to the pair.
          float distance = eucledianPointDistance(objVec[j]->GetCenter(),
                                                  currentObj->GetCenter());
          if (distance <= _pair_distance_maximum()) {
            pairs.push_back(ObjectPair(currentObj, objVec[j], _feat_factory));
          }
        }
      }

      // Send the target
      if (pairs.size() != 0) {
        // Here we assume that the form founded will be almost perfect, so
        // convexity is 1.
        // We get the best pair and hope for the best
        std::sort(pairs.begin(), pairs.end(), ObjectPair::ConvexitySort);

        Target target;
        contour_t obj1(pairs[0]._object1->GetContourCopy().Get()),
            obj2(pairs[0]._object2->GetContourCopy().Get());

        for (int i = 0, size = obj2.size(); i < size; i++) {
          obj1.push_back(obj2[i]);
        }
        ObjectFullData::Ptr object =
            std::make_shared<ObjectFullData>(originalImage, image, obj1);
        cv::Point center = object->GetCenter();
        setCameraOffset(&center, image.rows, image.cols);
        target.SetTarget(center.x, center.y, object->GetLength(),
                         object->GetLength(),
                         abs(object->GetRotatedRect().angle - 90));
        std::stringstream ss;
        ss << "train:" << target.OutputString();
        notify_str(ss.str().c_str());
        if (_debug_contour()) {
          contourList_t tmp;
          tmp.push_back(obj1);
          cv::drawContours(_output_image, tmp, 0, CV_RGB(0, 255, 0), 2);
          cv::circle(_output_image, object->GetCenter(), 3, CV_RGB(0, 255, 0),
                     3);
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
  BooleanParameter _enable, _debug_contour;
  IntegerParameter _pair_distance_maximum, _min_area;

  ObjectFeatureFactory _feat_factory;
};

}  // namespace vision_filter

#endif  // VISION_FILTER_TRAIN_DETECTOR_H_
