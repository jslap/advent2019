#include "AdventUtil.h"
#include "intMachine.h"
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <fmt/ranges.h>
#include <array>
#include <vector>

class Point
{
    public:
    Point() = default;
    constexpr Point(s32 _x, s32 _y) : x(_x), y(_y) {}

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

    Point& operator+=(Point const &rhs) 
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Point operator-(Point const & rhs) const
    {
        return {x-rhs.x, y-rhs.y};
    }
    s32 x{};
    s32 y{};
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
constexpr std::array<Point,4> dirArray{Point{0,1}, Point{1,0}, Point{0,-1}, Point{-1,0}};

class PaintGrid
{
    public:
    static constexpr s32 White = 1;
    static constexpr s32 Black = 0;
    PaintGrid(std::string const & s): state(s)
    {}

    void runOnce()
    {
        state.clearOutput();
        state.addInput({getColor()});
        state.runUntillOutput();
        state.runUntillOutput();
        s32 colorToUse = state.getOutput()[0];
        paint(colorToUse);
        s32 turnDir = state.getOutput()[1];
        if (turnDir == 0)
            turnLeft();
        else
            turnRight();
        pos += getDir();
    }
    PaintGrid& run()
    {
        while (!state.isFinal())
        {
            runOnce();
        }
        return *this;
    }
    PaintGrid& run(s32 nbMove)
    {
        using namespace ranges;
        for (auto i : view::iota(0, nbMove))
        {
            runOnce();
        }
        return *this;
    }
    Point getDir() const {return dirArray[dir%4];}
    void turnLeft() {dir--;}
    void turnRight() {dir++;}

    std::pair<Point, Point> getBB() const
    {
        using namespace ranges;
        return accumulate(painted | view::keys, std::pair<Point, Point>{}, [](std::pair<Point, Point> const & bb, Point const & p){
            std::pair<Point, Point> newBb;
            newBb.first.x = std::min(bb.first.x, p.x);
            newBb.first.y = std::min(bb.first.y, p.y);
            newBb.second.x = std::max(bb.second.x, p.x);
            newBb.second.y = std::max(bb.second.y, p.y);
            return newBb;
        });
    }

    void show() const
    {
        using namespace ranges;
        auto bb = getBB();
        bb.first += Point{-2,-2};
        bb.second += Point{2,2};
        for (auto y : view::iota(-bb.second.y, -bb.first.y))
        {
            for (auto x : view::iota(bb.first.x, bb.second.x))
            {
                std:: cout << ((getColor(Point{x,-y}) == 0 )? '#' : ' ');
            }
            std::cout << "\n";
        }

    }

    PaintGrid& paint(s32 col) 
    {
        painted[pos].push_back(col);
        return *this;
    }
    s32 getColor() const {return getColor(pos);}
    s32 getColor(Point const & p) const
    {
        auto foundIt = painted.find(p);
        if (foundIt == painted.end())
            return Black;
        return foundIt->second.back();
    }


    State state;
    std::map<Point, std::vector<s32>> painted{};
    Point pos{};
    s32 dir{99999 * 4};
};

int main()
{
    Tester t;
    t.expectTest(42, 42);

    t.expectTest(PaintGrid(fromFile("../dec11_1.txt")).painted.size(), 0U);
    t.expectTest(PaintGrid(fromFile("../dec11_1.txt")).run(0).painted.size(), 0U);
    t.expectTest(PaintGrid(fromFile("../dec11_1.txt")).run(0).pos, Point{0,0});
    t.expectTest(PaintGrid(fromFile("../dec11_1.txt")).run(0).getDir(), Point{0,1});

    t.expectTest(PaintGrid(fromFile("../dec11_1.txt")).run(1).painted.size(), 1U);
    t.expectTest(PaintGrid(fromFile("../dec11_1.txt")).run(1).pos, Point{-1,0});
    t.expectTest(PaintGrid(fromFile("../dec11_1.txt")).run(1).getDir(), Point{-1,0});

    t.expectTest(PaintGrid(fromFile("../dec11_1.txt")).run().painted.size(), 2255U);
    t.expectTest(PaintGrid(fromFile("../dec11_1.txt")).paint(PaintGrid::White).run().painted.size(), 2255U);
    PaintGrid(fromFile("../dec11_1.txt")).paint(PaintGrid::White).run().show();
    return 1;
}