#pragma once
// Place a message in the logs, marked as [Remote Control] and followed with a newline
// Used mostly for logging
void remote_log_puts(char* message);
// Put a formatted message in the logs, starting with [Remote Control]
void remote_log_printf(char* format, ...);
// A debugging message box, used when logs take too long to check
// Useful for solving concurrency, but shouldn't really be used in published code
void debug_message_box(char* message);