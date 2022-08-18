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

namespace inspector {

/**
 * @brief Tags to determine the type of trace event.
 *
 * These tags will be present in the published trace event. A reader can use
 * these values to determine the type of event. The tag values are chosen to be
 * consistent with catapult traceview:
 *
 * https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU
 *
 */
constexpr auto kSyncBeginTag = 'B';
constexpr auto kSyncEndTag = 'E';
constexpr auto kAsyncBeginTag = 'b';
constexpr auto kAsyncInstanceTag = 'n';
constexpr auto kAsyncEndTag = 'e';
constexpr auto kFlowBeginTag = 's';
constexpr auto kFlowInstanceTag = 't';
constexpr auto kFLowEndTag = 'f';

/**
 * @brief Keyword argument type.
 *
 * @tparam T The type of argument.
 */
template <class T>
using Kwarg = details::Kwarg<T>;

/**
 * @brief Utility method to create a keyword argument.
 *
 * @tparam T The type of argument.
 * @param name Name of the argument.
 * @param value Constant reference to the argument value.
 * @returns Keyword argument object.
 */
template <class T>
Kwarg<T> MakeKwarg(const char* name, const T& value) {
  return {name, value};
}

// ------------------------------------
// Synchronous Scope Trace Events
// =============================
//
//  Synchronous scope trace events are published to trace scopes within a method
//  call.

/**
 * @brief Create a synchronous begin trace event.
 *
 * The method publishes a synchronous begin trace event with provided additional
 * arguments to be attached with the event.
 *
 * @tparam Args Additional argument types.
 * @param name Constant reference to the name of the event.
 * @param args Constant reference to additional arguments.
 */
template <class... Args>
void SyncBegin(const std::string& name, const Args&... args) {
  details::TraceEvent event(kSyncBeginTag, name);
  event.SetArgs(args...);
}

/**
 * @brief
 *
 * @param name
 */
inline void SyncEnd(const std::string& name) {
  details::TraceEvent event(kSyncEndTag, name);
}

/**
 * @brief Utility class to write begin and end synchronous trace events during
 * CTOR and DTOR respectively.
 *
 */
class SyncScope {
 public:
  template <class... Args>
  SyncScope(const std::string& name, const Args&... args);
  ~SyncScope();

 private:
  std::string name_;
};

template <class... Args>
inline SyncScope::SyncScope(const std::string& name, const Args&... args)
    : name_(name) {
  SyncBegin(name, args...);
}

inline SyncScope::~SyncScope() { SyncEnd(name_); }

// ------------------------------------
// Asynchronous Scope Trace Events
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
  details::TraceEvent event(kAsyncBeginTag, name);
  event.SetArgs(args...);
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
  details::TraceEvent event(kAsyncInstanceTag, name);
  event.SetArgs(args...);
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
  details::TraceEvent event(kAsyncEndTag, name);
  event.SetArgs(args...);
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
  details::TraceEvent event(kFlowBeginTag, name);
  event.SetArgs(args...);
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
  details::TraceEvent event(kFlowInstanceTag, name);
  event.SetArgs(args...);
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
  details::TraceEvent event(kFLowEndTag, name);
  event.SetArgs(args...);
}

// ------------------------------------

}  // namespace inspector
