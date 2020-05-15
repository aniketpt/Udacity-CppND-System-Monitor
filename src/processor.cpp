#include <istream>
#include <sstream>
#include <string>
#include <iostream>

#include "processor.h"
#include "linux_parser.h"

using std::string;

float Processor::Utilization() { 
    string cpu {};
    long int user {}, nice {}, system {}, idle {}, io_wait {}, irq {}, soft_irq {}, steal {}, guest {}, guest_nice {};
    string line;

    std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
    if(filestream.is_open()) {
        std::getline(filestream, line);
        std::istringstream linestream(line);
        linestream >> cpu >> user >> nice >> system >> idle >> io_wait >> irq >> soft_irq >> steal >> guest >> guest_nice;
        return (float)(idle + io_wait)/(user + nice + system + idle + io_wait + irq + soft_irq + steal + guest + guest_nice);
    }

    return 0.0; 
}
