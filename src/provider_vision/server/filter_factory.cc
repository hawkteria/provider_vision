/**
 * \file	filter_factory.cc
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

#include "provider_vision/server/filter_factory.h"

namespace provider_vision {

Filter *FilterFactory::createInstance(const std::string &name,
                                      const GlobalParamHandler &globalParams) {
  if (name == "Blurr") {
    return new Blurr(globalParams);
  } else if (name == "Dilate") {
    return new Dilate(globalParams);
  } else if (name == "Erode") {
    return new Erode(globalParams);
  } else if (name == "MissionTestFakeString") {
    return new MissionTestFakeString(globalParams);
  } else if (name == "TestFilter") {
    return new TestFilter(globalParams);
  } else if (name == "BuoySingle") {
    return new BuoySingle(globalParams);
  } else if (name == "Morphology") {
    return new Morphology(globalParams);
  } else if (name == "OriginalImage") {
    return new OriginalImage(globalParams);
  } else if (name == "Scharr") {
    return new Scharr(globalParams);
  } else if (name == "ScharrAdding") {
    return new ScharrAdding(globalParams);
  } else if (name == "StatsThreshold") {
    return new StatsThreshold(globalParams);
  } else if (name == "SubtractAllPlanes") {
    return new SubtractAllPlanes(globalParams);
  } else if (name == "Threshold") {
    return new Threshold(globalParams);
  } else if (name == "BuoySingle") {
    return new BuoySingle(globalParams);
  } else if (name == "Rotate") {
    return new Rotate(globalParams);
  } else if (name == "FenceDetector") {
    return new FenceDetector(globalParams);
  } else if (name == "ImageAccumulator") {
    return new ImageAccumulator(globalParams);
  } else if (name == "ObjectFeatureCalculator") {
    return new ObjectFeatureCalculator(globalParams);
  } else if (name == "TrainDetector") {
    return new TrainDetector(globalParams);
  } else if (name == "ObjectFinder") {
    return new ObjectFinder(globalParams);
  } else if (name == "PipeDetector") {
    return new PipeDetector(globalParams);
  } else if (name == "TrackDetector") {
    return new TrackDetector(globalParams);
  } else if (name == "Sobel") {
    return new Sobel(globalParams);
  } else if (name == "DeloreanDetector") {
    return new DeloreanDetector(globalParams);
  } else if (name == "SubmarineFrameMasker") {
    return new SubmarineFrameMasker(globalParams);
  } else if (name == "InRange") {
    return new InRange(globalParams);
  } else if (name == "ConvexHull") {
    return new ConvexHull(globalParams);
  } else if (name == "TorpedoesDetector") {
    return new TorpedoesDetector(globalParams);
  } else if (name == "Laplacian") {
    return new Laplacian(globalParams);
  } else if (name == "Canny") {
    return new Canny(globalParams);
  } else if (name == "HoughLine") {
    return new HoughLine(globalParams);
  } else if (name == "AdaptiveThreshold") {
    return new AdaptiveThreshold(globalParams);
  } else if (name == "HandleDetector") {
    return new HandleDetector(globalParams);
  } else if (name == "WhiteNoiseTakedown") {
    return new WhiteNoiseTakedown(globalParams);
  } else if (name == "BilateralFilter") {
    return new BilateralFilter(globalParams);
  } else {
    return nullptr;
  }
}

std::string FilterFactory::GetFilterList() {
  return "Blurr;Dilate;Erode;MissionTestFakeString;TestFilter;"
         "BuoySingle;Morphology;OriginalImage;Scharr;ScharrAdding;"
         "StatsThreshold;SubtractAllPlanes;Threshold;BuoySingle;Rotate;"
         "FenceDetector;ImageAccumulator;ObjectFeatureCalculator;"
         "TrainDetector;ObjectFinder;PipeDetector;TrackDetector;Sobel;"
         "DeloreanDetector;SubmarineFrameMasker;InRange;ConvexHull;"
         "TorpedoesDetector;Laplacian;Canny;HoughLine;AdaptiveThreshold;"
         "HandleDetector;WhiteNoiseTakedown;BilateralFilter;";
}

}  // namespace provider_vision