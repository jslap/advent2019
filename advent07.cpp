#include "AdventUtil.h"
#include "intMachine.h"
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <fmt/ranges.h>
#include <vector>

class Ampli
{
    public:
    static constexpr s64 NbAmpli{5};
    using Sequence = std::array<s64, NbAmpli>;
    Ampli(std::string const & s): mOriginalState(s)
    {

    }

    s64 findMaxThruster()
    {
        s64 curMax = -9999;
        Sequence curSeq{0,1,2,3,4};
        do
        {
            s64 curValue = runSequence(curSeq);
            if (curValue > curMax)
                curMax = curValue;
        } while (std::next_permutation(curSeq.begin(), curSeq.end()));
        return curMax;
    }

    s64 findMaxThrusterFB()
    {
        s64 curMax = -9999;
        Sequence curSeq{5,6,7,8,9};
        do
        {
            s64 curValue = runSequenceFB(curSeq);
            if (curValue > curMax)
                curMax = curValue;
        } while (std::next_permutation(curSeq.begin(), curSeq.end()));
        return curMax;
    }


    s64 runSequence(Sequence const & seq)
    {
        s64 curInput{0};
        for (auto i = 0U; i<seq.size(); i++)
        {
            State::DataIO curInputData{seq[i], curInput};
            State curState{mOriginalState};
            curState.run(curInputData);
            curInput = curState.getOutput().back();
        }
        return curInput;
    }

    s64 runSequenceFB(Sequence const & seq)
    {
        using namespace ranges;
        std::vector<State> states{seq.size(), mOriginalState};

        for (auto const & p : views::zip(seq, states) )
        {
            p.second.addInput(State::DataIO{{p.first}});
        }
        
        s64 curInput{0};
        for (auto & s : states | views::cycle)
        {
            s.addInput(State::DataIO{{curInput}});
            curInput = s.runUntillOutput();
            if (s.isFinal())
                break;
        }
        return states.back().getOutput().back();
    }

    State mOriginalState;

};


int main()
{
    Tester t;
    t.expectTest(42, 42);
    testMachine1(t);
    testMachine05(t);

    // auto test1{"3,15,3,16,1002,16,10,16,1,16,15,15,4,15,99,0,0"};
    // t.expectTest(Ampli{test1}.runSequence({4,3,2,1,0}), 43210);
    // t.expectTest(Ampli{test1}.findMaxThruster(), 43210);

    // auto test2{"3,23,3,24,1002,24,10,24,1002,23,-1,23,101,5,23,23,1,24,23,23,4,23,99,0,0"};
    // t.expectTest(Ampli{test2}.runSequence({0,1,2,3,4}), 54321);
    // t.expectTest(Ampli{test2}.findMaxThruster(), 54321);

    // auto test3{"3,31,3,32,1002,32,10,32,1001,31,-2,31,1007,31,0,33,1002,33,7,33,1,33,31,31,1,32,31,31,4,31,99,0,0,0"};
    // t.expectTest(Ampli{test3}.runSequence({1,0,4,3,2}), 65210);
    // t.expectTest(Ampli{test3}.findMaxThruster(), 65210);

    // t.expectTest(Ampli{fromFile("../dec07_1.txt")}.findMaxThruster(), 366376);


    // auto test1_1{"3,26,1001,26,-4,26,3,27,1002,27,2,27,1,27,26,27,4,27,1001,28,-1,28,1005,28,6,99,0,0,5"};
    // t.expectTest(Ampli{test1_1}.runSequenceFB({9,8,7,6,5}), 139629729);
    // t.expectTest(Ampli{test1_1}.findMaxThrusterFB(), 139629729);

    // auto test2_2{"3,52,1001,52,-5,52,3,53,1,52,56,54,1007,54,5,55,1005,55,26,1001,54,-5,54,1105,1,12,1,53,54,53,1008,54,0,55,1001,55,1,55,2,53,55,53,4,53,1001,56,-1,56,1005,56,6,99,0,0,0,0,10"};
    // t.expectTest(Ampli{test2_2}.runSequenceFB({9,7,8,5,6}), 18216);
    // t.expectTest(Ampli{test2_2}.findMaxThrusterFB(), 18216);

    t.expectTest(Ampli{fromFile("../dec07_1.txt")}.findMaxThrusterFB(), 18216);
    t.expectTest(Ampli{fromFile("../dec07_2.txt")}.findMaxThrusterFB(), 17956613);

    return 1;
}