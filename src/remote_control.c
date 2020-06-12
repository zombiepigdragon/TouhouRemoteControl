#include "remote_control.h"
#include "server.h"
#include "server_threads.h"
#include "shared_data.h"
#include <windows.h>

// The configuration and state of the game
struct SharedData sharedData = {};

void log_print(const char* message)
{
    log_printf("[Remote Control] %s\n", message);
}

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
    log_print("Attempting to load json data.");
    json_t* configuration = json_object_get(runconfig_get(), "remote_addresses");
    const char* key; // FIXME: Naming (below)
    json_t* config_item; // An address, size, etc. named object
    json_object_foreach(configuration, key, config_item)
    {
        json_t* json_address = json_object_get(config_item, "addr");
        size_t address = str_address_value(json_string_value(json_address), NULL, NULL);
        if (!address)
        {
            log_printf("[Remote Control] Item %s has a null address, skipping.", key);
            continue;
        }

        create_remote_address(&sharedData, key, (void *)address);

        log_printf("[Remote Control] Found item %s at location 0x%p\n", key, address);
    }
    sharedData.port = "8646";
    return server_initialize(&sharedData); //0 if good, error is automatically logged otherwise
}

EXPORT int BP_main_loop(x86_reg_t *regs, json_t *bp_info)
{
    // TODO: Implement
    return 1;
}