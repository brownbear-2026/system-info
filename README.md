# System Info

Linux C++ program to collect and display system performance metrics.

## Features
- System information (hostname, OS, kernel, arch)
- CPU usage and model
- Load average (1/5/15 min)
- Memory (total, available, used)
- Disk usage
- Process count
- Uptime

## Build
```bash
g++ -std=c++17 -O2 -o system_info system_info.cpp
```

## Run
```bash
./system_info
```
