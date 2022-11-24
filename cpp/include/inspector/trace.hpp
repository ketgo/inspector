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

#include <inspector/details/trace_writer.hpp>

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
constexpr auto kFlowEndTag = 'f';
constexpr auto kCounterTag = 'C';

/**
 * @brief The class `Kwarg` represents a keyword argument.
 *
 * @tparam T The type of argument.
 */
template <class T>
class Kwarg {
 public:
  /**
   * @brief Construct a new keyword argument.
   *
   * @param name Name of the argument.
   * @param value Constant reference to the argument value.
   */
  Kwarg(const char* name, const T& value) : name_(name), value_(value) {}

  /**
   * @brief Write keyword argument to output stream.
   *
   * @param out Reference to the output stream.
   * @param arg Constant reference to the keyword argument.
   */
  friend std::ostream& operator<<(std::ostream& out, const Kwarg& arg) {
    out << arg.name_ << "=" << arg.value_;
    return out;
  }

 private:
  const char* name_;
  const T& value_;
};

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
// ====================================
//
//  Synchronous scope trace events are published to trace scopes within a method
//  call.

/**
 * @brief Create a synchronous begin trace event.
 *
 * The method publishes a synchronous begin trace event attaching the provided
 * additional arguments to the event.
 *
 * @tparam Args Additional argument types.
 * @param name Constant reference to the scope name.
 * @param args Constant reference to additional arguments.
 */
template <class... Args>
void SyncBegin(const std::string& name, const Args&... args) {
  details::WriteTraceEvent(kSyncBeginTag, name, args...);
}

/**
 * @brief Create a synchronous end trace event.
 *
 * The method publishes a synchronous end trace event.
 *
 * @param name Constant reference to the scope name.
 */
inline void SyncEnd(const std::string& name) {
  details::WriteTraceEvent(kSyncEndTag, name);
}

/**
 * @brief Utility class to publish a begin and end synchronous trace event
 * during the CTOR and DTOR respectively. An object of the class can be used to
 * trace a scope.
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
// ====================================

/**
 * @brief Create an asynchronous begin trace event.
 *
 * The method publishes an asynchronous begin trace event attaching the provided
 * additional arguments to the event.
 *
 * @tparam Args Additional argument types.
 * @param name Constant reference to the scope name.
 * @param args Constant reference to additional arguments.
 */
template <class... Args>
void AsyncBegin(const std::string& name, const Args&... args) {
  details::WriteTraceEvent(kAsyncBeginTag, name, args...);
}

/**
 * @brief Create an asynchronous instance trace event.
 *
 * The method publishes an asynchronous instance trace event attaching the
 * provided additional arguments to the event.
 *
 * @tparam Args Additional argument types.
 * @param name Constant reference to the scope name.
 * @param args Constant reference to additional arguments.
 */
template <class... Args>
void AsyncInstance(const std::string& name, const Args&... args) {
  details::WriteTraceEvent(kAsyncInstanceTag, name, args...);
}

/**
 * @brief Create an asynchronous end trace event.
 *
 * The method publishes an asynchronous end trace event attaching the provided
 * additional arguments to the event.
 *
 * @tparam Args Additional argument types.
 * @param name Constant reference to the scope name.
 * @param args Constant reference to additional arguments.
 */
template <class... Args>
void AsyncEnd(const std::string& name, const Args&... args) {
  details::WriteTraceEvent(kAsyncEndTag, name, args...);
}

// ------------------------------------
// Flow Scope Trace Events
// ====================================

/**
 * @brief Create a flow begin trace event.
 *
 * The method publishes a flow begin trace event attaching the provided
 * additional arguments to the event.
 *
 * @tparam Args Additional argument types.
 * @param name Constant reference to the scope name.
 * @param args Constant reference to additional arguments.
 */
template <class... Args>
void FlowBegin(const std::string& name, const Args&... args) {
  details::WriteTraceEvent(kFlowBeginTag, name, args...);
}

/**
 * @brief Create a flow instance trace event.
 *
 * The method publishes a flow instance trace event attaching the provided
 * additional arguments to the event.
 *
 * @tparam Args Additional argument types.
 * @param name Constant reference to the scope name.
 * @param args Constant reference to additional arguments.
 */
template <class... Args>
void FlowInstance(const std::string& name, const Args&... args) {
  details::WriteTraceEvent(kFlowInstanceTag, name, args...);
}

/**
 * @brief Create a flow end trace event.
 *
 * The method publishes a flow end trace event attaching the provided
 * additional arguments to the event.
 *
 * @tparam Args Additional argument types.
 * @param name Constant reference to the scope name.
 * @param args Constant reference to additional arguments.
 */
template <class... Args>
void FlowEnd(const std::string& name, const Args&... args) {
  details::WriteTraceEvent(kFlowEndTag, name, args...);
}

// ------------------------------------
// Counter Event
// ====================================

// TODO: Should be variadic keyword arguments having integer values
template <class... Args>
void Counters(const std::string& name, const Args&... args) {
  details::WriteTraceEvent(kCounterTag, name, args...);
}

// ------------------------------------

}  // namespace inspector

// ------------------------------------
// Convenience Macros
// ------------------------------------

// Utility macros to get unique scope name. These are meant for internal use.
#define __UNIQUE_MAKER__(name, counter) __##name##counter__
#define __MAKE_UNIQUE__(name) __UNIQUE_MAKER__(name, __COUNTER__)

/**
 * @brief Synchronous trace events.
 *
 */
#define TRACE() inspector::SyncScope __MAKE_UNIQUE__(sync_scope)(__func__)
#define TRACE_SCOPE(name) inspector::SyncScope __MAKE_UNIQUE__(sync_scope)(name)

/**
 * @brief Asynchronous trace events.
 *
 */
#define TRACE_ASYNC_BEGIN(name) inspector::AsyncBegin(name)
#define TRACE_ASYNC_INSTANCE(name) inspector::AsyncInstance(name)
#define TRACE_ASYNC_END(name) inspector::AsyncEnd(name)

/**
 * @brief Flow trace events
 *
 */
#define TRACE_FLOW_BEGIN(name) inspector::FlowBegin(name)
#define TRACE_FLOW_INSTANCE(name) inspector::FlowInstance(name)
#define TRACE_FLOW_END(name) inspector::FlowEnd(name)

/**
 * @brief Counter trace events
 *
 */
#define TRACE_COUNTER(name, ...) inspector::Counters(name, __VA_ARGS__)

// ------------------------------------
