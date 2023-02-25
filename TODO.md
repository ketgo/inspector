// Copyright 2022 Ketan Goyal
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

# Tasks

The file contains itemized tasks to be completed in the different parts of the repo.

## Core Lib

[-] Rigorous unit testing of circular queue, e.g. test behavior during overflow condition. Furthermore, there are some components which have missing tests. 
[X] Design and implement option to enable and disable publishing trace events. The design should support multiple languages, e.g. cpp, python, etc.
[X] Recover from stale cursors in the event queue.
[ ] Implement three policies to handle the shared memory circular queue full condition: Log Policy, Wait Policy, Ignore Policy.
[ ] Refactor initialization API for the inspector library. Maybe just expose a `InitInspector` method to set configuration settings. 
[ ] Design an approach for masterless shared object handling. That is, removal of a shared object should be done by the last process using the object. Furthermore, the approach should be fail safe. 
[ ] Need an improved recovery approach which better handles the false start_marker scenario. Furthermore, the approach should utilize the fact of having multiple concurrent consumers to quickly recover to a valid state.
[X] Extend trace event data struct in order to support implementing different types of events published from other tracers (CPU, GPU, kernel, etc).
[ ] Improved API for configuring lib.

## Monitors

[-] CLI monitor to view the published trace events on console in realtime or store them in a file.
[ ] Web monitor to view the published trace events on browser in realtime.
[ ] Traceview generator to convert the published trace events to the format accepted by Catapult Traceview.
[ ] Add binding for javascript
[ ] Add binding for Java
[ ] Tracers for monitoring CPU, GPU and kernel

## Others

[ ] User documentation
[ ] Usage examples in different languages
