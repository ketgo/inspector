/**
 * Copyright 2022 Ketan Goyal
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

#include <inspector/details/tracer.hpp>

// TODO: Better serialization and deserialization of the catapult schema.

namespace inspector {

/**
 * @brief Tags to determine the type of trace event.
 *
 * These tags will be present in the published trace event. A reader can use
 * these values to determine the type of event. The tag values are chosen to be
 * consitent with catapult traceview:
 *
 * https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU
 *
 */
constexpr auto kSyncBeginTag = "B";
constexpr auto kSyncEndTag = "E";
constexpr auto kAsyncBeginTag = "b";
constexpr auto kAsyncInstanceTag = "n";
constexpr auto kAsyncEndTag = "e";
constexpr auto kFlowBeginTag = "s";
constexpr auto kFlowInstanceTag = "t";
constexpr auto kFLowEndTag = "f";

// ------------------------------------
// Synchronus Scope Trace Events
// =============================
//
//  Synchronus scope trace events are published to trace scopes within a method
//  call.

/**
 * @brief
 *
 * @tparam Args
 * @param name
 * @param args
 */
template <class... Args>
void SyncBegin(const std::string& name, const Args&... args) {
  details::TraceEvent event(kSyncBeginTag, name, args...);
  details::TraceWriter().Write(event.String());
}

/**
 * @brief
 *
 * @param name
 */
inline void SyncEnd(const std::string& name) {
  details::TraceEvent event(kSyncEndTag, name);
  details::TraceWriter().Write(event.String());
}

// ------------------------------------
// Asynchronus Scope Trace Events
// ==============================

/**
 * @brief
 *
 * @tparam Args
 * @param name
 * @param args
 */
template <class... Args>
void AsyncBegin(const std::string& name, const Args&... args) {
  details::TraceEvent event(kAsyncBeginTag, name, args...);
  details::TraceWriter().Write(event.String());
}

/**
 * @brief
 *
 * @tparam Args
 * @param name
 * @param args
 */
template <class... Args>
void AsyncInstance(const std::string& name, const Args&... args) {
  details::TraceEvent event(kAsyncInstanceTag, name, args...);
  details::TraceWriter().Write(event.String());
}

/**
 * @brief
 *
 * @tparam Args
 * @param name
 * @param args
 */
template <class... Args>
void AsyncEnd(const std::string& name, const Args&... args) {
  details::TraceEvent event(kAsyncEndTag, name, args...);
  details::TraceWriter().Write(event.String());
}

// ------------------------------------
// Flow Scope Trace Events
// ==============================

/**
 * @brief
 *
 * @tparam Args
 * @param name
 * @param args
 */
template <class... Args>
void FlowBegin(const std::string& name, const Args&... args) {
  details::TraceEvent event(kFlowBeginTag, name, args...);
  details::TraceWriter().Write(event.String());
}

/**
 * @brief
 *
 * @tparam Args
 * @param name
 * @param args
 */
template <class... Args>
void FlowInstance(const std::string& name, const Args&... args) {
  details::TraceEvent event(kFlowInstanceTag, name, args...);
  details::TraceWriter().Write(event.String());
}

/**
 * @brief
 *
 * @tparam Args
 * @param name
 * @param args
 */
template <class... Args>
void FlowEnd(const std::string& name, const Args&... args) {
  details::TraceEvent event(kFLowEndTag, name, args...);
  details::TraceWriter().Write(event.String());
}

// ------------------------------------

}  // namespace inspector
