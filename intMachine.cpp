#include "intMachine.h"

s64 State::findInputValueForRes(s64 res) const
{
    auto tupleRes = findInputForRes(res);
    return std::get<0>(tupleRes) * 100 + std::get<1>(tupleRes);
}
std::tuple<s64, s64> State::findInputForRes(s64 res) const
{
    using namespace ranges;
    auto possibleValues = views::ints(0, 99);
    auto possiblePairs = views::cartesian_product(possibleValues, possibleValues);
    auto foundPair = find_if(possiblePairs,
            [this, res](auto const& curPair) { return State{*this}.setAlterInput(curPair).run().get(0) == res; });
    return {*foundPair};
}

State& State::alter(s64 pos, s64 val)
{
    inst[pos] = val;
    return *this;
}

State& State::setAlterInput(s64 noun, s64 verb)
{
    return alter(1, noun).alter(2, verb);
}


void testMachine1(Tester& t)
{
    t.expectTest(State("1,0,0,0,99").advance(), State{"2,0,0,0,99"});
    t.expectTest(State("1,0,0,0,99").run(), State{"2,0,0,0,99"});

    t.expectTest(State("2,3,0,3,99").advance(), State{"2,3,0,6,99"});
    t.expectTest(State("2,3,0,3,99").run(), State{"2,3,0,6,99"});

    t.expectTest(State("2,4,4,5,99,0").advance(), State{"2,4,4,5,99,9801"});
    t.expectTest(State("2,4,4,5,99,0").run(), State{"2,4,4,5,99,9801"});

    t.expectTest(State("1,1,1,4,99,5,6,0,99").advance(), State{"1,1,1,4,2,5,6,0,99"});
    t.expectTest(State("1,1,1,4,99,5,6,0,99").advance().advance(), State{"30,1,1,4,2,5,6,0,99"});
    t.expectTest(State("1,1,1,4,99,5,6,0,99").run(), State{"30,1,1,4,2,5,6,0,99"});

    t.expectTest(State("1,1,1,4,99,5,6,0,99").run().get(0), 30);

    t.expectTest(State(fromFile("../dec02_1.txt")).alter(1, 12).alter(2, 2).run().get(0), 3101844);
    t.expectTest(State(fromFile("../dec02_1.txt")).setAlterInput(12, 2).run().get(0), 3101844);

    t.expectTest(State(fromFile("../dec02_1.txt")).findInputValueForRes(19690720), 8478);
}

void testMachine05(Tester& t)
{
    t.expectTest(42, 42);


    t.expectTest(State("3,0,4,0,99").run({7}).getOutput(), State::DataIO{7});

    t.expectTest(State("1002,4,3,4,33").run().get(4), 99);

    t.expectTest(State("1101,100,-1,4,0").run().get(4), 99);

    t.expectTest(State(fromFile("../dec05_1.txt")).run({1}).getOutput(), State::DataIO{0, 0, 0, 0, 0, 0, 0, 0, 0, 7566643});

    t.expectTest(State("3,9,8,9,10,9,4,9,99,-1,8").run({7}).getOutput(), State::DataIO{0});
    t.expectTest(State("3,9,8,9,10,9,4,9,99,-1,8").run({8}).getOutput(), State::DataIO{1});

    t.expectTest(State("3,9,7,9,10,9,4,9,99,-1,8").run({5}).getOutput(), State::DataIO{1});
    t.expectTest(State("3,9,7,9,10,9,4,9,99,-1,8").run({7}).getOutput(), State::DataIO{1});
    t.expectTest(State("3,9,7,9,10,9,4,9,99,-1,8").run({8}).getOutput(), State::DataIO{0});
    t.expectTest(State("3,9,7,9,10,9,4,9,99,-1,8").run({9}).getOutput(), State::DataIO{0});


    t.expectTest(State("3,3,1108,-1,8,3,4,3,99").run({7}).getOutput(), State::DataIO{0});
    t.expectTest(State("3,3,1108,-1,8,3,4,3,99").run({8}).getOutput(), State::DataIO{1});

    t.expectTest(State("3,3,1107,-1,8,3,4,3,99").run({5}).getOutput(), State::DataIO{1});
    t.expectTest(State("3,3,1107,-1,8,3,4,3,99").run({7}).getOutput(), State::DataIO{1});
    t.expectTest(State("3,3,1107,-1,8,3,4,3,99").run({8}).getOutput(), State::DataIO{0});
    t.expectTest(State("3,3,1107,-1,8,3,4,3,99").run({9}).getOutput(), State::DataIO{0});

    t.expectTest(State("3,12,6,12,15,1,13,14,13,4,13,99,-1,0,1,9").run({0}).getOutput(), State::DataIO{0});
    t.expectTest(State("3,12,6,12,15,1,13,14,13,4,13,99,-1,0,1,9").run({1}).getOutput(), State::DataIO{1});
    t.expectTest(State("3,12,6,12,15,1,13,14,13,4,13,99,-1,0,1,9").run({-1}).getOutput(), State::DataIO{1});

    t.expectTest(State("3,21,1008,21,8,20,1005,20,22,107,8,21,20,1006,20,31,1106,0,36,98,0,0,1002,21,125,20,4,20,1105,1,46,104,999,1105,1,46,1101,1000,1,20,4,20,1105,1,46,98,99").
        run({5}).getOutput(), 
        State::DataIO{999});
    t.expectTest(State("3,21,1008,21,8,20,1005,20,22,107,8,21,20,1006,20,31,1106,0,36,98,0,0,1002,21,125,20,4,20,1105,1,46,104,999,1105,1,46,1101,1000,1,20,4,20,1105,1,46,98,99").
        run({8}).getOutput(), 
        State::DataIO{1000});
    t.expectTest(State("3,21,1008,21,8,20,1005,20,22,107,8,21,20,1006,20,31,1106,0,36,98,0,0,1002,21,125,20,4,20,1105,1,46,104,999,1105,1,46,1101,1000,1,20,4,20,1105,1,46,98,99").
        run({10}).getOutput(), 
        State::DataIO{1001});

    t.expectTest(State(fromFile("../dec05_1.txt")).run({5}).getOutput(), State::DataIO{9265694});
}
