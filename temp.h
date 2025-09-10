    explicit Thread_Pool(int thread_count, std::size_t queue_capacity = 1024)
    : thread_count_(thread_count > 0 ? thread_count : 1),
      cap_(queue_capacity ? queue_capacity : 1),
      ring_(cap_)
    {
        workers_.reserve(thread_count_);
        for (int tid = 0; tid < thread_count_; ++tid) {
            workers_.emplace_back([this, tid] {
                worker_loop(tid);
            });
        }
    }


    not_empty_.notify_all();
    not_full_.notify_all();
    done_.notify_all();
