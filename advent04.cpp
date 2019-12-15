#include "AdventUtil.h"
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <fmt/ranges.h>
#include <vector>
#include <array>

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using Code = std::array<u8, 6>;

bool hasTwoConsec(Code const & c)
{
    return std::adjacent_find(c.begin(), c.end()) != c.end();
}

bool hasTwoConsecLonely(Code const & c)
{
    return (c[0] == c[1] && c[1] != c[2]) || 
    (c[1] == c[2] && c[1] != c[0]&& c[1] != c[3]) || 
    (c[2] == c[3] && c[2] != c[1]&& c[2] != c[4]) || 
    (c[3] == c[4] && c[3] != c[2]&& c[3] != c[5]) ||     
    (c[4] == c[5] && c[4] != c[3]) ;
}

void nextCode(Code & c)
{
    for (auto it = c.rbegin(); it != c.rend(); it++)
    {
        auto & curDig = *it;
        auto const & nextDig = (it == c.rend()) ? 9 : *(std::next(it));
        if (curDig < 9)
        {
            curDig++;
            break;
        }
        else
        {
            if (nextDig == 9)
                curDig = 0;
            else
                curDig = nextDig+1;
            u8 nonDecVal = curDig;
            for (auto backIter = it.base(); backIter != c.end(); backIter++)
            {
                if (*backIter < nonDecVal)
                    *backIter = nonDecVal;
                if (*backIter > nonDecVal)
                    nonDecVal = *backIter;
            }
        }
    }
}

u32 solve(Code const & a, Code const & b)
{
    u32 retVal = 0;
    Code cur = a;
    do
    {
        if (hasTwoConsec(cur))
        {
            fmt::print("Code  should be {}\n", cur);
            retVal++;
        }
        nextCode(cur);

    } while (cur <= b);
    return retVal;
}


u32 solve2(Code const & a, Code const & b)
{
    u32 retVal = 0;
    Code cur = a;
    do
    {
        if (hasTwoConsecLonely(cur))
        {
            fmt::print("Code  should be {}\n", cur);
            retVal++;
        }
        nextCode(cur);

    } while (cur <= b);
    return retVal;
}

int main()
{
    Tester t;
    t.expectTest(42, 42);

    Code c1{2,3,5,7,7,7};
    Code c2{7,0,6,9,4,8};

    t.expectTest(solve(c1, c2), 1178U);

    t.expectTest(solve2(c1, c2), 1U);


    return 1;
}