/**
 * \file	object_frame_memory_test.cc
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

#include <gtest/gtest.h>
#include "provider_vision/algorithm/object_frame_memory.h"
#include "provider_vision/algorithm/contour_list.h"

using namespace provider_vision;

void CreateImage(cv::Mat &original, cv::Mat &binary)
{
  std::vector< std::vector<cv::Point> > contours;

  // square contour 100 of area, 1 of ratio
  // 100 filled, convexity 0 , circularity ?
  // last in area rank and length rank
  std::vector<cv::Point> tmpContour;
  tmpContour.push_back(cv::Point(5, 5));
  tmpContour.push_back(cv::Point(5, 15));
  tmpContour.push_back(cv::Point(15, 15));
  tmpContour.push_back(cv::Point(15, 5));
  contours.push_back(tmpContour);
  tmpContour.clear();

  // M like contour.
  tmpContour.push_back(cv::Point(100, 100));
  tmpContour.push_back(cv::Point(200, 100));
  tmpContour.push_back(cv::Point(150, 300));
  tmpContour.push_back(cv::Point(200, 500));
  tmpContour.push_back(cv::Point(100, 500));
  contours.push_back(tmpContour);
  tmpContour.clear();

  tmpContour.push_back(cv::Point(400, 700));
  tmpContour.push_back(cv::Point(750, 700));
  tmpContour.push_back(cv::Point(750, 800));
  tmpContour.push_back(cv::Point(400, 800));
  contours.push_back(tmpContour);
  tmpContour.clear();

  cv::drawContours(original, contours, -1, CV_RGB(255, 0, 255), -1);
  cv::cvtColor(original, binary, CV_BGR2GRAY);
}

TEST(FrameMemory, AllTest) {

  cv::Mat binaryImage(1000, 1000, CV_8UC1, cv::Scalar::all(0));
  cv::Mat originalImage(1000, 1000, CV_8UC3, cv::Scalar::all(0));

  CreateImage(originalImage, binaryImage);

  ObjectFullData::FullObjectPtrVec tmp;

  ContourList contours(binaryImage, ContourList::OUTER);

  tmp.push_back(
      ObjectFullData::Ptr(std::make_shared<ObjectFullData>(
          originalImage, binaryImage, contours[0])));
  tmp.push_back(
      ObjectFullData::Ptr(std::make_shared<ObjectFullData>(
          originalImage, binaryImage, contours[1])));
  tmp.push_back(
      ObjectFullData::Ptr(std::make_shared<ObjectFullData>(
          originalImage, binaryImage, contours[2])));

  // Testing retrieval by center and ratio

  // Split the M shape to prevent retrieval of the object via center.
  // We create a frame where we should miss the M form.
  cv::Mat tmpBinary, tmpOriginal= originalImage.clone();
  cv::rectangle(tmpOriginal, cv::Point(100, 290), cv::Point(200, 310), CV_RGB(0, 0, 0), -1);
  cv::cvtColor(tmpOriginal, tmpBinary, CV_BGR2GRAY);
  ContourList contoursTemp(tmpBinary, ContourList::OUTER);

  ASSERT_TRUE(contoursTemp.GetSize() == 4);

  // Push faulty object
  ObjectFullData::FullObjectPtrVec tmp2;
  for (int i = 0; i < contoursTemp.GetSize(); i++) {
    tmp2.push_back(
        ObjectFullData::Ptr(std::make_shared<ObjectFullData>(
            originalImage, binaryImage, contoursTemp[i])));
  }

  ObjectFrameMemory frameMemory(3);
  // Fill history buffer.
  frameMemory.AddFrameObjects(tmp);
  // Add the frame where the M form is missing
  frameMemory.AddFrameObjects(tmp2);
  // Add another frame where all the object are ok.
  // Update with real contour to find it in the past objects
  frameMemory.AddFrameObjects(tmp);

  // Here we altered only the biggest object in time, so the one with the
  // current highest area rating should have only 2 ancestor,
  // since one of them was split
  int objectSum = 0;
  for (int i = 0, size = tmp.size(); i < size; i++) {
    ObjectFullData::Ptr tmpObj = tmp[i];
    ObjectFullData::FullObjectPtrVec vec = frameMemory.GetPastObjectsViaCenter(
        tmpObj->GetCenter(), tmpObj->GetRatio());

    objectSum += vec.size();
  }
  // We have 3 contours in tmp. In the past, one of them ( the M )
  // has been split. Therefor, frame memory should not be able to find it
  // since the center is not there AND the ratio is not good. So we have 3 form,
  // in 3 frame
  // but one of them is missing so 3*3 - 1 = 8 retrieve form in the past.
  ASSERT_TRUE(objectSum == 8);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}