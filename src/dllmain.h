#include <windows.h>
#include <stddef.h>
#include <stdint.h>

#include <mutex>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <chrono>
#include <iostream>
#include <condition_variable>

namespace patches {
    namespace Plugins {
        typedef void (*CallBackTouchCard)     (int32_t, int32_t, uint8_t[168], uint64_t);
        typedef bool (*CommitCardCallback)    (std::string, std::string);
        typedef bool (*CommitQrCallback)      (std::vector<uint8_t> &);
        typedef bool (*CommitQrLoginCallback) (std::string);
    }
}