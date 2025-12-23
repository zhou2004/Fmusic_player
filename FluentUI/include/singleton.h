#pragma once

/*
 * 简易单例辅助模板
 *
 * 说明：
 * - 提供一个通用的单例实现：`Singleton<T>::getInstance()`。
 * - 使用函数内部的 `static T *instance = new T();` 来延迟初始化并保证在程序结束前一直存在。
 * - 由于 C++11 保证函数局部静态变量的线程安全初始化（magic statics），因此该实现在线程并发初始化时也是安全的。
 * - 使用 `new` 分配指针并不在此处删除它，单例对象的生命周期将持续到程序退出，操作系统回收内存。
 *
 * 注意事项：
 * - 该实现要求 `T` 有可访问的默认构造函数（`T()`）。如果 `T` 的构造需要参数或被删除，请使用自定义工厂函数或其他单例实现。
 * - 因为返回的是原始指针，调用方不应删除或管理该指针的生命周期。
 * - 如果需要可控析构（比如希望在某个时间点销毁单例），可用 `std::unique_ptr` + 自定义销毁或使用 `std::atexit` 注册析构函数。
 * - 如果希望避免使用 `new`（例如避免内存泄漏报告），可以改为 `static T instance;` 返回引用，但那会改变析构时序与可控性。
 *
 * 使用示例：
 *   class Foo { public: void hello(); };
 *   // 在类内部使用宏简化单例声明：
 *   //   class Foo { SINGLETON(Foo) ... };
 *   // 或者直接： Foo *f = Singleton<Foo>::getInstance();
 */

template <typename T>
class Singleton {
public:
    /**
     * 返回单例指针。
     * 首次调用会创建单例（在函数内部以 `new` 分配），之后返回同一实例地址。
     * 线程安全（C++11 及以上）。
     */
    static T *getInstance();
};

template <typename T>
T *Singleton<T>::getInstance() {
    // 这里使用函数局部静态指针并用 new 分配对象：
    // - 延迟初始化：只有在第一次调用时才分配
    // - 在程序结束前保持存活（不会自动 delete）
    static T *instance = new T();
    return instance;
}

/*
 * 辅助宏：在类内部放置此宏可以方便地为该类提供一个静态的 `getInstance()` 访问器。
 *
 * 用法：
 * 
 *   class MyClass {
 *       SINGLETON(MyClass)
 *   public:
 *       void foo();
 *   };
 *
 * 该宏会：
 * - 将 `Singleton<MyClass>` 声明为友元，使其可以访问 `MyClass` 的私有构造函数（如果需要）。
 * - 定义一个 `static MyClass* getInstance()` 方法，内部调用 `Singleton<MyClass>::getInstance()`。
 *
 * 注意：如果 `MyClass` 的默认构造函数是私有的，这个宏和 `friend` 声明允许 `Singleton<MyClass>` 在类外部创建实例。
 */
#define SINGLETON(Class)                                                                           \
private:                                                                                           \
    /* 允许模板 Singleton<Class> 访问 Class 的私有/保护成员（例如私有构造函数） */            \
    friend class Singleton<Class>;                                                                 \
                                                                                                   \
public:                                                                                            \
    /* 简便的访问器，转发到通用模板实现 */                                                          \
    static Class *getInstance() {                                                                  \
        return Singleton<Class>::getInstance();                                                    \
    }
