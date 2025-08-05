
// Copyright (c) 2018 brinkqiang (brink.qiang@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#ifndef __DMMODULEPTR_H_INCLUDE__
#define __DMMODULEPTR_H_INCLUDE__

#include <memory>
#include <mutex>

template <typename T>
struct DmReleaseDeleter {
    void operator()(T* ptr) const {
        if (ptr) {
            ptr->Release();
        }
    }
};

template <typename T>
using DmModulePtr = std::unique_ptr<T, DmReleaseDeleter<T>>;

template <typename T>
using DmUniquePtr = std::unique_ptr<T, DmReleaseDeleter<T>>;

template <typename T>
using DmSharedPtr = std::shared_ptr<T>;

template <typename T>
inline DmSharedPtr<T> CreateSharedPtr(T* raw_ptr) {
    return DmSharedPtr<T>(raw_ptr, DmReleaseDeleter<T>());
}

template <typename T, typename... Args>
inline std::shared_ptr<T> GetSingletonSharedPtr(Args&&... args)
{
    static std::once_flag once_flag;
    static std::shared_ptr<T> instance;
    std::call_once(once_flag, [&]() {
        instance = std::make_shared<T>(std::forward<Args>(args)...);
        });

    return instance;
}
#endif // __DMMODULEPTR_H_INCLUDE__