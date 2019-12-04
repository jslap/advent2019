#include "AdventUtil.h"
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <fmt/ranges.h>
#include <vector>

using s64 = std::int32_t;

class State
{
    public:
    State(std::string const & s)
    {
        using namespace ranges;
        std::vector<std::string> values{absl::StrSplit(s, ',')};
        inst =  values | 
            views::filter(ranges::not_fn(&std::string::empty)) |
            views::transform(AdventStoi) | ranges::to<std::vector>();

    }

    s64 findInputValueForRes(s64 res) const
    {
        auto tupleRes = findInputForRes(res);
        return std::get<0>(tupleRes)*100 + std::get<1>(tupleRes);
    }
    std::tuple<s64, s64> findInputForRes(s64 res) const
    {
        using namespace ranges;
        auto possibleValues = views::ints(0, 99);
        auto possiblePairs = views::cartesian_product(possibleValues, possibleValues);
        auto foundPair = find_if(possiblePairs, [this, res](auto const & curPair){
            return State{*this}.setInput(curPair).run().get(0) == res;
        });
        return {*foundPair};
    }

    s64 get(s64 pos) const {return inst[pos];}
    State& alter(s64 pos, s64 val) 
    {
        inst[pos] = val;
        return *this;
    }
    template <typename Pair>
    State& setInput(Pair const & p)
    {
        return setInput(std::get<0>(p), std::get<1>(p));
    }

    State& setInput(s64 noun, s64 verb)
    {
        return alter(1, noun).alter(2, verb);
    }
    State& run()
    {
        while (!isFinal())
        {
            advance();
        }
        return *this;
    } 

    State& advance() 
    {
        if (isFinal())
        {
            throw std::logic_error{"cannot advance final"};
        }
        s64 curOp = inst[curPos];

        if (curOp == 1)
        {
            inst[inst[curPos+3]] = inst[inst[curPos+1]] + inst[inst[curPos+2]];
            curPos += 4;
        }
        else if (curOp == 2)
        {
            inst[inst[curPos+3]] = inst[inst[curPos+1]] * inst[inst[curPos+2]];
            curPos += 4;
        }
        else
        {
            throw std::logic_error{"unknown opcode"};
        }
        
        return *this;

    }

    bool operator==(State const & rhs) const 
    {
        return inst == rhs.inst;
    }
    bool operator!=(State const & rhs) const 
    {
        return !(*this == rhs);
    }
    bool isFinal() const {return inst[curPos] == 99;}

    std::vector<s64> inst{};
    s64 curPos{0};
};

template<>
struct fmt::formatter<State>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {return ctx.begin();}

  template<typename FormatContext>
  auto format(State const& s, FormatContext& ctx) {return fmt::format_to(ctx.out(), "{0}:{1}", s.inst, s.curPos);}
};


int main()
{
    Tester t;
    t.expectTest(42, 42);

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
    t.expectTest(State(fromFile("../dec02_1.txt")).setInput(12, 2).run().get(0), 3101844);

    t.expectTest(State(fromFile("../dec02_1.txt")).findInputValueForRes(19690720), 0);

    return 1;
}