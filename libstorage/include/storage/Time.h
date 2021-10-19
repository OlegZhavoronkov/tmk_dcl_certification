#pragma once

#ifndef STORAGE_TIME_H
#define STORAGE_TIME_H

#include <chrono>

namespace tmk {

    using system_clock = std::chrono::system_clock;
    using TimeT = std::chrono::time_point<std::chrono::system_clock>;
}

#endif // !STORAGE_TIME_H