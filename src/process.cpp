#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid), cpuUsage_(CpuUtilization()) {}

// Returns this process's ID
int Process::Pid() { return this->pid_; }

// Returns this process's CPU utilization
float Process::CpuUtilization() {
    const std::vector<long int> processInfo = LinuxParser::ProcessInfo(pid_);
    long int total_time = processInfo[processInfoElem::utime_] + processInfo[processInfoElem::stime_];
    total_time += processInfo[processInfoElem::cutime_] + processInfo[processInfoElem::cstime_];
    const float seconds = (float) Process::UpTime();
    const float cpuUsage = ((total_time / sysconf(_SC_CLK_TCK)) / seconds);
    return cpuUsage; 
}

// Returns the command that generated this process
string Process::Command() {
    if (this->command_.length() == 0)
        this->command_ = LinuxParser::Command(this->pid_);
    return this->command_; 
}

// Returns this process's memory utilization
string Process::Ram() {
    return LinuxParser::Ram(this->pid_);
}

// Returns the user (name) that generated this process
string Process::User() {
    if (user_.length() == 0)
        user_ = LinuxParser::User(this->pid_);
    return user_; 
}

// Returns the age of this process (in seconds)
long int Process::UpTime() {
    return LinuxParser::UpTime(this->pid_); 
}


bool Process::operator<(Process const& a) const {
    return (cpuUsage_ > a.cpuUsage_);
}

bool Process::operator>(Process const& a) const {
    return (cpuUsage_ < a.cpuUsage_);
}