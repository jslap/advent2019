#include <fstream>
#include <iostream>

#include <algorithm>
#include <cwctype>
#include <deque>
#include <locale>
#include <numeric>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#define FMT_STRING_ALIAS 1
#include <fmt/format.h>

#include "absl/numeric/int128.h"
#include "absl/strings/str_split.h"

#include <range/v3/all.hpp>

using BigInt = absl::uint128;

namespace fmt
{
template <>
struct formatter<BigInt>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext &ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const BigInt &i, FormatContext &ctx)
    {
        return format_to(ctx.begin(), "({}, {})", absl::Uint128High64(i), absl::Uint128Low64(i));
    }
};

} // namespace fmt

int nbOk = 0;
int nbFail = 0;
int nbTotal = 0;
template <class T1, class T2>
void expectTest(const T1 &got, const T2 &expected)
{
    nbTotal++;
    if (got == expected)
    {
        nbOk++;
        fmt::print("Test OK");
    }
    else
    {
        nbFail++;
        fmt::print("Test Fail: got {} should be {}", got, expected);
    }
    std::cout << std::endl;
}
void printResult()
{
    fmt::print(fmt("End: Total nb of test: {}\n"), nbTotal);
    fmt::print(fmt("     Total nb of Pass: {}\n"), nbOk);
    fmt::print(fmt("     Total nb of Fail: {}\n"), nbFail);
}

std::string fromFile(const std::string &fileName)
{

    std::ifstream ifs(fileName);
    std::string srcStr((std::istreambuf_iterator<char>(ifs)),
                       (std::istreambuf_iterator<char>()));
    return srcStr;
}

using u32 = std::uint32_t;
struct Pos
{
    u32 x;
    u32 y;

    bool operator!=(Pos const & rhp) {return x!=rhp.x || y != rhp.y;}
    Pos down() {Pos retVal{*this}; retVal.y++; return retVal;}
    Pos right() {Pos retVal{*this}; retVal.x++; return retVal;}
    Pos left() {Pos retVal{*this}; retVal.x--; return retVal;}
};

struct SingleRange
{
    u32 start;
    u32 fin;
};

class Range
{
    public:

    Range(std::string const & s)
    {
        std::vector<std::string> rPair = absl::StrSplit(s, ',');
        for (auto const & r : rPair)
        {
            std::vector<std::string> assignSep = absl::StrSplit(r, '=');
            if (assignSep[0].back() == 'x')
                xRange = readRange(assignSep[1]);
            else
                yRange = readRange(assignSep[1]);
        }

    }
    static SingleRange readRange(std::string const & s)
    {
        std::vector<std::string> rValues = absl::StrSplit(s, '.');
        if (rValues.size() == 1)
        {
            u32 val = std::stoul(rValues.front());
            return SingleRange{val, val};
        }
        else
        {
            u32 val1 = std::stoul(rValues.front());
            u32 val2 = std::stoul(rValues.back());
            return SingleRange{val1, val2};
        }
    }

    SingleRange xRange;
    SingleRange yRange;
};

enum class Cell
{
    Empty, Clay, Source, Water, PastWater
};

class SoilMap
{
    public:
    SoilMap(std::vector<Range> const & rangeVec)
    {
        if (rangeVec.empty())
            return;

        mXRange = std::accumulate(rangeVec.begin(), rangeVec.end(), rangeVec.front().xRange, 
        [](SingleRange const & sr, Range const & rr){
            return SingleRange{std::min(sr.start, rr.xRange.start), std::max(sr.fin, rr.xRange.fin)};
        });
        mXRange.start--;
        mXRange.fin++;

        mYRange = std::accumulate(rangeVec.begin(), rangeVec.end(), rangeVec.front().yRange, 
        [](SingleRange const & sr, Range const & rr){
            return SingleRange{std::min(sr.start, rr.yRange.start), std::max(sr.fin, rr.yRange.fin)};
        });
        mRealStartY = mYRange.start;
        mYRange.start = 1;


        mmap = std::vector<Cell>{(mXRange.fin-mXRange.start + 1)*(mYRange.fin-mYRange.start + 1), Cell::Empty};
        print();

        for (auto const & r: rangeVec)
        {
            for (u32 x = r.xRange.start; x <= r.xRange.fin; x++)
            {
                for (u32 y = r.yRange.start; y <= r.yRange.fin; y++)
                {
                    get(Pos{x,y}) = Cell::Clay;
                }
            }
        }
        print();
    }

    void print(std::string const & fileName) const
    {
        std::ofstream out(fileName);
        out << print();
        out.close();
    }
    std::string print() const
    {
        std::string retVal;
        for (u32 y = mYRange.start; y <= mYRange.fin; y++)
        {
            for (u32 x = mXRange.start; x <= mXRange.fin; x++)
            {
                retVal += printCell(get(Pos{x,y}));
            }
            retVal += "\n";
        }
        retVal += "\n";
        return retVal;
    }
    static std::string printCell(Cell c)
    {
        switch (c)
        {
        case Cell::Empty:
            return ".";
            break;
        case Cell::Clay:
            return "#";
            break;
        case Cell::Source:
            return "+";
            break;
        case Cell::Water:
            return "~";
            break;
        case Cell::PastWater:
            return "|";
            break;
        default:
            return "&";
            break;
        }
    }

    bool set(Pos const & p, Cell newVal)
    {
        if (get(p) != newVal)
        {
            get(p) = newVal;
            return true;
        }
        return false;
    }

    bool putWater()
    {
        bool didChange = false;

        std::vector<Pos> addedWater{Pos{500,1}};
        while (!addedWater.empty())
        {
            Pos curWater = addedWater.back();
            addedWater.pop_back();

            bool dropDone = false;
            while (!dropDone)
            {
                Pos nextPos{curWater.down()};

                if (isContinue(nextPos) || isOutside(nextPos))
                {
                    if (set(curWater, Cell::PastWater))
                        didChange = true;
                    curWater = nextPos;
                }
                if (isOutside(nextPos))
                    break;
                
                if (isStop(nextPos))
                {
                    dropDone = true;
                    Pos goRight{curWater};
                    while (isContinue(goRight.right()) && isStop(goRight.down()))
                    {
                        if (set(goRight, Cell::PastWater))
                            didChange = true;
                        goRight = goRight.right();
                    }
                    if (set(goRight, Cell::PastWater))
                        didChange = true;

                    Pos goLeft{curWater};
                    while (isContinue(goLeft.left()) && isStop(goLeft.down()))
                    {
                        if (set(goLeft, Cell::PastWater))
                            didChange = true;
                        goLeft = goLeft.left();
                    }
                    if (set(goLeft, Cell::PastWater))
                        didChange = true;

                    if (isStop(goRight.down()) && isStop(goLeft.down()))
                    {
                        while (goRight != goLeft)
                        {
                            if (set(goLeft, Cell::Water))
                                didChange = true;
                            goLeft = goLeft.right();
                        }
                        if (set(goLeft, Cell::Water))
                            didChange = true;
                    }
                    else
                    {
                        if (!isStop(goRight.down()))
                            addedWater.push_back(goRight);
                        if (!isStop(goLeft.down()))
                            addedWater.push_back(goLeft);
                    }
                    
                }
            }
        }
        return didChange;
    }

    bool isStop(Pos const & p) const {return get(p)==Cell::Water || get(p)==Cell::Clay;}
    bool isContinue(Pos const & p) const {return get(p)==Cell::PastWater || get(p)==Cell::Empty;}
    bool isOutside(Pos const & p) const 
    {
        return !(
            (p.x >= mXRange.start) && (p.x <= mXRange.fin) && 
            (p.y >= mYRange.start) && (p.y <= mYRange.fin)); 
    }

    Cell& get(Pos const & p)
    {
        return mmap[getIndex(p)];
    }
    Cell const & get(Pos const & p) const
    {
        return mmap[getIndex(p)];
    }
    unsigned int getIndex(Pos const & p) const
    {
        return (p.y-mYRange.start) * (mXRange.fin-mXRange.start + 1) +  (p.x-mXRange.start);
    }
    u32 countWater() const
    {
        return std::count_if(mmap.begin(), mmap.end(), [](Cell c)
        {
            return (c==Cell::PastWater) || (c==Cell::Water);
        }) - (mYRange.start - mRealStartY);
    }

    SingleRange mXRange;
    SingleRange mYRange;
    u32 mRealStartY;
    std::vector<Cell> mmap;
};

int processFile(std::string const & s)
{
    std::vector<std::string> lines = absl::StrSplit(s, '\n');
    std::vector<Range> rvec;
    std::transform(lines.begin(), lines.end(), std::back_inserter(rvec), [](std::string const & s){return Range{s};});


    SoilMap sm{rvec};

    for (int i = 0; i < 2000; i++)
    {
        if (!sm.putWater())
            break;
        fmt::print("Begin pass {}\n", i);
        sm.print(fmt::format("OutMap{}.txt", i));
        fmt::print("Current water: {}\n", sm.countWater());
    }
    sm.print("FinMap.txt");

    return sm.countWater();
}



int main()
{
    expectTest(42, 42);

    // expectTest(possibleOpcode(State("Before: [3, 2, 1, 1]"), State("After:  [3, 2, 2, 1]"), InOuts("9 2 1 2")).size(), 3U);
    // expectTest(processFile(fromFile("dec17-1.txt")), 57);
    expectTest(processFile(fromFile("dec17-2.txt")), 57);


    // expectTest(compute(fromFile("dec16.txt")), 0);

    printResult();
    fmt::print("Done\n");

    return 0;
}