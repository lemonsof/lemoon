#include <cassert>
#include <iostream>
#include <lemon/timewheel.hpp>


namespace lemon {

    timewheel_t::timewheel_t(size_t millisecondsOfTick)
        :_millisecondsOfTick(millisecondsOfTick)
    {

    }

    timewheel_t::~timewheel_t() {

    }


    void timewheel_t::create_timer(timeout_t target, std::chrono::milliseconds timeout) {
        timer * t = new_timer(target, timeout);

        insert(t);
    }

    timewheel_t::timer * timewheel_t::new_timer(timeout_t target, std::chrono::milliseconds timeout) {
        size_t counter = timeout.count() / _millisecondsOfTick + 1;

        return new timer(target, counter);
    }

    void timewheel_t::free_timer(timewheel_t::timer * val) {
        delete val;
    }

    void timewheel_t::insert(timewheel_t::timer * t) {
        size_t buckets;

        cascade * cas;

        uint32_t tick = t->ticks;

        assert(t->next == NULL && t->prev == NULL);

        if (0 == tick) {

            t->target();

            free_timer(t);

            _timers--;

            return;
        }

        if ((tick >> 24) & 0xff) {
            buckets = ((tick >> 24) & 0xff);

            cas = &_cascades[3];

        }
        else if ((tick >> 16) & 0xff) {
            buckets = ((tick >> 16) & 0xff);

            cas = &_cascades[2];
        }
        else if ((tick >> 8) & 0xff) {
            buckets = ((tick >> 8) & 0xff);

            cas = &_cascades[1];
        }
        else {
            buckets = tick;

            cas = &_cascades[0];
        }

        buckets = (buckets + cas->cursor - 1) % 256;

        t->next = cas->buckets[buckets];

        t->prev = &cas->buckets[buckets];

        if (cas->buckets[buckets]) {

            assert(cas->buckets[buckets]->prev == &cas->buckets[buckets]);

            cas->buckets[buckets]->prev = &t->next;
        }

        cas->buckets[buckets] = t;
    }

    void timewheel_t::__cascade(size_t index) {
        cascade *cas = &_cascades[index];

        if (cas->cursor != 0 || index == 3) return;

        cascade *cascade_upper = &_cascades[++index];

        timer *timers = cascade_upper->buckets[cascade_upper->cursor];

        cascade_upper->buckets[cascade_upper->cursor++] = NULL;

        cascade_upper->cursor %= 256;

        __cascade(index);

        while (timers) {

            timer *next = timers->next;


            switch (index) {
            case 1:
            {
                timers->ticks &= 0xff;

                break;
            }
            case 2:
            {
                timers->ticks &= 0xffff;

                break;
            }
            case 3:
            {
                timers->ticks &= 0xffffff;

                break;
            }
            default:
            {
                assert(false && "not here");
            }
            }

            timers->next = NULL;
            timers->prev = NULL;

            insert(timers);

            timers = next;

        };
    }

    void timewheel_t::tick() {

        cascade *cas = &_cascades[0];


        timer * timers = cas->buckets[cas->cursor];

        cas->buckets[cas->cursor] = NULL;

        ++cas->cursor;

        cas->cursor %= 256;

        __cascade(0);

        while (timers) {

            _timers--;

            timers->target();

            timer * next = timers->next;

            timers->next = NULL;
            timers->prev = NULL;

            free_timer(timers);

            timers = next;
        }
    }

}

