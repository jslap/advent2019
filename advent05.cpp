#include "AdventUtil.h"
#include "intMachine.h"
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <fmt/ranges.h>
#include <vector>


int main()
{
    Tester t;

    testMachine1(t);
    testMachine05(t);

    return 1;
}