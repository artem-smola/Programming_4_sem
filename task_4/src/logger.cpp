#include "logger.hpp"
#include <random>
#include <thread>
#include <chrono>
#include <format>
#include <fstream>
#include <atomic>

ConcurrentLogger::ConcurrentLogger(size_t n_threads, size_t queue_size, size_t exe_time) : n_threads_(n_threads),
                                                    queue_size_(queue_size), exe_time_(exe_time) {}

void ConcurrentLogger::LogAction1(std::queue<Log> &queue, std::mutex &queue_mtx, std::condition_variable &cv, std::atomic<bool> &stop_requested, int index){
    std::random_device rd;
    std::mt19937 gen(rd() + index);
    std::uniform_int_distribution<> dis(10, 100);
    while (!stop_requested){
        std::string log_msg = "Thread " + std::to_string(index) + " performed at ";
        auto now = std::chrono::system_clock::now();
        log_msg += std::format("{:%H:%M:%S}", now) + ";\n";
        Log log{log_msg};
        {                
            std::unique_lock lck(queue_mtx);
            queue.push(log);
            cv.notify_one();
        }
        int sleep_time = dis(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }
}

void ConcurrentLogger::LogAction2(std::queue<Log> &queue, std::mutex &queue_mtx, std::condition_variable &cv, std::atomic<bool> &stop_requested, int index){
    std::random_device rd;
    std::mt19937 gen(rd() + index);
    std::uniform_int_distribution<> dis(10, 100);
    while (!stop_requested){
        std::string log_msg = "Thread " + std::to_string(index) + " performed at ";
        auto now = std::chrono::system_clock::now();
        log_msg += std::format("{:%H:%M:%S}", now) + ";\n";
        Log log{log_msg};
        while(true){
            std::unique_lock lck(queue_mtx);
            if (queue.size() < queue_size_){
                queue.push(log);
                cv.notify_one();
                break;
            }
            lck.unlock();
            int wait_time = dis(gen);
            std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
        }
        int sleep_time = dis(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }
}

void ConcurrentLogger::LogAction3(std::queue<Log> &queue, std::mutex &queue_mtx, std::atomic<bool> &stop_requested, int index){
    std::random_device rd;
    std::mt19937 gen(rd() + index);
    std::uniform_int_distribution<> dis(10, 100);
    while (!stop_requested){
        std::string log_msg = "Thread " + std::to_string(index) + " performed at ";
        auto now = std::chrono::system_clock::now();
        log_msg += std::format("{:%H:%M:%S}", now) + ";\n";
        Log log{log_msg};
        {                
            std::unique_lock lck(queue_mtx);
            queue.push(log);
        }
        int sleep_time = dis(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }
}

void ConcurrentLogger::LogAction4(std::queue<Log> &queue, std::mutex &queue_mtx, std::atomic<bool> &stop_requested, int index){
    std::random_device rd;
    std::mt19937 gen(rd() + index);
    std::uniform_int_distribution<> dis(10, 100);
    while (!stop_requested){
        std::string log_msg = "Thread " + std::to_string(index) + " performed at ";
        auto now = std::chrono::system_clock::now();
        log_msg += std::format("{:%H:%M:%S}", now) + ";\n";
        Log log{log_msg};
        while(true){
            std::unique_lock lck(queue_mtx);
            if (queue.size() < queue_size_){
                queue.push(log);
                break;
            }
            lck.unlock();
            int wait_time = dis(gen);
            std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
        }
        int sleep_time = dis(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }
}

void ConcurrentLogger::UniversalExecuteCase1_2(LogFunction1_2 log_function, std::string path_to_txt){
    std::vector<std::thread> executive_threads;
    executive_threads.reserve(n_threads_);
    std::queue<Log> queue;
    std::mutex queue_mtx;
    std::condition_variable cv;
    std::chrono::milliseconds duration(exe_time_);
    std::atomic<bool> stop_requested{false};

    for (size_t i = 0; i < n_threads_; i++){
        executive_threads.emplace_back(log_function, std::ref(queue), std::ref(queue_mtx), std::ref(cv), std::ref(stop_requested), i);
    }
    std::ofstream file(path_to_txt);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла!" << std::endl;
        stop_requested = true;
        for (auto &th:executive_threads){
            th.join();
        }
        return;
    }
    auto start_time = std::chrono::steady_clock::now();
    while(true){
        auto current_time = std::chrono::steady_clock::now();
        auto past = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time);
        if (past >= duration){
            stop_requested = true;
        }
        std::unique_lock lck(queue_mtx);

        if (cv.wait_for(lck, std::chrono::milliseconds(1500), [&queue]{ return !queue.empty(); })){
            std::string new_log = queue.front().GetText();
            int queue_size = queue.size();
            queue.pop();
            lck.unlock();
            file << new_log << "(queue size = " << queue_size << ")\n";
        }
        else{
            lck.unlock();
            if (stop_requested.load()){
                break;
            }
        }
    }
    for (auto &th:executive_threads){
        th.join();
    }
    while (!queue.empty()){
        std::string new_log = queue.front().GetText();
        queue.pop();

        file << new_log;
    }
    file.close();
}

void ConcurrentLogger::UniversalExecuteCase3_4(LogFunction3_4 log_function, std::string path_to_txt){
    std::vector<std::thread> executive_threads;
    executive_threads.reserve(n_threads_);
    std::queue<Log> queue;
    std::mutex queue_mtx;
    std::chrono::milliseconds duration(exe_time_);
    std::atomic<bool> stop_requested{false};

    for (size_t i = 0; i < n_threads_; i++){
        executive_threads.emplace_back(log_function, std::ref(queue), std::ref(queue_mtx), std::ref(stop_requested), i);
    }
    std::ofstream file(path_to_txt);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла!" << std::endl;
        stop_requested = true;
        for (auto &th:executive_threads){
            th.join();
        }
        return;
    }
    auto start_time = std::chrono::steady_clock::now();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(10, 100);
    while(true){
        auto current_time = std::chrono::steady_clock::now();
        auto past = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time);
        if (past >= duration){
            stop_requested = true;
        }
        std::unique_lock lck(queue_mtx);
        if (!queue.empty()){
            std::string new_log = queue.front().GetText();
            int queue_size = queue.size();
            queue.pop();
            lck.unlock();
            int sleep_time = dis(gen);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
            file << new_log << "(queue size = " << queue_size << ")\n";
            continue;
        }
        lck.unlock();
        if (stop_requested){
            break;
        }
        
    }
    for (auto &th:executive_threads){
        th.join();
    }
    while (!queue.empty()){
        std::string new_log = queue.front().GetText();
        queue.pop();

        file << new_log;
    }
    file.close();
}

void ConcurrentLogger::ExecuteCase1(){
    std::string path_to_txt = "../log1.txt";
    UniversalExecuteCase1_2([this](std::queue<Log> &queue, 
                                  std::mutex &queue_mtx, 
                                  std::condition_variable &cv, 
                                  std::atomic<bool> &stop_requested, 
                                  int index){
        this->LogAction1(queue, queue_mtx, cv, stop_requested, index);
    }, path_to_txt);
}

void ConcurrentLogger::ExecuteCase2(){
    std::string path_to_txt = "../log2.txt";
    UniversalExecuteCase1_2([this](std::queue<Log> &queue, 
                                  std::mutex &queue_mtx, 
                                  std::condition_variable &cv, 
                                  std::atomic<bool> &stop_requested, 
                                  int index){
        this->LogAction2(queue, queue_mtx, cv, stop_requested, index);
    }, path_to_txt);
}

void ConcurrentLogger::ExecuteCase3(){
    std::string path_to_txt = "../log3.txt";
    UniversalExecuteCase3_4([this](std::queue<Log> &queue, 
                                  std::mutex &queue_mtx, 
                                  std::atomic<bool> &stop_requested, 
                                  int index){
        this->LogAction3(queue, queue_mtx, stop_requested, index);
    }, path_to_txt);
}

void ConcurrentLogger::ExecuteCase4(){
    std::string path_to_txt = "../log4.txt";
    UniversalExecuteCase3_4([this](std::queue<Log> &queue, 
                                  std::mutex &queue_mtx, 
                                  std::atomic<bool> &stop_requested, 
                                  int index){
        this->LogAction4(queue, queue_mtx, stop_requested, index);
    }, path_to_txt);
}