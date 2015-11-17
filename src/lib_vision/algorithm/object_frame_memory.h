/**
 * \file	object_frame_memory.h
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

#ifndef LIB_VISION_ALGORITHM_OBJECT_FRAME_MEMORY_H_
#define LIB_VISION_ALGORITHM_OBJECT_FRAME_MEMORY_H_

#include <lib_vision/algorithm/object_full_data.h>
#include <lib_vision/algorithm/general_function.h>
#include <vector>

class ObjectFrameMemory {
 public:
  ObjectFrameMemory(unsigned int memorySize);

  ~ObjectFrameMemory() {}

  // When getting object in the past, we compare the center
  // and the ratio to make sure it stills fit the same object.
  // If the ratio difference is smaller than RATIO_MAX_DIFFERENCE
  // we consider it as good object.
  static const float DISTANCE_MAX_DIFFERENCE;
  static const float RATIO_MAX_DIFFERENCE;

  void AddFrameObjects(ObjectFullData::FullObjectPtrVec &objectVector);

  unsigned int GetMemorySize();

  // Use the center and the ratio to find an object in the past object list.
  ObjectFullData::FullObjectPtrVec GetPastObjectsViaCenter(
      const cv::Point &center, const float objectRatio);

 private:
  std::vector<ObjectFullData::FullObjectPtrVec> _previous_frames;
  unsigned int _memory_size;
};

inline unsigned int ObjectFrameMemory::GetMemorySize() { return _memory_size; }

#endif // LIB_VISION_ALGORITHM_OBJECT_FRAME_MEMORY_H_
