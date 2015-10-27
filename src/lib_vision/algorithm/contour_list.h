/*
 * contour_finder.h
 *
 *  Created on: Aug 24, 2015
 *      Author: jeremie
 */

#ifndef LIB_VISION_SRC_LIB_VISION_ALGORITHM_CONTOUR_LIST_H_
#define LIB_VISION_SRC_LIB_VISION_ALGORITHM_CONTOUR_LIST_H_

#include <opencv2/opencv.hpp>

#include "lib_vision/algorithm/contour.h"
// Contour finding

class ContourList {
 public:

  typedef std::vector<Contour::ContourVec> ContourListVector;

  // Contour navigation constant
  static const unsigned int NEXT = 0;
  static const unsigned int PREVIOUS = 1;
  static const unsigned int FIRST_CHILD = 2;
  static const unsigned int PARENT = 3;

  enum METHOD {
    ALL,             // All the contour in the image
    INNER,           // Every contour with a parent
    INNER_MOST,      // Every contour with a parent AND no child
    OUTER,           // Every contour without a parent
    OUTER_NO_CHILD,  // Ever contour without a parent AND no child
    HIERARCHY        // All the contours, fills the hierarchy member
  };

  ContourList(const cv::Mat &image, const METHOD method);

  void DrawContours(cv::Mat &img, const cv::Scalar &color, int thickness = 2);

  // Vector overload
  size_t size();
  Contour operator[](size_t index);

  std::vector<std::vector<cv::Point>> GetAsPoint();
  std::vector<Contour> GetAsContour();
  std::vector<cv::Vec4i> GetHierachy();

 private:
  bool HasChild(const cv::Vec4i &hierarchy_def);
  bool HasParent(const cv::Vec4i &hierarchy_def);

  // Retrieval method
  void retrieveAllContours(const cv::Mat &image);

  // Retrieve contour with hierachy. Sets the vector _hierachy of this object.
  void retrieveHiearchyContours(const cv::Mat &image);

  // All innermost contour i.e. no child
  void retrieveInnerContours(const cv::Mat &image);

  // All inner contour i.e. has a parent
  void retrieveInnerMostContours(const cv::Mat &image);

  // All outer contour i.e. has no parent
  void retrieveOuterContours(const cv::Mat &image);

  // All contour that has no child AND no parent
  void retrieveOutNoChildContours(const cv::Mat &image);

 public:
  ContourListVector contour_list_point_;
  std::vector<Contour> contour_vec_;
  // Contains the hierachy when METHOD used is HIERACHY
  std::vector<cv::Vec4i> _hierarchy;
};

//=============================================================================
//
//-----------------------------------------------------------------------------
//
inline size_t ContourList::size() { return contour_vec_.size(); }

//-----------------------------------------------------------------------------
//
inline std::vector<std::vector<cv::Point>> ContourList::GetAsPoint()
{
  return contour_list_point_;
}

//-----------------------------------------------------------------------------
//
inline std::vector<Contour> ContourList::GetAsContour()
{
  return contour_vec_;
}

//-----------------------------------------------------------------------------
//
inline std::vector<cv::Vec4i> ContourList::GetHierachy()
{
  return _hierarchy;
}

//-----------------------------------------------------------------------------
//
inline Contour ContourList::operator[](size_t index) {
  return contour_vec_[index];
}

//-----------------------------------------------------------------------------
//
inline bool ContourList::HasChild(const cv::Vec4i &hierarchy_def)
{
   return hierarchy_def[FIRST_CHILD] != -1;
}

//-----------------------------------------------------------------------------
//
inline bool ContourList::HasParent(const cv::Vec4i &hierarchy_def)
{
  return hierarchy_def[PARENT] != -1;
}

//-----------------------------------------------------------------------------
//
inline void ContourList::DrawContours(cv::Mat &img, const cv::Scalar &color,
                                      int thickness) {
  cv::drawContours(img, contour_list_point_, -1, color, thickness);
}

#endif /* LIB_VISION_SRC_LIB_VISION_ALGORITHM_CONTOUR_FINDER_H_ */
