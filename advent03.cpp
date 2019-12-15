#include "AdventUtil.h"
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/view/sliding.hpp>
#include <range/v3/view/join.hpp>

#include <fmt/ranges.h>
#include <vector>




enum class Dir : u8  {Right, Left, Up, Down};
class Move
{
    public:
    Move(std::string const & s)
    {
        char d = s[0];
        if (d=='R') dir = Dir::Right;
        if (d=='L') dir = Dir::Left;
        if (d=='U') dir = Dir::Up;
        if (d=='D') dir = Dir::Down;
        l = std::stoi(s.substr(1));
    }
    Dir dir;
    s32 l;
};
class Point
{
    public:
    Point() = default;
    Point(s32 _x, s32 _y) : x(_x), y(_y) {}

    Point move(Move const & m)
    {
        if (m.dir == Dir::Right) return Point{x+m.l, y};
        if (m.dir == Dir::Left) return Point{x-m.l, y};
        if (m.dir == Dir::Down) return Point{x, y+m.l};
        if (m.dir == Dir::Up) return Point{x, y-m.l};
        return {};
    }

    s32 x{};
    s32 y{};
};

std::vector<Point> wirePoints(std::string const & s)
{
    using namespace ranges;
    std::vector<std::string> values{absl::StrSplit(s, ',')};
    auto moveVec = values | views::transform([](auto const & s1){
        return Move{s1};
    });
    std::vector<Point> retVal{};
    retVal.emplace_back();
    for (auto const & m : moveVec)
    {
        retVal.emplace_back(retVal.back().move(m));
    }
    return retVal;
}

std::pair<Point, Point> getBB(std::vector<Point> const & pp)
{
    return std::accumulate(pp.begin(), pp.end(), std::pair<Point, Point>{}, [](std::pair<Point, Point> const & bb, Point const & p){
        std::pair<Point, Point> newBb;
        newBb.first.x = std::min(bb.first.x, p.x);
        newBb.first.y = std::min(bb.first.y, p.y);
        newBb.second.x = std::max(bb.second.x, p.x);
        newBb.second.y = std::max(bb.second.y, p.y);
        return newBb;
    });
}

    auto pairToVec = [] (auto const & posPair) -> std::vector<Point> {
        std::vector<Point> retVal;
        auto p1{posPair[0]};
        auto p2{posPair[1]};
        if (p1.x == p2.x)
        {
            s32 dir = p2.y-p1.y > 0 ? 1 : -1;
            for (s32 y = p1.y; y != p2.y; y += dir)
                retVal.push_back(Point(p1.x, y));
        }
        else
        {
            s32 dir = p2.x-p1.x > 0 ? 1 : -1;
            for (s32 x = p1.x; x != p2.x; x += dir)
                retVal.push_back(Point(x, p1.y));
        }
        return retVal;
    };

class Grid
{
    public:
    Grid(s32 _minx, s32 _maxx, s32 _miny, s32 _maxy): minx(_minx), maxx(_maxx), miny(_miny), maxy(_maxy)
    {
        sizeX = maxx-minx + 2;
        sizey = maxy-miny + 2;
        grid  = PosMap(sizey, std::vector <s32>(sizeX, 0));
    }
    Grid(Grid const &) = default;

    s32 get(Point const & p) const {return get(p.x, p.y);}
    s32 get(s32 x, s32 y) const
    {
        return grid[y-miny][x-minx];
    }

    void fill(std::vector<Point> const & points)
    {
        using namespace ranges;
        s32 curStep = 0;
        for (const auto pointSet: points | views::sliding(2) )
        {
            for (auto const & p : pairToVec(pointSet))
            {
                if (grid[p.y-miny][p.x-minx] == 0)
                    grid[p.y-miny][p.x-minx] = curStep;
                curStep++;
            }
        }
    }


    s32 minx;
    s32 maxx;
    s32 miny;
    s32 maxy;
    s32 sizeX;
    s32 sizey;
    using PosMap = std::vector <std::vector <s32> >;
    PosMap grid;
};

using DistFunc = std::function<s32(Point const&, Grid const&, Grid const&)>;
s32 solveGen(std::string const & s, DistFunc const & distFunc)
{
    using namespace ranges;
    std::vector<std::string> wires{absl::StrSplit(s, '\n')};
    std::string w1 = wires[0];
    std::string w2 = wires[1];

    auto wp1 = wirePoints(w1);
    auto wp2 = wirePoints(w2);

    auto bb1 = getBB(wp1);
    auto bb2 = getBB(wp2);

    s32 minx = std::min(bb1.first.x, bb2.first.x)-1;
    s32 miny = std::min(bb1.first.y, bb2.first.y)-1;
    s32 maxx = std::max(bb1.second.x, bb2.second.x)+1;
    s32 maxy = std::max(bb1.second.y, bb2.second.y)+1;

    Grid g1(minx, maxx, miny, maxy);
    Grid g2(g1);

    g1.fill(wp1);
    g2.fill(wp2);

    Point bestPos;
    s32 bestDist = 999999999;
    auto xRange = views::ints(g1.minx, g1.maxx+1);
    auto yRange = views::ints(g1.miny, g1.maxy+1);

    // std::cout << "xRange: " << xRange << "\n";

    for (auto const & ptup : views::cartesian_product(xRange, yRange))
    {
        s32 x = std::get<0>(ptup);
        s32 y = std::get<1>(ptup);
        if (g1.get(x, y) > 0 && g2.get(x,y) > 0)
        {
            s32 curDist = distFunc(Point(x,y), g1, g2);
            if ( curDist < bestDist && 
                    (x!=0 || y!=0))
            {
                bestPos = Point(x,y);
                bestDist = curDist;
            }

        }
    }

    return bestDist;
}

s32 f1(Point const& p, Grid const& , Grid const& )
{
    return std::abs(p.x)+std::abs(p.y);
}

s32 solve(std::string const & s)
{
    return solveGen(s, f1);
}

s32 f2(Point const& p, Grid const& g1, Grid const& g2)
{
    return g1.get(p) + g2.get(p);
}

s32 solve2(std::string const & s)
{
    return solveGen(s, f2);
}




int main()
{
    Tester t;
    t.expectTest(42, 42);


    t.expectTest(solve("R8,U5,L5,D3\nU7,R6,D4,L4"), 6);

    t.expectTest(solve("R75,D30,R83,U83,L12,D49,R71,U7,L72\nU62,R66,U55,R34,D71,R55,D58,R83"), 159);
    t.expectTest(solve("R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51\nU98,R91,D20,R16,D67,R40,U7,R15,U6,R7"), 135);
    t.expectTest(solve(fromFile("../dec03_1.txt")), 245);

    t.expectTest(solve2("R8,U5,L5,D3\nU7,R6,D4,L4"), 30);
    t.expectTest(solve2("R75,D30,R83,U83,L12,D49,R71,U7,L72\nU62,R66,U55,R34,D71,R55,D58,R83"), 610);
    t.expectTest(solve2("R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51\nU98,R91,D20,R16,D67,R40,U7,R15,U6,R7"), 410);
    t.expectTest(solve2(fromFile("../dec03_1.txt")), 48262);


    return 1;
}
