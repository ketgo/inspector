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

[x] Extend trace event data struct in order to support implementing different types of events published from other tracers (CPU, GPU, kernel, etc).
[x] BUG: in DebugArg implementation causing the following error:
```
{"timestamp":1703024038022413286,"pid":66486,"tid":66773,"type":"AsyncEnd","name":"boost::async_wait"}
terminate called after throwing an instance of 'std::runtime_error'
  what():  Invalid type specified for argument of type '0'.
```
[x] BUG: `tools/reader/reader_test` stuck
[ ] A complete scope needs begin and end events to be present. Its however possible for one of the two to get dropped when writing to SHM. In such a case we need a way to detect un-paired begin or end events during offline processing. Proposal: Add a new depth field for sync events. That in combination with counter can be used to detect dropped call stacks.
[ ] Debugging events desing and implementation. These events can be emitted by an application for debugging purposes. Thus we need to allow filtering based on different levels of info. 
[ ] Config to add file name and line number in debug events.

## Monitors

[ ] Detect and handle dropped events.
[x] CLI monitor to view the published trace events on stdout in realtime or store them in a file.
  [x] Add option for users to enable external sort. This can be an extenstion or improvement over the slidding window PQ.
  [x] Refactor the monitor to a periodic task implementation. 
[ ] Web monitor to view the published trace events on browser in realtime.
[-] Perfetto generator to convert the published trace events to the format accepted by Perfetto UI.
[ ] Add binding for javascript
[ ] Add binding for Java
[ ] Tracers for monitoring CPU, GPU and kernel

## Others

[ ] User documentation
[ ] Usage examples in different languages
