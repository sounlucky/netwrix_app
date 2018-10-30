#include "ou_stream.h"

std::optional<std::wofstream> ou_stream::_stream = {};
std::mutex ou_stream::_mutex = {};