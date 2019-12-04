#include "AdventUtil.h"
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <fmt/ranges.h>
#include <vector>


int main()
{
    Tester t;
    t.expectTest(42, 42);

    return 1;
}