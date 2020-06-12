#include "shared_data.h"

int create_remote_address(struct SharedData* shareddata, const char* name, void* game_address)
{
    // The one missing piece of information
    struct RemoteAddress* value;
    void* data_address;
    size_t size;
    if (strcmp(name, "score"))
    {
        value = &shareddata->values[RA_SCORE];
        data_address = &shareddata->score;
        size = sizeof(shareddata->score);
    }
    else
    {
        return 1; //Couldn't find the value
    }
    value->game_address = game_address;
    value->data_address = data_address;
    value->size = size;
    return 0;
}