#ifndef PROFILER_H
#define PROFILER_H

#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include <stack>
#include <fstream>
#include <atomic>
#include <sstream>

class Profiler {
public:
    static void StartProfiling(const std::string& functionName, const std::string& params) {
        std::string fullFunctionName = functionName + "(" + params + ")";
        auto startTime = std::chrono::high_resolution_clock::now();
        timestamps[fullFunctionName].push(startTime);
        callDepth[fullFunctionName]++;
    }

    static void EndProfiling(const std::string& functionName, const std::string& params) {
        std::string fullFunctionName = functionName + "(" + params + ")";
        auto endTime = std::chrono::high_resolution_clock::now();
        if (timestamps[fullFunctionName].empty()) return;
        
        auto startTime = timestamps[fullFunctionName].top();
        timestamps[fullFunctionName].pop();
        
        auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        if (!paused) {
            results[fullFunctionName] += duration;
        }
        callDepth[fullFunctionName]--;
    }

    static void PauseProfiling() {
        pauseStart = std::chrono::high_resolution_clock::now();
        paused = true;
    }

    static void ResumeProfiling() {
        if (!paused) return;
        auto pauseEnd = std::chrono::high_resolution_clock::now();
        auto pauseDuration = std::chrono::duration<double, std::milli>(pauseEnd - pauseStart).count();
        for (auto& [functionName, time] : results) {
            if (callDepth[functionName] > 0) {
                time -= pauseDuration;
            }
        }
        paused = false;
    }

    static void PrintResults() {
        std::cout << "\nProfiling Results:\n";
        for (const auto& [functionName, time] : results) {
            std::cout << "Function: " << functionName << " | Time: " << time << " ms\n";
        }
        std::cout << "Total Allocated Memory: " << totalAllocated.load() << " bytes\n";
    }

    static void SaveResultsToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << " for writing.\n";
            return;
        }
        file << "Function,Time (ms)\n";
        for (const auto& [functionName, time] : results) {
            file << functionName << "," << time << "\n";
        }
        file << "Total Allocated Memory," << totalAllocated.load() << " bytes\n";
        file.close();
        std::cout << "Profiling results saved to " << filename << "\n";
    }

    static void AddAllocation(size_t size) {
        totalAllocated.fetch_add(size, std::memory_order_relaxed);
    }

    static void RemoveAllocation(size_t size) {
        totalAllocated.fetch_sub(size, std::memory_order_relaxed);
    }

private:
    static inline std::map<std::string, std::stack<std::chrono::high_resolution_clock::time_point>> timestamps;
    static inline std::map<std::string, double> results;
    static inline std::map<std::string, int> callDepth;
    static inline bool paused = false;
    static inline std::chrono::high_resolution_clock::time_point pauseStart;
    static inline std::atomic<size_t> totalAllocated = 0;
};

class ScopedProfiler {
public:
    explicit ScopedProfiler(const std::string& name, const std::string& params = "") : functionName(name), params(params) {
        Profiler::StartProfiling(functionName, params);
    }
    ~ScopedProfiler() {
        Profiler::EndProfiling(functionName, params);
    }
private:
    std::string functionName;
    std::string params;
};

void* operator new(size_t size) {
    Profiler::AddAllocation(size);
    return malloc(size);
}

void operator delete(void* ptr, size_t size) noexcept {
    Profiler::RemoveAllocation(size);
    free(ptr);
}

#define START_PROFILING(name, params) Profiler::StartProfiling(name, params)
#define END_PROFILING(name, params) Profiler::EndProfiling(name, params)
#define SCOPED_PROFILING(name, params) ScopedProfiler profiler##__LINE__(name, params)
#define PAUSE_PROFILING() Profiler::PauseProfiling()
#define RESUME_PROFILING() Profiler::ResumeProfiling()
#define SAVE_PROFILING_RESULTS(filename) Profiler::SaveResultsToFile(filename)

#endif // PROFILER_H
