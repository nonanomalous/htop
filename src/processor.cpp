#include "linux_parser.h"
#include "processor.h"

Processor::Processor() {
    Processor::cached_total_ticks_ = LinuxParser::Jiffies();
    Processor::cached_active_ticks_ = LinuxParser::ActiveJiffies();
}
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    
    long total_ticks = LinuxParser::Jiffies();
    long active_ticks = LinuxParser::ActiveJiffies();

    long total_delta{total_ticks - Processor::cached_total_ticks_};    
    long active_delta{active_ticks - Processor::cached_active_ticks_};
    
    cached_total_ticks_ = total_ticks;
    cached_active_ticks_ = active_ticks;
    
    return (float)active_delta / (float)total_delta;
    }