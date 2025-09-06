#include "test.hpp"
#include <thread>
#include "random"

void TestSeqAddition1000RandomNums() {
    std::vector<std::thread> threads;
    threads.reserve(kNumThreads);
    ConcurrentHashTable table;
    for (int i = 0; i < kNumThreads; i++) {
        std::string str = "Thread " + std::to_string(i + 1);
        threads.emplace_back(&ConcurrentHashTable::SeqPut1000RandomNums, std::ref(table), str);
    }
    for (auto &th : threads) {
        th.join();
    }
    table.Show();
}

void TestSeqAdditionNumsFrom1To1000(){
    std::vector<std::thread> threads;
    threads.reserve(kNumThreads);
    ConcurrentHashTable table;
    for (int i = 0; i < kNumThreads; i++) {
        std::string str = "Thread " + std::to_string(i + 1);
        threads.emplace_back(&ConcurrentHashTable::SeqPutNumsFrom1To1000, std::ref(table), str);
    }
    for (auto &th : threads) {
        th.join();
    }
    table.Show();
}

void TestRandomAction(){
    std::vector<std::thread> threads;
    threads.reserve(kNumThreads);
    ConcurrentHashTable table;
    thread_local std::random_device rd;
    thread_local std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, M);
    for (int i = 0; i < kNumThreads; i++){
        int rand_num = dist(gen);
        threads.emplace_back(&ConcurrentHashTable::Random, std::ref(table), rand_num, i);
    }
    for (auto &th : threads) {
        th.join();
    }
    table.Show();
}
    