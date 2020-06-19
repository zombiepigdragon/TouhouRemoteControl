#include "read_data.h"

size_t get_address_size(json_t* remote_address)
{
    json_t *json_size = json_object_get(remote_address, "size");
    if (!json_is_integer(json_size))
    {
        return 0;
    }
    return json_integer_value(json_size);
}

int read_remote_address(void* buffer, json_t* remote_address)
{
    json_t* json_address = json_object_get(remote_address, "addr");
    size_t address = json_integer_value(json_address);
    return ReadProcessMemory(GetCurrentProcess(), (void*)address, buffer, get_address_size(remote_address), NULL);
}

int process_full_frame(struct SharedData* sharedData)
{
    json_t* json_remote_addresses = json_object_get(sharedData->run_configuration, "remote_addresses");
    json_t* json_output = sharedData->output;
    json_t* json_data = json_object_get(json_output, "data");

    json_t* json_item;
    const char* key;
    void* temp; // Used by json_object_foreach_safe to maintain state
    json_object_foreach_safe(json_remote_addresses, temp, key, json_item)
    {
        size_t bufferSize = get_address_size(json_item);
        char buffer[bufferSize];
        if(!read_remote_address(buffer, json_item))
        {
            remote_log_printf("[Remote Control] Warning: error reading memory for %s. Removing key from known keys.\n", key);
            json_object_del(json_remote_addresses, key);
            continue;
        }
        json_object_set_new(json_data, key, json_integer(*buffer));
    }
    return 0;
}