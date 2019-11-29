#ifndef queue_h
#define queue_h

#include <mutex>
#include <condition_variable>
#include <deque>

namespace wlp {
	template <typename T>
	class queue {
	public:
		queue() {
		}
		void push(const T &elem) {
			std::unique_lock<std::mutex> lock(mut);
			deq.push_front(elem);
			lock.unlock();
			cond.notify_one();
		}
		size_t size() {
			std::unique_lock<std::mutex> lock(mut);
			return deq.size();
		}
		bool empty() {
			return size() == 0;
		}
		T pop(uint64_t timeout = 0) {
			std::unique_lock<std::mutex> lock(mut);
			auto pred = [this] {
				return !deq.empty();
			};
			if(timeout != 0) {
				if(!cond.wait_for(lock, std::chrono::duration<uint64_t, std::micro>(timeout), pred)) {
					return nullptr;
				}
			} else {
				cond.wait(lock, pred);
			}
			T elem = deq.back();
			deq.pop_back();
			return elem;
		}
	private:
		std::mutex mut;
		std::condition_variable cond;
		std::deque<T> deq;
		queue(const queue &q);
		queue &operator=(const queue &q);
	};
}

#endif

