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
        if (diff.x == 0 && diff.y == 0)
            return {0, 0};
        if (diff.x == 0)
            return {0, std::signbit(diff.y) ? -1 : 1};
        if (diff.y == 0)
            return {std::signbit(diff.x) ? -1 : 1, 0};
        auto theDiv = std::gcd(diff.x, diff.y);
        return diff/theDiv;
    }
    double angle() const
    {
        // fmt::print("angle of ({},{}) -> {}\n",x,y,std::atan2<double>(y, x));
        return std::atan2<double>(y, x);
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
    class VisMapComp
    {
        public:
        // get signs, order: (T,T), (T,F), (F,F), (F,T)
        // if same sign, 
        VisMapComp() = default;
        bool normOperator(Point const & lhs, Point const & rhs) const
        {
            // if (lhs.x == 0 && rhs.x == 0)
            //     return (lhs.y < 0 && rhs.y > 0);
            // if (lhs.x == 0)

            if ( ((lhs.x < 0) == (rhs.x < 0))  && ((lhs.y < 0) == (rhs.y < 0)))
                return (lhs.angle() < rhs.angle());
            if (lhs.x < 0)
            {
                if (rhs.x < 0)
                {
                    return (lhs.y >= 0);
                }
                else
                {
                    return false;
                }
            }
            else
            {
                if (rhs.x < 0)
                {
                    return true;
                }
                else
                {
                    return (lhs.y < 0);
                }
            }
            // return (lhs-center).angle() < (rhs-center).angle();
        }
        bool operator()(Point const & lhs, Point const & rhs) const
        {
            if (lhs == rhs) 
                return false;
            // fmt::print("lhs: {} , rhs: {} -> {}\n", lhs, rhs, normOperator(lhs, rhs));
            return normOperator(lhs, rhs);
        }
    };

    class ViewMap2
    {
        public:
        ViewMap2(Point const & _center): 
            center{_center},
            m{}
            {};

        void add(std::vector<Point> const & pVec)
        {
            for (auto const & p : pVec)
                add(p);
        }
        void add(Point const & p)
        {
            // fmt::print("adding {} ...\n", p);
            if (p != center)
            {
                nbPoint++;
                Point curAngle = center.getAngleTo(p);
                auto it = m.find(curAngle);
                if (it == m.end())
                    m.emplace(curAngle, VisSet2{{p}, VisSetComp{center}});
                else
                    it->second.insert(p);
            }
            // fmt::print("adding {} -> {}\n", p, m);
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
            using namespace ranges;
            return *(m | view::values | view::transform([](auto const & s){return *s.begin();}) | view::drop_exactly(n)).begin();
            // return *m.cbegin()->second.cbegin();
        }

        s32 nbVisible() const
        {
            return m.size() ;
        }

        Point center{};
        s32 nbPoint{0};
        using VisSet2 = std::set<Point, VisSetComp>;
        using Map = std::map<Point, VisSet2, VisMapComp>;
        Map m;
    };

    Point findBestNthZap(s32 nth)
    {
        using namespace ranges;
        Point bestPoint = ranges::max(points, std::less<s32>{},  [this](Point const & p )  {return nbVisible(p);});
        // fmt::print("found {} at  {} \n", bestPoint, nbVisible(bestPoint));
        return findNthZap(bestPoint, nth);
    }

    Point findNthZap(Point const &from, s32 nth)
    {
        nth--; // non-zero-based

        ViewMap2 visMap2(from);
        visMap2.add(points);
        while (nth >= visMap2.nbAngle())
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
        // fmt::print("nbVisible for {} is {}\n", from, visMap2.nbVisible());
        return visMap2.nbVisible();
    }
    std::vector<Point> points;
};

int main()
{
    Tester t;
    t.expectTest(42, 42);

    t.expectTest(Grid::VisMapComp{}(Point{0,-1}, Point{1,-1}), true);

    std::vector<Point> testVec = {{0,-1}, {1,-1},{1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}};
    for (auto it1 = testVec.begin(); it1 != testVec.end(); ++it1)
        for (auto it2 = it1; it2 != testVec.end(); ++it2)
        {
            if (*it1 != *it2)
            {
                t.expectTest(Grid::VisMapComp{}(*it1, *it2), true);
                t.expectTest(Grid::VisMapComp{}(*it2, *it1), false);
            }
            else
            {
                t.expectTest(Grid::VisMapComp{}(*it1, *it2), false);
                t.expectTest(Grid::VisMapComp{}(*it2, *it1), false);                
            }
        }


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
    t.expectTest(Grid{fromFile("../dec10_4.txt")}.findNthZap(Point(8,3), 2), Point(9,0));
    t.expectTest(Grid{fromFile("../dec10_4.txt")}.findNthZap(Point(8,3), 3), Point(9,1));
    t.expectTest(Grid{fromFile("../dec10_4.txt")}.findNthZap(Point(8,3), 4), Point(10,0));
    t.expectTest(Grid{fromFile("../dec10_4.txt")}.findNthZap(Point(8,3), 9), Point(15,1));
    t.expectTest(Grid{fromFile("../dec10_4.txt")}.findNthZap(Point(8,3), 14), Point(12,3));
    t.expectTest(Grid{fromFile("../dec10_4.txt")}.findNthZap(Point(8,3), 30), Point(7,0));
    t.expectTest(Grid{fromFile("../dec10_4.txt")}.findNthZap(Point(8,3), 31), Point(8,0));

    t.expectTest(Grid{fromFile("../dec10_3.txt")}.findNthZap(Point(11,13), 1), Point(11,12));
    t.expectTest(Grid{fromFile("../dec10_3.txt")}.findNthZap(Point(11,13), 2), Point(12,1));
    t.expectTest(Grid{fromFile("../dec10_3.txt")}.findNthZap(Point(11,13), 3), Point(12,2));
    t.expectTest(Grid{fromFile("../dec10_3.txt")}.findNthZap(Point(11,13), 10), Point(12,8));
    t.expectTest(Grid{fromFile("../dec10_3.txt")}.findNthZap(Point(11,13), 20), Point(16,0));
    t.expectTest(Grid{fromFile("../dec10_3.txt")}.findNthZap(Point(11,13), 50), Point(16,9));
    t.expectTest(Grid{fromFile("../dec10_3.txt")}.findNthZap(Point(11,13), 100), Point(10,16));
    t.expectTest(Grid{fromFile("../dec10_3.txt")}.findNthZap(Point(11,13), 199), Point(9,6));
    t.expectTest(Grid{fromFile("../dec10_3.txt")}.findNthZap(Point(11,13), 200), Point(8,2));
    t.expectTest(Grid{fromFile("../dec10_3.txt")}.findNthZap(Point(11,13), 201), Point(10,9));
    t.expectTest(Grid{fromFile("../dec10_3.txt")}.findNthZap(Point(11,13), 299), Point(11,1));

    t.expectTest(Grid{fromFile("../dec10_3.txt")}.findBestNthZap(200), Point(8,2));

    t.expectTest(Grid{fromFile("../dec10_fin.txt")}.findBestNthZap(200), Point(8,2));

    return 1;
}