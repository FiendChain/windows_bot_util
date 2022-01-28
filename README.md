# Introduction
A set of window's api wrappers which make it easy for computer vision enabled bots to play games.

AutoGui: move the mouse around.
KeyboardListener: global key listener (not just inside current process window)
MSS: a thread safe screen shotter 

# Use case (MSS)
- AI thread: Grabs screen and does processing synchronously on buffer
- GUI thread: Display buffer without grabbing screen

Reason: Having two separate BltBit calls slows things down.
Thus have just one place that calls BltBit synchronously where it needs to. 
Other parts of the program such as GUI rendering can fetch that buffer asynchronously.
Note that the reading from the buffer if guaranteed to point to a valid buffer. However
that buffer is not read/write exclusive. 

This is so that the gui thread cannot slow down the AI thread when it grabs a screenshot and does preprocessing on it.

