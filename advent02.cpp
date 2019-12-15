#include "AdventUtil.h"
#include "intMachine.h"
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <fmt/ranges.h>
#include <vector>



int main()
{
    Tester t;
    t.expectTest(42, 42);

    testMachine1();
    // t.expectTest(State("1,0,0,0,99").advance(), State{"2,0,0,0,99"});
    // t.expectTest(State("1,0,0,0,99").run(), State{"2,0,0,0,99"});

    // t.expectTest(State("2,3,0,3,99").advance(), State{"2,3,0,6,99"});
    // t.expectTest(State("2,3,0,3,99").run(), State{"2,3,0,6,99"});

    // t.expectTest(State("2,4,4,5,99,0").advance(), State{"2,4,4,5,99,9801"});
    // t.expectTest(State("2,4,4,5,99,0").run(), State{"2,4,4,5,99,9801"});

    // t.expectTest(State("1,1,1,4,99,5,6,0,99").advance(), State{"1,1,1,4,2,5,6,0,99"});
    // t.expectTest(State("1,1,1,4,99,5,6,0,99").advance().advance(), State{"30,1,1,4,2,5,6,0,99"});
    // t.expectTest(State("1,1,1,4,99,5,6,0,99").run(), State{"30,1,1,4,2,5,6,0,99"});

    // t.expectTest(State("1,1,1,4,99,5,6,0,99").run().get(0), 30);

    // t.expectTest(State(fromFile("../dec02_1.txt")).alter(1, 12).alter(2, 2).run().get(0), 3101844);
    // t.expectTest(State(fromFile("../dec02_1.txt")).setAlterInput(12, 2).run().get(0), 3101844);

    // t.expectTest(State(fromFile("../dec02_1.txt")).findInputValueForRes(19690720), 8478);

    return 1;
}