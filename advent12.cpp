#include "AdventUtil.h"
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <fmt/ranges.h>
#include <vector>

class Point
{
    public:
    Point() = default;
    constexpr Point(s32 _x, s32 _y, s32 _z) : x(_x), y(_y) , z(_z){}

    bool operator==(Point const &rhs) const 
    {
        return rhs.x==x && rhs.y==y && rhs.z==z;
    }
    bool operator!=(Point const &rhs) const 
    {
        return !operator==(rhs);
    }

    bool operator<(Point const &rhs) const 
    {
        return std::tuple{x,y,z} < std::tuple{rhs.x,rhs.y, rhs.z};
    }

    Point& operator+=(Point const &rhs) 
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    Point operator-(Point const & rhs) const
    {
        return {x-rhs.x, y-rhs.y, z-rhs.z};
    }
    s32 x{};
    s32 y{};
    s32 z{};
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
        return fmt::format_to(ctx.out(), "({0},{1},{2})", s.x, s.y, s.z);
    }
};  

class Moon
{
    public:
    Moon(Point const & p): pos{p}, vel{} {}

    void applyGravityFrom(Moon const & other)
    {
        // fmt::print("apply gravity vel before {}  curPos {} other pos {} diff {}\n", vel, pos, other.pos, getGravDiff(other));
        vel += getGravDiff(other);
        // fmt::print("\tafter vel {}\n", vel);
    }
    static s32 coordSign(s32 c1, s32 c2)
    {
        return c1 < c2 ? 1 : (c1 > c2 ? -1 : 0);
    }
    Point getGravDiff(Moon const & other) const
    {
        return Point{
            coordSign(pos.x, other.pos.x),
            coordSign(pos.y, other.pos.y),
            coordSign(pos.z, other.pos.z)
        };
    }
    void applyVelocity()
    {
        pos += vel;
    }

    static s32 sumAbs(Point const & p)
    {
        return std::abs(p.x) + std::abs(p.y) + std::abs(p.z);
    }
    s32 energyPot() const
    {
        return sumAbs(pos);
    }
    s32 energyKin() const
    {
        return sumAbs(vel);
    }
    s32 energy() const
    {
        return energyPot()*energyKin();
    }

    Point pos;
    Point vel;
};

class System
{
    public:
    System(std::vector<Point> const & posVec)
    {
        using namespace ranges;
        moons = posVec | views::transform([](auto const & p){return Moon{p};}) | to<std::vector<Moon>>();
    }
    System(System const&) = default;
    System& step(s32 nbStep = 1)
    {
        using namespace ranges;
        for (auto i : view::iota(0, nbStep))
        {
            applyGravity();
            applyVelocity();
        }
        return *this;
    }
    s32 energy() const
    {
        using namespace ranges;
        return accumulate(moons | views::transform([](auto const & m){return m.energy();}), 0);
    }
    void applyGravity()
    {
        using namespace ranges;
        for (auto const & moonPair : views::cartesian_product(moons, moons))
        {
            Moon & m1{std::get<0>(moonPair)};
            Moon const & m2{std::get<1>(moonPair)};
            m1.applyGravityFrom(m2);
        }

    }
    void applyVelocity()
    {
        for (auto & m : moons)
            m.applyVelocity();
    }

    std::vector<Moon> moons;
};

int main()
{
    Tester t;
    t.expectTest(42, 42);

    // <x=-1, y=0, z=2> <x=2, y=-10, z=-7> <x=4, y=-8, z=8> <x=3, y=5, z=-1>
    System system1{{{-1, 0, 2}, {2, -10, -7}, {4,-8,8}, {3,5,-1}}};
    t.expectTest(system1.moons.size(), 4U);
    t.expectTest(System(system1).moons[0].vel, Point{0,0,0});
    t.expectTest(System(system1).moons[1].vel, Point{0,0,0});
    t.expectTest(System(system1).moons[2].vel, Point{0,0,0});
    t.expectTest(System(system1).moons[3].vel, Point{0,0,0});

    t.expectTest(System(system1).step().moons[0].pos, Point{2,-1,1});
    t.expectTest(System(system1).step().moons[0].vel, Point{3,-1,-1});
    t.expectTest(System(system1).step().moons[3].pos, Point{2,2,0});
    t.expectTest(System(system1).step().moons[3].vel, Point{-1,-3,1});

    t.expectTest(System(system1).step(10).energy(), 179);

    //             <x=14, y=15, z=-2>  <x=17, y=-3, z=4> <x=6, y=12, z=-13> <x=-2, y=10, z=-8>
    System systemF{{{14,15,-2},        {17,-3,4},         {6,12,-13},        {-2,10,-8}}};
    t.expectTest(System(systemF).step(1000).energy(), 179);

    return 1;
}