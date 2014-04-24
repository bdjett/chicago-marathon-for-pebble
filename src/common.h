/**********************************
* Chicago Marathon for Pebble
*
* NOTE: NOT ASSOCIATED WITH THE CHICAGO MARATHON OR BANK OF AMERICA IN ANY
*       WAY. EVERYTHING IS COPYRIGHT OF ITS RESPECTIVE OWNER.
*
* Created by: Brian Jett (http://logicalpixels.com)
*
* common.h
*/

#pragma once

#include <pebble.h>
#include "pebble-assist.h"
#include "runners.h"
#include "splits.h"
#include "error.h"

typedef struct {
	int index;
	char name[128];
	char subtitle[128];
} Runner;

typedef struct {
	int index;
	char location[128];
	char time[128];
} Split;

enum {
	RUNNER_INDEX = 0x0,
	RUNNER_NAME = 0x1,
	SUBTITLE = 0x2,
	SPLIT_INDEX = 0x3,
	SPLIT_LOCATION = 0x4,
	SPLIT_TIME = 0x5,
	GET_SPLITS = 0x6,
	CLEAN_LIST = 0x7,
	ERROR = 0x8
};
