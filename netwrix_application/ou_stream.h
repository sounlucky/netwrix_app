#pragma once
#include <fstream>
#include <optional>
#include <mutex>

class ou_stream
{
	ou_stream() = default;
	~ou_stream() = default;
	static std::optional<std::wofstream> _stream;
	static std::mutex _mutex;
public:
	ou_stream(const ou_stream&) = delete;
	template <class T> static void init(T&& arg) { _stream.emplace(arg); };

	template <class T>
	void static forward(T&& arg) {
		if (!_stream) {
			throw std::runtime_error("output stream is requested but not yet initialized");
		}
		std::lock_guard<std::mutex> lock(_mutex);
		_stream.value() << arg << L'\n';
	}
};
