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
#include <string>
#include <type_traits>

namespace inspector {

/**
 * @brief Enumerated set of event types. The types are taken from the catapult
 * types in
 * https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU
 *
 */
enum class EventType : event_type_t {
  kSyncBeginTag = 0,
  kSyncEndTag,
  kAsyncBeginTag,
  kAsyncInstanceTag,
  kAsyncEndTag,
  kFlowBeginTag,
  kFlowInstanceTag,
  kFlowEndTag,
  kCounterTag,
};

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
 * @param name Scope name in c-string format.
 * @param args Constant reference to additional arguments.
 */
template <class... Args>
void syncBegin(const char* name, const Args&... args) {
  details::writeTraceEvent(static_cast<event_type_t>(EventType::kSyncBeginTag),
                           name, args...);
}

/**
 * @brief Create a synchronous end trace event.
 *
 * The method publishes a synchronous end trace event.
 *
 * @param name Scope name in c-string format.
 */
void syncEnd(const char* name);

/**
 * @brief Utility class to publish a begin and end synchronous trace event
 * during the CTOR and DTOR respectively. An object of the class can be used to
 * trace a scope. Note that the class is designed primarily for string literals.
 * When using std::string, the c_str() method can be used to pass a null
 * terminated string. However, the string must remain valid until the end of the
 * scope.
 *
 */
class SyncScope {
 public:
  template <class... Args>
  SyncScope(const char* name, const Args&... args);

  ~SyncScope();

 private:
  const char* name_;
};

template <class... Args>
SyncScope::SyncScope(const char* name, const Args&... args) : name_(name) {
  syncBegin(name_, args...);
}

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
 * @param name Scope name in c-string format.
 * @param args Constant reference to additional arguments.
 */
template <class... Args>
void asyncBegin(const char* name, const Args&... args) {
  details::writeTraceEvent(static_cast<event_type_t>(EventType::kAsyncBeginTag),
                           name, args...);
}

/**
 * @brief Create an asynchronous instance trace event.
 *
 * The method publishes an asynchronous instance trace event attaching the
 * provided additional arguments to the event.
 *
 * @tparam Args Additional argument types.
 * @param name Scope name in c-string format.
 * @param args Constant reference to additional arguments.
 */
template <class... Args>
void asyncInstance(const char* name, const Args&... args) {
  details::writeTraceEvent(
      static_cast<event_type_t>(EventType::kAsyncInstanceTag), name, args...);
}

/**
 * @brief Create an asynchronous end trace event.
 *
 * The method publishes an asynchronous end trace event attaching the provided
 * additional arguments to the event.
 *
 * @tparam Args Additional argument types.
 * @param name Scope name in c-string format.
 * @param args Constant reference to additional arguments.
 */
template <class... Args>
void asyncEnd(const char* name, const Args&... args) {
  details::writeTraceEvent(static_cast<event_type_t>(EventType::kAsyncEndTag),
                           name, args...);
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
 * @param name Scope name in c-string format.
 * @param args Constant reference to additional arguments.
 */
template <class... Args>
void flowBegin(const char* name, const Args&... args) {
  details::writeTraceEvent(static_cast<event_type_t>(EventType::kFlowBeginTag),
                           name, args...);
}

/**
 * @brief Create a flow instance trace event.
 *
 * The method publishes a flow instance trace event attaching the provided
 * additional arguments to the event.
 *
 * @tparam Args Additional argument types.
 * @param name Scope name in c-string format.
 * @param args Constant reference to additional arguments.
 */
template <class... Args>
void flowInstance(const char* name, const Args&... args) {
  details::writeTraceEvent(
      static_cast<event_type_t>(EventType::kFlowInstanceTag), name, args...);
}

/**
 * @brief Create a flow end trace event.
 *
 * The method publishes a flow end trace event attaching the provided
 * additional arguments to the event.
 *
 * @tparam Args Additional argument types.
 * @param name Scope name in c-string format.
 * @param args Constant reference to additional arguments.
 */
template <class... Args>
void flowEnd(const char* name, const Args&... args) {
  details::writeTraceEvent(static_cast<event_type_t>(EventType::kFlowEndTag),
                           name, args...);
}

// ------------------------------------
// Counter Event
// ====================================

/**
 * @brief Create a counter metric event.
 *
 * @tparam T Type of counter value.
 * @param name Name of counter in c-string format.
 * @param arg Value of counter.
 */
template <class T>
void counter(const char* name, const T& arg) {
  details::writeTraceEvent(static_cast<event_type_t>(EventType::kCounterTag),
                           name, arg);
}

// ------------------------------------

}  // namespace inspector

// ------------------------------------
// Convenience Macros
// ------------------------------------

// Utility macros to get unique scope name. These are meant for internal use.
#define __UNIQUE_MAKER__(name, counter) __##name##__##counter##__
#define __MAKE_UNIQUE__(name) __UNIQUE_MAKER__(name, __COUNTER__)

/**
 * @brief Macro to create a keyword debug argument.
 */
#define KWARG(name, value) inspector::details::makeKeywordArg(name, value)

/**
 * @brief Synchronous trace events.
 *
 */

#define TRACE() inspector::SyncScope __MAKE_UNIQUE__(sync_scope)(__func__)
#define TRACE_WITH_ARGS(...) \
  inspector::SyncScope __MAKE_UNIQUE__(sync_scope)(__func__, __VA_ARGS__)

#define TRACE_SCOPE(name) inspector::SyncScope __MAKE_UNIQUE__(sync_scope)(name)
#define TRACE_SCOPE_WITH_ARGS(name, ...) \
  inspector::SyncScope __MAKE_UNIQUE__(sync_scope)(name, __VA_ARGS__)

/**
 * @brief Asynchronous trace events.
 *
 */

#define TRACE_ASYNC_BEGIN(name) inspector::asyncBegin(name)
#define TRACE_ASYNC_BEGIN_WITH_ARGS(name, ...) \
  inspector::asyncBegin(name, __VA_ARGS__)

#define TRACE_ASYNC_INSTANCE(name) inspector::asyncInstance(name)
#define TRACE_ASYNC_INSTANCE_WITH_ARGS(name, ...) \
  inspector::asyncInstance(name, __VA_ARGS__)

#define TRACE_ASYNC_END(name) inspector::asyncEnd(name)
#define TRACE_ASYNC_END_WITH_ARGS(name, ...) \
  inspector::asyncEnd(name, __VA_ARGS__)

/**
 * @brief Flow trace events
 *
 */

#define TRACE_FLOW_BEGIN(name, ...) inspector::flowBegin(name)
#define TRACE_FLOW_BEGIN_WITH_ARGS(name, ...) \
  inspector::flowBegin(name, __VA_ARGS__)

#define TRACE_FLOW_INSTANCE(name) inspector::flowInstance(name)
#define TRACE_FLOW_INSTANCE_WITH_ARGS(name, ...) \
  inspector::flowInstance(name, __VA_ARGS__)

#define TRACE_FLOW_END(name) inspector::flowEnd(name)
#define TRACE_FLOW_END_WITH_ARGS(name, ...) \
  inspector::flowEnd(name, __VA_ARGS__)

/**
 * @brief Counter trace events
 *
 */
#define TRACE_COUNTER(name, value) inspector::counter(name, value)

// ------------------------------------
