#include "AdventUtil.h"
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/max.hpp>
#include <fmt/ranges.h>
#include <vector>

template< class Key, class T, class Compare, class Alloc, class Pred >
void erase_if(std::map<Key,T,Compare,Alloc>& c, Pred pred)
{
    for (auto i = c.begin(), last = c.end(); i != last; ) 
    {
        if (pred(*i)) 
        {
            i = c.erase(i);
        } 
        else 
        {
            ++i;
        }
    }    
}

class Point
{
    public:
    Point() = default;
    Point(s32 _x, s32 _y) : x(_x), y(_y) {}

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

    Point operator/(s32 div) const
    {
        return {x/div, y/div};
    }

    s32 manNorm() const {return std::abs(x) + std::abs(y);}

    Point getAngleTo(Point const & other) const
    {
        Point diff{other - *this};
        auto theDiv = std::gcd(diff.x, diff.y);
        return diff/theDiv;
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


class Grid
{
    public:
    Grid(std::string const & s)
    {
        using namespace ranges;
        std::vector<std::string> lines{absl::StrSplit(s, '\n')};
        for (auto&& [y, line] : lines | ranges::views::enumerate) 
        {
            for (auto&& [x, val] : line | ranges::views::enumerate) 
            {
                if (val == '#' )
                    points.emplace_back(x,y);
            }
        }
    }

    s32 maxVisible() const
    {
        using namespace ranges;
        return ranges::max(points | view::transform([this](Point const & p )  {return nbVisible(p);}));
    }

    class VisSetComp
    {
        public:
        VisSetComp(Point const & _center): center{_center} {};
        bool operator()(Point const & lhs, Point const & rhs) const
        {
            return (lhs-center).manNorm() < (rhs-center).manNorm();
        }
        Point center;
    };
    class ViewMap2
    {
        public:
        ViewMap2(Point const & _center): center{_center} {};

        void add(std::vector<Point> const & pVec)
        {
            for (auto const & p : pVec)
                add(p);
        }
        void add(Point const & p)
        {
            if (p != center)
            {
                nbPoint++;
                Point curAngle = center.getAngleTo(p);
                auto it = m.find(curAngle);
                if (it == m.end())
                    m.emplace(curAngle, VisSet2{{p}, VisSetComp{center}});
                else
                    it->second.emplace(p);
            }
        }
        s32 nbAngle() const
        {
            return m.size();
        }
        void oneRevolution()
        {
            for (auto & p: m)
                p.second.erase(p.second.begin());
            erase_if(m, [](auto & s){return s.second.empty();});
        }
        Point nth(s32 n) const 
        {
            return *m.cbegin()->second.cbegin();
        }

        s32 nbVisible() const
        {
            using namespace ranges;
            s32 nbNotVis = accumulate(m | views::values, 0, [](s32 prevRes, VisSet2 const & curVisSet){
                return prevRes + (curVisSet.size() > 1 ? curVisSet.size()-1 : 0);
            } );
            return nbPoint - nbNotVis;
        }

        Point center{};
        s32 nbPoint{0};
        using VisSet2 = std::set<Point, VisSetComp>;
        using Map = std::map<Point, VisSet2>;
        Map m{};
    };
    using VisSet = std::set<Point>;
    using ViewMap = std::map<Point, VisSet>;
    ViewMap genMap(Point const &from) const
    {
        ViewMap visMap;
        for (auto const & curAst : points)
        {
            if (curAst != from)
            {
                Point curAngle = from.getAngleTo(curAst);
                visMap[curAngle].emplace(curAst);
            }
        }
        return visMap;
    }

    Point findNthZap(Point const &from, s32 nth)
    {
        ViewMap2 visMap2(from);
        visMap2.add(points);
        while (nth > visMap2.nbAngle())
        {
            nth -= visMap2.nbAngle();
            visMap2.oneRevolution();
        }
        return visMap2.nth(nth);
    }

    s32 nbVisible(Point const &from) const
    {
        ViewMap2 visMap2(from);
        visMap2.add(points);
        return visMap2.nbVisible();
    }
    std::vector<Point> points;
};

int main()
{
    Tester t;
    t.expectTest(42, 42);


    t.expectTest(Point(0,0).getAngleTo(Point{1,1}), Point(1,1));
    t.expectTest(Point(0,0).getAngleTo(Point{3,3}), Point(1,1));
    t.expectTest(Point(5,5).getAngleTo(Point{3,3}), Point(-1,-1));

    t.expectTest(Point(5,5).getAngleTo(Point{8,2}), Point(1,-1));

    t.expectTest(Grid{fromFile("../dec10_1.txt")}.points.size(), 10U);
    t.expectTest(Grid{fromFile("../dec10_1.txt")}.nbVisible(Point{1,0}), 7);
    t.expectTest(Grid{fromFile("../dec10_1.txt")}.nbVisible(Point{4,2}), 5);
    t.expectTest(Grid{fromFile("../dec10_1.txt")}.nbVisible(Point{3,4}), 8);
    t.expectTest(Grid{fromFile("../dec10_1.txt")}.maxVisible(), 8);
    t.expectTest(Grid{fromFile("../dec10_2.txt")}.maxVisible(), 33);
    t.expectTest(Grid{fromFile("../dec10_3.txt")}.maxVisible(), 210);
    
    t.expectTest(Grid{fromFile("../dec10_fin.txt")}.maxVisible(), 347);

    t.expectTest(Grid{fromFile("../dec10_4.txt")}.findNthZap(Point(8,3), 1), Point(8,1));


    return 1;
}