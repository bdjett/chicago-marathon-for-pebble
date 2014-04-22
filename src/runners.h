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

void runners_init(void);
void runners_show();
void runners_destroy(void);
void runners_in_received_handler(DictionaryIterator *iter);
bool runners_is_on_top();
