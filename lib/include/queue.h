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
		T pop() {
			std::unique_lock<std::mutex> lock(mut);
			cond.wait(lock, [this] {
				return !deq.empty();
			});
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

