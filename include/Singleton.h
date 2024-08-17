// Singleton.h
#ifndef SINGLETON_H
#define SINGLETON_H

template <typename T>
class Singleton {
protected:
    Singleton() {}
    virtual ~Singleton() {}

public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static T& getInstance() {
        static T instance;
        return instance;
    }
};

#endif // SINGLETON_H
