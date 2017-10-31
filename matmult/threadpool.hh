#include "safeQueue.hh"
#include <atomic>
#include <thread>
#include <unordered_map>

class join_threads
{
  std::vector<std::thread> &threads;

  public:
    explicit join_threads(std::vector<std::thread> &threads_) : threads(threads_) {}
    ~join_threads() 
    {
      // std::cerr << "destructing joiner\n";
      for (unsigned long i = 0; i < threads.size(); ++i) {
        if (threads[i].joinable())
          threads[i].join();
      }
    }
};

class thread_pool
{
  std::atomic_bool done;
  threadsafe_queue<std::function<void()> > work_queue;
  std::vector<std::thread> threads;
  join_threads joiner;

  //////////////////////////////////////////////////////
  // std::unordered_map<typename K, typename V> threadsMap;
  // std::mutex threadsMapMtx;
  //////////////////////////////////////////////////////

  void worker_thread()
  {
    while(!done)
    {
      std::function<void()> task;
      if(work_queue.try_pop(task))
      {
        ////////////////////////////////////////////////
        // {
        //   threadsMapMtx.lock();
        //   auto tid = std::this_thread::get_id();
        //   if(threadsMap.count(tid) <= 0){
        //     threadsMap.insert(tid, 1);
        //   } else {
        //     int tcount = threadsMap.get(tid);
        //     threadsMap.insert(tid, ++tcount);
        //   }
        //   threadsMapMtx.unlock();
        // }
        ////////////////////////////////////////////////
        task();
      } 
      else
      {
        std::this_thread::yield();
      }  
    }
  }

  public:
    thread_pool():done(false),joiner(threads)
    {
      unsigned const thread_count=std::thread::hardware_concurrency();
      try
      {
        for(unsigned i=0;i<thread_count;++i)
        {
          threads.push_back(std::thread(&thread_pool::worker_thread,this));
        }
      }
      catch(...)
      {
        done=true;
        throw;
      }
    }

    ~thread_pool()
    {
      done=true;
    }

    template<typename FunctionType>
    void submit(FunctionType f)
    {
      work_queue.push(std::function<void()>(f));
    }
};
