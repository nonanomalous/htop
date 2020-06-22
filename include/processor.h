#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
 Processor();
 float Utilization();
 long GetTotalJiffies();

 private:
 long cached_total_ticks_{0};
 long cached_active_ticks_{0};
};

#endif