#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/utsname.h>

#include <unistd.h>
#include <sys/vfs.h>

// Read /proc file helper
std::string readProcFile(const std::string& path) {
    std::ifstream file(path);
    std::string content;
    if (file.is_open()) {
        std::getline(file, content);
        file.close();
    }
    return content;
}

// Get CPU usage
double getCpuUsage() {
    static long prev_Idle = 0, prev_total = 0;
    
    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line);
    file.close();
    
    std::istringstream iss(line);
    std::string cpu;
    long user, nice, system, idle, iowait, irq, softirq;
    iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
    
    long idle_time = idle + iowait;
    long total_Time = user + nice + system + idle + iowait + irq + softirq;
    
    long delta_idle = idle_time - prev_Idle;
    long delta_total = total_Time - prev_total;
    
    prev_Idle = idle_time;
    prev_total = total_Time;
    
    if (delta_total == 0) return 0.0;
    return 100.0 * (1.0 - (double)delta_idle / delta_total);
}

// Get memory info
void getMemoryInfo(long& total, long& available, long& used) {
    std::ifstream file("/proc/meminfo");
    std::string line;
    total = available = used = 0;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        long value;
        iss >> key >> value;
        
        if (key == "MemTotal:") total = value;
        else if (key == "MemAvailable:") available = value;
    }
    file.close();
    
    used = total - available;
}

// Get load average
void getLoadAvg(double& load1, double& load5, double& load15) {
    std::ifstream file("/proc/loadavg");
    file >> load1 >> load5 >> load15;
    file.close();
}

// Get disk info
void getDiskInfo(std::string& mount, long& total, long& free) {
    std::ifstream file("/proc/mounts");
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.find(" / ") != std::string::npos && line.find("/dev/") != std::string::npos) {
            std::istringstream iss(line);
            iss >> mount;
            struct statfs fs;
            if (statfs(mount.c_str(), &fs) == 0) {
                total = fs.f_blocks * (fs.f_bsize / 10);
                free = fs.f_bavail * (fs.f_bsize / 10);
            }
            break;
        }
    }
    file.close();
}

// Get process count
int getProcessCount() {
    std::ifstream file("/proc/stat");
    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, 7) == "process") {
            std::istringstream iss(line);
            std::string key;
            int value;
            iss >> key >> value;
            return value;
        }
    }
    file.close();
    return 0;
}

// Get uptime
std::string getUptime() {
    std::ifstream file("/proc/uptime");
    double uptime;
    file >> uptime;
    file.close();
    
    int days = (int)(uptime / 86400);
    int hours = (int)((uptime - days * 86400) / 3600);
    int mins = (int)((uptime - days * 86400 - hours * 3600) / 60);
    
    std::ostringstream oss;
    oss << days << "d " << hours << "h " << mins << "m";
    return oss.str();
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "       System Performance Report       " << std::endl;
    std::cout << "========================================" << std::endl;
    
    // System info
    struct utsname un;
    uname(&un);
    std::cout << "\n[System]" << std::endl;
    std::cout << "  Hostname: " << un.nodename << std::endl;
    std::cout << "  OS: " << un.sysname << " " << un.release << std::endl;
    std::cout << "  Kernel: " << un.version << std::endl;
    std::cout << "  Arch: " << un.machine << std::endl;
    
    // Uptime
    std::cout << "\n[Uptime]" << std::endl;
    std::cout << "  " << getUptime() << std::endl;
    
    // CPU info
    std::cout << "\n[CPU]" << std::endl;
    std::cout << "  Usage: " << getCpuUsage() << "%" << std::endl;
    std::ifstream cpuFile("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuFile, line)) {
        if (line.substr(0, 10) == "model name") {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                std::cout << "  Model:" << line.substr(pos + 2) << std::endl;
                break;
            }
        }
    }
    cpuFile.close();
    
    // Load
    double load1, load5, load15;
    getLoadAvg(load1, load5, load15);
    std::cout << "\n[Load Average]" << std::endl;
    std::cout << "  1min:  " << load1 << std::endl;
    std::cout << "  5min:  " << load5 << std::endl;
    std::cout << "  15min: " << load15 << std::endl;
    
    // Memory
    long memTotal, memAvail, memUsed;
    getMemoryInfo(memTotal, memAvail, memUsed);
    std::cout << "\n[Memory]" << std::endl;
    std::cout << "  Total:    " << memTotal / 1024 << " MB" << std::endl;
    std::cout << "  Available:" << memAvail / 1024 << " MB" << std::endl;
    std::cout << "  Used:     " << memUsed / 1024 << " MB" << std::endl;
    std::cout << "  Usage:    " << (memUsed * 100.0 / memTotal) << "%" << std::endl;
    
    // Disk
    std::string mount;
    long diskTotal, diskFree;
    getDiskInfo(mount, diskTotal, diskFree);
    std::cout << "\n[Disk]" << std::endl;
    std::cout << "  Mount: " << mount << std::endl;
    std::cout << "  Total: " << diskTotal / 1024 / 1024 << " GB" << std::endl;
    std::cout << "  Free:  " << diskFree / 1024 / 1024 << " GB" << std::endl;
    std::cout << "  Usage: " << ((diskTotal - diskFree) * 100.0 / diskTotal) << "%" << std::endl;
    
    // Processes
    std::cout << "\n[Processes]" << std::endl;
    std::cout << "  Total: " << getProcessCount() << std::endl;
    
    std::cout << "\n========================================" << std::endl;
    
    return 0;
}
