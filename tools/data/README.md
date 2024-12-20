<!--
 Copyright 2022 Ketan Goyal
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
     http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
-->

# Data 

The package contains reader and writer for loading and storing trace events and metrics. They are designed to meet the followng requirements:

- Fast write and fast to medium read performance.
- Readers should load events in chronological order. We use external sort to fulfil this requirement.
- Thread safety in writers in order to supported multi-threaded write operations.

## Writer

The writer stores chronologically sorted events in chunks of fixed size files. Each file contains a header and body section. The header sction will consist of following metadata information:

- Number of events
- Time duration associated with the file.
- Timestamps of latest and oldest event in the file.

The body will contain the captured trace events in raw binary format.

## Reader

The reader loads events stored using the writer in chronologically sorted order. In order to achive fast read performance, readers have a max durational window for sorting events. Any event falling outside this window is considered too out of order to be sortable. In such case, the reader can either skip these events or forwarded it to the user to handle accordingly. We thus have two modes of loading the events: AlwaysChronological and AlmostChronological.  