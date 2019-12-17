#ifndef _INT_MACHINE_H__
#define _INT_MACHINE_H__

#include "AdventUtil.h"

#include <fmt/ranges.h>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/view/transform.hpp>
#include <vector>

using s64 = std::int64_t;
using u8 = std::uint8_t;

class State
{
public:
    using DataIO = std::deque<s64>;

    State(std::string const& s)
    {
        using namespace ranges;
        std::vector<std::string> values{absl::StrSplit(s, ',')};
        inst = values | views::filter(ranges::not_fn(&std::string::empty)) | views::transform(AdventStoi) |
                ranges::to<std::vector<s64>>();
        inst.insert(inst.end(), 1024, 0);
    }

    s64 findInputValueForRes(s64 res) const;
    std::tuple<s64, s64> findInputForRes(s64 res) const;

    s64 get(s64 pos) const
    {
        return inst[pos];
    }
    State& alter(s64 pos, s64 val);
    template<typename Pair>
    State& setAlterInput(Pair const& p)
    {
        return setAlterInput(std::get<0>(p), std::get<1>(p));
    }

    State& setAlterInput(s64 noun, s64 verb);

    State& run(DataIO const& data = {})
    {
        addInput(data);
        while (!isFinal())
        {
            advance();
        }
        return *this;
    }

    void runUntillOutput(u32 outputSize)
    {
        while (!isFinal() && output.size() < outputSize)
        {
            advance();
        }
    }

    void runUntillAskInput()
    {
        while (!isFinal() && !(isInput() && input.empty()))
        {
            advance();
        }

    }

    s64 runUntillOutput()
    {
        while (!isFinal() && !isOutput())
        {
            advance();
        }
        if (isOutput())
        {
            advance();
            return output.back();
        }
        return 0;
    }

    enum class ParamMode : u8
    {
        Positional = 0,
        Immediate = 1,
        Relative = 2
    };

    template<int paramIndex>
    s64& getParam()
    {
        s64 firstInstruction = inst[curPos];
        constexpr s64 divider{myPow(10, paramIndex + 2)};
        ParamMode m = (ParamMode)((firstInstruction / (divider)) % 10);

        s64 const paramOffset{curPos + paramIndex + 1};
        s64&  paramValue{inst[paramOffset]};
        switch (m)
        {
            case ParamMode::Positional:
                return inst[paramValue];
            case ParamMode::Immediate:
                return paramValue;
            case ParamMode::Relative:
                return inst[relBase + paramValue];
        }
        throw std::logic_error{"Unknown Param Mode"};
    }

    enum class Opcode : u8
    {
        Add = 1, Mult = 2, 
        Input = 3, Output = 4, 
        JumpIf = 5, JumpIfNot = 6, 
        IsLess = 7, IsEqual = 8,
        AdjRelBase = 9,
        Halt = 99
    };

    Opcode getOpcode() const
    {
        return Opcode{static_cast<u8>(inst[curPos] % 100)};
    }
    State& advance()
    {
        if (isFinal())
        {
            throw std::logic_error{"cannot advance final"};
        }

        Opcode curOp = getOpcode();
        switch (curOp)
        {
            case Opcode::Add:
            {
                getParam<2>() = getParam<0>() + getParam<1>();
                curPos += 4;
            }
            break;
            case Opcode::Mult:
            {
                getParam<2>() = getParam<0>() * getParam<1>();
                curPos += 4;
            }
            break;
            case Opcode::Input:
            {
                if (input.empty())
                    throw std::logic_error{"Not enough input."};
                getParam<0>() = input.front();
                input.pop_front();
                curPos += 2;
            }
            break;
            case Opcode::Output:
            {
                output.push_back(getParam<0>());
                curPos += 2;
            }
            break;
            case Opcode::JumpIf:
            {
                if (getParam<0>() != 0)
                    curPos = getParam<1>();
                else
                    curPos += 3;
            }
            break;
            case Opcode::JumpIfNot:
            {
                if (getParam<0>() == 0)
                    curPos = getParam<1>();
                else
                    curPos += 3;
            }
            break;
            case Opcode::IsLess:
            {
                getParam<2>() = (getParam<0>() < getParam<1>()) ? 1 : 0;
                curPos += 4;
            }
            break;
            case Opcode::IsEqual:
            {
                getParam<2>() = (getParam<0>() == getParam<1>()) ? 1 : 0;
                curPos += 4;
            }
            break;
            case Opcode::AdjRelBase:
            {
                relBase += getParam<0>();
                curPos += 2;
            }
            break;

            default:
                throw std::logic_error{"unknown opcode"};
        }

        return *this;
    }

    bool operator==(State const& rhs) const
    {
        return inst == rhs.inst;
    }
    bool operator!=(State const& rhs) const
    {
        return !(*this == rhs);
    }
    bool isOutput() const
    {
        return getOpcode() == Opcode::Output;
    }
    bool isInput() const
    {
        return getOpcode() == Opcode::Input;
    }
    bool isFinal() const
    {
        return getOpcode() == Opcode::Halt;
    }
    void clearOutput()
    {
        output.clear();
    }
    DataIO const& getOutput() const
    {
        return output;
    }
    void addInput(DataIO i)
    {
        input.insert(input.end(), i.begin(), i.end());
    }


    std::vector<s64> inst{};
    s64 curPos{0};
    s64 relBase{0};
    DataIO input{};
    DataIO output{};
};

template<>
struct fmt::formatter<State>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(State const& s, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "{0}:{1}", s.inst, s.curPos);
    }
};

void testMachine1(Tester& t);
void testMachine05(Tester& t);

#endif //_INT_MACHINE_H__