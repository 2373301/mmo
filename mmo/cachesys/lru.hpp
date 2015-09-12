#pragma  once
#include "typedef.hpp"
#include "list.hpp"

class lru
    :boost::noncopyable
{
public:
    lru()
        :max_size_(0)
    {}

    lru(uint64_t max_size)
        :max_size_(max_size)
    {
    }

    void set_size(uint64_t size)
    {
        max_size_ = size;
    }

    // 返回true 如果有过期的key
    bool update(uint64_t key, uint64_t& removed_key)
    {   
        // 已存在
        auto it = key2node_.find(key);
        if( it != key2node_.end())
        {
            update_list_.Erase(it->second);
            update_list_.PushBack(it->second);
            return false;
        }
       
        // new, not full
        if(max_size_  >  key2node_.size())
        {
            key_node *node = new key_node;
            node->key = key;
            update_list_.PushBack(node);
            key2node_.insert(std::make_pair(key, node));
            return false;
        }

        // new , full
        key_node* old = update_list_.PopFront();
        removed_key = old->key;
        old->key = key;
        update_list_.PushBack(old);
        return true;
    }

private:
    struct key_node
    {
        uint64_t key;
        key_node* pre;
        key_node* next;
    };

    uint64_t max_size_;
    typedef boost::unordered_map<uint64_t, key_node* > keys_map_t;
    keys_map_t key2node_;
    delist<key_node> update_list_;
};