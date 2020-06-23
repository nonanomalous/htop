#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(const int pid, const long jiffies) : pid_(pid) {
    Process::cpu_utilization_ = (float)LinuxParser::ActiveJiffies(Process::pid_) / (float)jiffies;
}

int Process::Pid() { return Process::pid_; }

float Process::CpuUtilization() { 
    Process::cpu_utilization_ = (float)LinuxParser::ActiveJiffies(Process::pid_) / (float)LinuxParser::ActiveJiffies();
    return Process::cpu_utilization_; 
    }

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const { 
    return (this->cpu_utilization_ > a.cpu_utilization_) ? true : false;
}