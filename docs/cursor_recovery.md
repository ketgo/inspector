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

# Cursor Recovery Approach

A stale cursor can be resulted when a process is abruptly terminated while writing or reading from the circular queue. This document describes the approach taken to properly recover stale cursors without impacting the queue functioning and performance. We can divide the approach into two parts: detecting presence of a stale cursor, and recovering the queue. The two are discussed in the following sections.

## Detecting Stale Cursor

In order to detect staleness we store a timestamp into the cursor state data structure. The sturcture is as shown below:

```cpp
struct CursorState {
  bool allocated : 1;
  uint64_t timestamp: 63;
};
```

We keep the state 64-bit in order to make it lock free using the `std::atomic` lib. The `allocated` field indicates if the cursor is allocated and the `timestamp` field contains the monotonic timestamp when the cursor was allocated. The `timestamp` field is used to detect for staleness. We compare the elapsed time since the cursor was allocated with a pre-configured threshold value. If the elapsed time is greater than the threshold then the cursor is considered stale.

## Queue Recovery

Presence of a stale cursor indicates an incomplete memory block has been written or read. In certain cases there might be no associated memory block. To better understand how a stale cursor effects the queue, we first revisit how a memory block is allocated on the queue for write/read.

| #   | READ                                                                                                                                                                                                       | WRITE                                                                                                                                                                                                                                      |
| --- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| 1.  | Allocate a free cursor from read cursor pool                                                                                                                                                               | Allocate a free cursor from write cursor pool                                                                                                                                                                                              |
| 2.  | Load the read and write head values                                                                                                                                                                        | Load the read and write head values                                                                                                                                                                                                        |
| 3.  | The memory block header is read to obtain the block size                                                                                                                                                   | Use the requested size to get the ending location of the to be created memory block. Make sure that this location is behind all the read cursors and the read head. Also make sure the loaded read head is equal or behind the write head. |
| 4.  | Use the block size to get the ending location of the block. Make sure that this location is behind all the write cursors and the write head. Also make sure the loaded read head is behind the write head. | Store the write head value into the allocated cursor.                                                                                                                                                                                      |
| 5.  | Store the read head value into the allocated cursor                                                                                                                                                        | Update the write head value if not already updated by another writer                                                                                                                                                                       |
| 6.  | Update the read head value if not already updated                                                                                                                                                          | Write the block header                                                                                                                                                                                                                     |
| 7.  | Read the data in the memory block                                                                                                                                                                          | Write data to the memory block                                                                                                                                                                                                             |
| 8.  | Release the allocated cursor                                                                                                                                                                               | Release the allocated cursor                                                                                                                                                                                                               |

A process abruptly terminating anywhere between step 1 and 8 will result in a stale cursor. Furthermore, it can be infered from step 3 for writers and step 4 for readers that when a cursor becomes stale, it will eventually block all the consumers (readers) and producers (writers) respectvely. To prevent such a scenario from happening, we propose the following recovery strategy.

1. Along with the block size, we also store a start marker and checksum value in the memory block header. The data structure of the memory block becomes:
```cpp
template <class T>
struct __attribute__((packed)) MemoryBlock {
  // Ensures at compile time that the parameter T has trivial memory layout.
  static_assert(std::is_trivial<T>::value,
                "The data type used does not have a trivial memory layout.");

  std::size_t size;
  uint32_t magic;
  uint32_t checksum;
  T data[0];
};
```
2. We perform recovery only when a producer or consumer hits a memory block associated with a stale cursor. We will refer to such a memory block as stale block. The check of hitting a stale block is done in step 3 of the table. The below table explains the approach taken for readers and writers.

| READER                                                                                                                                                                                                                                                                            | WRITER                                                                                                                                                                                                                                                                            |
| --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Check timestamp of each allocated cursor in the `CursorPool::IsBehind` method. If the elapsed time since the allocation is above threshold, and the cursor position is behind the given end position, we are hitting a stale block. Release the cursor and notify the `Allocator` | Check timestamp of each allocated cursor in the `CursorPool::IsAhead` method. If the elapsed time since the allocation is above threshold, and the cursor position is behind the given end position, we are hitting a stale block. Release the cursor and notify the `Allocator`. |

1. When a writer hits a stale block, no new operation is performed expecept for releasing the stale cursor in the above step. When a reader hits a stale block the following is performed (Note that this works only when a writer writes the block size before writing the start marker in the block header):
   1. Check if start marker is present in the header. 
   2. If the start marker is present, use the block size in the header to increase the read head and reserve the stale block. Then validate the checksum. If checksum is invalid then skip the block. Otherewise process the data in the block normally. Note that the process of validating the checksum can also be left to the user and we can remove it from the recovery process. This simplifies the implementation and is what is done in the library. Furthermore, we removed the checksum value from the header as it is not needed by the recovery process.
   3. If the start marker is not present, recover the block size by traversing the buffer till a start marker is observed or the write head is reached. If the write head is reached then skip any operation and return kBufferFull status. After succesful block size recovery, use it to increase the read head so that the stale block can be reserved. Then perform no-op to skip the block since the block is incomplete.

## Limitations

a. One edge case that needs to be addressed in the above approach is release of a stale cursor allocated to a process which takes longer than the threshold to complete. In such a case the process is still alive and would result in data race. In fact, the above approach still works if a consumer always performs checksum validation before using the stored data. Once the alive process writes on the stale block, any consumer will ignore it as the checksum will become invalid.

b. While recovering the block size, it is possible to reach a false margic mark if the message contains the same mark in its contents. 
SOLUTIONS:
- Keep the size information in a seprate queue?
- Use user provided marker and warn the user to not use that marker in message content?
- Update recovery algorithm to test the block size of the next found block?