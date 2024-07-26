# Common

This directory contains the common code shared by the client and server. These sources are written in header-only C++ style,
and easy to use in both client and server.

This document describes the modules in the common directory:

- Scoped Threading
- Channel-based IPC

## Scoped Threading

This module provides a simple way to use threads without the need to manage the thread release, it will automatically join and release the thread when the scope ends.

Based on the `ScopedThread` virtual class, this module provides a `WorkerThread` class that can be used to create a thread
running as a worker, that developer can start, stop, pause and resume it.

## Channel-based IPC

In JSAR, we use a channel-based IPC to communicate between the client and server. This module provides classes to establish
connections, serialize and deserialize messages, and send and receive them, this module leverages a specific C++ class ability to transfer between client and server side.
