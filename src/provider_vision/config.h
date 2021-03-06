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

#ifndef PROVIDER_VISION_UTILS_CONFIG_H_
#define PROVIDER_VISION_UTILS_CONFIG_H_

#include <lib_atlas/config.h>
#include <string>

namespace provider_vision {

const std::string kRosNodeName = "/provider_vision/";

const std::string kProjectPath =
    atlas::kWorkspaceRoot + "/src/provider_vision/";

const std::string kConfigPath = kProjectPath + "/config/";

const std::string kFilterchainPath = kConfigPath + "/filterchain/";

const std::string kCameraConfigPath = kConfigPath + "/camera/";

const std::string kFilterchainExt = ".yaml";

};  // namespace provider_vision

#endif  // PROVIDER_VISION_UTILS_CONFIG_H_
