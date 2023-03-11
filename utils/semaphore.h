#include <condition_variable>
#include <mutex>

class Semaphore {
    private:
        std::mutex mtx;
        std::condition_variable cv;
        int count = 0;

    public:
        Semaphore(int count_ = 0) : count(count_) {}

        inline void signal() {
            std::unique_lock<std::mutex> lock(mtx);
            count++;
            cv.notify_one();
        }

        inline void wait() {
            std::unique_lock<std::mutex> lock(mtx);
            while(count == 0) {
                cv.wait(lock);
            }
            count--;
        }

        // Assignment
        Semaphore& operator=(const Semaphore& other) {
            count = other.count;
            return *this;
        }

};
