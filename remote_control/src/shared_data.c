#include "shared_data.h"
#include "remote_control.h"

const char* const DEFAULT_PORT = "8646";
const int DEFAULT_SIZE = 4;

int set_remote_port(struct SharedData* sharedData, json_t* json_port)
{
    if (json_port && json_is_string(json_port))
    {
        sharedData->port = json_string_value(json_port);
    }
    else
    {
        remote_log_puts("No port set, using default.");
        sharedData->port = DEFAULT_PORT;
    }
    // Check for a valid port
    for (int length = strlen(sharedData->port), i = 0; i < length; i++)
    {
        char digit = sharedData->port[i];
        if (digit < '0' || digit > '9')
        {
            remote_log_printf("[Remote Control] The given port \"%s\" is invalid. Using default.\n", sharedData->port);
            sharedData->port = DEFAULT_PORT;
            break;
        }
    }
    remote_log_printf("[Remote Control] Using port %s.\n", sharedData->port);
    return 1;
}

int process_remote_addresses(json_t* config_addresses, json_t* validated_addresses, json_t* output_data)
{
    int remote_address_count = json_object_size(config_addresses);
    if (!remote_address_count)
    {
        remote_log_puts("No remote_addresses found.");
        return 0;
    }
    const char* key;
    json_t* json_item = NULL;
    int i = 0;
    json_object_foreach(config_addresses, key, json_item)
    {
        i++;
        remote_log_printf("[Remote Control] Remote address %d/%d: ", i, remote_address_count);

        json_t* json_address = json_object_get(json_item, "addr");
        str_address_ret_t str_address_ret;
        ZeroMemory(&str_address_ret, sizeof(str_address_ret));
        size_t address = str_address_value(json_string_value(json_address), NULL, &str_address_ret);
        if (str_address_ret.error != STR_ADDRESS_ERROR_NONE)
        {
            log_printf("Error getting address for \"%s\", skipping.\n", key);
            continue;
        }

        json_t* json_size = json_object_get(json_item, "size");
        size_t size;
        size = json_size ? json_integer_value(json_size) : DEFAULT_SIZE;
        if (size == 0)
        {
            log_printf("%s", "(Error-size substituted)");
            size = DEFAULT_SIZE;
        }

        json_t* result = json_object();
        json_object_set_new(result, "addr", json_integer(address));
        json_object_set_new(result, "size", json_integer(size));

        json_object_set_new(validated_addresses, key, result);
        json_object_set_new(output_data, key, json_integer(0));
        
        log_printf("%d bytes , 0x%p \"%s\"\n", size, address, key);
    }
    return 1;
}

int generate_shareddata(struct SharedData* sharedData, json_t* runconfig)
{
    // If we've already set this up, clean up first
    if (sharedData->run_configuration)
    {
        json_decref(sharedData->run_configuration);
    }
    if (sharedData->output)
    {
        json_decref(sharedData->output);
    }

    set_remote_port(sharedData, json_object_get(runconfig, "remote_port"));

    json_t* json_run_configuration = json_object();
    json_t* json_output = json_object();
    json_t* json_data = json_object();
    json_object_set_new(json_output, "data", json_data); // Give json_data the same lifetime
    json_object_set_new(json_output, "errors", json_object()); // Filled on request from client, empty by default

    json_t* remote_addresses = json_deep_copy(json_object_get(runconfig, "remote_addresses"));
    json_t* json_run_remote_addresses = json_object();
    
    if (!process_remote_addresses(remote_addresses, json_run_remote_addresses, json_data))
    {
        return 1;
    }

    json_object_set_new(json_run_configuration, "remote_addresses", json_run_remote_addresses); // set_new because we no longer use it

    // For debugging, should probably remove before stable release
    char* output = json_dumps(json_run_configuration, JSON_INDENT(4));
    remote_log_puts("Run configuration generated:\n");
    remote_log_puts(output);
    free(output);
    sharedData->run_configuration = json_run_configuration;
    sharedData->output = json_output;
    return 0;
}