#include "AdventUtil.h"
#include <range/v3/view/transform.hpp>
#include <range/v3/numeric/accumulate.hpp>

using GetFuelFunc = std::function<int(int)>;

int solveImpl(std::string const & s, GetFuelFunc const & getFuelFunc)
{
    using namespace ranges;

    std::vector<std::string> const lines = absl::StrSplit(s, '\n');
    return accumulate(lines | 
        views::filter(ranges::not_fn(&std::string::empty)) |
        views::transform(AdventStoi) | 
        views::transform(getFuelFunc), 0);
}

auto getFuel = [] (int mass) {
    return mass/3 - 2;
    };

auto makeFuelRec(GetFuelFunc const & getFuelFunc) {
    auto const theRecFunction = [&getFuelFunc] (int m, auto & self) -> int  {
        int const curFuel = getFuelFunc(m);
        return (curFuel>0) ? curFuel + self(curFuel, self) : 0;
    };
    return [theRecFunction](int m) -> int {return theRecFunction(m, theRecFunction);};
}

int solve(std::string const & s)
{
    return solveImpl(s, getFuel);
}

int solve2(std::string const & s)
{
    return solveImpl(s, makeFuelRec(getFuel));
}


int main()
{
    Tester t;
    t.expectTest(42, 42);

    t.expectTest(solve("12"), 2);
    t.expectTest(solve("14"), 2);
    t.expectTest(solve("1969"), 654);
    t.expectTest(solve("100756"), 33583);
    t.expectTest(solve("100756\n14"), 33585);

    t.expectTest(solve(fromFile("../dec01_1.txt")), 3262991);

    t.expectTest(solve2("12"), 2);
    t.expectTest(solve2("14"), 2);
    t.expectTest(solve2("1969"), 966);
    t.expectTest(solve2("100756"), 50346);
    t.expectTest(solve2("100756\n14"), 50348);

    t.expectTest(solve2(fromFile("../dec01_1.txt")), 4891620);

    return 1;
}