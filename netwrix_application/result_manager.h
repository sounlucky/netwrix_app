#pragma once
#include <set>
#include <thread>
#include <algorithm>

#include "file_processing.h"

class result_manager
{
	std::vector<std::future<void>> pending_results;

	template<typename R>
	bool static is_ready(std::future<R> const& f) {
		return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}
	void update();
	result_manager() = default;
	~result_manager() = default;
public:
	result_manager(const result_manager&) = delete;
	static result_manager& instance();
	template <class T> void insert(T&& task) { pending_results.push_back(std::async(std::move(task))); update(); };
	void wait();
};

