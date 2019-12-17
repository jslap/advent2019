#include "AdventUtil.h"
#include "intMachine.h"
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <fmt/ranges.h>
#include <vector>

class Point
{
    public:
    Point() = default;
    Point(s64 _x, s64 _y) : x(_x), y(_y) {}

    bool operator==(Point const &rhs) const 
    {
        return rhs.x==x && rhs.y==y;
    }
    bool operator!=(Point const &rhs) const 
    {
        return !operator==(rhs);
    }

    bool operator<(Point const &rhs) const 
    {
        return std::pair{x,y} < std::pair{rhs.x,rhs.y};
    }

    Point operator-(Point const & rhs) const
    {
        return {x-rhs.x, y-rhs.y};
    }

    Point operator/(s64 div) const
    {
        return {x/div, y/div};
    }

    s64 x{};
    s64 y{};
};

template<>
struct fmt::formatter<Point>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(Point const& s, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "({0},{1})", s.x, s.y);
    }
};


class BrickGame
{
    public:
    BrickGame(std::string const & s): state(s)
    {}

    BrickGame& run()
    {
        while (!state.isFinal())
        {
            doOneStep();
        }
        return *this;
    }

    BrickGame& runPlay()
    {
        while (!state.isFinal())
        {
            doOneStep();
            show();
            if (lastPadle.x < lastBall.x)
                state.addInput({1});
            else if (lastPadle.x > lastBall.x)
                state.addInput({-1});
            else
                state.addInput({0});
        }
        return *this;
    }

    s32 getTile(Point const & p) const
    {
        auto it = screen.find(p);
        if (it == screen.end())
        {
            fmt::print("error: !\n");
            return 0;
        }
        return it->second;
    }
    char tileToChar(s32 tile) const
    {
        switch (tile)
        {
            case 0: return ' ';
            case 1: return '#';
            case 2: return '=';
            case 3: return '_';
            case 4: return 'o';
        }
        return 'A';
    }
    void show() const
    {
        using namespace ranges;
        fmt::print("Score: {}\n", lastScore);
        for (auto y : view::iota(0, 20))
        {
            for (auto x : view::iota(0,37))
            {
                std::cout << tileToChar(getTile(Point{x,y}));
            }
            std::cout << "\n";
        }
    }

    BrickGame& step(s32 nbStep = 1)
    {
        using namespace ranges;
        for (auto i : view::iota(0, nbStep))
        {
            if (state.isFinal())
                break;
            doOneStep();
        }
        return *this;
    }
    void doOneStep()
    {
        using namespace ranges;
        state.clearOutput();
        state.runUntillAskInput();
        lastOutput = state.getOutput();
        for (auto curTile : lastOutput | views::chunk(3))
        {
            if (curTile[0] == -1 && curTile[1]==0)
                lastScore = curTile[2];
            else
            {
                Point curPos{curTile[0], curTile[1]};
                s32 curTileVal = curTile[2];
                screen[curPos] = curTileVal;
                if (curTileVal == 4)
                    lastBall = curPos;
                if (curTileVal == 3)
                    lastPadle = curPos;
            }
        }
    }
    s32 nbBrick() const
    {
        using namespace ranges;
        return ranges::count(screen | views::values, 2);
    }

    State  state;
    State::DataIO lastOutput;
    std::map<Point, s32> screen;
    s32 lastScore{};
    Point lastBall{};
    Point lastPadle{};
};

int main()
{
    Tester t;
    t.expectTest(42, 42);

    BrickGame bg{fromFile("../dec13_1.txt")};
    t.expectTest(bg.step().nbBrick(), 42);

    BrickGame bg2{fromFile("../dec13_1.txt")};
    bg2.state.alter(0,2);
    bg2.runPlay();

    return 1;
}