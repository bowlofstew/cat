#include <cat/functional.hpp>
#include <cat/utility/constexpr.hpp>

#include <string>
#include <iostream>
#include <functional>
#include <type_traits>

#include <yats.hpp>

using namespace yats;
using namespace cat;


Context(callable_test)
{
    int f0(int, std::string, char, bool)
    {
        return 42;
    }

    int negate(int a)
    {
        return -a;
    }

    int add(int a, int  b)
    {
        return a+b;
    }

    Test(basic_callable)
    {
        auto val = callable(negate)(1);
        Assert(val, is_equal_to(-1));
    }


    Test(simple_callable)
    {
        auto add_ = callable(add)(1);
        Assert(add_(2), is_equal_to(3));
    }


    Test(mixed_callable)
    {
        auto f1 = callable(f0)(42, "hello");
        auto f2 = f1('x');

        Assert(f2(true), is_equal_to(42));
    }

    Test(total_callable)
    {
        Assert(callable(f0)(42)("hello")('x')(true), is_equal_to(42));
    }


    void increment(int &n)
    {
        n++;
    }

    Test(lvalue)
    {
        int a = 0;
        callable(increment)(a);
        Assert(a, is_equal_to(1));
    }

    void temporary(int &&n)
    {
        n++;
    }

    Test(rvalue)
    {
        int a = 0;

        callable(temporary)(std::move(a));
        Assert(a, is_equal_to(1));
    }

    Test(apply)
    {
        auto f = callable(f0);
        auto f1 = f.apply(0);
        auto f2 = f1.apply("hello",'x');
        auto f3 = f2.apply(true);

        Assert(arity<decltype(f) >::value == 4);
        Assert(arity<decltype(f1)>::value == 3);
        Assert(arity<decltype(f2)>::value == 1);
        Assert(arity<decltype(f3)>::value == 0);

        Assert(!std::is_same<decltype(f3), int>::value);

        Assert(f3(), is_equal_to(42));
    }

    constexpr auto cfun = callable(identity);

    Test(constexpr)
    {
        assert_constexpr(cfun);
    }

    int next(int a)
    {
        return a+1;
    }

    int constant() { return 42; }

    int sum(int a, int b)
    {
        return a - b;
    }

    Test(composition)
    {
        auto h1 = compose(sum,next);
        auto h2 = compose(callable(sum),next);
        auto h3 = compose(sum,callable(next));
        auto h4 = compose(callable(sum),callable(next));

        Assert(h1(10,1)  == 10);
        Assert(h2(10)(1) == 10);
        Assert(h3(10,1)  == 10);
        Assert(h4(10)(1)  == 10);

        auto l1 = compose(callable(sum), [](int n) { return n+1; });
        auto l2 = l1(10);

        Assert( l2(1) == 10);

        auto x1 = compose(sum, constant);
        Assert(x1(1) == 41);

        auto x2 = callable(sum) ^ (constant);
        Assert(x2(1) == 41);
    }


    Test(unspecified)
    {
        auto h1 = callable(unspecified<2>([](auto a, auto b) { return a+b;}));

        Assert(h1(1,2) == 3);
        Assert(h1(1)(2) == 3);
    }

}


int
main(int argc, char * argv[])
{
    return yats::run(argc, argv);
}


