#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    string hrs {}, mins {}, secs {};
    string elapsed_time {};
    hrs = std::to_string(seconds/3600);
    seconds %= 3600;
    mins = std::to_string(seconds/60);
    secs += std::to_string(seconds%60);

    if(hrs.size() == 1) hrs = '0' + hrs;
    if(mins.size() == 1) mins = '0' + mins;
    if(secs.size() == 1) secs = '0' + secs;

    return hrs + ':' + mins + ':' + secs;
}