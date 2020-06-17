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
    size_t address = str_address_value(json_string_value(json_address), NULL, NULL);
    return ReadProcessMemory(GetCurrentProcess(), (void*)address, buffer, get_address_size(remote_address), NULL);
}

int process_full_frame(struct SharedData* sharedData)
{
    // TODO: Implement
    return 0;
}