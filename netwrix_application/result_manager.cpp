#include "result_manager.h"

result_manager& result_manager::instance() {
	static result_manager inst;
	return inst;
}

void result_manager::update(){
	auto it = std::remove_if(std::begin(pending_results), std::end(pending_results),
		[](const auto& res) {
			return is_ready(res);
		}
	);
	if (it != std::end(pending_results)) {
		pending_results.erase(it, pending_results.end());
	}
}

void result_manager::wait() {
	for (auto& e : pending_results) {
		e.get();
	}
}

