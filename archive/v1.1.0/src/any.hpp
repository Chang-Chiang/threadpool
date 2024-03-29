/**
 * @file any.hpp
 * @author Chang Chiang (Chang_Chiang@outlook.com)
 * @brief 所有类型的基类（C++17 有提供）,用于接收任意数据类型
 * （模板类代码只能都写在头文件中）
 * @version 0.1
 * @date 2023-05-27
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <memory>

class Any {
private:
    // 基类类型
    class Base {
    public:
        virtual ~Base() = default;
    };

    // 派生类类型
    template <typename T>
    class Derive : public Base {
    public:
        Derive(T data) : data_(data) {}

        T data_; // 保存了任意的其它类型
    };

private:
    // 定义基类指针
    std::unique_ptr<Base> base_;

public:
    Any() = default;
    ~Any() = default;
    Any(const Any&) = delete;
    Any& operator=(const Any&) = delete;
    Any(Any&&) = default;
    Any& operator=(Any&&) = default;

public:
    // 让 Any 类型接收任意其它的数据类型
    template <typename T> // T:int    Derive<int>
    Any(T data) : base_(std::make_unique<Derive<T>>(data)) {}

    // 提取 Any 对象里面存储的 data 数据类型
    template <typename T>
    T cast_() {
        // 从 base_ 找到它所指向的 Derive 对象，从它里面取出 data 成员变量
        // 基类指针转为派生类指针   RTTI
        Derive<T>* pd = dynamic_cast<Derive<T>*>(base_.get());
        if (pd == nullptr) {
            throw "type unmatched!";
        }
        return pd->data_;
    }
};