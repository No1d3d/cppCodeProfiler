#ifndef PROFILER_H
#define PROFILER_H

#include <chrono>
#include <iostream>
#include <map>
#include <string>

class Profiler {
public:
    static void StartProfiling(const std::string& functionName) {
        auto startTime = std::chrono::high_resolution_clock::now();
        timestamps[functionName] = startTime;
    }

    static void EndProfiling(const std::string& functionName) {
        auto endTime = std::chrono::high_resolution_clock::now();
        if (timestamps.find(functionName) != timestamps.end()) {
            auto duration = std::chrono::duration<double, std::milli>(endTime - timestamps[functionName]).count();
            results[functionName] += duration;
            timestamps.erase(functionName);
        }
    }

    static void PrintResults() {
        std::cout << "\nProfiling Results:\n";
        for (const auto& [functionName, time] : results) {
            std::cout << "Function: " << functionName << " | Time: " << time << " ms\n";
        }
    }

private:
    static inline std::map<std::string, std::chrono::high_resolution_clock::time_point> timestamps;
    static inline std::map<std::string, double> results;
};

#define START_PROFILING(name) Profiler::StartProfiling(name)
#define END_PROFILING(name) Profiler::EndProfiling(name)

#endif // PROFILER_H
