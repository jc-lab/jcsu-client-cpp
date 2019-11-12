#ifndef __JCSU_INTL_UNIOBJ_H__
#define __JCSU_INTL_UNIOBJ_H__

#include <memory>

namespace jcsu {
    template<class T>
    class UniObj {
    private:
        std::unique_ptr<T> up_;
        std::shared_ptr<T> sp_;
        T *ptr_;

    public:
        UniObj() {
            ptr_ = nullptr;
        }

        explicit UniObj(std::unique_ptr<T> obj) : up_(std::move(obj)) {
            ptr_ = up_.get();
        }

        explicit UniObj(std::shared_ptr<T> obj) : sp_(std::move(obj)) {
            ptr_ = sp_.get();
        }

        explicit UniObj(T *ptr) : ptr_(ptr) {}

        UniObj<T> &operator=(UniObj<T>&& obj) noexcept {
            up_ = std::move(obj.up_);
            sp_ = std::move(obj.sp_);
            ptr_ = obj.ptr_;
            obj.ptr_ = nullptr;
            return *this;
        }

        UniObj<T> &operator=(std::unique_ptr<T> &&obj) {
            up_ = std::move(obj);
            sp_.reset();
            ptr_ = up_.get();
            return *this;
        }

        UniObj<T> &operator=(std::shared_ptr<T> obj) {
            up_.reset(nullptr);
            sp_ = obj;
            ptr_ = up_.get();
            return *this;
        }

        UniObj<T> &operator=(T *ptr) {
            up_.reset(nullptr);
            sp_.reset();
            ptr_ = ptr;
            return *this;
        }

        T* get() const {
            return ptr_;
        }

        void reset() {
            up_.reset(nullptr);
            sp_.reset();
            ptr_ = nullptr;
        }
    };
}

#endif /* __JCSU_INTL_UNIOBJ_H__ */
