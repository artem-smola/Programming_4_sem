#pragma once
#include <iostream>
#include <vector>
#include <utility>
#include <mutex>
#include <thread>

using ValueType = std::pair<std::string, int>;
using KeyValueType = std::pair<int, ValueType>;

constexpr int kSize = 10;
constexpr int kNumIterations = 50;
constexpr int kNumThreads = 200;
constexpr int M = 1000;


struct Node {
    Node(KeyValueType val) : val_(val), next_(nullptr) {}
    KeyValueType val_;
    Node *next_;
    std::mutex mtx_;
};
using Container = std::vector<Node*>;

class ConcurrentHashTable {
public:
    ConcurrentHashTable();
    ~ConcurrentHashTable();
    void Clear();
    int GetHash(int key);

    void Put(KeyValueType value);
    bool Remove(int key, ValueType *value = nullptr);
    bool Check(int key, ValueType *value = nullptr);

    void SeqPut1000RandomNums(std::string str);
    void SeqPutNumsFrom1To1000(std::string str);
    void Show();
    void Random(int key, int num_of_thread);
private:
    Container container_;
    std::vector<std::mutex> head_mutexes_;
};