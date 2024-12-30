/**
 * Copyright 2023 Ketan Goyal
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "protos/perfetto/trace/trace.pb.h"

namespace inspector {
namespace tools {

/**
 * @brief Class to manage creation of perfetto tracks.
 *
 */
class PerfettoTrackManager {
 public:
  /**
   * @brief Construct a PerfettoTrackManager object.
   *
   * @param trace_packets Reference to trace packets proto.
   */
  explicit PerfettoTrackManager(perfetto::protos::Trace& trace_packets);

  /**
   * @brief Get or create a process track. The method returns the track UUID.
   *
   * @param pid Process identifier.
   * @returns Track UUID.
   */
  uint64_t getOrCreateProcessTrack(const int32_t pid);

  /**
   * @brief Get or create a thread track. The method returns the track UUID.
   *
   * @param pid Process identifier.
   * @param tid Thread identifier.
   * @returns Track UUID.
   */
  uint64_t getOrCreateThreadTrack(const int32_t pid, const int32_t tid);

  /**
   * @brief Get or create an async track. The method returns the track UUID.
   *
   * @param name Name of the track to create.
   * @param parent_tracke_uuid Parent track UUID.
   * @param previoud Flag to re-use previously created track UUID for the same
   * track name.
   * @returns Track UUID.
   */
  uint64_t getOrCreateAsyncTrack(const std::string& name,
                                 const uint64_t parent_track_uuid = 0,
                                 const bool previous = false);

  /**
   * @brief Get or create a counter track. The method returns the track UUID.
   *
   * @param name Name of the track.
   * @param parent_tracke_uuid Parent track UUID.
   * @returns Track UUID.
   */
  uint64_t getOrCreateCounterTrack(const std::string& name,
                                   const uint64_t parent_track_uuid = 0);

 private:
  perfetto::protos::Trace* trace_packets_;
  std::unordered_set<uint64_t> track_uuids_;
  std::unordered_map<std::string, uint64_t> track_counts_;
  std::hash<std::string> hash_;
};

/**
 * @brief CLass to create perfetto events.
 *
 */
class PerfettoEventManager {
 public:
  /**
   * @brief Construct a PerfettoEventManager object.
   *
   * @param trace_packets Reference to trace packets proto.
   */
  explicit PerfettoEventManager(perfetto::protos::Trace& trace_packets);

  /**
   * @brief Create a slice begin track event.
   *
   * @param track_uuid UUID of track on which to create event.
   * @param timestamp_ns Timestamp of the event.
   * @param name Name of the event.
   * @param flow_ids Set of flow IDs assocaited with the event.
   * @param terminating_flow_ids Set of terminating flow IDs which can be
   * reused.
   * @returns Mutable pointer to the created track event.
   */
  perfetto::protos::TrackEvent* createSliceBegin(
      const uint64_t track_uuid, const int64_t timestamp_ns,
      const std::string& name, const std::unordered_set<uint64_t> flow_ids = {},
      const std::unordered_set<uint64_t> terminating_flow_ids = {});

  /**
   * @brief Create a slice begin track event.
   *
   * @param track_uuid UUID of track on which to create event.
   * @param timestamp_ns Timestamp of the event.
   * @returns Mutable pointer to the created track event.
   */
  perfetto::protos::TrackEvent* createSliceEnd(const uint64_t track_uuid,
                                               const int64_t timestamp_ns);

  /**
   * @brief Create a complete slice.
   *
   * @param track_uuid UUID of track on which to create event.
   * @param timestamp_ns Timestamp of the event.
   * @param name Name of the event.
   * @param flow_ids Set of flow IDs assocaited with the event.
   * @param terminating_flow_ids Set of terminating flow IDs which can be
   * reused.
   * @returns Mutable pointers to the created begin and end track events.
   */
  std::pair<perfetto::protos::TrackEvent*, perfetto::protos::TrackEvent*>
  createSlice(const uint64_t track_uuid, const int64_t timestamp_ns,
              const std::string& name,
              const std::unordered_set<uint64_t> flow_ids = {},
              const std::unordered_set<uint64_t> terminating_flow_ids = {});

  /**
   * @brief Create an instance event.
   *
   * @param track_uuid UUID of track on which to create event.
   * @param timestamp_ns Timestamp of the event.
   * @param name Name of the event.
   * @param flow_ids Set of flow IDs assocaited with the event.
   * @param terminating_flow_ids Set of terminating flow IDs which can be
   * reused.
   * @returns Mutable pointer to the created track event.
   */
  perfetto::protos::TrackEvent* createInstanceEvent(
      const uint64_t track_uuid, const int64_t timestamp_ns,
      const std::string& name, const std::unordered_set<uint64_t> flow_ids = {},
      const std::unordered_set<uint64_t> terminating_flow_ids = {});

  /**
   * @brief Create a counter event.
   *
   * @param track_uuid UUID of track on which to create event.
   * @param timestamp_ns Timestamp of the event.
   * @returns Mutable pointer to the created track event.
   */
  perfetto::protos::TrackEvent* createCounterEvent(const uint64_t track_uuid,
                                                   const int64_t timestamp_ns);

 private:
  perfetto::protos::Trace* trace_packets_;
};

}  // namespace tools
}  // namespace inspector
