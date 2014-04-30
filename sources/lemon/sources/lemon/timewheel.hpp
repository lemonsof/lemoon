/**
 * 
 * @file     timewheel
 * @brief    Copyright (C) 2014  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2014/02/08
 */
#ifndef HELIX_TIMEWHEEL_HPP
#define HELIX_TIMEWHEEL_HPP
#include <mutex>
#include <chrono>
#include <thread>
#include <stdint.h>
#include <chrono>
#include <functional>
#include <algorithm>
#include <lemon/nocopyable.hpp>
namespace lemon {

    class timewheel_t : private nocopyable
    {
    public:
        typedef std::function<void()> timeout_t;
        static uintptr_t event() {
            static const uintptr_t unknown = 0;

            return (uintptr_t)&unknown;
        }

        struct timer : private nocopyable {

            timer(timeout_t tt, size_t tk)
            : next(nullptr)
            , prev(nullptr)
            , target(tt)
            , ticks((uint32_t) tk) {

            }

            timer *next;

            timer **prev;

            timeout_t target;

            uint32_t ticks;
        };

        struct cascade {

            cascade() :cursor(0){
                std::fill(std::begin(buckets), std::end(buckets), nullptr);
            }

            uint32_t cursor;

            timer *buckets[256];
        };

        timewheel_t(size_t millisecondsOfTick);

        ~timewheel_t();

    public:
        
        void tick();

        void create_timer(timeout_t target, std::chrono::milliseconds timeout);

    private:

        timer * new_timer(timeout_t target, std::chrono::milliseconds timeout);

        void free_timer(timer * val);

        void proc();

        void insert(timer * val);

    

        void __cascade(size_t index);

    private:
        
        size_t _millisecondsOfTick;

        size_t _timers;

        cascade _cascades[4];
    };
}

#endif //