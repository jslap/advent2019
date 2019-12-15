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

    testMachine1(t);
    testMachine05(t);

    t.expectTest(State("109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99").run({}).getOutput(), 
        State::DataIO{109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99});

    t.expectTest(State("1102,34915192,34915192,7,4,7,99,0").run({}).getOutput(), 
        State::DataIO{1219070632396864});
    t.expectTest(State("104,1125899906842624,99").run({}).getOutput(), 
        State::DataIO{1125899906842624});

    t.expectTest(State(fromFile("../dec09_1.txt")).run({1}).getOutput(), 
        State::DataIO{3598076521});

    t.expectTest(State(fromFile("../dec09_1.txt")).run({2}).getOutput(), 
        State::DataIO{3598076521});


    return 1;
}