#include "remote_control.h"
#include "server.h"
#include "server_threads.h"
#include "shared_data.h"
#include "read_data.h"
#include <windows.h>

// The configuration and state of the game
struct SharedData sharedData;

EXPORT int thcrap_plugin_init(void)
{
    if(stack_remove_if_unneeded("remote_control"))
    {
		return 1;
	}
    return 0;
}

EXPORT int remote_mod_init(void)
{
    remote_log_puts("Attempting to load json data.");
    ZeroMemory(&sharedData, sizeof(struct SharedData));
    generate_shareddata(&sharedData, runconfig_get());
    return server_initialize(&sharedData); //0 if good, error is automatically logged otherwise
}

EXPORT int BP_main_loop(x86_reg_t *regs, json_t *bp_info)
{
    (void)regs, (void)bp_info; // The portable way to remove unused parameter warnings
    if (!lock_server_mutex())
    {
        remote_log_puts("Couldn't lock the server mutex.");
        return 0;
    }
    process_full_frame(&sharedData);
    unlock_server_mutex();
    return 1;
}