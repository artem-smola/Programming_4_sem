#pragma once
#include "log.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

using LogFunction1_2 = std::function<void(std::queue<Log>&, std::mutex&, std::condition_variable&, std::atomic<bool>&, int)>;
using LogFunction3_4 = std::function<void(std::queue<Log>&, std::mutex&,  std::atomic<bool>&, int)>;

class ConcurrentLogger{
public:
    ConcurrentLogger(size_t n_threads, size_t queue_size, size_t exe_time);

    void ExecuteCase1();
    void ExecuteCase2();
    void ExecuteCase3();
    void ExecuteCase4();
private:
    size_t n_threads_;
    size_t queue_size_;
    size_t exe_time_;
    void LogAction1(std::queue<Log> &queue, std::mutex &queue_mtx, std::condition_variable &cv, std::atomic<bool> &stop_requested, int index);
    void LogAction2(std::queue<Log> &queue, std::mutex &queue_mtx, std::condition_variable &cv, std::atomic<bool> &stop_requested, int index);
    void LogAction3(std::queue<Log> &queue, std::mutex &queue_mtx, std::atomic<bool> &stop_requested, int index);
    void LogAction4(std::queue<Log> &queue, std::mutex &queue_mtx, std::atomic<bool> &stop_requested, int index);

    void UniversalExecuteCase1_2(LogFunction1_2, std::string path_to_txt);
    void UniversalExecuteCase3_4(LogFunction3_4, std::string path_to_txt);
};