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

#include "tools/viewers/perfetto/common.hpp"

/**
 * Macro to pack PID and TID into a single 64bit data type.
 *
 */
#define PACK_PID_TID(pid, tid) (static_cast<int64_t>(pid) << 32) | tid

/**
 * Perfetto trusted packet sequence ID.
 *
 */
#define PACKET_SEQ_ID 125483184

namespace inspector {
namespace tools {

// ----------------------------------------------------------
// PerfettoTrackManager
// ----------------------------------------------------------

PerfettoTrackManager::PerfettoTrackManager(
    perfetto::protos::Trace& trace_packets)
    : trace_packets_(std::addressof(trace_packets)) {}

uint64_t PerfettoTrackManager::getOrCreateProcessTrack(const int32_t pid) {
  uint64_t process_track_uuid = pid;
  if (!track_uuids_.count(process_track_uuid)) {
    auto* trace_packet_ptr = trace_packets_->add_packet();
    auto* descriptor_ptr = trace_packet_ptr->mutable_track_descriptor();
    descriptor_ptr->set_uuid(process_track_uuid);
    auto* process_descriptor_ptr = descriptor_ptr->mutable_process();
    process_descriptor_ptr->set_pid(pid);
    track_uuids_.insert(process_track_uuid);
  }

  return process_track_uuid;
}

uint64_t PerfettoTrackManager::getOrCreateThreadTrack(const int32_t pid,
                                                      const int32_t tid) {
  uint64_t process_track_uuid = getOrCreateProcessTrack(pid);
  if (pid == tid) {
    return process_track_uuid;
  }

  uint64_t thread_track_uuid = PACK_PID_TID(pid, tid);
  if (!track_uuids_.count(thread_track_uuid)) {
    auto* trace_packet_ptr = trace_packets_->add_packet();
    auto* descriptor_ptr = trace_packet_ptr->mutable_track_descriptor();
    descriptor_ptr->set_uuid(thread_track_uuid);
    descriptor_ptr->set_parent_uuid(process_track_uuid);
    auto* thread_descriptor_ptr = descriptor_ptr->mutable_thread();
    thread_descriptor_ptr->set_pid(pid);
    thread_descriptor_ptr->set_tid(tid);
    track_uuids_.insert(thread_track_uuid);
  }

  return thread_track_uuid;
}

uint64_t PerfettoTrackManager::getOrCreateAsyncTrack(
    const std::string& name, const uint64_t parent_track_uuid,
    const bool previous) {
  const auto count = previous ? track_counts_[name] : ++track_counts_[name];
  const uint64_t track_uuid = hash_(name + "~" + std::to_string(count));
  if (track_uuids_.count(track_uuid)) {
    return track_uuid;
  }

  auto* trace_packet_ptr = trace_packets_->add_packet();
  auto* descriptor_ptr = trace_packet_ptr->mutable_track_descriptor();
  descriptor_ptr->set_uuid(track_uuid);
  descriptor_ptr->set_parent_uuid(parent_track_uuid);
  descriptor_ptr->set_name(name);
  track_uuids_.insert(track_uuid);

  return track_uuid;
}

uint64_t PerfettoTrackManager::getOrCreateCounterTrack(
    const std::string& name, const uint64_t parent_track_uuid) {
  const uint64_t track_uuid =
      hash_(name + "~" + std::to_string(parent_track_uuid));
  if (track_uuids_.count(track_uuid)) {
    return track_uuid;
  }

  auto* trace_packet_ptr = trace_packets_->add_packet();
  auto* descriptor_ptr = trace_packet_ptr->mutable_track_descriptor();
  descriptor_ptr->set_uuid(track_uuid);
  descriptor_ptr->set_parent_uuid(parent_track_uuid);
  descriptor_ptr->set_name(name);
  auto* counter_descriptor_ptr = descriptor_ptr->mutable_counter();
  counter_descriptor_ptr->set_type(
      perfetto::protos::CounterDescriptor::COUNTER_UNSPECIFIED);
  counter_descriptor_ptr->set_unit(
      perfetto::protos::CounterDescriptor::UNIT_UNSPECIFIED);

  return track_uuid;
}

// ----------------------------------------------------------
// PerfettoEventManager
// ----------------------------------------------------------

PerfettoEventManager::PerfettoEventManager(
    perfetto::protos::Trace& trace_packets)
    : trace_packets_(std::addressof(trace_packets)) {}

perfetto::protos::TrackEvent* PerfettoEventManager::createSliceBegin(
    const uint64_t track_uuid, const int64_t timestamp_ns,
    const std::string& name, const std::unordered_set<uint64_t> flow_ids,
    const std::unordered_set<uint64_t> terminating_flow_ids) {
  auto* trace_packet_ptr = trace_packets_->add_packet();
  trace_packet_ptr->set_trusted_packet_sequence_id(PACKET_SEQ_ID);
  trace_packet_ptr->set_timestamp(timestamp_ns);
  auto* track_event_ptr = trace_packet_ptr->mutable_track_event();
  track_event_ptr->set_type(perfetto::protos::TrackEvent::TYPE_SLICE_BEGIN);
  track_event_ptr->set_track_uuid(track_uuid);
  track_event_ptr->set_name(name);
  for (const auto& flow_id : flow_ids) {
    track_event_ptr->add_flow_ids(flow_id);
  }
  for (const auto terminating_flow_id : terminating_flow_ids) {
    track_event_ptr->add_terminating_flow_ids(terminating_flow_id);
  }

  return track_event_ptr;
}

perfetto::protos::TrackEvent* PerfettoEventManager::createSliceEnd(
    const uint64_t track_uuid, const int64_t timestamp_ns) {
  auto* trace_packet_ptr = trace_packets_->add_packet();
  trace_packet_ptr->set_trusted_packet_sequence_id(PACKET_SEQ_ID);
  trace_packet_ptr->set_timestamp(timestamp_ns);
  auto* track_event_ptr = trace_packet_ptr->mutable_track_event();
  track_event_ptr->set_type(perfetto::protos::TrackEvent::TYPE_SLICE_END);
  track_event_ptr->set_track_uuid(track_uuid);

  return track_event_ptr;
}

std::pair<perfetto::protos::TrackEvent*, perfetto::protos::TrackEvent*>
PerfettoEventManager::createSlice(
    const uint64_t track_uuid, const int64_t timestamp_ns,
    const std::string& name, const std::unordered_set<uint64_t> flow_ids,
    const std::unordered_set<uint64_t> terminating_flow_ids) {
  return {createSliceBegin(track_uuid, timestamp_ns, name, flow_ids,
                           terminating_flow_ids),
          createSliceEnd(track_uuid, timestamp_ns)};
}

perfetto::protos::TrackEvent* PerfettoEventManager::createInstanceEvent(
    const uint64_t track_uuid, const int64_t timestamp_ns,
    const std::string& name, const std::unordered_set<uint64_t> flow_ids,
    const std::unordered_set<uint64_t> terminating_flow_ids) {
  auto* trace_packet_ptr = trace_packets_->add_packet();
  trace_packet_ptr->set_trusted_packet_sequence_id(PACKET_SEQ_ID);
  trace_packet_ptr->set_timestamp(timestamp_ns);
  auto* track_event_ptr = trace_packet_ptr->mutable_track_event();
  track_event_ptr->set_type(perfetto::protos::TrackEvent::TYPE_INSTANT);
  track_event_ptr->set_track_uuid(track_uuid);
  track_event_ptr->set_name(name);
  for (const auto& flow_id : flow_ids) {
    track_event_ptr->add_flow_ids(flow_id);
  }
  for (const auto terminating_flow_id : terminating_flow_ids) {
    track_event_ptr->add_terminating_flow_ids(terminating_flow_id);
  }

  return track_event_ptr;
}

perfetto::protos::TrackEvent* PerfettoEventManager::createCounterEvent(
    const uint64_t track_uuid, const int64_t timestamp_ns) {
  auto* trace_packet_ptr = trace_packets_->add_packet();
  trace_packet_ptr->set_trusted_packet_sequence_id(PACKET_SEQ_ID);
  trace_packet_ptr->set_timestamp(timestamp_ns);
  auto* track_event_ptr = trace_packet_ptr->mutable_track_event();
  track_event_ptr->set_type(perfetto::protos::TrackEvent::TYPE_COUNTER);
  track_event_ptr->set_track_uuid(track_uuid);

  return track_event_ptr;
}

}  // namespace tools
}  // namespace inspector