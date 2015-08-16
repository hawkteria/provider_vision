/**
 * \file	object_ranker.h
 * \author  Jérémie St-Jules Prévôt <jeremie.st.jules.prevost@gmail.com>
 * \date	1/01/2014
 * \copyright	Copyright (c) 2015 SONIA AUV ETS. All rights reserved.
 * Use of this source code is governed by the MIT license that can be
 * found in the LICENSE file.
 */

#ifndef VISION_FILTER_OBJECT_RANKER_H_
#define VISION_FILTER_OBJECT_RANKER_H_

#include <lib_vision/algorithm/object_full_data.h>

// Class that simply rank the object
// with different value.
class ObjectRanker {
 public:
  // sort and set the value in each objects.
  static void RankByArea(ObjectFullData::FullObjectPtrVec objects);

  static void RankByLength(ObjectFullData::FullObjectPtrVec objects);

  // Function for std::sort function
  static bool AreaSortFunction(std::shared_ptr<ObjectFullData> a,
                               std::shared_ptr<ObjectFullData> b);

  static bool LengthSortFunction(std::shared_ptr<ObjectFullData> a,
                                 std::shared_ptr<ObjectFullData> b);
};

//-----------------------------------------------------------------------------
//
inline bool ObjectRanker::AreaSortFunction(std::shared_ptr<ObjectFullData> a,
                                           std::shared_ptr<ObjectFullData> b) {
  if (a.get() != nullptr && b.get() != nullptr) {
    return a->GetArea() > b->GetArea();
  }
  return false;
}

//-----------------------------------------------------------------------------
//
inline bool ObjectRanker::LengthSortFunction(
    std::shared_ptr<ObjectFullData> a, std::shared_ptr<ObjectFullData> b) {
  if (a.get() != nullptr && b.get() != nullptr) {
    return a->GetLength() > b->GetLength();
  }
  return false;
}

#endif
