#include "safeQueue.hh"
#include <atomic>
#include <thread>

class join_threads {
  std::vector<std::thread> &threads;

public:
  explicit join_threads(std::vector<std::thread> &threads_)
      : threads(threads_) {}
  ~join_threads() {
    // std::cerr << "destructing joiner\n";
    for (unsigned long i = 0; i < threads.size(); ++i) {
      if (threads[i].joinable())
        threads[i].join();
    }
  }
};

class thread_pool {

  std::atomic_bool done;
  threadsafe_queue<std::function<void()>> work_queue;
  std::vector<std::thread> threads;
  join_threads *joiner;

  void worker_thread() {
    while (!done) {
      // cout << "this is worker thread!\n";
      std::function<void()> task;
      if (work_queue.try_pop(task)) {
        // std::cout << std::this_thread::get_id() << std::endl;
        task();
      } else {
        // std::cerr << "yield" << std::endl;
        std::this_thread::yield();
      }
    }
    // cout << "Queue is empty!\n";
  }

public:
  thread_pool() : done(false), joiner(new join_threads(threads)) {
    unsigned const thread_count = std::thread::hardware_concurrency();
    // std::cerr << "Creating pool with " << thread_count << " threads" << endl;
    try {
      for (unsigned i = 0; i < thread_count; ++i) {
        threads.push_back( //
            std::thread(&thread_pool::worker_thread, this));
      }
    } catch (...) {
      done = true;
      throw;
    }
  }
  ~thread_pool() {
    joiner->~join_threads();
    done = true;
    // std::string s("Destructing pool ");
    // s += std::to_string(work_queue.empty());
    // s += '\n';
    // std::cerr << s;
  }
  template <typename FunctionType> void submit(FunctionType f) {
    work_queue.push(std::function<void()>(f));
    // std::cout << std::this_thread::get_id() << std::endl;
    // std::cout << "hola" << std::endl;
  }
};
