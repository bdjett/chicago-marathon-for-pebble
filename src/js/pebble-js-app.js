/**********************************
 * Chicago Marathon for Pebble
 *
 * NOTE: NOT ASSOCIATED WITH THE CHICAGO MARATHON OR BANK OF AMERICA IN ANY
 *       WAY. EVERYTHING IS COPYRIGHT OF ITS RESPECTIVE OWNER.
 *
 * Created by: Brian Jett (http://logicalpixels.com)
 *
 * pebble-js-app.js
 */

var maxAppMessageBuffer = 100;
var maxAppMessageTries = 3;
var appMessageRetryTimeout = 3000;
var appMessageTimeout = 100;
var httpTimeout = 12000;
var appMessageQueue = [];

var eventName = "BACM2013";
var appId = "502c333ae39f5eb1d4b586d0";

//localStorage.trackedRunners = "[]";
//localStorage.igt = "";

// register
// --------
// Register current session with rtrt.me and receive a session token in return
var register = function() {
    var req = new XMLHttpRequest();
    var requestUrl = "http://api2.rtrt.me/register?appid=" + appId;
    console.log(requestUrl);
    req.open('GET', requestUrl, false);
    req.onload = function(e) {
        if (req.readyState == 4) {
            if (req.status == 200) {
                if (req.responseText) {
                    var response = JSON.parse(req.responseText);
                    if (response.token) {
                        localStorage.sessionToken = response.token;
                        console.log(localStorage.sessionToken);
                    } else {
                        console.log("Unable to authenticate with app ID: " + appId);
                    }
                }
            }
        }
    };
    req.ontimeout = function() {
        console.log("Timed out");
    };
    req.onerror = function() {
        console.log("Connection failed");
    };
    req.send(null);
};

// getSplitsOfRunner
// -----------------
// takes a runner ID or bib number and returns a list of the splits for the runner
// in descending order
var getSplitsOfRunner = function(runnerId) {
    var req = new XMLHttpRequest();
    var requestUrl = "http://api2.rtrt.me/events/" + eventName + "/profiles/" + runnerId + "/splits?timesort=1&appid=" + appId + "&token=" + localStorage.sessionToken;
    console.log(requestUrl);
    req.open('GET', requestUrl, true);
    req.onload = function(e) {
        if (req.readyState == 4) {
            if (req.status == 200) {
                if (req.responseText) {
                    var response = JSON.parse(req.responseText);
                    if (response.list) {
                        splitsArray = response.list;
                        splitsArray.forEach(function(element, index, array) {
                            console.log(element.label + " - " + element.time);
                            appMessageQueue.push({'message': {
                                'split_index': index,
                                'split_location': element.label,
                                'split_time': element.time
                            }});
                        });
                    } else {
                        console.log("No splits");
                    }
                }
            }
        }
        sendAppMessage();
    };
    req.ontimeout = function() {
        console.log("Timed out");
    };
    req.onerror = function() {
        console.log("Connection failed");
    };
    req.send(null);
};

// getNewSplits
// ------------
// getNewSplits polls for newly inserted splits since the last check and returns
// a notification of these new splits
var getNewSplits = function() {
    var req = new XMLHttpRequest();
    var runners = JSON.parse(localStorage.trackedRunners).join();
    var requestUrl = "http://api2.rtrt.me/events/" + eventName + "/profiles/" + runners + "/splits?timesort=1&appid=" + appId + "&token=" + localStorage.sessionToken + "&igt=" + localStorage.igt;
    //console.log(requestUrl);
    req.open('GET', requestUrl, true);
    req.onload = function(e) {
        if (req.readyState == 4) {
            if (req.status == 200) {
                if (req.responseText) {
                    var response = JSON.parse(req.responseText);
                    if (response.list) {
                        var splitsArray = response.list;
                        splitsArray.forEach(function(element, index, array) {
                            console.log(element.label + " - " + element.time);
                            if (localStorage.igt != "") {
                                Pebble.showSimpleNotificationOnPebble(element.name, element.label + " - " + element.time);
                            }
                        });
                        console.log(localStorage.igt);
                        localStorage.igt = response.info.lasti;
                    } else {
                        console.log("No results");
                    }
                }
            }
        }
    };
    req.ontimeout = function() {
        console.log("Timed out");
    };
    req.onerror = function() {
        console.log("Connection failed");
    };
    req.send(null);
};

// poll
// ----
// Constantly poll for new split times
var poll = function() {
    setTimeout(function() {
        getNewSplits();
        poll();
    }, 1000);
};

// getTrackedRunners
// -----------------
// returns the info for all of the tracked runners and sends them via app message
// to Pebble
var getTrackedRunners = function() {
    var runners = JSON.parse(localStorage.trackedRunners);
    console.log(runners);
    if (runners == "") {
        console.log("no runners");
        sendError("Please add runners to track in the Pebble app on your phone.");
        return;
    } else {
        runners.forEach(function(element, index, array) {
            var req = new XMLHttpRequest();
            var requestUrl = "http://api2.rtrt.me/events/" + eventName + "/profiles/" + element + "?appid=" + appId + "&token=" + localStorage.sessionToken;
            console.log(requestUrl);
            req.open('GET', requestUrl, false);
            req.onload = function(e) {
                if (req.readyState == 4) {
                    if (req.status == 200) {
                        if (req.responseText) {
                            var response = JSON.parse(req.responseText);
                            if (response.list) {
                                var person = response.list[0];
                                var name = person.fname + " " + person.lname;
                                var subtitle = person.bib + " - " + person.sex + " - " + person.country;
                                appMessageQueue.push({'message': {
                                    'index': index,
                                    'name': name,
                                    'subtitle': subtitle
                                }});
                            } else {
                                getRunnerInfo();
                            }
                        }
                    }
                }
            };
            req.ontimeout = function() {
                console.log("Timed out");
            };
            req.onerror = function() {
                console.log("Connectioin failed");
            };
            req.send(null);
        });
        console.log("Sending message...");
        sendAppMessage();
    }
};

var clearList = function() {
    appMessageQueue.push({'message': {
        'clean_list': 1
    }});
};

// sendAppMessage
// --------------
// sends app message queue to the Pebble
var sendAppMessage = function() {
    if (appMessageQueue.length > 0) {
        console.log("got an app message queue");
        var currentAppMessage = appMessageQueue[0];
        currentAppMessage.numTries = currentAppMessage.numTries || 0;
        currentAppMessage.transactionId = currentAppMessage.transactionId || -1;

        if (currentAppMessage.numTries < maxAppMessageTries) {
            console.log("Sending message");
            Pebble.sendAppMessage(
                currentAppMessage.message,
                function(e) {
                    appMessageQueue.shift();
                    setTimeout(function() {
                        sendAppMessage();
                    }, appMessageTimeout);
                }, function(e) {
                    console.log("Faled sending AppMessage for transactionId: " + e.data.transactionId + ". Error: " + e.data.error.message);
                    appMessageQueue[0].transactionId = e.data.transactionId;
                    appMessageQueue[0].numTries++;
                    setTimeout(function() {
                        sendAppMessage();
                    }, appMessageRetryTimeout);
                }
            );
        } else {
            console.log("Faled sending AppMessage after multiple attemps for transactionId: " + currentAppMessage.transactionId + ". Error: None. Here's the message: " + JSON.stringify(currentAppMessage.message));
        }
    }
};

// sendError
// ---------
// sends error to the pebble
var sendError = function(error) {
    appMessageQueue.push({'message': {
        'error': error
    }});
    sendAppMessage();
};

Pebble.addEventListener("appmessage", function(e) {
    console.log("Got a message" + JSON.stringify(e.payload));
    if (e.payload.hasOwnProperty('get_splits')) {
        console.log("Getting splits");
        var runners = JSON.parse(localStorage.trackedRunners);
        getSplitsOfRunner(runners[e.payload.get_splits]);
    }
});

Pebble.addEventListener("showConfiguration", function(e) {
    var data = encodeURIComponent(localStorage.trackedRunners);
    Pebble.openURL("http://logicalpixels.com/chicagomarathon/index.html#" + data);
});

Pebble.addEventListener("webviewclosed", function(e) {
    var configuration = decodeURIComponent(e.response);
    if (configuration) {
        localStorage.trackedRunners = configuration;
        clearList();
        getTrackedRunners();
    } else {

    }
});

Pebble.addEventListener("ready", function(e) {
    register();
    if (!localStorage.trackedRunners) {
        localStorage.trackedRunners = '[]';
    }
    if (!localStorage.igt) {
        localStorage.igt = "";
    }
    getTrackedRunners();
    poll();
});
