#pragma once

/*
 * stdafx.h
 *
 * 一组用于简化 Qt 对象属性声明的宏。
 * 这些宏为属性自动生成 Q_PROPERTY 声明、对应的通知信号（signal）、setter、getter 以及底层私有成员。
 *
 * 约定：
 *  - 参数 `TYPE` 表示属性的类型（例如 `int`、`QString`、自定义类型等）。
 *  - 参数 `M` 表示属性名（标识符），宏会生成成员 `_M`、信号 `MChanged()`、方法 `M()` 和 `M(...)`。
 *
 * 注意：
 *  - 生成的 setter/getter 使用了不同的签名（按宏设计）：
 *      - `Q_PROPERTY_AUTO_P` 的 setter 接受按值的 `TYPE`（适用于小型可拷贝类型或 POD）。
 *      - `Q_PROPERTY_AUTO` 的 setter 接受 `const TYPE &` 并在值变化时才发出通知（用于昂贵拷贝/大对象）。
 *      - `Q_PROPERTY_READONLY_AUTO` 生成只读属性的 Q_PROPERTY（只有 READ 和 NOTIFY，没有写入器），但仍提供内部写入器供类内部使用。
 *  - 这些宏会把成员命名为 `_<name>`，请避免在类中手工声明同名成员以免冲突。
 */

#define Q_PROPERTY_AUTO_P(TYPE, M)                                                                 \
    /*
     * 自动生成：
     * - Q_PROPERTY(TYPE M MEMBER _M NOTIFY MChanged)
     * - public: 信号 MChanged(); setter void M(TYPE in_M)（按值）; getter TYPE M() const;
     * - private: TYPE _M;
     *
     * 适用场景：类型较小、按值传递开销小（例如整型、枚举、小结构体）。
     */                                                                \
    Q_PROPERTY(TYPE M MEMBER _##M NOTIFY M##Changed)                                               \
public:                                                                                            \
    Q_SIGNAL void M##Changed();                                                                    \
    void M(TYPE in_##M) {                                                                          \
        _##M = in_##M;                                                                             \
        Q_EMIT M##Changed();                                                                       \
    }                                                                                              \
    TYPE M() const {                                                                               \
        return _##M;                                                                               \
    }                                                                                              \
                                                                                                   \
private:                                                                                           \
    TYPE _##M;

#define Q_PROPERTY_AUTO(TYPE, M)                                                                   \
    /*
     * 自动生成：
     * - Q_PROPERTY(TYPE M MEMBER _M NOTIFY MChanged)
     * - public: 信号 MChanged(); setter void M(const TYPE &in_M)（按引用并做等值检查）; getter TYPE M() const;
     * - private: TYPE _M;
     *
     * 特点：setter 在新值与旧值不同时才写入并触发通知，适合昂贵拷贝或需要抑制重复通知的类型（例如 QString、QList、复杂结构等）。
     */                                                               \
    Q_PROPERTY(TYPE M MEMBER _##M NOTIFY M##Changed)                                               \
public:                                                                                            \
    Q_SIGNAL void M##Changed();                                                                    \
    void M(const TYPE &in_##M) {                                                                   \
        if (_##M != in_##M) {                                                                      \
            _##M = in_##M;                                                                         \
            Q_EMIT M##Changed();                                                                   \
        }                                                                                          \
    }                                                                                              \
    TYPE M() const {                                                                               \
        return _##M;                                                                               \
    }                                                                                              \
                                                                                                   \
private:                                                                                           \
    TYPE _##M;


#define Q_PROPERTY_READONLY_AUTO(TYPE, M)                                                          \
    /*
     * 只读属性宏：
     * - Q_PROPERTY(TYPE M READ M NOTIFY MChanged FINAL)
     * - 提供 public getter `TYPE M() const` 与通知信号 `MChanged()`。
     * - 同时在类内部依然会生成一个写入接口 `void M(const TYPE &in_M)`（供类内部使用），以便在需要时修改值并触发通知。
     * - 使用 `FINAL` 标记属性为最终（阻止子类重写属性）。
     *
     * 适用场景：对外只读但类内部需要更新的属性（例如缓存、计算结果等）。
     */                                                               \
    Q_PROPERTY(TYPE M READ M NOTIFY M##Changed FINAL)                                              \
public:                                                                                            \
    Q_SIGNAL void M##Changed();                                                                    \
    void M(const TYPE &in_##M) {                                                                   \
        if (_##M != in_##M) {                                                                      \
            _##M = in_##M;                                                                         \
            Q_EMIT M##Changed();                                                                   \
        }                                                                                          \
    }                                                                                              \
    TYPE M() const {                                                                               \
        return _##M;                                                                               \
    }                                                                                              \
                                                                                                   \
private:                                                                                           \
    TYPE _##M;
