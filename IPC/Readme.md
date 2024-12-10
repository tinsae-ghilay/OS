## IPC
Interprocess communication.

communication between two processes,in this case *sender* and *receiver* using *MessageQueue* and between *reader* and *writer* using *SharedMemory*. cteating maintaining and closing of message queue and shared memory done in respective header and source files.

e.g.
```C
// to create message queue for sending we can do
mdq_t m_queue = createForSender(); // and that is done.
```