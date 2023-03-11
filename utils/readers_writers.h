#include "semaphore.h"


// Multiple writers can write at the same time, but if there are writers, readers must wait.
// Multiple readers can read at the same time, but if there are readers, writers must wait.
// This is a class that implements this functionality, and ensures that readers and writers are fair (i.e. no starvation).
// This is also known as the unisex bathroom problem.
class FairReadersWriters {

    private:
        std::shared_ptr<Semaphore> readSem_;  // Readers wait on this semaphore.
        std::shared_ptr<Semaphore> writeSem_;  // Writers wait on this semaphore.
        int numReadersInQueue_;  // Number of readers waiting to read.
        int numWritersInQueue_;  // Number of writers waiting to write.
        std::mutex varsLock_;  // Lock for the variables below.
        int numReadersInUse_;  // Number of readers currently reading.
        int numWritersInUse_;  // Number of writers currently writing.

    public:
        inline FairReadersWriters() {
            numReadersInQueue_ = 0;
            numWritersInQueue_ = 0;
            numReadersInUse_ = 0;
            numWritersInUse_ = 0;
            readSem_ = std::make_shared<Semaphore>(0);
            writeSem_ = std::make_shared<Semaphore>(0);
        }
        inline void startRead() {
            // printf("startRead\n");
            varsLock_.lock();
            if (numWritersInQueue_ == 0 && numWritersInUse_ == 0) {
                // printf("Incrementing numReadersInUse_ from %d to %d\n", numReadersInUse_, numReadersInUse_+1);
                numReadersInUse_++; varsLock_.unlock();  // No writers, so we can start reading.
            } else {
                // printf("readerNidQueue\n");
                // printf("Incrementing numReadersInQueue_ from %d to %d\n", numReadersInQueue_, numReadersInQueue_+1);
                numReadersInQueue_++; varsLock_.unlock();  // There are writers, so we must wait.
                readSem_->wait();  // Wait until we can read. Last writer will signal all readers waiting here.
            }
        }
        inline void endRead() {
            // printf("endRead\n");
            varsLock_.lock();
            // printf("Decrementing numReadersInUse_ from %d to %d\n", numReadersInUse_, numReadersInUse_-1);
            if (--numReadersInUse_ == 0) {  // If we were the last reader, then we can signal all waiting writers.
                while (numWritersInQueue_ > 0) {  // For each writer waiting, signal them.
                    // printf("Decrementing numWritersInQueue_ from %d to %d\n", numWritersInQueue_, numWritersInQueue_-1);
                    // printf("Incrementing numWritersInUse_ from %d to %d\n", numWritersInUse_, numWritersInUse_+1);
                    writeSem_->signal(); numWritersInQueue_--; numWritersInUse_++;
                }
            }
            varsLock_.unlock();
        }
        inline void startWrite() {
            // printf("startWrite\n");
            varsLock_.lock();
            if (numReadersInQueue_ == 0 && numReadersInUse_ == 0) { 
                // printf("Incrementing numWritersInUse_ from %d to %d\n", numWritersInUse_, numWritersInUse_+1);
                numWritersInUse_++; varsLock_.unlock();
            } else { 
                // printf("Decrementing numWritersInQueue_ from %d to %d\n", numWritersInQueue_, numWritersInQueue_-1);
                numWritersInQueue_++; varsLock_.unlock(); writeSem_->wait(); 
            }
        }
        inline void endWrite() {
            // printf("endWrite\n");
            varsLock_.lock();
            // printf("Decrementing numWritersInUse_ from %d to %d\n", numWritersInUse_, numWritersInUse_-1);
            if (--numWritersInUse_ == 0) { 
                while (numReadersInQueue_ > 0) {
                    // printf("Decrementing numReadersInQueue_ from %d to %d\n", numReadersInQueue_, numReadersInQueue_-1);
                    // printf("Incrementing numReadersInUse_ from %d to %d\n", numReadersInUse_, numReadersInUse_+1);
                    readSem_->signal(); numReadersInQueue_--; numReadersInUse_++;
                }
            }
            varsLock_.unlock();
        }
};
