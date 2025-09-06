#include "concurrent_hash_table.hpp"
#include "random"

ConcurrentHashTable::ConcurrentHashTable() 
    : container_(kSize, nullptr), head_mutexes_(kSize) {}

ConcurrentHashTable::~ConcurrentHashTable() {
    Clear();
}

void ConcurrentHashTable::Clear() {
    for (int i = 0; i < kSize; i++) {
        Node* curr = container_[i];
        while (curr != nullptr) {
            Node* to_delete = curr;
            curr = curr->next_;
            delete to_delete;
        }
        container_[i] = nullptr;
    }
}

int ConcurrentHashTable::GetHash(int key){
    return key % kSize;
}

void ConcurrentHashTable::Put(KeyValueType value){
    int hash = GetHash(value.first);
    std::unique_lock head_lock(head_mutexes_[hash]);
    if (container_[hash] == nullptr){
        container_[hash] = new Node(value);
        return;
    }
    Node *curr = container_[hash];
    if (curr->val_.first == value.first){
        curr->val_.second = value.second;
        return;
    }
    if (curr->next_ == nullptr){
        curr->next_ = new Node(value);
        return;
    }
    std::unique_lock curr_lock(curr->next_->mtx_);
    curr = curr->next_;
    head_lock.unlock();
    while(true){
        if (curr->val_.first == value.first){
            curr->val_.second = value.second;
            return;
        }
        if (curr->next_ == nullptr){
            break;
        }
        std::unique_lock new_lock(curr->next_->mtx_);
        curr = curr->next_;
        curr_lock.unlock();
        curr_lock = std::move(new_lock);
    }
    curr->next_ = new Node(value);
}

bool ConcurrentHashTable::Remove(int key, ValueType *value){
    int hash = GetHash(key);
    std::unique_lock prev_lock(head_mutexes_[hash]);
    if(container_[hash] == nullptr){
        return false;
    }
    Node *prev = container_[hash];
    if (prev->val_.first == key){
        std::unique_lock<std::mutex> lck2;
        if (prev->next_ != nullptr){
            lck2 = std::move(std::unique_lock(prev->next_->mtx_));
        }
        if (value != nullptr){
            *value = prev->val_.second;
        }
        Node *next = prev->next_;
        delete prev;
        container_[hash] = next;
        return true;
    }
    if (prev->next_ == nullptr){
        return false;
    }
    std::unique_lock curr_lock(prev->next_->mtx_);
    Node *curr = prev->next_;
    while(true){
        if (curr->val_.first == key){
            if (value != nullptr){
                *value = curr->val_.second;
            }
            prev->next_ = curr->next_;
            delete curr;
            return true;
        }
        if (curr->next_ == nullptr){
            return false;
        }
        prev = curr;
        prev_lock.unlock();
        prev_lock = std::move(curr_lock);
        curr_lock = std::unique_lock(curr->next_->mtx_);
        curr = curr->next_;
    }
    return false;
}

bool ConcurrentHashTable::Check(int key, ValueType *value){
    int hash = GetHash(key);
    std::unique_lock head_lock(head_mutexes_[hash]);
    if (container_[hash] == nullptr){
        return false;
    }
    Node *curr = container_[hash];
    if (curr->val_.first == key){
        if (value != nullptr){
            *value = curr->val_.second;
        }
        return true;
    }
    if (curr->next_ == nullptr){
        return false;
    }
    std::unique_lock curr_lock(curr->next_->mtx_);
    curr = curr->next_;
    head_lock.unlock();
    while(true){
        if (curr->val_.first == key){
            if (value != nullptr){
                *value = curr->val_.second;
            }
            return true;
        }
        if (curr->next_ == nullptr){
            return false;
        }
        std::unique_lock new_lock(curr->next_->mtx_);
        curr = curr->next_;
        curr_lock.unlock();
        curr_lock = std::move(new_lock);
    }
    return false;
}

void ConcurrentHashTable::SeqPut1000RandomNums(std::string str) {
    thread_local std::random_device rd;
    thread_local std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, kNumIterations - 1);

    for (int i = 0; i < kNumIterations; i++) {
        int key = dist(gen);
        ValueType value{str, i};
        KeyValueType key_value{key, value};
        Put(key_value);
    }
}

void ConcurrentHashTable::SeqPutNumsFrom1To1000(std::string str){
    for (int i = 0; i < kNumIterations; i++){
        ValueType value{str, i};
        KeyValueType key_value{i, value};
        Put(key_value);
    }
}

void ConcurrentHashTable::Random(int key, int num_of_thread){
    thread_local std::random_device rd;
    thread_local std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 2);
    int rand_num = dist(gen);
    switch(rand_num){
        case 0:{
            ValueType value("", key);
            KeyValueType key_value{key, value};
            Put(key_value);
            break;
        }
        case 1:{
            ValueType val;
            bool status = Remove(key, &val);
            break;
        }
        case 2:{
            ValueType val;
            bool status = Check(key, &val);
            break;
        }
    }
}

void ConcurrentHashTable::Show(){
    for (int i = 0; i < kSize; i++){
        if (container_[i] == nullptr){
            continue;
        }
        std::cout << "Chain " << i << ": ";
        Node *curr = container_[i];
        while(true){
            std::cout << "Key = " << curr->val_.first << ", ";
            std::cout << "Value = " << "(" << curr->val_.second.first << ", " << curr->val_.second.second << "); ";
            if (curr->next_ == nullptr){
                break;
            }
            curr = curr->next_;
        }
        std::cout << std::endl;
    }
}