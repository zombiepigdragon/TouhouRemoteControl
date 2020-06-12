#pragma once
#include <stdint.h>
// Information on the currently running game

enum RemoteAddressTypes
{
    RA_SCORE
};

struct RemoteAddress
{
    void* game_address;
    void* data_address;
    size_t size;
};

// The configuration and data from the game
struct SharedData
{
    const char* port;
    struct RemoteAddress values[1];
    uint32_t score;
};

// Fills in the GameData with the information of the shown event
int create_remote_address(struct SharedData* shareddata, const char* name, void* game_address);