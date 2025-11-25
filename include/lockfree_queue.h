//其中信号量采用linux的semaphore.h，如果希望在windows上使用，需要进行一定的修改
#pragma once
#include <atomic>
#include <vector>
#include <chrono>
#include <semaphore.h>

template <typename T>
class lockfree_queue {

struct element{
    std::atomic<bool> exist;
    T data;
};

public:
    lockfree_queue(size_t __capacity = 4096) : m_capacity(__capacity),
                               m_read(0),
                               m_write(0),
                               m_data(m_capacity) {
                                    sem_init(&m_sem_full, 0, m_capacity);
                                    sem_init(&m_sem_empty, 0, 0);
                               }

    ~lockfree_queue(){
        sem_destroy(&m_sem_full);
        sem_destroy(&m_sem_empty);
    }

    /**
     * @brief 弹出数据
     * @param __item 对应的数据
     * @param __block true-当无法弹出时自旋等待直到可以插入 false-无法弹出时返回失败
     * @return true-弹出成功 false-弹出失败
     */
    bool pop(T& __item, bool __block = false) {
        return pop_impl(
            [&__item](T&& data) { __item = std::move(data); },
            __block
        );
    }

    /**
     * @brief 弹出数据
     * @param func 获取数据的函数
     * @param __block true-当无法弹出时自旋等待直到可以插入 false-无法弹出时返回失败(默认)
     * @return true-弹出成功 false-弹出失败
     */
    template <typename Func>
    bool pop(Func&& func, bool __block = false) {
        return pop_impl(std::forward<Func>(func), __block);
    }

    /**
     * @brief 插入数据
     * @param __item 对应的数据
     * @param __block true-当无法插入时自旋等待直到可以插入 false-无法插入时返回失败(默认)
     * @return true-插入成功 false-插入失败
     */
    bool push(const T& __item, bool __block = false) { return push_impl(__item,__block); }

    /**
     * @brief 插入数据
     * @param __item 对应的数据
     * @param __block true-当无法插入时自旋等待直到可以插入 false-无法插入时返回失败
     * @return true-插入成功 false-插入失败
     */
    bool push(T&& __item, bool __block = false) { return push_impl(std::move(__item),__block); }

    /**
     * @return true-队列空 false-队列非空 
     */
    bool empty() {
        return size() == 0;
    }

    /**
     * @return true-队列满 false-队列非满 
     */
    bool full() {
		int val = 0;
		sem_getvalue(&m_sem_full, &val);
        return val == 0;
    }

    int size(){
		int val = 0;
		sem_getvalue(&m_sem_empty, &val);
        return val;
    }

private:

    /**
     * @brief 获取对应队列元素的idx
     * @param __idx 未处理的idx
     * @return 处理后对应队列元素的idx
     */
    size_t get_idx(const std::size_t& __idx) const {
        return __idx % m_capacity;
    }

    /**
     * @brief 弹出对应数据
     * @tparam Func 对应函数类型
     * @param __func 处理函数
     * @param __block true-弹出直到成功 false-可能弹出失败 不建议混用，否则可能导致出现空跑
     * @return true-弹出成功 false-弹出失败(暂不存在)
     */
    template <typename Func>
    bool pop_impl(Func&& __func,bool __block) {
        size_t r_idx = m_read.load(std::memory_order_relaxed);
        if(__block){
            sem_wait(&m_sem_empty);
            do{
                // 如果对应读点没有数据，获取新读点
                for(;m_data[r_idx].exist.load(std::memory_order_relaxed) == false;// 当前读点不可用
                r_idx = m_read.load(std::memory_order_relaxed));
            }while(!m_read.compare_exchange_strong(r_idx,get_idx(r_idx+1),
            std::memory_order_relaxed,std::memory_order_relaxed));// 当前持有读点是否符合预期，符合预期分配读点,否则重新检查
        } else {
            do{
                // 如果对应读点没有数据，获取新读点
                if(m_data[r_idx].exist.load(std::memory_order_relaxed) == false){ return false;}
            }while(!m_read.compare_exchange_strong(r_idx,get_idx(r_idx+1),
            std::memory_order_relaxed,std::memory_order_relaxed));// 当前持有读点是否符合预期，符合预期分配读点,否则重新检查
            sem_trywait(&m_sem_empty);
        }
        auto& _element =  m_data[r_idx];
        __func(std::move(_element.data));
        _element.exist.store(false,std::memory_order_relaxed);
        sem_post(&m_sem_full);
        return true;
    }


    /**
     * @brief 插入数据实际处理
     * @tparam Item 对应的数据类型
     * @param __item 对应的数据
     * @param block true-插入直到成功 false-可能插入失败 不建议混用，否则可能导致出现空跑高
     * @return true-插入成功 false-插入失败(暂不存在)
     */
    template <typename Item>
    bool push_impl(Item&& __item,bool __block) {
        size_t w_idx = m_write.load(std::memory_order_relaxed);
        if(__block){// 阻塞
            sem_wait(&m_sem_full);
            do{
                // 如果对应读点没有数据，获取新读点
                for(;m_data[w_idx].exist.load(std::memory_order_relaxed) == true;// 当前读点不可用
                w_idx = m_write.load(std::memory_order_relaxed));
            }while(!m_write.compare_exchange_strong(w_idx,get_idx(w_idx+1),
            std::memory_order_relaxed,std::memory_order_relaxed));
        } else {// 非阻塞
            do{
                if(m_data[w_idx].exist.load(std::memory_order_relaxed) == true){return false;}
            }while(!m_write.compare_exchange_strong(w_idx,get_idx(w_idx+1),
            std::memory_order_relaxed,std::memory_order_relaxed));
            sem_trywait(&m_sem_full);// 非阻塞信号量去除靠后，建议不要和阻塞混用，否则突然空跑后者可能会大量消耗cpu
        }
        auto& _element =  m_data[w_idx];
        _element.data = std::forward<Item>(__item);
        _element.exist.store(true,std::memory_order_relaxed);
        sem_post(&m_sem_empty);
        return true;
    }

    size_t m_capacity;        // 队列容量
    std::atomic<size_t> m_read;// 将分配的读点
    std::atomic<size_t> m_write; // 将分配的写点
	std::vector<element> m_data;// 缓存区
    sem_t m_sem_full;
    sem_t m_sem_empty;
};