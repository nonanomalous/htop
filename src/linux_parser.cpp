#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <numeric>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::accumulate;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line, key, value;
  vector<float> meminfo;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    for (int i=0; i<2; ++i) { 
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> key >> value;
      meminfo.push_back(stof(value));
    }
  }
  return (meminfo.at(0) - meminfo.at(1)) / meminfo.at(0);
  }

long LinuxParser::UpTime() { 
  long uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
  }


long LinuxParser::Jiffies() { 
  vector<long> cpu_stats = LinuxParser::CpuUtilization();
  return accumulate(cpu_stats.begin(), cpu_stats.end(), 0);
  }

long LinuxParser::ActiveJiffies(int pid) { 
  string line, value;
  long total{0};
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    // 14 utime, 15 stime, 16 cutime, 17 cstime (all -1) //
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i=0; i<17; i++) {
      linestream >> value;
      if (i >= 13)
        total += stol(value);
    }
  }
  return total;
  }

long LinuxParser::ActiveJiffies() { 
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();  
  }


long LinuxParser::IdleJiffies() { 
  vector<long> cpu_stats = LinuxParser::CpuUtilization();
  return cpu_stats.at(CPUStates::kIdle_) + cpu_stats.at(CPUStates::kIOwait_);
  //return 0;
  }

/*
comment on LinuxParser::CpuUtilization()
During first approach I pop "Cpu" and then read longs in istringstream while loop
I kept getting byes for kUser_  (\020UC\b\000\000\000\000\065\067\062\071)
which would give this behavior:
(gdb) info locals
cpu_stats = std::vector of length 10, capacity 16 = {1725729, 0, 785136, 233498411, 0, 24843, 0, 0, 0, 0}
(gdb) n
100       vector<long> cpu_stats = LinuxParser::CpuUtilization();
(gdb) info locals
cpu_stats = std::vector of length 10, capacity 16 = {1725729, 0, 785136, 233498411, 0, 24843, 0, 0, 0, 0}
(gdb) n
101       return accumulate(cpu_stats.begin(), cpu_stats.end(), 0L);
(gdb) info locals
cpu_stats = std::vector of length 10, capacity 16 = {0, 0, 785136, 233498411, 0, 24843, 0, 0, 0, 0}
(gdb) n
102       }
after that first value turned to zero, the percent became >100 and the program would error
*/

vector<long> LinuxParser::CpuUtilization() { 
  string line, stat;
  vector<long> stats{};
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> stat) {
      if (std::all_of(stat.begin(), stat.end(), isdigit)) {
        long s = stol(stat);
        stats.push_back(s);
      }
    }
  }
  return stats; 
  }

int LinuxParser::TotalProcesses() { 
  string line, key;
  int value{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  }
  return value;
  }

int LinuxParser::RunningProcesses() { 
  string line, key;
  int value{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  }
  return value;
  }
  
string LinuxParser::Command(int pid) { 
    string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return line;
}


string LinuxParser::Ram(int pid) {
  string line, key, value{};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          //https://stackoverflow.com/questions/16605967
          std::ostringstream out;
          out.precision(1);
          out << std::fixed << stof(value) / 1024.0;
          return out.str();
        }
      }
    }
  }
  return value;
}

string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::User(int pid) { 
  string line, key, value{};
  string uid = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), 'x', ' ');
      std::istringstream linestream(line);
      linestream >> value >> key;
      if (key == uid)
        return value;
    }
  }
  return value; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }