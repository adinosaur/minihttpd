//
// Date: 2015/09/09
// Author: Dinosaur W.
//
// drive NonCopyable for non copyable.

#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

class NonCopyable
{
    public:
        NonCopyable() = default;
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;
};

#endif
