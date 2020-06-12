#pragma once
#define EXPORT __declspec(dllimport)
#include <stdint.h>
#include <jansson.h>
#include <thcrap.h>
#include <thcrap_tsa.h>

// Print a message to logs, tagging as from this plugin
// Used mostly for debug prints
void log_print(const char*);

// Called very early in the thcrap start process, checks to make sure we're available
EXPORT int thcrap_plugin_init(void);

/// remote_mod_* functions
/// Part of thcrap, called on the appropriate event
/// The source is in /thcrap/thcrap/src/plugin.h:29

// Called when everything is loaded, used to prepare data structures and the server
EXPORT int remote_mod_init(void);

// Breakpoint for main loop
// Updates the server's struct
EXPORT int BP_main_loop(x86_reg_t *regs, json_t *bp_info);