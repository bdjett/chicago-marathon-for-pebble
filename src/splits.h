/**********************************
* Chicago Marathon for Pebble
*
* NOTE: NOT ASSOCIATED WITH THE CHICAGO MARATHON OR BANK OF AMERICA IN ANY
*       WAY. EVERYTHING IS COPYRIGHT OF ITS RESPECTIVE OWNER.
*
* Created by: Brian Jett (http://logicalpixels.com)
*
* runners.h
*/

#pragma once

void splits_init(void);
void splits_show();
void splits_destroy(void);
void splits_in_received_handler(DictionaryIterator *iter);
bool splits_is_on_top();
void get_splits(int index, char *name);
