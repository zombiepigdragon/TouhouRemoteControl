#pragma once
#include <jansson.h>
#include "remote_control.h"
#include "shared_data.h"

// Returns the size needed to allocate the remote_address
size_t get_address_size(json_t* remote_address);
// Copies the memory at the location into the buffer
int read_remote_address(void* buffer, json_t* remote_address);
// Fill in all of the data from the runconfig into the output json
// Call after taking the mutex
int process_full_frame(struct SharedData* sharedData);