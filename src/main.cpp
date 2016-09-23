#include <inttypes.h>
#include <stdbool.h>

#ifndef JUNIPER_H
#define JUNIPER_H

#include <stdlib.h>

namespace juniper
{
    template<typename Result, typename ...Args>
    struct abstract_function
    {
        virtual Result operator()(Args... args) = 0;
        virtual abstract_function *clone() const = 0;
        virtual ~abstract_function() = default;
    };

    template<typename Func, typename Result, typename ...Args>
    class concrete_function : public abstract_function<Result, Args...>
    {
        Func f;
    public:
        concrete_function(const Func &x)
            : f(x)
        {}
        Result operator()(Args... args) override
        {
            return f(args...);
        }
        concrete_function *clone() const override
        {
            return new concrete_function{ f };
        }
    };

    template<typename Func>
    struct func_filter
    {
        typedef Func type;
    };
    template<typename Result, typename ...Args>
    struct func_filter<Result(Args...)>
    {
        typedef Result(*type)(Args...);
    };

    template<typename signature>
    class function;

    template<typename Result, typename ...Args>
    class function<Result(Args...)>
    {
        abstract_function<Result, Args...> *f;
    public:
        function()
            : f(nullptr)
        {}
        template<typename Func> function(const Func &x)
            : f(new concrete_function<typename func_filter<Func>::type, Result, Args...>(x))
        {}
        function(const function &rhs)
            : f(rhs.f ? rhs.f->clone() : nullptr)
        {}
        function &operator=(const function &rhs)
        {
            if ((&rhs != this) && (rhs.f))
            {
                auto *temp = rhs.f->clone();
                delete f;
                f = temp;
            }
            return *this;
        }
        template<typename Func> function &operator=(const Func &x)
        {
            auto *temp = new concrete_function<typename func_filter<Func>::type, Result, Args...>(x);
            delete f;
            f = temp;
            return *this;
        }
        Result operator()(Args... args)
        {
            if (f)
                return (*f)(args...);
            else
                return Result{};
        }
        ~function()
        {
            delete f;
        }
    };

    template <class T>
    void swap(T& a, T& b) {
        T c(a);
        a = b;
        b = c;
    }

    template <typename contained>
    class shared_ptr {
    public:
        shared_ptr() : ptr_(NULL), ref_count_(NULL) { }

        shared_ptr(contained * p)
            : ptr_(p), ref_count_(new int)
        {
            *ref_count_ = 0;
            inc_ref();
        }

        shared_ptr(const shared_ptr& rhs)
            : ptr_(rhs.ptr_), ref_count_(rhs.ref_count_)
        {
            inc_ref();
        }

        ~shared_ptr() {
            if (ref_count_ && 0 == dec_ref()) {
                if (ptr_) {
                    delete ptr_;
                }
                delete ref_count_;
            }
        }

        void set(contained* p) {
          ptr_ = p;
        }

        contained* get() { return ptr_; }
        const contained* get() const { return ptr_; }

        void swap(shared_ptr& rhs) {
            juniper::swap(ptr_, rhs.ptr_);
            juniper::swap(ref_count_, rhs.ref_count_);
        }

        shared_ptr& operator=(const shared_ptr& rhs) {
            shared_ptr tmp(rhs);
            this->swap(tmp);
            return *this;
        }

        //contained& operator*() {
        //    return *ptr_;
        //}

        contained* operator->() {
            return ptr_;
        }

        bool operator==(shared_ptr& rhs) {
            return ptr_ == rhs.ptr_;
        }

        bool operator!=(shared_ptr& rhs) { return !(rhs == *this); }
    private:
        void inc_ref() {
            if (ref_count_) {
                ++(*ref_count_);
            }
        }

        int dec_ref() {
            return --(*ref_count_);
        }

        contained * ptr_;
        int * ref_count_;
    };

    template<typename T, size_t N>
    class array {
    public:
        array<T, N>& fill(T fillWith) {
            for (size_t i = 0; i < N; i++) {
                data[i] = fillWith;
            }

            return *this;
        }

        T& operator[](int i) {
            return data[i];
        }

        bool operator==(array<T, N>& rhs) {
            for (auto i = 0; i < N; i++) {
                if (data[i] != rhs[i]) {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(array<T, N>& rhs) { return !(rhs == *this); }

        T data[N];
    };

    template<typename T>
    T quit() {
        exit(1);
    }
}

#endif

#include <Arduino.h>

namespace Prelude {}
namespace List {}
namespace Signal {}
namespace Io {}
namespace Maybe {}
namespace Time {}
namespace Math {}
namespace Button {}
namespace Vector {}
namespace SoundBar {}
namespace List {
    using namespace Prelude;

}

namespace Signal {
    using namespace Prelude;

}

namespace Io {
    using namespace Prelude;

}

namespace Maybe {
    using namespace Prelude;

}

namespace Time {
    using namespace Prelude;

}

namespace Math {
    using namespace Prelude;

}

namespace Button {
    using namespace Prelude;
    using namespace Io;

}

namespace Vector {
    using namespace Prelude;
    using namespace List;
    using namespace Math;

}

namespace SoundBar {
    using namespace Prelude;

}

namespace Prelude {
    struct unit {
        bool operator==(unit rhs) {
            return true;
        }

        bool operator!=(unit rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a>
    struct maybe {
        uint8_t tag;
        bool operator==(maybe rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->just == rhs.just;
                case 1:
                    return this->nothing == rhs.nothing;
            }
            return false;
        }

        bool operator!=(maybe rhs) { return !(rhs == *this); }
        union {
            a just;
            uint8_t nothing;
        };
    };

    template<typename a>
    Prelude::maybe<a> just(a data) {
        return (([&]() -> Prelude::maybe<a> { Prelude::maybe<a> ret; ret.tag = 0; ret.just = data; return ret; })());
    }

    template<typename a>
    Prelude::maybe<a> nothing() {
        return (([&]() -> Prelude::maybe<a> { Prelude::maybe<a> ret; ret.tag = 1; ret.nothing = 0; return ret; })());
    }


}

namespace Prelude {
    template<typename a, typename b>
    struct either {
        uint8_t tag;
        bool operator==(either rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->left == rhs.left;
                case 1:
                    return this->right == rhs.right;
            }
            return false;
        }

        bool operator!=(either rhs) { return !(rhs == *this); }
        union {
            a left;
            b right;
        };
    };

    template<typename a, typename b>
    Prelude::either<a, b> left(a data) {
        return (([&]() -> Prelude::either<a, b> { Prelude::either<a, b> ret; ret.tag = 0; ret.left = data; return ret; })());
    }

    template<typename a, typename b>
    Prelude::either<a, b> right(b data) {
        return (([&]() -> Prelude::either<a, b> { Prelude::either<a, b> ret; ret.tag = 1; ret.right = data; return ret; })());
    }


}

namespace Prelude {
    template<typename a, int n>
    struct list {
        juniper::array<a, n> data;
        uint32_t length;
        bool operator==(list rhs) {
            return true && data == rhs.data && length == rhs.length;
        }

        bool operator!=(list rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<int n>
    struct string {
        Prelude::list<uint8_t, n> characters;
        bool operator==(string rhs) {
            return true && characters == rhs.characters;
        }

        bool operator!=(string rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a>
    struct sig {
        uint8_t tag;
        bool operator==(sig rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->signal == rhs.signal;
            }
            return false;
        }

        bool operator!=(sig rhs) { return !(rhs == *this); }
        union {
            Prelude::maybe<a> signal;
        };
    };

    template<typename a>
    Prelude::sig<a> signal(Prelude::maybe<a> data) {
        return (([&]() -> Prelude::sig<a> { Prelude::sig<a> ret; ret.tag = 0; ret.signal = data; return ret; })());
    }


}

namespace Prelude {
    template<typename a, typename b>
    struct tuple2 {
        a e1;
        b e2;
        bool operator==(tuple2 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2;
        }

        bool operator!=(tuple2 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a, typename b, typename c>
    struct tuple3 {
        a e1;
        b e2;
        c e3;
        bool operator==(tuple3 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3;
        }

        bool operator!=(tuple3 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a, typename b, typename c, typename d>
    struct tuple4 {
        a e1;
        b e2;
        c e3;
        d e4;
        bool operator==(tuple4 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4;
        }

        bool operator!=(tuple4 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a, typename b, typename c, typename d, typename e>
    struct tuple5 {
        a e1;
        b e2;
        c e3;
        d e4;
        e e5;
        bool operator==(tuple5 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5;
        }

        bool operator!=(tuple5 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a, typename b, typename c, typename d, typename e, typename f>
    struct tuple6 {
        a e1;
        b e2;
        c e3;
        d e4;
        e e5;
        f e6;
        bool operator==(tuple6 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5 && e6 == rhs.e6;
        }

        bool operator!=(tuple6 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a, typename b, typename c, typename d, typename e, typename f, typename g>
    struct tuple7 {
        a e1;
        b e2;
        c e3;
        d e4;
        e e5;
        f e6;
        g e7;
        bool operator==(tuple7 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5 && e6 == rhs.e6 && e7 == rhs.e7;
        }

        bool operator!=(tuple7 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a, typename b, typename c, typename d, typename e, typename f, typename g, typename h>
    struct tuple8 {
        a e1;
        b e2;
        c e3;
        d e4;
        e e5;
        f e6;
        g e7;
        h e8;
        bool operator==(tuple8 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5 && e6 == rhs.e6 && e7 == rhs.e7 && e8 == rhs.e8;
        }

        bool operator!=(tuple8 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a, typename b, typename c, typename d, typename e, typename f, typename g, typename h, typename i>
    struct tuple9 {
        a e1;
        b e2;
        c e3;
        d e4;
        e e5;
        f e6;
        g e7;
        h e8;
        i e9;
        bool operator==(tuple9 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5 && e6 == rhs.e6 && e7 == rhs.e7 && e8 == rhs.e8 && e9 == rhs.e9;
        }

        bool operator!=(tuple9 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename a, typename b, typename c, typename d, typename e, typename f, typename g, typename h, typename i, typename j>
    struct tuple10 {
        a e1;
        b e2;
        c e3;
        d e4;
        e e5;
        f e6;
        g e7;
        h e8;
        i e9;
        j e10;
        bool operator==(tuple10 rhs) {
            return true && e1 == rhs.e1 && e2 == rhs.e2 && e3 == rhs.e3 && e4 == rhs.e4 && e5 == rhs.e5 && e6 == rhs.e6 && e7 == rhs.e7 && e8 == rhs.e8 && e9 == rhs.e9 && e10 == rhs.e10;
        }

        bool operator!=(tuple10 rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Io {
    struct pinState {
        uint8_t tag;
        bool operator==(pinState rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->high == rhs.high;
                case 1:
                    return this->low == rhs.low;
            }
            return false;
        }

        bool operator!=(pinState rhs) { return !(rhs == *this); }
        union {
            uint8_t high;
            uint8_t low;
        };
    };

    Io::pinState high() {
        return (([&]() -> Io::pinState { Io::pinState ret; ret.tag = 0; ret.high = 0; return ret; })());
    }

    Io::pinState low() {
        return (([&]() -> Io::pinState { Io::pinState ret; ret.tag = 1; ret.low = 0; return ret; })());
    }


}

namespace Io {
    struct mode {
        uint8_t tag;
        bool operator==(mode rhs) {
            if (this->tag != rhs.tag) { return false; }
            switch (this->tag) {
                case 0:
                    return this->input == rhs.input;
                case 1:
                    return this->output == rhs.output;
                case 2:
                    return this->inputPullup == rhs.inputPullup;
            }
            return false;
        }

        bool operator!=(mode rhs) { return !(rhs == *this); }
        union {
            uint8_t input;
            uint8_t output;
            uint8_t inputPullup;
        };
    };

    Io::mode input() {
        return (([&]() -> Io::mode { Io::mode ret; ret.tag = 0; ret.input = 0; return ret; })());
    }

    Io::mode output() {
        return (([&]() -> Io::mode { Io::mode ret; ret.tag = 1; ret.output = 0; return ret; })());
    }

    Io::mode inputPullup() {
        return (([&]() -> Io::mode { Io::mode ret; ret.tag = 2; ret.inputPullup = 0; return ret; })());
    }


}

namespace Time {
    struct timerState {
        uint32_t lastPulse;
        bool operator==(timerState rhs) {
            return true && lastPulse == rhs.lastPulse;
        }

        bool operator!=(timerState rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Button {
    struct buttonState {
        Io::pinState actualState;
        Io::pinState lastState;
        uint32_t lastDebounceTime;
        bool operator==(buttonState rhs) {
            return true && actualState == rhs.actualState && lastState == rhs.lastState && lastDebounceTime == rhs.lastDebounceTime;
        }

        bool operator!=(buttonState rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Vector {
    template<typename a, int n>
    struct vector {
        juniper::array<a, n> data;
        bool operator==(vector rhs) {
            return true && data == rhs.data;
        }

        bool operator!=(vector rhs) {
            return !(rhs == *this);
        }
    };
}

namespace Prelude {
    template<typename t5, typename t3, typename t4>
    juniper::function<t4(t5)> compose(juniper::function<t4(t3)> f, juniper::function<t3(t5)> g);
}

namespace Prelude {
    template<typename t14, typename t15, typename t13>
    juniper::function<juniper::function<t13(t15)>(t14)> curry(juniper::function<t13(t14,t15)> f);
}

namespace Prelude {
    template<typename t24, typename t25, typename t26, typename t23>
    juniper::function<juniper::function<juniper::function<t23(t26)>(t25)>(t24)> curry3(juniper::function<t23(t24,t25,t26)> f);
}

namespace Prelude {
    template<typename t32>
    bool equal(t32 x, t32 y);
}

namespace Prelude {
    template<typename t36, typename t37>
    t36 fst(Prelude::tuple2<t36,t37> tup);
}

namespace Prelude {
    template<typename t41, typename t42>
    t42 snd(Prelude::tuple2<t41,t42> tup);
}

namespace Prelude {
    template<typename t44>
    t44 add(t44 numA, t44 numB);
}

namespace Prelude {
    template<typename t46>
    t46 sub(t46 numA, t46 numB);
}

namespace Prelude {
    template<typename t48>
    t48 mul(t48 numA, t48 numB);
}

namespace Prelude {
    template<typename t50>
    t50 div(t50 numA, t50 numB);
}

namespace List {
    template<typename t52, typename t53, int c1>
    Prelude::list<t53, c1> map(juniper::function<t53(t52)> f, Prelude::list<t52, c1> lst);
}

namespace List {
    template<typename t62, typename t63, int c4>
    t63 foldl(juniper::function<t63(t62,t63)> f, t63 initState, Prelude::list<t62, c4> lst);
}

namespace List {
    template<typename t71, typename t72, int c6>
    t72 foldr(juniper::function<t72(t71,t72)> f, t72 initState, Prelude::list<t71, c6> lst);
}

namespace List {
    template<typename t80, int c8, int c9, int c10>
    Prelude::list<t80, c10> append(Prelude::list<t80, c8> lstA, Prelude::list<t80, c9> lstB);
}

namespace List {
    template<typename t96, int c16>
    t96 nth(uint32_t i, Prelude::list<t96, c16> lst);
}

namespace List {
    template<typename t98, int c17, int c18>
    Prelude::list<t98, (c17)*(c18)> flattenSafe(Prelude::list<Prelude::list<t98, c17>, c18> listOfLists);
}

namespace List {
    template<typename t109, int c23, int c24>
    Prelude::list<t109, c24> resize(Prelude::list<t109, c23> lst);
}

namespace List {
    template<typename t116, int c27>
    bool all(juniper::function<bool(t116)> pred, Prelude::list<t116, c27> lst);
}

namespace List {
    template<typename t123, int c29>
    bool any(juniper::function<bool(t123)> pred, Prelude::list<t123, c29> lst);
}

namespace List {
    template<typename t130, int c31>
    Prelude::list<t130, c31> pushBack(t130 elem, Prelude::list<t130, c31> lst);
}

namespace List {
    template<typename t138, int c33>
    Prelude::list<t138, c33> pushOffFront(t138 elem, Prelude::list<t138, c33> lst);
}

namespace List {
    template<typename t149, int c37>
    Prelude::list<t149, c37> setNth(uint32_t index, t149 elem, Prelude::list<t149, c37> lst);
}

namespace List {
    template<typename t154, int c39>
    Prelude::list<t154, c39> replicate(uint32_t numOfElements, t154 elem);
}

namespace List {
    template<typename t156, int c40>
    Prelude::list<t156, c40> remove(t156 elem, Prelude::list<t156, c40> lst);
}

namespace List {
    template<typename t168, int c44>
    Prelude::list<t168, c44> dropLast(Prelude::list<t168, c44> lst);
}

namespace List {
    template<typename t173, int c45>
    Prelude::unit foreach(juniper::function<Prelude::unit(t173)> f, Prelude::list<t173, c45> lst);
}

namespace List {
    template<typename t181, int c48>
    t181 last(Prelude::list<t181, c48> lst);
}

namespace List {
    template<typename t186, int c49>
    t186 max_(Prelude::list<t186, c49> lst);
}

namespace List {
    template<typename t196, int c53>
    t196 min_(Prelude::list<t196, c53> lst);
}

namespace List {
    template<typename t203, int c57>
    bool member(t203 elem, Prelude::list<t203, c57> lst);
}

namespace List {
    template<typename t208, typename t209, int c59>
    Prelude::list<Prelude::tuple2<t208,t209>, c59> zip(Prelude::list<t208, c59> lstA, Prelude::list<t209, c59> lstB);
}

namespace List {
    template<typename t221, typename t222, int c63>
    Prelude::tuple2<Prelude::list<t221, c63>,Prelude::list<t222, c63>> unzip(Prelude::list<Prelude::tuple2<t221,t222>, c63> lst);
}

namespace List {
    template<typename t227, int c64>
    t227 sum(Prelude::list<t227, c64> lst);
}

namespace List {
    template<typename t236, int c65>
    t236 average(Prelude::list<t236, c65> lst);
}

namespace Signal {
    template<typename t238, typename t239>
    Prelude::sig<t239> map(juniper::function<t239(t238)> f, Prelude::sig<t238> s);
}

namespace Signal {
    template<typename t250>
    Prelude::unit sink(juniper::function<Prelude::unit(t250)> f, Prelude::sig<t250> s);
}

namespace Signal {
    template<typename t254>
    Prelude::sig<t254> filter(juniper::function<bool(t254)> f, Prelude::sig<t254> s);
}

namespace Signal {
    template<typename t264>
    Prelude::sig<t264> merge(Prelude::sig<t264> sigA, Prelude::sig<t264> sigB);
}

namespace Signal {
    template<typename t266, int c66>
    Prelude::sig<t266> mergeMany(Prelude::list<Prelude::sig<t266>, c66> sigs);
}

namespace Signal {
    template<typename t274, typename t275>
    Prelude::sig<Prelude::either<t274, t275>> join(Prelude::sig<t274> sigA, Prelude::sig<t275> sigB);
}

namespace Signal {
    template<typename t297>
    Prelude::sig<Prelude::unit> toUnit(Prelude::sig<t297> s);
}

namespace Signal {
    template<typename t302, typename t308>
    Prelude::sig<t308> foldP(juniper::function<t308(t302,t308)> f, juniper::shared_ptr<t308> state0, Prelude::sig<t302> incoming);
}

namespace Signal {
    template<typename t318>
    Prelude::sig<t318> dropRepeats(Prelude::sig<t318> incoming, juniper::shared_ptr<Prelude::maybe<t318>> maybePrevValue);
}

namespace Signal {
    template<typename t328>
    Prelude::sig<t328> latch(Prelude::sig<t328> incoming, juniper::shared_ptr<t328> prevValue);
}

namespace Signal {
    template<typename t339, typename t342, typename t337>
    Prelude::sig<t337> map2(juniper::function<t337(t339,t342)> f, Prelude::sig<t339> incomingA, Prelude::sig<t342> incomingB, juniper::shared_ptr<Prelude::tuple2<t339,t342>> state);
}

namespace Signal {
    template<typename t358, int c67>
    Prelude::sig<Prelude::list<t358, c67>> record(Prelude::sig<t358> incoming, juniper::shared_ptr<Prelude::list<t358, c67>> pastValues);
}

namespace Io {
    Io::pinState toggle(Io::pinState p);
}

namespace Io {
    template<int c68>
    Prelude::unit printStr(Prelude::string<c68> str);
}

namespace Io {
    Prelude::unit printFloat(float f);
}

namespace Io {
    Prelude::unit beginSerial(uint32_t speed);
}

namespace Io {
    int32_t pinStateToInt(Io::pinState value);
}

namespace Io {
    Io::pinState intToPinState(uint8_t value);
}

namespace Io {
    Prelude::unit digWrite(uint16_t pin, Io::pinState value);
}

namespace Io {
    Io::pinState digRead(uint16_t pin);
}

namespace Io {
    Prelude::sig<Io::pinState> digIn(uint16_t pin);
}

namespace Io {
    Prelude::unit digOut(uint16_t pin, Prelude::sig<Io::pinState> sig);
}

namespace Io {
    int32_t anaRead(uint16_t pin);
}

namespace Io {
    Prelude::unit anaWrite(uint16_t pin, uint8_t value);
}

namespace Io {
    Prelude::sig<uint16_t> anaIn(uint16_t pin);
}

namespace Io {
    Prelude::unit anaOut(uint16_t pin, Prelude::sig<uint16_t> sig);
}

namespace Io {
    int32_t pinModeToInt(Io::mode m);
}

namespace Io {
    Io::mode intToPinMode(uint8_t m);
}

namespace Io {
    Prelude::unit setPinMode(uint16_t pin, Io::mode m);
}

namespace Io {
    Prelude::sig<Prelude::unit> risingEdge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState);
}

namespace Io {
    Prelude::sig<Prelude::unit> fallingEdge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState);
}

namespace Io {
    Prelude::sig<Io::pinState> edge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState);
}

namespace Maybe {
    template<typename t453, typename t454>
    Prelude::maybe<t454> map(juniper::function<t454(t453)> f, Prelude::maybe<t453> maybeVal);
}

namespace Maybe {
    template<typename t461>
    t461 get(Prelude::maybe<t461> maybeVal);
}

namespace Maybe {
    template<typename t463>
    bool isJust(Prelude::maybe<t463> maybeVal);
}

namespace Maybe {
    template<typename t465>
    bool isNothing(Prelude::maybe<t465> maybeVal);
}

namespace Maybe {
    template<typename t469>
    int32_t count(Prelude::maybe<t469> maybeVal);
}

namespace Maybe {
    template<typename t471, typename t472>
    t472 foldl(juniper::function<t472(t471,t472)> f, t472 initState, Prelude::maybe<t471> maybeVal);
}

namespace Maybe {
    template<typename t477, typename t478>
    t478 fodlr(juniper::function<t478(t477,t478)> f, t478 initState, Prelude::maybe<t477> maybeVal);
}

namespace Maybe {
    template<typename t484>
    Prelude::unit iter(juniper::function<Prelude::unit(t484)> f, Prelude::maybe<t484> maybeVal);
}

namespace Time {
    Prelude::unit wait(uint32_t time);
}

namespace Time {
    int32_t now();
}

namespace Time {
    juniper::shared_ptr<Time::timerState> state();
}

namespace Time {
    Prelude::sig<uint32_t> every(uint32_t interval, juniper::shared_ptr<Time::timerState> state);
}

namespace Math {
    double degToRad(double degrees);
}

namespace Math {
    double radToDeg(double radians);
}

namespace Math {
    double acos_(double x);
}

namespace Math {
    double asin_(double x);
}

namespace Math {
    double atan_(double x);
}

namespace Math {
    double atan2_(double y, double x);
}

namespace Math {
    double cos_(double x);
}

namespace Math {
    double cosh_(double x);
}

namespace Math {
    double sin_(double x);
}

namespace Math {
    double sinh_(double x);
}

namespace Math {
    double tan_(double x);
}

namespace Math {
    double tanh_(double x);
}

namespace Math {
    double exp_(double x);
}

namespace Math {
    Prelude::tuple2<double,int16_t> frexp_(double x);
}

namespace Math {
    double ldexp_(double x, int16_t exponent);
}

namespace Math {
    double log_(double x);
}

namespace Math {
    double log10_(double x);
}

namespace Math {
    Prelude::tuple2<double,double> modf_(double x);
}

namespace Math {
    double pow_(double x, double y);
}

namespace Math {
    double sqrt_(double x);
}

namespace Math {
    double ceil_(double x);
}

namespace Math {
    double fabs_(double x);
}

namespace Math {
    double floor_(double x);
}

namespace Math {
    double fmod_(double x, double y);
}

namespace Math {
    double round_(double x);
}

namespace Math {
    double min_(double x, double y);
}

namespace Math {
    double max_(double x, double y);
}

namespace Math {
    double mapRange(double x, double a1, double a2, double b1, double b2);
}

namespace Math {
    template<typename t537>
    t537 clamp(t537 x, t537 min, t537 max);
}

namespace Math {
    template<typename t539>
    int32_t sign(t539 n);
}

namespace Button {
    juniper::shared_ptr<Button::buttonState> state();
}

namespace Button {
    Prelude::sig<Io::pinState> debounceDelay(Prelude::sig<Io::pinState> incoming, uint16_t delay, juniper::shared_ptr<Button::buttonState> buttonState);
}

namespace Button {
    Prelude::sig<Io::pinState> debounce(Prelude::sig<Io::pinState> incoming, juniper::shared_ptr<Button::buttonState> buttonState);
}

namespace Vector {
    template<typename t562, int c69>
    Vector::vector<t562, c69> make(juniper::array<t562, c69> d);
}

namespace Vector {
    template<typename t565, int c71>
    t565 get(uint32_t i, Vector::vector<t565, c71> v);
}

namespace Vector {
    template<typename t567, int c72>
    Vector::vector<t567, c72> add(Vector::vector<t567, c72> v1, Vector::vector<t567, c72> v2);
}

namespace Vector {
    template<typename t576, int c76>
    Vector::vector<t576, c76> zero();
}

namespace Vector {
    template<typename t578, int c77>
    Vector::vector<t578, c77> subtract(Vector::vector<t578, c77> v1, Vector::vector<t578, c77> v2);
}

namespace Vector {
    template<typename t586, int c81>
    Vector::vector<t586, c81> scale(t586 scalar, Vector::vector<t586, c81> v);
}

namespace Vector {
    template<typename t592, int c84>
    t592 dot(Vector::vector<t592, c84> v1, Vector::vector<t592, c84> v2);
}

namespace Vector {
    template<typename t598, int c87>
    t598 magnitude2(Vector::vector<t598, c87> v);
}

namespace Vector {
    template<typename t604, int c90>
    double magnitude(Vector::vector<t604, c90> v);
}

namespace Vector {
    template<typename t610, int c91>
    Vector::vector<t610, c91> multiply(Vector::vector<t610, c91> u, Vector::vector<t610, c91> v);
}

namespace Vector {
    template<typename t618, int c95>
    Vector::vector<t618, c95> normalize(Vector::vector<t618, c95> v);
}

namespace Vector {
    template<typename t626, int c98>
    double angle(Vector::vector<t626, c98> v1, Vector::vector<t626, c98> v2);
}

namespace Vector {
    template<typename t664>
    Vector::vector<t664, 3> cross(Vector::vector<t664, 3> u, Vector::vector<t664, 3> v);
}

namespace Vector {
    template<typename t666, int c111>
    Vector::vector<t666, c111> project(Vector::vector<t666, c111> a, Vector::vector<t666, c111> b);
}

namespace Vector {
    template<typename t676, int c112>
    Vector::vector<t676, c112> projectPlane(Vector::vector<t676, c112> a, Vector::vector<t676, c112> m);
}

namespace SoundBar {
    Prelude::unit setup();
}

namespace SoundBar {
    Prelude::unit resetBar();
}

namespace SoundBar {
    Prelude::unit drawBar(uint16_t level);
}

namespace SoundBar {
    Prelude::unit main();
}

namespace Prelude {
    template<typename t5, typename t3, typename t4>
    juniper::function<t4(t5)> compose(juniper::function<t4(t3)> f, juniper::function<t3(t5)> g) {
        return juniper::function<t4(t5)>([=](t5 x) mutable -> t4 { 
            return f(g(x));
         });
    }
}

namespace Prelude {
    template<typename t14, typename t15, typename t13>
    juniper::function<juniper::function<t13(t15)>(t14)> curry(juniper::function<t13(t14,t15)> f) {
        return juniper::function<juniper::function<t13(t15)>(t14)>([=](t14 valueA) mutable -> juniper::function<t13(t15)> { 
            return juniper::function<t13(t15)>([=](t15 valueB) mutable -> t13 { 
                return f(valueA, valueB);
             });
         });
    }
}

namespace Prelude {
    template<typename t24, typename t25, typename t26, typename t23>
    juniper::function<juniper::function<juniper::function<t23(t26)>(t25)>(t24)> curry3(juniper::function<t23(t24,t25,t26)> f) {
        return juniper::function<juniper::function<juniper::function<t23(t26)>(t25)>(t24)>([=](t24 valueA) mutable -> juniper::function<juniper::function<t23(t26)>(t25)> { 
            return juniper::function<juniper::function<t23(t26)>(t25)>([=](t25 valueB) mutable -> juniper::function<t23(t26)> { 
                return juniper::function<t23(t26)>([=](t26 valueC) mutable -> t23 { 
                    return f(valueA, valueB, valueC);
                 });
             });
         });
    }
}

namespace Prelude {
    template<typename t32>
    bool equal(t32 x, t32 y) {
        return (x == y);
    }
}

namespace Prelude {
    template<typename t36, typename t37>
    t36 fst(Prelude::tuple2<t36,t37> tup) {
        return (([&]() -> t36 {
            auto guid0 = tup;
            return (true ? 
                (([&]() -> t36 {
                    auto x = (guid0).e1;
                    return x;
                })())
            :
                juniper::quit<t36>());
        })());
    }
}

namespace Prelude {
    template<typename t41, typename t42>
    t42 snd(Prelude::tuple2<t41,t42> tup) {
        return (([&]() -> t42 {
            auto guid1 = tup;
            return (true ? 
                (([&]() -> t42 {
                    auto x = (guid1).e2;
                    return x;
                })())
            :
                juniper::quit<t42>());
        })());
    }
}

namespace Prelude {
    template<typename t44>
    t44 add(t44 numA, t44 numB) {
        return (numA + numB);
    }
}

namespace Prelude {
    template<typename t46>
    t46 sub(t46 numA, t46 numB) {
        return (numA - numB);
    }
}

namespace Prelude {
    template<typename t48>
    t48 mul(t48 numA, t48 numB) {
        return (numA * numB);
    }
}

namespace Prelude {
    template<typename t50>
    t50 div(t50 numA, t50 numB) {
        return (numA / numB);
    }
}

namespace List {
    template<typename t52, typename t53, int c1>
    Prelude::list<t53, c1> map(juniper::function<t53(t52)> f, Prelude::list<t52, c1> lst) {
        return (([&]() -> Prelude::list<t53, c1> {
            auto n = c1;
            return (([&]() -> Prelude::list<t53, c1> {
                auto guid2 = (juniper::array<t53, c1>());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto ret = guid2;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid3 = 0;
                    uint32_t guid4 = ((lst).length - 1);
                    for (uint32_t i = guid3; i <= guid4; i++) {
                        ((ret)[i] = f(((lst).data)[i]));
                    }
                    return {};
                })());
                return (([&]() -> Prelude::list<t53, c1>{
                    Prelude::list<t53, c1> guid5;
                    guid5.data = ret;
                    guid5.length = (lst).length;
                    return guid5;
                })());
            })());
        })());
    }
}

namespace List {
    template<typename t62, typename t63, int c4>
    t63 foldl(juniper::function<t63(t62,t63)> f, t63 initState, Prelude::list<t62, c4> lst) {
        return (([&]() -> t63 {
            auto n = c4;
            return (([&]() -> t63 {
                auto guid6 = initState;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto s = guid6;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid7 = 0;
                    uint32_t guid8 = ((lst).length - 1);
                    for (uint32_t i = guid7; i <= guid8; i++) {
                        (s = f(((lst).data)[i], s));
                    }
                    return {};
                })());
                return s;
            })());
        })());
    }
}

namespace List {
    template<typename t71, typename t72, int c6>
    t72 foldr(juniper::function<t72(t71,t72)> f, t72 initState, Prelude::list<t71, c6> lst) {
        return (([&]() -> t72 {
            auto n = c6;
            return (([&]() -> t72 {
                auto guid9 = initState;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto s = guid9;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid10 = ((lst).length - 1);
                    uint32_t guid11 = 0;
                    for (uint32_t i = guid10; i >= guid11; i--) {
                        (s = f(((lst).data)[i], s));
                    }
                    return {};
                })());
                return s;
            })());
        })());
    }
}

namespace List {
    template<typename t80, int c8, int c9, int c10>
    Prelude::list<t80, c10> append(Prelude::list<t80, c8> lstA, Prelude::list<t80, c9> lstB) {
        return (([&]() -> Prelude::list<t80, c10> {
            auto aCap = c8;
            auto bCap = c9;
            auto retCap = c10;
            return (([&]() -> Prelude::list<t80, c10> {
                auto guid12 = 0;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto j = guid12;
                
                auto guid13 = (([&]() -> Prelude::list<t80, c10>{
                    Prelude::list<t80, c10> guid14;
                    guid14.data = (juniper::array<t80, c10>());
                    guid14.length = ((lstA).length + (lstB).length);
                    return guid14;
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto out = guid13;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid15 = 0;
                    uint32_t guid16 = ((lstA).length - 1);
                    for (uint32_t i = guid15; i <= guid16; i++) {
                        (([&]() -> Prelude::unit {
                            (((out).data)[j] = ((lstA).data)[i]);
                            (j = (j + 1));
                            return Prelude::unit();
                        })());
                    }
                    return {};
                })());
                (([&]() -> Prelude::unit {
                    uint32_t guid17 = 0;
                    uint32_t guid18 = ((lstB).length - 1);
                    for (uint32_t i = guid17; i <= guid18; i++) {
                        (([&]() -> Prelude::unit {
                            (((out).data)[j] = ((lstB).data)[i]);
                            (j = (j + 1));
                            return Prelude::unit();
                        })());
                    }
                    return {};
                })());
                return out;
            })());
        })());
    }
}

namespace List {
    template<typename t96, int c16>
    t96 nth(uint32_t i, Prelude::list<t96, c16> lst) {
        return (([&]() -> t96 {
            auto n = c16;
            return ((i < (lst).length) ? 
                (([&]() -> t96 {
                    auto guid19 = lst;
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    auto data = (guid19).data;
                    
                    return (data)[i];
                })())
            :
                juniper::quit<t96>());
        })());
    }
}

namespace List {
    template<typename t98, int c17, int c18>
    Prelude::list<t98, (c17)*(c18)> flattenSafe(Prelude::list<Prelude::list<t98, c17>, c18> listOfLists) {
        return (([&]() -> Prelude::list<t98, (c17)*(c18)> {
            auto m = c17;
            auto n = c18;
            return (([&]() -> Prelude::list<t98, (c17)*(c18)> {
                auto guid20 = (juniper::array<t98, (c17)*(c18)>());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto ret = guid20;
                
                auto guid21 = 0;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto index = guid21;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid22 = 0;
                    uint32_t guid23 = ((listOfLists).length - 1);
                    for (uint32_t i = guid22; i <= guid23; i++) {
                        (([&]() -> Prelude::unit {
                            uint32_t guid24 = 0;
                            uint32_t guid25 = ((((listOfLists).data)[i]).length - 1);
                            for (uint32_t j = guid24; j <= guid25; j++) {
                                (([&]() -> int32_t {
                                    ((ret)[index] = ((((listOfLists).data)[i]).data)[j]);
                                    return (index = (index + 1));
                                })());
                            }
                            return {};
                        })());
                    }
                    return {};
                })());
                return (([&]() -> Prelude::list<t98, (c17)*(c18)>{
                    Prelude::list<t98, (c17)*(c18)> guid26;
                    guid26.data = ret;
                    guid26.length = index;
                    return guid26;
                })());
            })());
        })());
    }
}

namespace List {
    template<typename t109, int c23, int c24>
    Prelude::list<t109, c24> resize(Prelude::list<t109, c23> lst) {
        return (([&]() -> Prelude::list<t109, c24> {
            auto n = c23;
            auto m = c24;
            return (([&]() -> Prelude::list<t109, c24> {
                auto guid27 = (juniper::array<t109, c24>());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto ret = guid27;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid28 = 0;
                    uint32_t guid29 = ((lst).length - 1);
                    for (uint32_t i = guid28; i <= guid29; i++) {
                        ((ret)[i] = ((lst).data)[i]);
                    }
                    return {};
                })());
                return (([&]() -> Prelude::list<t109, c24>{
                    Prelude::list<t109, c24> guid30;
                    guid30.data = ret;
                    guid30.length = (lst).length;
                    return guid30;
                })());
            })());
        })());
    }
}

namespace List {
    template<typename t116, int c27>
    bool all(juniper::function<bool(t116)> pred, Prelude::list<t116, c27> lst) {
        return (([&]() -> bool {
            auto n = c27;
            return (([&]() -> bool {
                auto guid31 = true;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto satisfied = guid31;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid32 = 0;
                    uint32_t guid33 = ((lst).length - 1);
                    for (uint32_t i = guid32; i <= guid33; i++) {
                        (satisfied ? 
                            (([&]() -> Prelude::unit {
                                (satisfied = pred(((lst).data)[i]));
                                return Prelude::unit();
                            })())
                        :
                            Prelude::unit());
                    }
                    return {};
                })());
                return satisfied;
            })());
        })());
    }
}

namespace List {
    template<typename t123, int c29>
    bool any(juniper::function<bool(t123)> pred, Prelude::list<t123, c29> lst) {
        return (([&]() -> bool {
            auto n = c29;
            return (([&]() -> bool {
                auto guid34 = false;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto satisfied = guid34;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid35 = 0;
                    uint32_t guid36 = ((lst).length - 1);
                    for (uint32_t i = guid35; i <= guid36; i++) {
                        (!(satisfied) ? 
                            (([&]() -> Prelude::unit {
                                (satisfied = pred(((lst).data)[i]));
                                return Prelude::unit();
                            })())
                        :
                            Prelude::unit());
                    }
                    return {};
                })());
                return satisfied;
            })());
        })());
    }
}

namespace List {
    template<typename t130, int c31>
    Prelude::list<t130, c31> pushBack(t130 elem, Prelude::list<t130, c31> lst) {
        return (([&]() -> Prelude::list<t130, c31> {
            auto n = c31;
            return (((lst).length >= n) ? 
                juniper::quit<Prelude::list<t130, c31>>()
            :
                (([&]() -> Prelude::list<t130, c31> {
                    auto guid37 = lst;
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    auto ret = guid37;
                    
                    (((ret).data)[(lst).length] = elem);
                    ((ret).length = ((lst).length + 1));
                    return ret;
                })()));
        })());
    }
}

namespace List {
    template<typename t138, int c33>
    Prelude::list<t138, c33> pushOffFront(t138 elem, Prelude::list<t138, c33> lst) {
        return (([&]() -> Prelude::list<t138, c33> {
            auto n = c33;
            return (([&]() -> Prelude::list<t138, c33> {
                auto guid38 = lst;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto ret = guid38;
                
                (([&]() -> Prelude::unit {
                    int32_t guid39 = (n - 2);
                    int32_t guid40 = 0;
                    for (int32_t i = guid39; i >= guid40; i--) {
                        (((ret).data)[(i + 1)] = ((ret).data)[i]);
                    }
                    return {};
                })());
                (((ret).data)[0] = elem);
                return (((ret).length == n) ? 
                    ret
                :
                    (([&]() -> Prelude::list<t138, c33> {
                        ((ret).length = ((lst).length + 1));
                        return ret;
                    })()));
            })());
        })());
    }
}

namespace List {
    template<typename t149, int c37>
    Prelude::list<t149, c37> setNth(uint32_t index, t149 elem, Prelude::list<t149, c37> lst) {
        return (([&]() -> Prelude::list<t149, c37> {
            auto n = c37;
            return (((lst).length <= index) ? 
                juniper::quit<Prelude::list<t149, c37>>()
            :
                (([&]() -> Prelude::list<t149, c37> {
                    auto guid41 = lst;
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    auto ret = guid41;
                    
                    (((ret).data)[index] = elem);
                    return ret;
                })()));
        })());
    }
}

namespace List {
    template<typename t154, int c39>
    Prelude::list<t154, c39> replicate(uint32_t numOfElements, t154 elem) {
        return (([&]() -> Prelude::list<t154, c39> {
            auto n = c39;
            return (([&]() -> Prelude::list<t154, c39>{
                Prelude::list<t154, c39> guid42;
                guid42.data = (juniper::array<t154, c39>().fill(elem));
                guid42.length = numOfElements;
                return guid42;
            })());
        })());
    }
}

namespace List {
    template<typename t156, int c40>
    Prelude::list<t156, c40> remove(t156 elem, Prelude::list<t156, c40> lst) {
        return (([&]() -> Prelude::list<t156, c40> {
            auto n = c40;
            return (([&]() -> Prelude::list<t156, c40> {
                auto guid43 = 0;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto index = guid43;
                
                auto guid44 = false;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto found = guid44;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid45 = 0;
                    uint32_t guid46 = ((lst).length - 1);
                    for (uint32_t i = guid45; i <= guid46; i++) {
                        ((!(found) && (((lst).data)[i] == elem)) ? 
                            (([&]() -> Prelude::unit {
                                (index = i);
                                (found = true);
                                return Prelude::unit();
                            })())
                        :
                            Prelude::unit());
                    }
                    return {};
                })());
                return (found ? 
                    (([&]() -> Prelude::list<t156, c40> {
                        auto guid47 = lst;
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        auto ret = guid47;
                        
                        ((ret).length = ((lst).length - 1));
                        (([&]() -> Prelude::unit {
                            uint32_t guid48 = index;
                            uint32_t guid49 = ((lst).length - 2);
                            for (uint32_t i = guid48; i <= guid49; i++) {
                                (((ret).data)[i] = ((lst).data)[(i + 1)]);
                            }
                            return {};
                        })());
                        return ret;
                    })())
                :
                    lst);
            })());
        })());
    }
}

namespace List {
    template<typename t168, int c44>
    Prelude::list<t168, c44> dropLast(Prelude::list<t168, c44> lst) {
        return (([&]() -> Prelude::list<t168, c44> {
            auto n = c44;
            return (((lst).length == 0) ? 
                juniper::quit<Prelude::list<t168, c44>>()
            :
                (([&]() -> Prelude::list<t168, c44>{
                    Prelude::list<t168, c44> guid50;
                    guid50.data = (lst).data;
                    guid50.length = ((lst).length - 1);
                    return guid50;
                })()));
        })());
    }
}

namespace List {
    template<typename t173, int c45>
    Prelude::unit foreach(juniper::function<Prelude::unit(t173)> f, Prelude::list<t173, c45> lst) {
        return (([&]() -> Prelude::unit {
            auto n = c45;
            return (([&]() -> Prelude::unit {
                uint32_t guid51 = 0;
                uint32_t guid52 = ((lst).length - 1);
                for (uint32_t i = guid51; i <= guid52; i++) {
                    f(((lst).data)[i]);
                }
                return {};
            })());
        })());
    }
}

namespace List {
    template<typename t181, int c48>
    t181 last(Prelude::list<t181, c48> lst) {
        return (([&]() -> t181 {
            auto n = c48;
            return (([&]() -> t181 {
                auto guid53 = lst;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto length = (guid53).length;
                auto data = (guid53).data;
                
                return (data)[(length - 1)];
            })());
        })());
    }
}

namespace List {
    template<typename t186, int c49>
    t186 max_(Prelude::list<t186, c49> lst) {
        return (([&]() -> t186 {
            auto n = c49;
            return ((((lst).length == 0) || (n == 0)) ? 
                juniper::quit<t186>()
            :
                (([&]() -> t186 {
                    auto guid54 = ((lst).data)[0];
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    auto maxVal = guid54;
                    
                    (([&]() -> Prelude::unit {
                        uint32_t guid55 = 1;
                        uint32_t guid56 = ((lst).length - 1);
                        for (uint32_t i = guid55; i <= guid56; i++) {
                            ((((lst).data)[i] > maxVal) ? 
                                (([&]() -> Prelude::unit {
                                    (maxVal = ((lst).data)[i]);
                                    return Prelude::unit();
                                })())
                            :
                                Prelude::unit());
                        }
                        return {};
                    })());
                    return maxVal;
                })()));
        })());
    }
}

namespace List {
    template<typename t196, int c53>
    t196 min_(Prelude::list<t196, c53> lst) {
        return (([&]() -> t196 {
            auto n = c53;
            return ((((lst).length == 0) || (n == 0)) ? 
                juniper::quit<t196>()
            :
                (([&]() -> t196 {
                    auto guid57 = ((lst).data)[0];
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    auto minVal = guid57;
                    
                    (([&]() -> Prelude::unit {
                        uint32_t guid58 = 1;
                        uint32_t guid59 = ((lst).length - 1);
                        for (uint32_t i = guid58; i <= guid59; i++) {
                            ((((lst).data)[i] < minVal) ? 
                                (([&]() -> Prelude::unit {
                                    (minVal = ((lst).data)[i]);
                                    return Prelude::unit();
                                })())
                            :
                                Prelude::unit());
                        }
                        return {};
                    })());
                    return minVal;
                })()));
        })());
    }
}

namespace List {
    template<typename t203, int c57>
    bool member(t203 elem, Prelude::list<t203, c57> lst) {
        return (([&]() -> bool {
            auto n = c57;
            return (([&]() -> bool {
                auto guid60 = false;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto found = guid60;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid61 = 0;
                    uint32_t guid62 = ((lst).length - 1);
                    for (uint32_t i = guid61; i <= guid62; i++) {
                        ((!(found) && (((lst).data)[i] == elem)) ? 
                            (([&]() -> Prelude::unit {
                                (found = true);
                                return Prelude::unit();
                            })())
                        :
                            Prelude::unit());
                    }
                    return {};
                })());
                return found;
            })());
        })());
    }
}

namespace List {
    template<typename t208, typename t209, int c59>
    Prelude::list<Prelude::tuple2<t208,t209>, c59> zip(Prelude::list<t208, c59> lstA, Prelude::list<t209, c59> lstB) {
        return (([&]() -> Prelude::list<Prelude::tuple2<t208,t209>, c59> {
            auto n = c59;
            return (((lstA).length == (lstB).length) ? 
                (([&]() -> Prelude::list<Prelude::tuple2<t208,t209>, c59> {
                    auto guid63 = (([&]() -> Prelude::list<Prelude::tuple2<t208,t209>, c59>{
                        Prelude::list<Prelude::tuple2<t208,t209>, c59> guid64;
                        guid64.data = (juniper::array<Prelude::tuple2<t208,t209>, c59>());
                        guid64.length = (lstA).length;
                        return guid64;
                    })());
                    if (!(true)) {
                        juniper::quit<Prelude::unit>();
                    }
                    auto ret = guid63;
                    
                    (([&]() -> Prelude::unit {
                        uint32_t guid65 = 0;
                        uint32_t guid66 = (lstA).length;
                        for (uint32_t i = guid65; i <= guid66; i++) {
                            (([&]() -> Prelude::unit {
                                (((ret).data)[i] = (Prelude::tuple2<t208,t209>{((lstA).data)[i], ((lstB).data)[i]}));
                                return Prelude::unit();
                            })());
                        }
                        return {};
                    })());
                    return ret;
                })())
            :
                juniper::quit<Prelude::list<Prelude::tuple2<t208,t209>, c59>>());
        })());
    }
}

namespace List {
    template<typename t221, typename t222, int c63>
    Prelude::tuple2<Prelude::list<t221, c63>,Prelude::list<t222, c63>> unzip(Prelude::list<Prelude::tuple2<t221,t222>, c63> lst) {
        return (([&]() -> Prelude::tuple2<Prelude::list<t221, c63>,Prelude::list<t222, c63>> {
            auto n = c63;
            return (([&]() -> Prelude::tuple2<Prelude::list<t221, c63>,Prelude::list<t222, c63>> {
                auto guid67 = (([&]() -> Prelude::list<t221, c63>{
                    Prelude::list<t221, c63> guid68;
                    guid68.data = (juniper::array<t221, c63>());
                    guid68.length = (lst).length;
                    return guid68;
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto retA = guid67;
                
                auto guid69 = (([&]() -> Prelude::list<t222, c63>{
                    Prelude::list<t222, c63> guid70;
                    guid70.data = (juniper::array<t222, c63>());
                    guid70.length = (lst).length;
                    return guid70;
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto retB = guid69;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid71 = 0;
                    uint32_t guid72 = ((lst).length - 1);
                    for (uint32_t i = guid71; i <= guid72; i++) {
                        (([&]() -> Prelude::unit {
                            retA.data[i] = lst.data[i].e1;
         retB.data[i] = lst.data[i].e2;
                            return {};
                        })());
                    }
                    return {};
                })());
                return (Prelude::tuple2<Prelude::list<t221, c63>,Prelude::list<t222, c63>>{retA, retB});
            })());
        })());
    }
}

namespace List {
    template<typename t227, int c64>
    t227 sum(Prelude::list<t227, c64> lst) {
        return (([&]() -> t227 {
            auto n = c64;
            return List::foldl<t227, t227, c64>(add<t227>, 0, lst);
        })());
    }
}

namespace List {
    template<typename t236, int c65>
    t236 average(Prelude::list<t236, c65> lst) {
        return (([&]() -> t236 {
            auto n = c65;
            return (sum<t236, c65>(lst) / (lst).length);
        })());
    }
}

namespace Signal {
    template<typename t238, typename t239>
    Prelude::sig<t239> map(juniper::function<t239(t238)> f, Prelude::sig<t238> s) {
        return (([&]() -> Prelude::sig<t239> {
            auto guid73 = s;
            return ((((guid73).tag == 0) && ((((guid73).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<t239> {
                    auto val = ((guid73).signal).just;
                    return signal<t239>(just<t239>(f(val)));
                })())
            :
                (true ? 
                    (([&]() -> Prelude::sig<t239> {
                        return signal<t239>(nothing<t239>());
                    })())
                :
                    juniper::quit<Prelude::sig<t239>>()));
        })());
    }
}

namespace Signal {
    template<typename t250>
    Prelude::unit sink(juniper::function<Prelude::unit(t250)> f, Prelude::sig<t250> s) {
        return (([&]() -> Prelude::unit {
            auto guid74 = s;
            return ((((guid74).tag == 0) && ((((guid74).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::unit {
                    auto val = ((guid74).signal).just;
                    return f(val);
                })())
            :
                (true ? 
                    (([&]() -> Prelude::unit {
                        return Prelude::unit();
                    })())
                :
                    juniper::quit<Prelude::unit>()));
        })());
    }
}

namespace Signal {
    template<typename t254>
    Prelude::sig<t254> filter(juniper::function<bool(t254)> f, Prelude::sig<t254> s) {
        return (([&]() -> Prelude::sig<t254> {
            auto guid75 = s;
            return ((((guid75).tag == 0) && ((((guid75).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<t254> {
                    auto val = ((guid75).signal).just;
                    return (f(val) ? 
                        signal<t254>(nothing<t254>())
                    :
                        s);
                })())
            :
                (true ? 
                    (([&]() -> Prelude::sig<t254> {
                        return signal<t254>(nothing<t254>());
                    })())
                :
                    juniper::quit<Prelude::sig<t254>>()));
        })());
    }
}

namespace Signal {
    template<typename t264>
    Prelude::sig<t264> merge(Prelude::sig<t264> sigA, Prelude::sig<t264> sigB) {
        return (([&]() -> Prelude::sig<t264> {
            auto guid76 = sigA;
            return ((((guid76).tag == 0) && ((((guid76).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<t264> {
                    return sigA;
                })())
            :
                (true ? 
                    (([&]() -> Prelude::sig<t264> {
                        return sigB;
                    })())
                :
                    juniper::quit<Prelude::sig<t264>>()));
        })());
    }
}

namespace Signal {
    template<typename t266, int c66>
    Prelude::sig<t266> mergeMany(Prelude::list<Prelude::sig<t266>, c66> sigs) {
        return (([&]() -> Prelude::sig<t266> {
            auto n = c66;
            return (([&]() -> Prelude::sig<t266> {
                auto guid77 = nothing<t266>();
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto ret = guid77;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid78 = 0;
                    uint32_t guid79 = (n - 1);
                    for (uint32_t i = guid78; i <= guid79; i++) {
                        (([&]() -> Prelude::unit {
                            auto guid80 = ret;
                            return ((((guid80).tag == 1) && true) ? 
                                (([&]() -> Prelude::unit {
                                    return (([&]() -> Prelude::unit {
                                        auto guid81 = List::nth<Prelude::sig<t266>, c66>(i, sigs);
                                        if (!((((guid81).tag == 0) && true))) {
                                            juniper::quit<Prelude::unit>();
                                        }
                                        auto heldValue = (guid81).signal;
                                        
                                        (ret = heldValue);
                                        return Prelude::unit();
                                    })());
                                })())
                            :
                                (true ? 
                                    (([&]() -> Prelude::unit {
                                        return Prelude::unit();
                                    })())
                                :
                                    juniper::quit<Prelude::unit>()));
                        })());
                    }
                    return {};
                })());
                return signal<t266>(ret);
            })());
        })());
    }
}

namespace Signal {
    template<typename t274, typename t275>
    Prelude::sig<Prelude::either<t274, t275>> join(Prelude::sig<t274> sigA, Prelude::sig<t275> sigB) {
        return (([&]() -> Prelude::sig<Prelude::either<t274, t275>> {
            auto guid82 = (Prelude::tuple2<Prelude::sig<t274>,Prelude::sig<t275>>{sigA, sigB});
            return (((((guid82).e1).tag == 0) && (((((guid82).e1).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<Prelude::either<t274, t275>> {
                    auto value = (((guid82).e1).signal).just;
                    return signal<Prelude::either<t274, t275>>(just<Prelude::either<t274, t275>>(left<t274, t275>(value)));
                })())
            :
                (((((guid82).e2).tag == 0) && (((((guid82).e2).signal).tag == 0) && true)) ? 
                    (([&]() -> Prelude::sig<Prelude::either<t274, t275>> {
                        auto value = (((guid82).e2).signal).just;
                        return signal<Prelude::either<t274, t275>>(just<Prelude::either<t274, t275>>(right<t274, t275>(value)));
                    })())
                :
                    (true ? 
                        (([&]() -> Prelude::sig<Prelude::either<t274, t275>> {
                            return signal<Prelude::either<t274, t275>>(nothing<Prelude::either<t274, t275>>());
                        })())
                    :
                        juniper::quit<Prelude::sig<Prelude::either<t274, t275>>>())));
        })());
    }
}

namespace Signal {
    template<typename t297>
    Prelude::sig<Prelude::unit> toUnit(Prelude::sig<t297> s) {
        return map<t297, Prelude::unit>(juniper::function<Prelude::unit(t297)>([=](t297 x) mutable -> Prelude::unit { 
            return Prelude::unit();
         }), s);
    }
}

namespace Signal {
    template<typename t302, typename t308>
    Prelude::sig<t308> foldP(juniper::function<t308(t302,t308)> f, juniper::shared_ptr<t308> state0, Prelude::sig<t302> incoming) {
        return (([&]() -> Prelude::sig<t308> {
            auto guid83 = incoming;
            return ((((guid83).tag == 0) && ((((guid83).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<t308> {
                    auto val = ((guid83).signal).just;
                    return (([&]() -> Prelude::sig<t308> {
                        auto guid84 = f(val, (*((state0).get())));
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        auto state1 = guid84;
                        
                        (*((t308*) (state0.get())) = state1);
                        return signal<t308>(just<t308>(state1));
                    })());
                })())
            :
                (true ? 
                    (([&]() -> Prelude::sig<t308> {
                        return signal<t308>(nothing<t308>());
                    })())
                :
                    juniper::quit<Prelude::sig<t308>>()));
        })());
    }
}

namespace Signal {
    template<typename t318>
    Prelude::sig<t318> dropRepeats(Prelude::sig<t318> incoming, juniper::shared_ptr<Prelude::maybe<t318>> maybePrevValue) {
        return filter<t318>(juniper::function<bool(t318)>([=](t318 value) mutable -> bool { 
            return (([&]() -> bool {
                auto guid85 = (([&]() -> bool {
                    auto guid86 = (*((maybePrevValue).get()));
                    return ((((guid86).tag == 1) && true) ? 
                        (([&]() -> bool {
                            return false;
                        })())
                    :
                        ((((guid86).tag == 0) && true) ? 
                            (([&]() -> bool {
                                auto prevValue = (guid86).just;
                                return (value == prevValue);
                            })())
                        :
                            juniper::quit<bool>()));
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto filtered = guid85;
                
                (!(filtered) ? 
                    (([&]() -> Prelude::unit {
                        (*((Prelude::maybe<t318>*) (maybePrevValue.get())) = just<t318>(value));
                        return Prelude::unit();
                    })())
                :
                    Prelude::unit());
                return filtered;
            })());
         }), incoming);
    }
}

namespace Signal {
    template<typename t328>
    Prelude::sig<t328> latch(Prelude::sig<t328> incoming, juniper::shared_ptr<t328> prevValue) {
        return (([&]() -> Prelude::sig<t328> {
            auto guid87 = incoming;
            return ((((guid87).tag == 0) && ((((guid87).signal).tag == 0) && true)) ? 
                (([&]() -> Prelude::sig<t328> {
                    auto val = ((guid87).signal).just;
                    return (([&]() -> Prelude::sig<t328> {
                        (*((t328*) (prevValue.get())) = val);
                        return incoming;
                    })());
                })())
            :
                (true ? 
                    (([&]() -> Prelude::sig<t328> {
                        return signal<t328>(just<t328>((*((prevValue).get()))));
                    })())
                :
                    juniper::quit<Prelude::sig<t328>>()));
        })());
    }
}

namespace Signal {
    template<typename t339, typename t342, typename t337>
    Prelude::sig<t337> map2(juniper::function<t337(t339,t342)> f, Prelude::sig<t339> incomingA, Prelude::sig<t342> incomingB, juniper::shared_ptr<Prelude::tuple2<t339,t342>> state) {
        return (([&]() -> Prelude::sig<t337> {
            auto guid88 = (([&]() -> t339 {
                auto guid89 = incomingA;
                return ((((guid89).tag == 0) && ((((guid89).signal).tag == 0) && true)) ? 
                    (([&]() -> t339 {
                        auto val1 = ((guid89).signal).just;
                        return val1;
                    })())
                :
                    (true ? 
                        (([&]() -> t339 {
                            return fst<t339, t342>((*((state).get())));
                        })())
                    :
                        juniper::quit<t339>()));
            })());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto valA = guid88;
            
            auto guid90 = (([&]() -> t342 {
                auto guid91 = incomingB;
                return ((((guid91).tag == 0) && ((((guid91).signal).tag == 0) && true)) ? 
                    (([&]() -> t342 {
                        auto val2 = ((guid91).signal).just;
                        return val2;
                    })())
                :
                    (true ? 
                        (([&]() -> t342 {
                            return snd<t339, t342>((*((state).get())));
                        })())
                    :
                        juniper::quit<t342>()));
            })());
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto valB = guid90;
            
            (*((Prelude::tuple2<t339,t342>*) (state.get())) = (Prelude::tuple2<t339,t342>{valA, valB}));
            return (([&]() -> Prelude::sig<t337> {
                auto guid92 = (Prelude::tuple2<Prelude::sig<t339>,Prelude::sig<t342>>{incomingA, incomingB});
                return (((((guid92).e2).tag == 0) && (((((guid92).e2).signal).tag == 1) && ((((guid92).e1).tag == 0) && (((((guid92).e1).signal).tag == 1) && true)))) ? 
                    (([&]() -> Prelude::sig<t337> {
                        return signal<t337>(nothing<t337>());
                    })())
                :
                    (true ? 
                        (([&]() -> Prelude::sig<t337> {
                            return signal<t337>(just<t337>(f(valA, valB)));
                        })())
                    :
                        juniper::quit<Prelude::sig<t337>>()));
            })());
        })());
    }
}

namespace Signal {
    template<typename t358, int c67>
    Prelude::sig<Prelude::list<t358, c67>> record(Prelude::sig<t358> incoming, juniper::shared_ptr<Prelude::list<t358, c67>> pastValues) {
        return (([&]() -> Prelude::sig<Prelude::list<t358, c67>> {
            auto n = c67;
            return foldP<t358, Prelude::list<t358, c67>>(List::pushOffFront<t358, c67>, pastValues, incoming);
        })());
    }
}

namespace Io {
    Io::pinState toggle(Io::pinState p) {
        return (([&]() -> Io::pinState {
            auto guid93 = p;
            return ((((guid93).tag == 0) && true) ? 
                (([&]() -> Io::pinState {
                    return low();
                })())
            :
                ((((guid93).tag == 1) && true) ? 
                    (([&]() -> Io::pinState {
                        return high();
                    })())
                :
                    juniper::quit<Io::pinState>()));
        })());
    }
}

namespace Io {
    template<int c68>
    Prelude::unit printStr(Prelude::string<c68> str) {
        return (([&]() -> Prelude::unit {
            auto n = c68;
            return (([&]() -> Prelude::unit {
                Serial.print(&str.characters.data);
                return {};
            })());
        })());
    }
}

namespace Io {
    Prelude::unit printFloat(float f) {
        return (([&]() -> Prelude::unit {
            Serial.print(f);
            return {};
        })());
    }
}

namespace Io {
    Prelude::unit beginSerial(uint32_t speed) {
        return (([&]() -> Prelude::unit {
            Serial.begin(speed);
            return {};
        })());
    }
}

namespace Io {
    int32_t pinStateToInt(Io::pinState value) {
        return (([&]() -> int32_t {
            auto guid94 = value;
            return ((((guid94).tag == 1) && true) ? 
                (([&]() -> int32_t {
                    return 0;
                })())
            :
                ((((guid94).tag == 0) && true) ? 
                    (([&]() -> int32_t {
                        return 1;
                    })())
                :
                    juniper::quit<int32_t>()));
        })());
    }
}

namespace Io {
    Io::pinState intToPinState(uint8_t value) {
        return ((value == 0) ? 
            low()
        :
            high());
    }
}

namespace Io {
    Prelude::unit digWrite(uint16_t pin, Io::pinState value) {
        return (([&]() -> Prelude::unit {
            auto guid95 = pinStateToInt(value);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto intVal = guid95;
            
            return (([&]() -> Prelude::unit {
                digitalWrite(pin, intVal);
                return {};
            })());
        })());
    }
}

namespace Io {
    Io::pinState digRead(uint16_t pin) {
        return (([&]() -> Io::pinState {
            auto guid96 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto intVal = guid96;
            
            (([&]() -> Prelude::unit {
                intVal = digitalRead(pin);
                return {};
            })());
            return intToPinState(intVal);
        })());
    }
}

namespace Io {
    Prelude::sig<Io::pinState> digIn(uint16_t pin) {
        return signal<Io::pinState>(just<Io::pinState>(digRead(pin)));
    }
}

namespace Io {
    Prelude::unit digOut(uint16_t pin, Prelude::sig<Io::pinState> sig) {
        return Signal::sink<Io::pinState>(juniper::function<Prelude::unit(Io::pinState)>([=](Io::pinState value) mutable -> Prelude::unit { 
            return digWrite(pin, value);
         }), sig);
    }
}

namespace Io {
    int32_t anaRead(uint16_t pin) {
        return (([&]() -> int32_t {
            auto guid97 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto value = guid97;
            
            (([&]() -> Prelude::unit {
                value = analogRead(pin);
                return {};
            })());
            return value;
        })());
    }
}

namespace Io {
    Prelude::unit anaWrite(uint16_t pin, uint8_t value) {
        return (([&]() -> Prelude::unit {
            analogWrite(pin, value);
            return {};
        })());
    }
}

namespace Io {
    Prelude::sig<uint16_t> anaIn(uint16_t pin) {
        return signal<uint16_t>(just<uint16_t>(anaRead(pin)));
    }
}

namespace Io {
    Prelude::unit anaOut(uint16_t pin, Prelude::sig<uint16_t> sig) {
        return Signal::sink<uint16_t>(juniper::function<Prelude::unit(uint16_t)>([=](uint16_t value) mutable -> Prelude::unit { 
            return anaWrite(pin, value);
         }), sig);
    }
}

namespace Io {
    int32_t pinModeToInt(Io::mode m) {
        return (([&]() -> int32_t {
            auto guid98 = m;
            return ((((guid98).tag == 0) && true) ? 
                (([&]() -> int32_t {
                    return 0;
                })())
            :
                ((((guid98).tag == 1) && true) ? 
                    (([&]() -> int32_t {
                        return 1;
                    })())
                :
                    ((((guid98).tag == 2) && true) ? 
                        (([&]() -> int32_t {
                            return 2;
                        })())
                    :
                        juniper::quit<int32_t>())));
        })());
    }
}

namespace Io {
    Io::mode intToPinMode(uint8_t m) {
        return (([&]() -> Io::mode {
            auto guid99 = m;
            return (((guid99 == 0) && true) ? 
                (([&]() -> Io::mode {
                    return input();
                })())
            :
                (((guid99 == 1) && true) ? 
                    (([&]() -> Io::mode {
                        return output();
                    })())
                :
                    (((guid99 == 2) && true) ? 
                        (([&]() -> Io::mode {
                            return inputPullup();
                        })())
                    :
                        juniper::quit<Io::mode>())));
        })());
    }
}

namespace Io {
    Prelude::unit setPinMode(uint16_t pin, Io::mode m) {
        return (([&]() -> Prelude::unit {
            auto guid100 = pinModeToInt(m);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto m2 = guid100;
            
            return (([&]() -> Prelude::unit {
                pinMode(pin, m2);
                return {};
            })());
        })());
    }
}

namespace Io {
    Prelude::sig<Prelude::unit> risingEdge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState) {
        return Signal::toUnit<Io::pinState>(Signal::filter<Io::pinState>(juniper::function<bool(Io::pinState)>([=](Io::pinState currState) mutable -> bool { 
            return (([&]() -> bool {
                auto guid101 = (([&]() -> bool {
                    auto guid102 = (Prelude::tuple2<Io::pinState,Io::pinState>{currState, (*((prevState).get()))});
                    return (((((guid102).e2).tag == 1) && ((((guid102).e1).tag == 0) && true)) ? 
                        (([&]() -> bool {
                            return false;
                        })())
                    :
                        (true ? 
                            (([&]() -> bool {
                                return true;
                            })())
                        :
                            juniper::quit<bool>()));
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto ret = guid101;
                
                (*((Io::pinState*) (prevState.get())) = currState);
                return ret;
            })());
         }), sig));
    }
}

namespace Io {
    Prelude::sig<Prelude::unit> fallingEdge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState) {
        return Signal::toUnit<Io::pinState>(Signal::filter<Io::pinState>(juniper::function<bool(Io::pinState)>([=](Io::pinState currState) mutable -> bool { 
            return (([&]() -> bool {
                auto guid103 = (([&]() -> bool {
                    auto guid104 = (Prelude::tuple2<Io::pinState,Io::pinState>{currState, (*((prevState).get()))});
                    return (((((guid104).e2).tag == 0) && ((((guid104).e1).tag == 1) && true)) ? 
                        (([&]() -> bool {
                            return false;
                        })())
                    :
                        (true ? 
                            (([&]() -> bool {
                                return true;
                            })())
                        :
                            juniper::quit<bool>()));
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto ret = guid103;
                
                (*((Io::pinState*) (prevState.get())) = currState);
                return ret;
            })());
         }), sig));
    }
}

namespace Io {
    Prelude::sig<Io::pinState> edge(Prelude::sig<Io::pinState> sig, juniper::shared_ptr<Io::pinState> prevState) {
        return Signal::filter<Io::pinState>(juniper::function<bool(Io::pinState)>([=](Io::pinState currState) mutable -> bool { 
            return (([&]() -> bool {
                auto guid105 = (([&]() -> bool {
                    auto guid106 = (Prelude::tuple2<Io::pinState,Io::pinState>{currState, (*((prevState).get()))});
                    return (((((guid106).e2).tag == 1) && ((((guid106).e1).tag == 0) && true)) ? 
                        (([&]() -> bool {
                            return false;
                        })())
                    :
                        (((((guid106).e2).tag == 0) && ((((guid106).e1).tag == 1) && true)) ? 
                            (([&]() -> bool {
                                return false;
                            })())
                        :
                            (true ? 
                                (([&]() -> bool {
                                    return true;
                                })())
                            :
                                juniper::quit<bool>())));
                })());
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto ret = guid105;
                
                (*((Io::pinState*) (prevState.get())) = currState);
                return ret;
            })());
         }), sig);
    }
}

namespace Maybe {
    template<typename t453, typename t454>
    Prelude::maybe<t454> map(juniper::function<t454(t453)> f, Prelude::maybe<t453> maybeVal) {
        return (([&]() -> Prelude::maybe<t454> {
            auto guid107 = maybeVal;
            return ((((guid107).tag == 0) && true) ? 
                (([&]() -> Prelude::maybe<t454> {
                    auto val = (guid107).just;
                    return just<t454>(f(val));
                })())
            :
                (true ? 
                    (([&]() -> Prelude::maybe<t454> {
                        return nothing<t454>();
                    })())
                :
                    juniper::quit<Prelude::maybe<t454>>()));
        })());
    }
}

namespace Maybe {
    template<typename t461>
    t461 get(Prelude::maybe<t461> maybeVal) {
        return (([&]() -> t461 {
            auto guid108 = maybeVal;
            return ((((guid108).tag == 0) && true) ? 
                (([&]() -> t461 {
                    auto val = (guid108).just;
                    return val;
                })())
            :
                juniper::quit<t461>());
        })());
    }
}

namespace Maybe {
    template<typename t463>
    bool isJust(Prelude::maybe<t463> maybeVal) {
        return (([&]() -> bool {
            auto guid109 = maybeVal;
            return ((((guid109).tag == 0) && true) ? 
                (([&]() -> bool {
                    return true;
                })())
            :
                (true ? 
                    (([&]() -> bool {
                        return false;
                    })())
                :
                    juniper::quit<bool>()));
        })());
    }
}

namespace Maybe {
    template<typename t465>
    bool isNothing(Prelude::maybe<t465> maybeVal) {
        return !(isJust<t465>(maybeVal));
    }
}

namespace Maybe {
    template<typename t469>
    int32_t count(Prelude::maybe<t469> maybeVal) {
        return (([&]() -> int32_t {
            auto guid110 = maybeVal;
            return ((((guid110).tag == 0) && true) ? 
                (([&]() -> int32_t {
                    return 1;
                })())
            :
                (true ? 
                    (([&]() -> int32_t {
                        return 0;
                    })())
                :
                    juniper::quit<int32_t>()));
        })());
    }
}

namespace Maybe {
    template<typename t471, typename t472>
    t472 foldl(juniper::function<t472(t471,t472)> f, t472 initState, Prelude::maybe<t471> maybeVal) {
        return (([&]() -> t472 {
            auto guid111 = maybeVal;
            return ((((guid111).tag == 0) && true) ? 
                (([&]() -> t472 {
                    auto val = (guid111).just;
                    return f(val, initState);
                })())
            :
                (true ? 
                    (([&]() -> t472 {
                        return initState;
                    })())
                :
                    juniper::quit<t472>()));
        })());
    }
}

namespace Maybe {
    template<typename t477, typename t478>
    t478 fodlr(juniper::function<t478(t477,t478)> f, t478 initState, Prelude::maybe<t477> maybeVal) {
        return foldl<t477, t478>(f, initState, maybeVal);
    }
}

namespace Maybe {
    template<typename t484>
    Prelude::unit iter(juniper::function<Prelude::unit(t484)> f, Prelude::maybe<t484> maybeVal) {
        return (([&]() -> Prelude::unit {
            auto guid112 = maybeVal;
            return ((((guid112).tag == 0) && true) ? 
                (([&]() -> Prelude::unit {
                    auto val = (guid112).just;
                    return f(val);
                })())
            :
                (true ? 
                    (([&]() -> Prelude::unit {
                        auto nothing = guid112;
                        return Prelude::unit();
                    })())
                :
                    juniper::quit<Prelude::unit>()));
        })());
    }
}

namespace Time {
    Prelude::unit wait(uint32_t time) {
        return (([&]() -> Prelude::unit {
            delay(time);
            return {};
        })());
    }
}

namespace Time {
    int32_t now() {
        return (([&]() -> int32_t {
            auto guid113 = 0;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid113;
            
            (([&]() -> Prelude::unit {
                ret = millis();
                return {};
            })());
            return ret;
        })());
    }
}

namespace Time {
    juniper::shared_ptr<Time::timerState> state() {
        return (juniper::shared_ptr<Time::timerState>(new Time::timerState((([&]() -> Time::timerState{
            Time::timerState guid114;
            guid114.lastPulse = 0;
            return guid114;
        })()))));
    }
}

namespace Time {
    Prelude::sig<uint32_t> every(uint32_t interval, juniper::shared_ptr<Time::timerState> state) {
        return (([&]() -> Prelude::sig<uint32_t> {
            auto guid115 = now();
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto t = guid115;
            
            auto guid116 = ((interval == 0) ? 
                t
            :
                ((t / interval) * interval));
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto lastWindow = guid116;
            
            return ((((*((state).get()))).lastPulse >= lastWindow) ? 
                signal<uint32_t>(nothing<uint32_t>())
            :
                (([&]() -> Prelude::sig<uint32_t> {
                    (*((Time::timerState*) (state.get())) = (([&]() -> Time::timerState{
                        Time::timerState guid117;
                        guid117.lastPulse = t;
                        return guid117;
                    })()));
                    return signal<uint32_t>(just<uint32_t>(t));
                })()));
        })());
    }
}

namespace Math {
    double pi = 3.141593;
}

namespace Math {
    double e = 2.718282;
}

namespace Math {
    double degToRad(double degrees) {
        return (degrees * 0.017453);
    }
}

namespace Math {
    double radToDeg(double radians) {
        return (radians * 57.295780);
    }
}

namespace Math {
    double acos_(double x) {
        return (([&]() -> double {
            auto guid118 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid118;
            
            (([&]() -> Prelude::unit {
                ret = acos(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double asin_(double x) {
        return (([&]() -> double {
            auto guid119 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid119;
            
            (([&]() -> Prelude::unit {
                ret = asin(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double atan_(double x) {
        return (([&]() -> double {
            auto guid120 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid120;
            
            (([&]() -> Prelude::unit {
                ret = atan(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double atan2_(double y, double x) {
        return (([&]() -> double {
            auto guid121 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid121;
            
            (([&]() -> Prelude::unit {
                ret = atan2(y, x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double cos_(double x) {
        return (([&]() -> double {
            auto guid122 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid122;
            
            (([&]() -> Prelude::unit {
                ret = cos(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double cosh_(double x) {
        return (([&]() -> double {
            auto guid123 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid123;
            
            (([&]() -> Prelude::unit {
                ret = cosh(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double sin_(double x) {
        return (([&]() -> double {
            auto guid124 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid124;
            
            (([&]() -> Prelude::unit {
                ret = sin(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double sinh_(double x) {
        return (([&]() -> double {
            auto guid125 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid125;
            
            (([&]() -> Prelude::unit {
                ret = sinh(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double tan_(double x) {
        return (sin_(x) / cos_(x));
    }
}

namespace Math {
    double tanh_(double x) {
        return (([&]() -> double {
            auto guid126 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid126;
            
            (([&]() -> Prelude::unit {
                ret = tanh(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double exp_(double x) {
        return (([&]() -> double {
            auto guid127 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid127;
            
            (([&]() -> Prelude::unit {
                ret = exp(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    Prelude::tuple2<double,int16_t> frexp_(double x) {
        return (([&]() -> Prelude::tuple2<double,int16_t> {
            auto guid128 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid128;
            
            auto guid129 = ((int16_t) 0);
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto exponent = guid129;
            
            (([&]() -> Prelude::unit {
                int exponent2 = (int) exponent;
    ret = frexp(x, &exponent2);
                return {};
            })());
            return (Prelude::tuple2<double,int16_t>{ret, exponent});
        })());
    }
}

namespace Math {
    double ldexp_(double x, int16_t exponent) {
        return (([&]() -> double {
            auto guid130 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid130;
            
            (([&]() -> Prelude::unit {
                ret = ldexp(x, exponent);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double log_(double x) {
        return (([&]() -> double {
            auto guid131 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid131;
            
            (([&]() -> Prelude::unit {
                ret = log(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double log10_(double x) {
        return (([&]() -> double {
            auto guid132 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid132;
            
            (([&]() -> Prelude::unit {
                ret = log10(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    Prelude::tuple2<double,double> modf_(double x) {
        return (([&]() -> Prelude::tuple2<double,double> {
            auto guid133 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid133;
            
            auto guid134 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto integer = guid134;
            
            (([&]() -> Prelude::unit {
                ret = modf(x, &integer);
                return {};
            })());
            return (Prelude::tuple2<double,double>{ret, integer});
        })());
    }
}

namespace Math {
    double pow_(double x, double y) {
        return (([&]() -> double {
            auto guid135 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid135;
            
            (([&]() -> Prelude::unit {
                ret = pow(x, y);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double sqrt_(double x) {
        return (([&]() -> double {
            auto guid136 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid136;
            
            (([&]() -> Prelude::unit {
                ret = sqrt(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double ceil_(double x) {
        return (([&]() -> double {
            auto guid137 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid137;
            
            (([&]() -> Prelude::unit {
                ret = ceil(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double fabs_(double x) {
        return (([&]() -> double {
            auto guid138 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid138;
            
            (([&]() -> Prelude::unit {
                ret = fabs(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double floor_(double x) {
        return (([&]() -> double {
            auto guid139 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid139;
            
            (([&]() -> Prelude::unit {
                ret = floor(x);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double fmod_(double x, double y) {
        return (([&]() -> double {
            auto guid140 = 0.000000;
            if (!(true)) {
                juniper::quit<Prelude::unit>();
            }
            auto ret = guid140;
            
            (([&]() -> Prelude::unit {
                ret = fmod(x, y);
                return {};
            })());
            return ret;
        })());
    }
}

namespace Math {
    double round_(double x) {
        return floor_((x + 0.500000));
    }
}

namespace Math {
    double min_(double x, double y) {
        return ((x > y) ? 
            y
        :
            x);
    }
}

namespace Math {
    double max_(double x, double y) {
        return ((x > y) ? 
            x
        :
            y);
    }
}

namespace Math {
    double mapRange(double x, double a1, double a2, double b1, double b2) {
        return (b1 + (((x - a1) * (b2 - b1)) / (a2 - a1)));
    }
}

namespace Math {
    template<typename t537>
    t537 clamp(t537 x, t537 min, t537 max) {
        return ((min > x) ? 
            min
        :
            ((x > max) ? 
                max
            :
                x));
    }
}

namespace Math {
    template<typename t539>
    int32_t sign(t539 n) {
        return ((n == 0) ? 
            0
        :
            ((n > 0) ? 
                1
            :
                -(1)));
    }
}

namespace Button {
    juniper::shared_ptr<Button::buttonState> state() {
        return (juniper::shared_ptr<Button::buttonState>(new Button::buttonState((([&]() -> Button::buttonState{
            Button::buttonState guid141;
            guid141.actualState = Io::low();
            guid141.lastState = Io::low();
            guid141.lastDebounceTime = 0;
            return guid141;
        })()))));
    }
}

namespace Button {
    Prelude::sig<Io::pinState> debounceDelay(Prelude::sig<Io::pinState> incoming, uint16_t delay, juniper::shared_ptr<Button::buttonState> buttonState) {
        return Signal::map<Io::pinState, Io::pinState>(juniper::function<Io::pinState(Io::pinState)>([=](Io::pinState currentState) mutable -> Io::pinState { 
            return (([&]() -> Io::pinState {
                auto guid142 = (*((buttonState).get()));
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto lastDebounceTime = (guid142).lastDebounceTime;
                auto lastState = (guid142).lastState;
                auto actualState = (guid142).actualState;
                
                return ((currentState != lastState) ? 
                    (([&]() -> Io::pinState {
                        (*((Button::buttonState*) (buttonState.get())) = (([&]() -> Button::buttonState{
                            Button::buttonState guid143;
                            guid143.actualState = actualState;
                            guid143.lastState = currentState;
                            guid143.lastDebounceTime = Time::now();
                            return guid143;
                        })()));
                        return actualState;
                    })())
                :
                    (((currentState != actualState) && ((Time::now() - ((*((buttonState).get()))).lastDebounceTime) > delay)) ? 
                        (([&]() -> Io::pinState {
                            (*((Button::buttonState*) (buttonState.get())) = (([&]() -> Button::buttonState{
                                Button::buttonState guid144;
                                guid144.actualState = currentState;
                                guid144.lastState = currentState;
                                guid144.lastDebounceTime = lastDebounceTime;
                                return guid144;
                            })()));
                            return currentState;
                        })())
                    :
                        (([&]() -> Io::pinState {
                            (*((Button::buttonState*) (buttonState.get())) = (([&]() -> Button::buttonState{
                                Button::buttonState guid145;
                                guid145.actualState = actualState;
                                guid145.lastState = currentState;
                                guid145.lastDebounceTime = lastDebounceTime;
                                return guid145;
                            })()));
                            return actualState;
                        })())));
            })());
         }), incoming);
    }
}

namespace Button {
    Prelude::sig<Io::pinState> debounce(Prelude::sig<Io::pinState> incoming, juniper::shared_ptr<Button::buttonState> buttonState) {
        return debounceDelay(incoming, 50, buttonState);
    }
}

namespace Vector {
    int32_t x = 0;
}

namespace Vector {
    int32_t y = 1;
}

namespace Vector {
    int32_t z = 2;
}

namespace Vector {
    template<typename t562, int c69>
    Vector::vector<t562, c69> make(juniper::array<t562, c69> d) {
        return (([&]() -> Vector::vector<t562, c69> {
            auto n = c69;
            return (([&]() -> Vector::vector<t562, c69>{
                Vector::vector<t562, c69> guid146;
                guid146.data = d;
                return guid146;
            })());
        })());
    }
}

namespace Vector {
    template<typename t565, int c71>
    t565 get(uint32_t i, Vector::vector<t565, c71> v) {
        return (([&]() -> t565 {
            auto n = c71;
            return (([&]() -> t565 {
                auto guid147 = v;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto data = (guid147).data;
                
                return (data)[i];
            })());
        })());
    }
}

namespace Vector {
    template<typename t567, int c72>
    Vector::vector<t567, c72> add(Vector::vector<t567, c72> v1, Vector::vector<t567, c72> v2) {
        return (([&]() -> Vector::vector<t567, c72> {
            auto n = c72;
            return (([&]() -> Vector::vector<t567, c72> {
                auto guid148 = v1;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto result = guid148;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid149 = 0;
                    uint32_t guid150 = (n - 1);
                    for (uint32_t i = guid149; i <= guid150; i++) {
                        (((result).data)[i] = (((result).data)[i] + ((v2).data)[i]()));
                    }
                    return {};
                })());
                return result;
            })());
        })());
    }
}

namespace Vector {
    template<typename t576, int c76>
    Vector::vector<t576, c76> zero() {
        return (([&]() -> Vector::vector<t576, c76> {
            auto n = c76;
            return (([&]() -> Vector::vector<t576, c76>{
                Vector::vector<t576, c76> guid151;
                guid151.data = (juniper::array<t576, c76>().fill(0));
                return guid151;
            })());
        })());
    }
}

namespace Vector {
    template<typename t578, int c77>
    Vector::vector<t578, c77> subtract(Vector::vector<t578, c77> v1, Vector::vector<t578, c77> v2) {
        return (([&]() -> Vector::vector<t578, c77> {
            auto n = c77;
            return (([&]() -> Vector::vector<t578, c77> {
                auto guid152 = v1;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto result = guid152;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid153 = 0;
                    uint32_t guid154 = (n - 1);
                    for (uint32_t i = guid153; i <= guid154; i++) {
                        (([&]() -> Prelude::unit {
                            (((result).data)[i] = (((result).data)[i] - ((v2).data)[i]));
                            return Prelude::unit();
                        })());
                    }
                    return {};
                })());
                return result;
            })());
        })());
    }
}

namespace Vector {
    template<typename t586, int c81>
    Vector::vector<t586, c81> scale(t586 scalar, Vector::vector<t586, c81> v) {
        return (([&]() -> Vector::vector<t586, c81> {
            auto n = c81;
            return (([&]() -> Vector::vector<t586, c81> {
                auto guid155 = v;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto result = guid155;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid156 = 0;
                    uint32_t guid157 = (n - 1);
                    for (uint32_t i = guid156; i <= guid157; i++) {
                        (([&]() -> Prelude::unit {
                            (((result).data)[i] = (((result).data)[i] * scalar));
                            return Prelude::unit();
                        })());
                    }
                    return {};
                })());
                return result;
            })());
        })());
    }
}

namespace Vector {
    template<typename t592, int c84>
    t592 dot(Vector::vector<t592, c84> v1, Vector::vector<t592, c84> v2) {
        return (([&]() -> t592 {
            auto n = c84;
            return (([&]() -> t592 {
                auto guid158 = 0;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto sum = guid158;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid159 = 0;
                    uint32_t guid160 = (n - 1);
                    for (uint32_t i = guid159; i <= guid160; i++) {
                        (([&]() -> Prelude::unit {
                            (sum = (sum + (((v1).data)[i] * ((v2).data)[i])));
                            return Prelude::unit();
                        })());
                    }
                    return {};
                })());
                return sum;
            })());
        })());
    }
}

namespace Vector {
    template<typename t598, int c87>
    t598 magnitude2(Vector::vector<t598, c87> v) {
        return (([&]() -> t598 {
            auto n = c87;
            return (([&]() -> t598 {
                auto guid161 = 0;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto sum = guid161;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid162 = 0;
                    uint32_t guid163 = (n - 1);
                    for (uint32_t i = guid162; i <= guid163; i++) {
                        (([&]() -> Prelude::unit {
                            (sum = (sum + (((v).data)[i] * ((v).data)[i])));
                            return Prelude::unit();
                        })());
                    }
                    return {};
                })());
                return sum;
            })());
        })());
    }
}

namespace Vector {
    template<typename t604, int c90>
    double magnitude(Vector::vector<t604, c90> v) {
        return (([&]() -> double {
            auto n = c90;
            return sqrt_(magnitude2<t604, c90>(v));
        })());
    }
}

namespace Vector {
    template<typename t610, int c91>
    Vector::vector<t610, c91> multiply(Vector::vector<t610, c91> u, Vector::vector<t610, c91> v) {
        return (([&]() -> Vector::vector<t610, c91> {
            auto n = c91;
            return (([&]() -> Vector::vector<t610, c91> {
                auto guid164 = u;
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto result = guid164;
                
                (([&]() -> Prelude::unit {
                    uint32_t guid165 = 0;
                    uint32_t guid166 = (n - 1);
                    for (uint32_t i = guid165; i <= guid166; i++) {
                        (([&]() -> Prelude::unit {
                            (((result).data)[i] = (((result).data)[i] * ((v).data)[i]));
                            return Prelude::unit();
                        })());
                    }
                    return {};
                })());
                return result;
            })());
        })());
    }
}

namespace Vector {
    template<typename t618, int c95>
    Vector::vector<t618, c95> normalize(Vector::vector<t618, c95> v) {
        return (([&]() -> Vector::vector<t618, c95> {
            auto n = c95;
            return (([&]() -> Vector::vector<t618, c95> {
                auto guid167 = magnitude<t618, c95>(v);
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto mag = guid167;
                
                return ((mag > 0) ? 
                    (([&]() -> Vector::vector<t618, c95> {
                        auto guid168 = v;
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        auto result = guid168;
                        
                        (([&]() -> Prelude::unit {
                            uint32_t guid169 = 0;
                            uint32_t guid170 = (n - 1);
                            for (uint32_t i = guid169; i <= guid170; i++) {
                                (([&]() -> Prelude::unit {
                                    (((result).data)[i] = (((result).data)[i] / mag));
                                    return Prelude::unit();
                                })());
                            }
                            return {};
                        })());
                        return result;
                    })())
                :
                    v);
            })());
        })());
    }
}

namespace Vector {
    template<typename t626, int c98>
    double angle(Vector::vector<t626, c98> v1, Vector::vector<t626, c98> v2) {
        return (([&]() -> double {
            auto n = c98;
            return acos_((dot<t626, c98>(v1, v2) / sqrt_((magnitude2<t626, c98>(v1) * magnitude2<t626, c98>(v2)))));
        })());
    }
}

namespace Vector {
    template<typename t664>
    Vector::vector<t664, 3> cross(Vector::vector<t664, 3> u, Vector::vector<t664, 3> v) {
        return (([&]() -> Vector::vector<t664, 3>{
            Vector::vector<t664, 3> guid171;
            guid171.data = (juniper::array<t664, 3> { {((((u).data)[1] * ((v).data)[2]) - (((u).data)[2] * ((v).data)[1])), ((((u).data)[2] * ((v).data)[0]) - (((u).data)[0] * ((v).data)[2])), ((((u).data)[0] * ((v).data)[1]) - (((u).data)[1] * ((v).data)[0]))} });
            return guid171;
        })());
    }
}

namespace Vector {
    template<typename t666, int c111>
    Vector::vector<t666, c111> project(Vector::vector<t666, c111> a, Vector::vector<t666, c111> b) {
        return (([&]() -> Vector::vector<t666, c111> {
            auto n = c111;
            return (([&]() -> Vector::vector<t666, c111> {
                auto guid172 = normalize<t666, c111>(b);
                if (!(true)) {
                    juniper::quit<Prelude::unit>();
                }
                auto bn = guid172;
                
                return scale<t666, c111>(dot<t666, c111>(a, bn), bn);
            })());
        })());
    }
}

namespace Vector {
    template<typename t676, int c112>
    Vector::vector<t676, c112> projectPlane(Vector::vector<t676, c112> a, Vector::vector<t676, c112> m) {
        return (([&]() -> Vector::vector<t676, c112> {
            auto n = c112;
            return subtract<t676, c112>(a, project<t676, c112>(a, m));
        })());
    }
}

namespace SoundBar {
    int32_t microphonePin = 15;
}

namespace SoundBar {
    juniper::array<int32_t, 8> barPins = (juniper::array<int32_t, 8> { {9, 8, 7, 6, 5, 4, 3, 2} });
}

namespace SoundBar {
    int32_t numBarPins = 8;
}

namespace SoundBar {
    juniper::shared_ptr<Prelude::list<uint16_t, 5>> state = (juniper::shared_ptr<Prelude::list<uint16_t, 5>>(new Prelude::list<uint16_t, 5>(List::replicate<uint16_t, 5>(0, 0))));
}

namespace SoundBar {
    Prelude::unit setup() {
        return (([&]() -> Prelude::unit {
            Io::setPinMode(microphonePin, Io::input());
            return (([&]() -> Prelude::unit {
                uint16_t guid173 = 0;
                uint16_t guid174 = (numBarPins - 1);
                for (uint16_t i = guid173; i <= guid174; i++) {
                    Io::setPinMode((barPins)[i], Io::output());
                }
                return {};
            })());
        })());
    }
}

namespace SoundBar {
    Prelude::unit resetBar() {
        return (([&]() -> Prelude::unit {
            int32_t guid175 = 0;
            int32_t guid176 = (numBarPins - 1);
            for (int32_t i = guid175; i <= guid176; i++) {
                Io::digWrite((barPins)[i], Io::low());
            }
            return {};
        })());
    }
}

namespace SoundBar {
    Prelude::unit drawBar(uint16_t level) {
        return (([&]() -> Prelude::unit {
            (([&]() -> Prelude::unit {
                int32_t guid177 = 0;
                int32_t guid178 = level;
                for (int32_t i = guid177; i <= guid178; i++) {
                    Io::digWrite((barPins)[i], Io::high());
                }
                return {};
            })());
            return (([&]() -> Prelude::unit {
                int32_t guid179 = (level + 1);
                int32_t guid180 = (numBarPins - 1);
                for (int32_t i = guid179; i <= guid180; i++) {
                    Io::digWrite((barPins)[i], Io::low());
                }
                return {};
            })());
        })());
    }
}

namespace SoundBar {
    Prelude::unit main() {
        return (([&]() -> Prelude::unit {
            setup();
            return (([&]() -> Prelude::unit {
                while (true) {
                    (([&]() -> Prelude::unit {
                        resetBar();
                        auto guid181 = Io::digIn(microphonePin);
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        auto micSig = guid181;
                        
                        auto guid182 = Signal::map<Io::pinState, uint16_t>(juniper::function<uint16_t(Io::pinState)>([=](Io::pinState digVal) mutable -> uint16_t { 
                            return (([&]() -> uint16_t {
                                auto guid183 = digVal;
                                return ((((guid183).tag == 1) && true) ? 
                                    (([&]() -> uint16_t {
                                        return ((uint16_t) 7);
                                    })())
                                :
                                    ((((guid183).tag == 0) && true) ? 
                                        (([&]() -> uint16_t {
                                            return ((uint16_t) 0);
                                        })())
                                    :
                                        juniper::quit<uint16_t>()));
                            })());
                         }), micSig);
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        auto barSig = guid182;
                        
                        auto guid184 = Signal::record<uint16_t, 5>(barSig, state);
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        auto pastBarSig = guid184;
                        
                        auto guid185 = Signal::map<Prelude::list<uint16_t, 5>, uint16_t>(List::average<uint16_t, 5>, pastBarSig);
                        if (!(true)) {
                            juniper::quit<Prelude::unit>();
                        }
                        auto meanBarSig = guid185;
                        
                        return Signal::sink<uint16_t>(drawBar, meanBarSig);
                    })());
                }
                return {};
            })());
        })());
    }
}

int main() {
    init();
    SoundBar::main();
    return 0;
}