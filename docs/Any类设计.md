# Any 类设计

## 类定义

```c++
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
```

## 说明

构建一个 `Any` 类型，接收任意的其他类型 --> 模板。

考虑：用任意一个类型（**基类类型**） 指向 其他任意的类型（**派生类类型**）
