#include <stdint.h>
#include <thcrap.h>
#include "logging.h"

void remote_log_puts(char* message)
{
    log_printf("[Remote Control] %s\n", message);
}

void remote_log_printf(char* str, ...)
{
	if(str) {
		va_list va;
		va_start(va, str);
		log_vprintf(str, va);
		va_end(va);
	}
}

void debug_message_box(char* message)
{
    MessageBox(NULL, message, "Touhou Remote Control Debug Box", MB_OK);
}