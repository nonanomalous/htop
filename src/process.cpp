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
    Process::SetCpuUtilization(jiffies);
}

void Process::SetCpuUtilization(const long jiffies) {
    long active_jiffies = LinuxParser::ActiveJiffies(Process::pid_);
    Process::cpu_utilization_ = (float)active_jiffies / (float)jiffies;
}

int Process::Pid() { 
    return 0;
    //Process::pid_; 
    }

float Process::CpuUtilization() { return Process::cpu_utilization_; }

string Process::Command() { return LinuxParser::Command(pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return 0; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }