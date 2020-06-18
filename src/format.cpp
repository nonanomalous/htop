#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include "format.h"

std::string Format::Pad(int n) {
    if (n<10)
        return "0" + std::to_string(n);
    else
        return std::to_string(n);
}

std::string Format::ElapsedTime(long seconds) { 
    // HH:MM:SS
    int dd, hh, mm, ss;
    std::string uptime;
    dd = seconds / 86400;
    hh = (seconds % 86400) / 3600;
    mm = (seconds % 3600) / 60;
    ss = seconds % 60;
    if (dd==0) { 
        return Pad(hh) + ":" + Pad(mm) + ":" + Pad(ss); }
    else { 
        return Pad(dd) + " days " + Pad(hh) + ":" + Pad(mm) + ":" + Pad(ss); }
    }