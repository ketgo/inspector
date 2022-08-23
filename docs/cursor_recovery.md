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

The following sections describe the approach taken to resolve the different issues faced in order to recover from an abrupt process termination leaving the circular queue in an invalid state.

## ISSUE 1

How to discover if a cursor is stale due to an abrupt process termination.

### SOLUTION

- Cursor:
  - 0: overflow
  - 1-63: position

- CursorState
  - 0: allocate
  - 1-63: timestamp (monotonic)

The approach is to check for the allocation timestamp of the cursor in the `IsAhead`, `IsBehind` and `Allocate` method of the `CursorPool`. If the differen between the current monotonic timestamp and the allocate timestamp is greater than some threshold then consider the cursor as stale.

## ISSUE 2

How to recover a stale cursor.

### SOLUTION

a. Release the stale cursor
b. If the stale cursor is a write cursor, any block writen using the cursor should be skipped by the consumer. This can be achived by marking the block as corrupt.
c. If the stale cursor is a read cursor, then it should be either passed to another consumer so that the message can be re-consumed, or it should just be released so that the message is skipped. The former results in ATLEAST_ONCE delivery semantic while the latter results in ATMOST_ONCE.

## ISSUE 3 

The `Consume` method of the circular queue returns a read span which can be used by the user to write data onto the queue. Need a way to invalidate the use of the span.

### SOLUTION 

Change API for `Consume` and `Produce` where the method takes a `DynamicSpan` object which satisfies the following concepts. Furthermore, the methods should handle copying of data from/to the queue and buffer.

| Expression   | Return | Description                             |
| ------------ | ------ | --------------------------------------- |
| S.Reserve(X) | void   | Reserve X bytes in the dynamic span S   |
| S.Data()     | Byte*  | Get the starting location of the span S |
| S.Size()     | size_t | Get the size in bytes of the span S     |

## ISSUE 4

How to detect if a memory block is writen by using a stale cursor ?

### SOLUTION

We use the following data block header

    Header:
      - [16-bit] magic mark
      - [32-bit] block size
      - [32-bit] CRC checksum

and perform the following steps after detecting a stale cursor.

List of failure cases of a process:
a. Failed after reserving cursor but before reserving space on the buffer
b. Failed after reserving space on the buffer but before writing header magic mark
c. Failed after writing magic mark but before writing the block size
d. Failed after writing block size but before writing the checksum. This includes failure while writing the actual message data.
e. Failed after writing checksum but before releasing the cursor

1. Read the header at the location contained on the cursor.
2. Check if the magic mark is present
  N: This will happen for failure cases a and b.
    1. First step is to detect which one of the two cases happened. The following table lists the detection techinque
      | Cases | Detection                                     |
      | ----- | --------------------------------------------- |
      | a     | write head value is same as the cursor value  |
      | b     | write head value not same as the cursor value |
    2. In case of a we just release the cursor. In case of b we recover the block size by transversing till we hit the next magic mark. If we hit the write head before observing the next magic mark then the recovery is stopped until the next message is completely written. Note that this is not a limitation since there is no consumable message present in the queue at this point.
    3. Set the magic mark, computed block size, and a default checksum in the header. This allows the consumer to properly skip the block by checking for the default checksum.   
  Y: This will happen for the cases a, c, d and e.
    1. First step is to detect which one of the four cases happened. The following table lists the detection techinque
      | Cases | Detection              |
      | ----- | ---------------------- |
      | a, e  | Valid checksum value   |
      | c, d  | Invalid checksum value |
      We can re-compute the checksum along with the block size by transversing to the next magic marker.  If we hit the write head before observing the next magic mark then the recovery is stopped until the next message is completely written. Note that this is not a limitation since there is no consumable message present in the queue at this point.
    2. For the cases a and e we just release the cursor for reuse. If any of the other two cases happen then we compute and set the block size and the default checksum. 

NOTE: Possible data race when checking for the next magic marker or write head. To reslove this always load the write head value first before starting the recovery algorithm.

## ISSUE 5

How to handle the case of a false release in which the process owning the pressumed stale cursor is still alive.

### SOLUTION

Just ignore the write since its suppose to be skipped by the consumers.
