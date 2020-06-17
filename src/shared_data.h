#pragma once
#include <stdint.h>
#include <jansson.h>

// The configuration and data from the game
struct SharedData
{
    const char* port;
    json_t* run_configuration;
    json_t* output;
};

// Validate and set the port,called by generate_shareddata, returns true if default was used, false otherwise
int set_remote_port(struct SharedData* sharedData, json_t* json_port);

// Validate and create the list of remote addresses for both our own runconfig and our output
int process_remote_addresses(json_t* config_addresses, json_t* validated_addresses, json_t* output_data);

// Process the runconfig and make a valid SharedData from it
// Also sanitizes and validates input
int generate_shareddata(struct SharedData* sharedData, json_t* runconfig);