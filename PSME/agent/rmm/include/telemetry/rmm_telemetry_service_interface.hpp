/*!
 * @brief Declaration of RmmTelemetryServiceInterface class
 *
 * @copyright Copyright (c) 2017-2018 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Files}
 * @file rmm_telemetry_service_interface.hpp
 */

#pragma once

#include "discovery/helpers/types.hpp"



namespace agent {
namespace rmm {

class RmmTelemetryServiceInterface {
public:

    virtual ~RmmTelemetryServiceInterface();


    virtual const std::vector<discovery::helpers::ResourceSensorPtr>& get_resource_sensors() const = 0;

};

}
}
