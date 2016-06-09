/// \author	Pierluc Bédard <pierlucbed@gmail.com>
/// \author	Jérémie St-Jules Prévôt <jeremie.st.jules.prevost@gmail.com>
/// \author	Thibaut Mattio <thibaut.mattio@gmail.com>
/// \copyright Copyright (c) 2015 S.O.N.I.A. All rights reserved.
/// \section LICENSE
/// This file is part of S.O.N.I.A. software.
///
/// S.O.N.I.A. software is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// S.O.N.I.A. software is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with S.O.N.I.A. software. If not, see <http://www.gnu.org/licenses/>.

#include "provider_vision/server/vision_server.h"

namespace provider_vision {

//==============================================================================
// C / D T O R S   S E C T I O N

//------------------------------------------------------------------------------
//
VisionServer::VisionServer(const ros::NodeHandle &nh)
    : atlas::ServiceServerManager<VisionServer>(),
      nh_(nh),
      media_mgr_(nh),
      filterchain_mgr_() {
  auto base_node_name = std::string{kRosNodeName};

  RegisterService<execute_cmd>(base_node_name + "execute_cmd",
                               &VisionServer::CallbackExecutionCMD, *this);

  RegisterService<get_information_list>(base_node_name + "get_information_list",
                                        &VisionServer::CallbackInfoListCMD,
                                        *this);

  RegisterService<copy_filterchain>(base_node_name + "copy_filterchain",
                                    &VisionServer::CallbackCopyFc, *this);

  RegisterService<get_filterchain_filter_all_param>(
      base_node_name + "get_filterchain_filter_all_param",
      &VisionServer::CallbackGetFilterAllParam, *this);

  RegisterService<get_filterchain_filter_param>(
      base_node_name + "get_filterchain_filter_param",
      &VisionServer::CallbackGetFilterParam, *this);

  RegisterService<set_filterchain_filter_param>(
      base_node_name + "set_filterchain_filter_param",
      &VisionServer::CallbackSetFilterParam, *this);

  RegisterService<get_filterchain_filter>(
      base_node_name + "get_filterchain_filter",
      &VisionServer::CallbackGetFilter, *this);

  RegisterService<manage_filterchain_filter>(
      base_node_name + "manage_filterchain_filter",
      &VisionServer::CallbackManageFilter, *this);

  RegisterService<manage_filterchain>(base_node_name + "manage_filterchain",
                                      &VisionServer::CallbackManageFc, *this);

  RegisterService<save_filterchain>(base_node_name + "save_filterchain",
                                    &VisionServer::CallbackSaveFc, *this);

  RegisterService<set_filterchain_filter_order>(
      base_node_name + "set_filterchain_filter_order",
      &VisionServer::CallbackSetFcOrder, *this);

  RegisterService<get_filterchain_from_execution>(
      base_node_name + "get_filterchain_from_execution",
      &VisionServer::CallbackGetFcFromExec, *this);

  RegisterService<get_media_from_execution>(
      base_node_name + "get_media_from_execution",
      &VisionServer::CallbackGetMediaFromExec, *this);

  RegisterService<set_filterchain_filter_observer>(
      base_node_name + "set_filterchain_filter_observer",
      &VisionServer::CallbackSetObserver, *this);

  // This is the services that are part of the new version of the provider
  // vision.
  RegisterService<sonia_msgs::GetCameraFeature>(
      base_node_name + "get_camera_feature",
      &VisionServer::CallbackGetCameraFeature, *this);
  RegisterService<sonia_msgs::SetCameraFeature>(
      base_node_name + "set_camera_feature",
      &VisionServer::CallbackSetCameraFeature, *this);
}

//------------------------------------------------------------------------------
//
VisionServer::~VisionServer() {}

//==============================================================================
// M E T H O D   S E C T I O N

//------------------------------------------------------------------------------
//
bool VisionServer::CallbackExecutionCMD(execute_cmd::Request &rqst,
                                        execute_cmd::Response &rep) {
  if (rqst.cmd == rqst.START) {
    try {
      ROS_INFO("--- Starting Execution ---");
      ROS_INFO("Node: %s, Filterchain: %s, Media: %s", rqst.node_name.c_str(),
               rqst.filterchain_name.c_str(), rqst.media_name.c_str());

      MediaStreamer::Ptr media =
          media_mgr_.StartStreamingMedia(rqst.media_name);
      if (!media) {
        ROS_ERROR("Failure to start the streaming.");
        return false;
      }
      Filterchain::Ptr filterchain =
          filterchain_mgr_.InstanciateFilterchain(rqst.filterchain_name);

      rep.response = detection_task_mgr_.StartDetectionTask(media, filterchain,
                                                            rqst.node_name);
      ROS_INFO("Starting topic: %s", rep.response.c_str());
    } catch (const std::exception &e) {
      ROS_ERROR("Starting execution error: %s", e.what());
      rep.response = "";
      return false;
    }
  } else if (rqst.cmd == rqst.STOP) {
    try {
      ROS_INFO(" ");
      ROS_INFO("--- Stoping Execution ---");
      ROS_INFO("Node: %s, Filterchain: %s, Media: %s", rqst.node_name.c_str(),
               rqst.filterchain_name.c_str(), rqst.media_name.c_str());

      auto media_streamer =
          detection_task_mgr_.GetMediaStreamerFromDetectionTask(rqst.node_name);

      if (media_streamer == nullptr) {
        ROS_ERROR("Streamer does not exist, cannot close execution.");
        return false;
      }

      auto fc =
          detection_task_mgr_.GetFilterchainFromDetectionTask(rqst.node_name);
      if (fc == nullptr) {
        ROS_ERROR("Filterchain does not exist, cannot close execution.");
        return false;
      }

      std::string media_name = media_streamer->GetMediaName();

      detection_task_mgr_.StopDetectionTask(rqst.node_name);

      filterchain_mgr_.StopFilterchain(fc);

      media_mgr_.StopStreamingMedia(media_name);

    } catch (const std::exception &e) {
      ROS_ERROR("Closing execution error: %s", e.what());
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------
//
bool provider_vision::VisionServer::CallbackInfoListCMD(
    get_information_list::Request &rqst, get_information_list::Response &rep) {
  if (rqst.cmd == rqst.EXEC) {
    rep.list = BuildRosMessage(detection_task_mgr_.GetAllDetectionTasksName());
  } else if (rqst.cmd == rqst.FILTERCHAIN) {
    rep.list = BuildRosMessage(filterchain_mgr_.GetAllFilterchainName());
  } else if (rqst.cmd == rqst.FILTERS) {
    rep.list = provider_vision::FilterFactory::GetFilterList();
  } else if (rqst.cmd == rqst.MEDIA) {
    rep.list = BuildRosMessage(media_mgr_.GetAllMediasName());
  }

  return true;
}

//------------------------------------------------------------------------------
//
bool VisionServer::CallbackCopyFc(copy_filterchain::Request &rqst,
                                  copy_filterchain::Response &rep) {
  std::ifstream src(
      filterchain_mgr_.GetFilterchainPath(rqst.filterchain_to_copy).c_str(),
      std::ios::binary);

  std::ofstream dst(
      filterchain_mgr_.GetFilterchainPath(rqst.filterchain_new_name).c_str(),
      std::ios::binary);
  dst << src.rdbuf();
  rep.success = true;
  return rep.success;
}

//------------------------------------------------------------------------------
//
bool VisionServer::CallbackGetFilterParam(
    get_filterchain_filter_param::Request &rqst,
    get_filterchain_filter_param::Response &rep) {
  rep.list = "";

  std::string execution_name(rqst.exec_name),
      filterchain_name(rqst.filterchain);
  Filterchain::Ptr filterchain =
      detection_task_mgr_.GetFilterchainFromDetectionTask(execution_name);

  if (filterchain != nullptr) {
    rep.list = filterchain->GetFilterParameterValue(
        ExtractFilterIndexFromUIName(rqst.filter), rqst.parameter);
    return true;
  }
  ROS_INFO(
      "DetectionTask %s does not exist or does not use this "
      "filterchain: %s on get filter's param request.",
      execution_name.c_str(), filterchain_name.c_str());
  return false;
}

//------------------------------------------------------------------------------
//
bool VisionServer::CallbackGetFilterAllParam(
    get_filterchain_filter_all_param::Request &rqst,
    get_filterchain_filter_all_param::Response &rep) {
  rep.list = "";

  std::string execution_name(rqst.exec_name),
      filterchain_name(rqst.filterchain);
  Filterchain::Ptr filterchain =
      detection_task_mgr_.GetFilterchainFromDetectionTask(execution_name);

  if (filterchain != nullptr) {
    auto parameters = filterchain->GetFilterAllParameters(
        ExtractFilterIndexFromUIName(rqst.filter));
    std::vector<std::string> parameter_names;
    for (const auto &parameter : parameters) {
      parameter_names.push_back(parameter->ToString());
    }
    rep.list = BuildRosMessage(parameter_names);
    return true;
  }
  ROS_INFO(
      "DetectionTask %s does not exist or does not use this "
      "filterchain: %s on get filter's param request.",
      execution_name.c_str(), filterchain_name.c_str());
  return false;
}

//------------------------------------------------------------------------------
//
bool VisionServer::CallbackSetFilterParam(
    set_filterchain_filter_param::Request &rqst,
    set_filterchain_filter_param::Response &rep) {
  rep.success = rep.FAIL;

  std::string execution_name(rqst.exec_name),
      filterchain_name(rqst.filterchain);
  Filterchain::Ptr filterchain =
      detection_task_mgr_.GetFilterchainFromDetectionTask(execution_name);

  if (filterchain != nullptr) {
    filterchain->SetFilterParameterValue(
        ExtractFilterIndexFromUIName(rqst.filter), rqst.parameter, rqst.value);
    rep.success = rep.SUCCESS;
    return true;
  }
  ROS_INFO(
      "DetectionTask %s does not exist or does not use this "
      "filterchain: %s on get filter's param request.",
      execution_name.c_str(), filterchain_name.c_str());
  return false;
}

//------------------------------------------------------------------------------
//
bool VisionServer::CallbackGetFilter(get_filterchain_filter::Request &rqst,
                                     get_filterchain_filter::Response &rep) {
  rep.list = "";

  std::string execution_name(rqst.exec_name),
      filterchain_name(rqst.filterchain);
  Filterchain::Ptr filterchain =
      detection_task_mgr_.GetFilterchainFromDetectionTask(execution_name);

  if (filterchain != nullptr) {
    auto filters = filterchain->GetAllFilters();
    std::vector<std::string> filter_names;
    for (size_t i = 0; i < filters.size(); ++i) {
      filter_names.push_back(
          ConstructFilterUIName(filters.at(i)->GetName(), i));
    }
    rep.list = BuildRosMessage(filter_names);
    return true;
  }

  std::string log_txt = "DetectionTask " + execution_name +
                        " does not exist or does not use this filterchain: " +
                        filterchain_name + " on get filter's param request.";
  ROS_INFO(
      "DetectionTask %s does not exist or does not use this "
      "filterchain: %s on get filter's param request.",
      execution_name.c_str(), filterchain_name.c_str());
  return false;
}

//------------------------------------------------------------------------------
//
bool VisionServer::CallbackSetObserver(
    set_filterchain_filter_observer::Request &rqst,
    set_filterchain_filter_observer::Response &rep) {
  // For now ignoring filterchain name, but when we will have multiple,
  // we will have to check the name and find the good filterchain
  Filterchain::Ptr filterchain =
      detection_task_mgr_.GetFilterchainFromDetectionTask(rqst.execution);

  if (filterchain != nullptr) {
    rep.result = rep.SUCCESS;
    filterchain->SetObserver(ExtractFilterIndexFromUIName(rqst.filter));
    return true;
  }

  rep.result = rep.FAIL;
  ROS_INFO(
      "DetectionTask %s does not exist or does not use this "
      "filterchain: %s on get filters request",
      rqst.execution.c_str(), rqst.filterchain.c_str());
  return false;
}

//------------------------------------------------------------------------------
//
bool VisionServer::CallbackManageFilter(
    manage_filterchain_filter::Request &rqst,
    manage_filterchain_filter::Response &rep) {
  const auto &filterchain =
      detection_task_mgr_.GetFilterchainFromDetectionTask(rqst.exec_name);
  rep.success = 1;
  if (filterchain != nullptr) {
    if (rqst.cmd == rqst.ADD) {
      filterchain->AddFilter(rqst.filter);
    } else if (rqst.cmd == rqst.DELETE) {
      filterchain->RemoveFilter(ExtractFilterIndexFromUIName(rqst.filter));
    }
  } else {
    rep.success = 0;
  }

  return rep.success;
}

//------------------------------------------------------------------------------
//
bool VisionServer::CallbackManageFc(manage_filterchain::Request &rqst,
                                    manage_filterchain::Response &rep) {
  std::string filterchain_name(rqst.filterchain);
  bool response = true;
  if (rqst.cmd == rqst.ADD) {
    filterchain_mgr_.CreateFilterchain(filterchain_name);
    rep.success = rep.SUCCESS;
  } else if (rqst.cmd == rqst.DELETE) {
    filterchain_mgr_.EraseFilterchain(filterchain_name);
  }
  return response;
}

//------------------------------------------------------------------------------
//
bool VisionServer::CallbackSaveFc(save_filterchain::Request &rqst,
                                  save_filterchain::Response &rep) {
  std::string execution_name(rqst.exec_name);
  std::string filterchain_name(rqst.filterchain);
  if (rqst.cmd == rqst.SAVE) {
    detection_task_mgr_.GetFilterchainFromDetectionTask(rqst.exec_name)
        ->Serialize();
    rep.success = rep.SUCCESS;
  }
  return true;
}

//------------------------------------------------------------------------------
//
bool VisionServer::CallbackSetFcOrder(
    set_filterchain_filter_order::Request &rqst,
    set_filterchain_filter_order::Response &rep) {
  ROS_INFO("Call to set_filterchain_filter_order.");

  rep.success = rep.SUCCESS;
  auto filterchain =
      detection_task_mgr_.GetFilterchainFromDetectionTask(rqst.exec_name);
  if (rqst.cmd == rqst.UP) {
    filterchain->MoveFilterUp(rqst.filter_index);
  } else if (rqst.cmd == rqst.DOWN) {
    filterchain->MoveFilterDown(rqst.filter_index);
  } else {
    ROS_INFO("Filter index provided was invalid");
    rep.success = rep.FAIL;
  }

  return rep.success;
}

//------------------------------------------------------------------------------
//
bool VisionServer::CallbackGetFcFromExec(
    get_filterchain_from_execution::Request &rqst,
    get_filterchain_from_execution::Response &rep) {
  ROS_INFO("Call to get_filterchain_from_execution.");
  std::string execution_name(rqst.exec_name);
  Filterchain::Ptr filterchain =
      detection_task_mgr_.GetFilterchainFromDetectionTask(execution_name);

  if (filterchain != nullptr) {
    rep.list = filterchain->GetName();
    return true;
  }
  ROS_INFO(
      "DetectionTask %s does not exist or does not use this filterchain "
      "on get filters request.",
      execution_name.c_str());
  return false;
}

//------------------------------------------------------------------------------
//
bool VisionServer::CallbackGetMediaFromExec(
    get_media_from_execution::Request &rqst,
    get_media_from_execution::Response &rep) {
  ROS_INFO("Call to get_media_from_execution.");
  auto response = "media_" + rqst.exec_name;
  rep.list = response;
  return true;
}

//------------------------------------------------------------------------------
//
bool VisionServer::CallbackGetCameraFeature(
    sonia_msgs::GetCameraFeature::Request &rqst,
    sonia_msgs::GetCameraFeature::Response &rep) {
  boost::any feature_value;
  media_mgr_.GetCameraFeature(rqst.camera_name, rqst.camera_feature,
                              feature_value);
  std::stringstream ss;
  if (feature_value.type() == typeid(bool)) {
    rep.feature_type = sonia_msgs::GetCameraFeature::Request::FEATURE_BOOL;
    ss << boost::any_cast<bool>(feature_value);
  } else if (feature_value.type() == typeid(int)) {
    rep.feature_type = sonia_msgs::GetCameraFeature::Request::FEATURE_INT;
    ss << boost::any_cast<int>(feature_value);
  } else if (feature_value.type() == typeid(double)) {
    rep.feature_type = sonia_msgs::GetCameraFeature::Request::FEATURE_DOUBLE;
    ss << boost::any_cast<double>(feature_value);
  } else {
    ROS_ERROR("The feature type is not supported.");
    return false;
  }
  rep.feature_value = ss.str();
  return true;
}

//------------------------------------------------------------------------------
//
bool VisionServer::CallbackSetCameraFeature(
    sonia_msgs::SetCameraFeature::Request &rqst,
    sonia_msgs::SetCameraFeature::Response &rep) {
  if (rqst.feature_type == sonia_msgs::GetCameraFeatureRequest::FEATURE_BOOL) {
    boost::any value = boost::lexical_cast<bool>(rqst.feature_value);
    media_mgr_.SetCameraFeature(rqst.camera_name, rqst.camera_feature, value);
  } else if (rqst.feature_type ==
             sonia_msgs::GetCameraFeatureRequest::FEATURE_DOUBLE) {
    boost::any value = boost::lexical_cast<double>(rqst.feature_value);
    media_mgr_.SetCameraFeature(rqst.camera_name, rqst.camera_feature, value);
  } else if (rqst.feature_type ==
             sonia_msgs::GetCameraFeatureRequest::FEATURE_INT) {
    boost::any value = boost::lexical_cast<int>(rqst.feature_value);
    media_mgr_.SetCameraFeature(rqst.camera_name, rqst.camera_feature, value);
  } else {
    ROS_ERROR("The feature type is not supported.");
    return false;
  }
  return true;
}

}  // namespace provider_vision
