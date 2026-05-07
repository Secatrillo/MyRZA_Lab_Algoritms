#pragma once

#include <condition_variable>
#include <mutex>

/**
 * Барьер на конец одного шага симуляции для нескольких IED.
 * Гарантирует, что ни один поток не начнёт выборку i+1, пока все не
 * завершили выборку i (в т.ч. обмен PSCH за один и тот же t[i]).
 */
class IedStepBarrier {
public:
    explicit IedStepBarrier(unsigned parties) : parties_(parties) {}

    void arriveAndWait()
    {
        std::unique_lock<std::mutex> lk(m_);
        const unsigned g = generation_;
        if (++arrived_ == parties_) {
            arrived_ = 0;
            ++generation_;
            cv_.notify_all();
        } else {
            cv_.wait(lk, [&] { return generation_ != g; });
        }
    }

private:
    std::mutex m_;
    std::condition_variable cv_;
    unsigned arrived_{0};
    unsigned generation_{0};
    const unsigned parties_;
};
