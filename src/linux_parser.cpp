#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;


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
  string os, version_text, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version_text >> kernel;
  }
  return kernel;
}


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

// Reads and returns the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string placeholder, line;
  long total_memory {}, free_memory {};

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if(filestream.is_open()) {
    // Extracting Total Memory info
    std::getline(filestream, line);
    std::istringstream linestream_total(line);
    linestream_total >> placeholder >> total_memory;

    // Extracting Free Memory info
    std::getline(filestream, line);
    std::istringstream linestream_free(line);
    linestream_free >> placeholder >> free_memory;

    return (float)(free_memory) / total_memory;
  }  
  return 0.0;
}

// Reads and returns the system uptime
long LinuxParser::UpTime() { 
  long uptime {};
  string line {};

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return uptime;
  }
  return 0; 
}

// Reads and returns the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::UpTime() * sysconf(_SC_CLK_TCK);
}

// Reads and returns the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  long active_jiffies {};
  string line {}, token {};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    
    const std::vector<string> all_info {std::istream_iterator<string>{linestream}, std::istream_iterator<string>{}};
    for(int i=13; i<=16; i++) {
      active_jiffies += std::stol(all_info[i]);
    }
    // for(int i=0; i<17; i++) {
    //   linestream >> token;
    //   if(13 <= i && i <= 16) active_jiffies += std::stol(token);
    // }
    return active_jiffies;
  }

  return 0;
}

// Reads and returns the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  string line {}, value {};
  long active_jiffies {};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    const std::vector<string> all_info {std::istream_iterator<string>{linestream}, std::istream_iterator<string>{}};
    for(int i=1; i<=10; i++)
      active_jiffies += std::stol(all_info[i]);

    return active_jiffies;
  }

  return 0; 
}

// Reads and returns the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  string line {}, value {};
  long idle_jiffies {};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    const std::vector<string> all_info {std::istream_iterator<string>{linestream}, std::istream_iterator<string>{}};
    idle_jiffies = std::stol(all_info[5]) + std::stol(all_info[6]);

    return idle_jiffies;
  }

  return 0; 
}

// Reads and returns CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line {};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::vector<string> cpu_util_info{std::istream_iterator<string>{linestream}, std::istream_iterator<string>{}};
    cpu_util_info.erase(cpu_util_info.begin());
    return cpu_util_info;
  }
  return {};
}

// Reads and returns the total number of processes
int LinuxParser::TotalProcesses() { 
  string line {}, info_title {};
  int process_count {};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> info_title;
      if(info_title == "processes") {
        linestream >> process_count;
        return process_count;
      }
    }
  }
  return 0;
}

// Reads and returns the number of running processes
int LinuxParser::RunningProcesses() { 
  string line {}, info_title {};
  int running_processes {};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()) {
    while(std::getline(filestream ,line)) {
      std::istringstream linestream(line);
      linestream >> info_title;
      if(info_title == "procs_running") {
        linestream >> running_processes;
        return running_processes;
      }
    }
  }

  return 0; 
}

// Reads and returns the command associated with a process
string LinuxParser::Command(int pid) {
  string line {};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }

  return string();
}

// Reads and returns the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line {}, info_title {};
  long ram_kb {};
  long ram_mb {};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> info_title;
      if(info_title == "VmSize:") {
        linestream >> ram_kb;
        ram_mb = ram_kb / 1024;
        return std::to_string(ram_mb);
      }
    }
  }

  return string(); 
}

// Reads and returns the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, info_title {}, uid {};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> info_title;
      if(info_title == "Uid:") {
        linestream >> uid;
        return uid;
      }
    }
  }

  return string();
}

// Reads and returns the user associated with a process
string LinuxParser::User(int pid) { 
  string line {}, username {}, x {}, user_id {};
  std::ifstream filestream(LinuxParser::kPasswordPath);
  if(filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> username >> x >> user_id;
      if(user_id == LinuxParser::Uid(pid))
        return username;
    }
  }

  return string();
}

// Reads and returns the uptime of a process
long int LinuxParser::UpTime(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::vector<string> data{std::istream_iterator<string>{linestream}, std::istream_iterator<string>{}};
    return LinuxParser::UpTime() - std::stol(data[21])/sysconf(_SC_CLK_TCK);
  }

  return 0;
}

std::vector<long int> LinuxParser::ProcessInfo(int pid) {
  std::vector<long int> process_info = {};
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    const std::vector<string> all_info {std::istream_iterator<string>{linestream}, std::istream_iterator<string>{}};
    process_info.push_back(std::stol(all_info[14]));
    process_info.push_back(std::stol(all_info[13]));
    process_info.push_back(std::stol(all_info[15]));
    process_info.push_back(std::stol(all_info[16]));
    process_info.push_back(std::stol(all_info[21]));
    return process_info;
  }
  return {};
}