#pragma once

#include <string>
#include <functional>

namespace web {

using LogCallback = std::function<void(const std::string&)>;
bool start(LogCallback log_cb);

}
