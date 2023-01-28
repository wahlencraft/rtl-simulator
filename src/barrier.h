#ifndef BARRIER_H_
#define BARRIER_H_

#include <mutex>
#include <condition_variable>

class Barrier {
public:
    Barrier(unsigned max_count): max_count{max_count} {}
    void arrive() {
        {
            std::unique_lock lock(mtx);
            count = (count + 1) % max_count;
        }
        cv.notify_all();
    }
    void arrive_and_wait() {
        {
            std::unique_lock lock(mtx);
            count = (count + 1) % max_count;
            cv.wait(lock, [this]() { return count == 0; });
        }
        cv.notify_all();
    }

private:
    unsigned const max_count;
    unsigned count = 0;
    std::mutex mtx{};
    std::condition_variable cv{};
};

#endif  // BARRIER_H_

