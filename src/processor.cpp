#include "linux_parser.h"
#include "processor.h"

Processor::Processor() {
    Processor::cached_total_ticks_ = LinuxParser::Jiffies();
    Processor::cached_active_ticks_ = LinuxParser::ActiveJiffies();
}

long Processor::GetTotalJiffies() { return cached_total_ticks_; }

float Processor::Utilization() { 
    float utilization{0.0};
    long total_ticks = LinuxParser::Jiffies();
    long active_ticks = LinuxParser::ActiveJiffies();

    long total_delta{total_ticks - Processor::cached_total_ticks_};    
    long active_delta{active_ticks - Processor::cached_active_ticks_};
    
    cached_total_ticks_ = total_ticks;
    cached_active_ticks_ = active_ticks;
    utilization = (float)active_delta / (float)total_delta;
    if (utilization > 1)
        return 0.99;
    return utilization;
    }