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

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "tools/viewers/perfetto/generator.hpp"

#include <fstream>
#include <inspector/trace.hpp>
#include <inspector/trace_event.hpp>

#include "tools/common/storage/storage.hpp"
#include "tools/viewers/perfetto/common.hpp"

// ---
#include <glog/logging.h>

namespace inspector {
namespace tools {
namespace {

void createDebugAnnotation(perfetto::protos::DebugAnnotation& debug_annotation,
                           const DebugArg& arg) {
  switch (arg.type()) {
    case DebugArg::Type::TYPE_STRING:
    case DebugArg::Type::TYPE_CHAR: {
      debug_annotation.set_string_value(arg.value<std::string>());
      break;
    }

    case DebugArg::Type::TYPE_INT16: {
      debug_annotation.set_int_value(arg.value<int16_t>());
      break;
    }

    case DebugArg::Type::TYPE_INT32: {
      debug_annotation.set_int_value(arg.value<int32_t>());
      break;
    }

    case DebugArg::Type::TYPE_INT64: {
      debug_annotation.set_int_value(arg.value<int64_t>());
      break;
    }

    case DebugArg::Type::TYPE_UINT8: {
      debug_annotation.set_uint_value(arg.value<uint8_t>());
      break;
    }

    case DebugArg::Type::TYPE_UINT16: {
      debug_annotation.set_uint_value(arg.value<uint16_t>());
      break;
    }

    case DebugArg::Type::TYPE_UINT32: {
      debug_annotation.set_uint_value(arg.value<uint32_t>());
      break;
    }

    case DebugArg::Type::TYPE_UINT64: {
      debug_annotation.set_uint_value(arg.value<uint64_t>());
      break;
    }

    case DebugArg::Type::TYPE_FLOAT: {
      debug_annotation.set_double_value(arg.value<float>());
      break;
    }

    case DebugArg::Type::TYPE_DOUBLE: {
      debug_annotation.set_double_value(arg.value<double>());
      break;
    }

    case DebugArg::Type::TYPE_KWARG: {
      const auto kwarg = arg.value<KeywordArg>();
      auto* debug_annotation_ptr = debug_annotation.add_dict_entries();
      debug_annotation_ptr->set_name(kwarg.name());
      createDebugAnnotation(*debug_annotation_ptr, kwarg);
      break;
    }

    default:
      break;
  }
}

/**
 * @brief Utility method to create debug annotations.
 *
 */
void createDebugAnnotations(perfetto::protos::TrackEvent& track_event,
                            const TraceEvent& trace_event) {
  const auto debug_args = trace_event.debugArgs();
  auto* debug_annotation_ptr = track_event.add_debug_annotations();
  debug_annotation_ptr->set_name("args");
  for (const auto& arg : debug_args) {
    createDebugAnnotation(*(debug_annotation_ptr->add_array_values()), arg);
  }
}

}  // namespace

#undef PACK_PID_TID
#undef PACKET_SEQ_ID

void generatePerfetto(const std::string& input_dir,
                      const std::string output_file) {
  LOG_IF(FATAL, input_dir.empty()) << "No input path provided.";
  LOG_IF(FATAL, output_file.empty()) << "No output file provided.";

  perfetto::protos::Trace trace_packets;

  LOG(INFO) << "Processing events in '" << input_dir << "'...";
  PerfettoTrackManager track_manager(trace_packets);
  PerfettoEventManager event_manager(trace_packets);
  storage::Reader reader(input_dir);
  for (auto& record : reader) {
    std::vector<uint8_t> buffer(record.size);
    std::memcpy(buffer.data(), record.src, record.size);
    TraceEvent event(std::move(buffer));
    switch (static_cast<EventType>(event.type())) {
      case EventType::kSyncBeginTag: {
        const auto track_uuid =
            track_manager.getOrCreateThreadTrack(event.pid(), event.tid());
        auto* track_event_ptr = event_manager.createSliceBegin(
            track_uuid, event.timestampNs(), event.name());
        createDebugAnnotations(*track_event_ptr, event);
        break;
      }

      case EventType::kSyncEndTag: {
        const auto track_uuid =
            track_manager.getOrCreateThreadTrack(event.pid(), event.tid());
        event_manager.createSliceEnd(track_uuid, event.timestampNs());
        break;
      }

      case EventType::kCounterTag: {
        const auto debug_args = event.debugArgs();
        if (debug_args.size() == 0) {
          continue;
        }
        const auto parent_track_uuid =
            track_manager.getOrCreateThreadTrack(event.pid(), event.tid());
        size_t count = 0;
        for (const auto& arg : debug_args) {
          const std::string suffix =
              debug_args.size() > 1 ? " [" + std::to_string(count) + "]" : "";
          const auto track_name =
              std::string("COUNTER: ") + std::string(event.name()) + suffix;
          const auto track_uuid = track_manager.getOrCreateCounterTrack(
              track_name, parent_track_uuid);
          auto* track_event_ptr =
              event_manager.createCounterEvent(track_uuid, event.timestampNs());
          switch (arg.type()) {
            case DebugArg::Type::TYPE_STRING:
            case DebugArg::Type::TYPE_CHAR: {
              LOG(INFO) << "String counter value not supported.";
              break;
            }

            case DebugArg::Type::TYPE_INT16: {
              track_event_ptr->set_counter_value(arg.value<int16_t>());
              break;
            }

            case DebugArg::Type::TYPE_INT32: {
              track_event_ptr->set_counter_value(arg.value<int32_t>());
              break;
            }

            case DebugArg::Type::TYPE_INT64: {
              track_event_ptr->set_counter_value(arg.value<int64_t>());
              break;
            }

            case DebugArg::Type::TYPE_UINT8: {
              track_event_ptr->set_counter_value(arg.value<uint8_t>());
              break;
            }

            case DebugArg::Type::TYPE_UINT16: {
              track_event_ptr->set_counter_value(arg.value<uint16_t>());
              break;
            }

            case DebugArg::Type::TYPE_UINT32: {
              track_event_ptr->set_counter_value(arg.value<uint32_t>());
              break;
            }

            case DebugArg::Type::TYPE_UINT64: {
              track_event_ptr->set_counter_value(arg.value<uint64_t>());
              break;
            }

            case DebugArg::Type::TYPE_FLOAT: {
              track_event_ptr->set_double_counter_value(arg.value<float>());
              break;
            }

            case DebugArg::Type::TYPE_DOUBLE: {
              track_event_ptr->set_double_counter_value(arg.value<double>());
              break;
            }

            default:
              break;
          }
        }

        break;
      }

      default: {
        LOG(ERROR) << "Unsupported event type observed";
        break;
      }
    }
  }

  if (trace_packets.packet().size() == 0) {
    LOG(ERROR) << "No events found.";
  } else {
    LOG(INFO) << "Saving file: " << output_file;
    std::ofstream file(output_file, std::ios_base::out | std::ios_base::binary);
    trace_packets.SerializeToOstream(&file);
  }
}

}  // namespace tools
}  // namespace inspector