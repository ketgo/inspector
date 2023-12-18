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

#include <glog/logging.h>
#include <inspector/trace.hpp>

#include "examples/cpp/init.hpp"
#include "examples/cpp/number_generator.hpp"
#include "examples/cpp/periodic_task.hpp"

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = 1;

  // Initialize all third party libs
  inspector::examples::init();

  LOG(INFO) << "Starting Trace Generator...";
  inspector::examples::PrimeNumberGenerator generator;
  inspector::examples::PeriodicTask task(1000000000UL, generator);
  task.Run();

  return 0;
}
