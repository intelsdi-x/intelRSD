/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
 * */

#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/compute_commands.hpp"

using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;

namespace {
    void process_collections(System& system) {
        if (get_manager<agent_framework::model::Processor>().get_entry_count(system.get_uuid()) > 0) {
            system.add_collection(attribute::Collection(
                enums::CollectionName::Processors,
                enums::CollectionType::Processors, ""));
        }
        if (ComputeComponents::get_instance()->
                get_memory_manager().get_entry_count(system.get_uuid()) > 0) {
            system.add_collection(attribute::Collection(
                enums::CollectionName::Memory,
                enums::CollectionType::Memory,""));
        }
        if (ComputeComponents::get_instance()->
                get_storage_controller_manager().get_entry_count(system.get_uuid()) > 0) {
            system.add_collection(attribute::Collection(
                enums::CollectionName::StorageControllers,
                enums::CollectionType::StorageControllers,""));
        }
        if (get_manager<NetworkInterface>().get_entry_count(system.get_uuid()) > 0) {
            system.add_collection(attribute::Collection(
                enums::CollectionName::NetworkInterfaces,
                enums::CollectionType::NetworkInterfaces,""));
        }
    }
}

REGISTER_COMMAND(GetSystemInfo,
    [] (const GetSystemInfo::Request& req, GetSystemInfo::Response& rsp) {
        log_debug("agent", "Getting system info.");
        rsp = CommonComponents::get_instance()->
            get_system_manager().get_entry(req.get_uuid());
        ::process_collections(rsp);
    }
);
