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
#include <stdio.h>
#include "pebble-assist.h"
#include "runners.h"

typedef struct {
	int index;
	char name[128];
	char subtitle[128];
} Runner;

typedef struct {
	char location[128];
	char time[128];
} Split;

enum {
	RUNNER_INDEX = 0x0,
	RUNNER_NAME = 0x1,
	SUBTITLE = 0x2
};
