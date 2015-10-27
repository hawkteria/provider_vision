/**
 * \file	feature_factory.cpp
 * \author  Jérémie St-Jules Prévôt <jeremie.st.jules.prevost@gmail.com>
 * \date	1/01/2014
 * \copyright	Copyright (c) 2015 SONIA AUV ETS. All rights reserved.
 * Use of this source code is governed by the MIT license that can be
 * found in the LICENSE file.
 */

#include "lib_vision/algorithm/object_feature_factory.h"

//=============================================================================
// 	CONSTRUCTOR CODE SECTION
//-----------------------------------------------------------------------------
//
ObjectFeatureFactory::ObjectFeatureFactory(unsigned int memorySize)
    :_frame_memory(memorySize) {
  using namespace std::placeholders;
  feature_fct_map.emplace(ObjectFeatureData::Feature::RATIO,
                          std::bind(&ObjectFeatureFactory::RatioFeature, this, _1));
  feature_fct_map.emplace(ObjectFeatureData::Feature::CONVEXITY,
                          std::bind(&ObjectFeatureFactory::ConvexityFeature, this, _1));
  feature_fct_map.emplace(ObjectFeatureData::Feature::PERCENT_FILLED,
                          std::bind(&ObjectFeatureFactory::PercentFilledFeature, this, _1));
  feature_fct_map.emplace(ObjectFeatureData::Feature::CIRCULARITY,
                          std::bind(&ObjectFeatureFactory::CircularityFeature, this, _1));
  feature_fct_map.emplace(ObjectFeatureData::Feature::PRESENCE_CONSISTENCY,
                          std::bind(&ObjectFeatureFactory::PresenceConsistencyFeature, this, _1));
  feature_fct_map.emplace(ObjectFeatureData::Feature::HUE_MEAN,
                          std::bind(&ObjectFeatureFactory::HueMeanFeature, this, _1));
  feature_fct_map.emplace(ObjectFeatureData::Feature::SAT_MEAN,
                          std::bind(&ObjectFeatureFactory::SatMeanFeature, this, _1));
  feature_fct_map.emplace(ObjectFeatureData::Feature::INTENSITY_MEAN,
                          std::bind(&ObjectFeatureFactory::IntensityMeanFeature, this, _1));
  feature_fct_map.emplace(ObjectFeatureData::Feature::RED_MEAN,
                          std::bind(&ObjectFeatureFactory::RedMeanFeature, this, _1));
  feature_fct_map.emplace(ObjectFeatureData::Feature::GREEN_MEAN,
                          std::bind(&ObjectFeatureFactory::GreenMeanFeature, this, _1));
  feature_fct_map.emplace(ObjectFeatureData::Feature::BLUE_MEAN,
                          std::bind(&ObjectFeatureFactory::BlueMeanFeature, this, _1));
  feature_fct_map.emplace(ObjectFeatureData::Feature::GRAY_MEAN,
                          std::bind(&ObjectFeatureFactory::GrayMeanFeature, this, _1));
}

//=============================================================================
// 	METHOD CODE SECTION
//-----------------------------------------------------------------------------
//
void ObjectFeatureFactory::PercentFilledFeature(ObjectFullData::Ptr object) {

  if ((object.get() != nullptr) && (object->GetPercentFilled() == -1.0f)) {
    float percentFilled = 0.0f;

    cv::Mat original_image = object->GetBinaryImage();
    cv::cvtColor(original_image, original_image, CV_GRAY2BGR);

    cv::Rect rect = object->GetUprightRect();
    cv::Mat roi_original = original_image(rect);
    cv::Mat roi_to_draw = roi_original.clone();
    object->GetContourCopy().DrawContours(roi_to_draw, CV_RGB(255, 255 ,255), CV_FILLED);
    cv::imshow("test", roi_to_draw);
    cv::waitKey(-1);
    std::vector<Contour::ContourVec> ctrs;
    ctrs.push_back(object->GetContourCopy().Get());
    cv::drawContours(roi_to_draw, ctrs, -1, CV_RGB(255, 0 ,255), CV_FILLED);
    cv::imshow("test", roi_to_draw);
    cv::waitKey(-1);

    cv::cvtColor(roi_to_draw, roi_to_draw, CV_BGR2GRAY);
    int nb = cv::countNonZero(roi_to_draw);
    if( nb == 0)
      return;
    cv::subtract(roi_to_draw, roi_original, roi_to_draw);
    int result = cv::countNonZero(roi_to_draw)/nb;
    std::cout << "CLACULATION " << result << std::endl;

    cv::Size imageSize = object->GetImageSize();
    cv::Mat drawImage(imageSize, CV_8UC3, cv::Scalar::all(0));
    contourList_t contours;
    cv::Point2f pts[4];
    RotRect rrect = object->GetRotatedRect();
    rrect.points(pts);
    contour_t contour(4);
    for (int i = 0; i < 4; i++) {
      contour[i].x = int(pts[i].x);
      contour[i].y = int(pts[i].y);
    }
    contours.push_back(contour);
    contours.push_back(object->GetContourCopy().Get());

    // Draw the biggest one (contour by corners)
    // Then draw the contour in black over the biggest one.
    cv::drawContours(drawImage, contours, 0, CV_RGB(255, 255, 255), -1);
    cv::drawContours(drawImage, contours, 1, CV_RGB(0, 0, 0), -1);

    cv::cvtColor(drawImage, drawImage, CV_BGR2GRAY);
    float notCovered = cv::countNonZero(drawImage);
    // safety, should not happen
    float rrectArea = rrect.size.area();
    if (rrectArea != 0) percentFilled = 1.0f - (notCovered / rrectArea);
    object->SetPercentFilled(percentFilled);
  }
}

//-----------------------------------------------------------------------------
//
float ObjectFeatureFactory::CalculatePlaneMean(ObjectFullData::Ptr object,
                                               int plane) {
  float mean = 0.0f;
  if (object.get() != nullptr) {
    cv::Mat binaryImage(object->GetImageSize(), CV_8UC3, cv::Scalar::all(0));
    contourList_t contours;
    contours.push_back(object->GetContourCopy().Get());
    cv::drawContours(binaryImage, contours, -1, CV_RGB(255, 255, 255), -1);
    cv::cvtColor(binaryImage, binaryImage, CV_BGR2GRAY);
    cv::Mat colorbinaryImage;

    cv::bitwise_and(object->GetPlanes(plane),
                    cv::Scalar::all(255), colorbinaryImage, binaryImage);
    long unsigned int accumulator = 0, nbPixel = 0;

    int rows = colorbinaryImage.rows, cols = colorbinaryImage.cols;
    for (int y = 0; y < rows; y++) {
      uchar *ptr = colorbinaryImage.ptr<uchar>(y);
      for (int x = 0; x < cols; x++) {
        if (ptr[x] != 0) {
          accumulator += ptr[x];
          nbPixel++;
        }
      }
    }
    if (nbPixel != 0) {
      mean = (float(accumulator) / float(nbPixel * 255));
    }
  }
  return mean;
}