#include "AdventUtil.h"
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <fmt/ranges.h>
#include <vector>


class Orbit
{
    public:
    Orbit(std::string const & s)
    {
        using namespace ranges;
        std::vector<std::string> values{absl::StrSplit(s, '\n')};
        auto pairRange = values | views::transform([](auto const & s1){
            std::vector<std::string> valPair{absl::StrSplit(s1, ')')};
            return Link{valPair[0], valPair[1]};
        });
        for (auto const & p : pairRange)
        {
            linkmap.insert(p);
        }
    }

    using Path = std::vector<std::string>;
    Path findPathRec(std::string const & dst, std::string const & cur = "COM", Path curPath = {}) const
    {
        if (dst == cur)
            return curPath;
        auto keyRange{linkmap.equal_range(cur)};
        for (auto  p  = keyRange.first; p != keyRange.second; ++p)
        {
            auto newCurPath = curPath;
            newCurPath.push_back(cur);
            auto foundPath = findPathRec(dst, p->second, newCurPath);
            if (!foundPath.empty())
                return foundPath;
        }
        return {};
    }

    Path findPath(std::string const & dst) const
    {
        return findPathRec(dst);
    }

    Path findPath(std::string const & src, std::string const & dst) const
    {
        auto pSrc = findPath(src);
        auto pDst = findPath(dst);
        auto foundMisMatch = std::mismatch(pSrc.begin(), pSrc.end(), pDst.begin(), pDst.end());
        Path result{pSrc.rbegin(), std::make_reverse_iterator(foundMisMatch.first)};
        result.insert(result.end(), foundMisMatch.second, pDst.end());
        return result;
    }

    s64 nbOrbInt(std::string const & src, s64 depth = 0) const
    {
        auto keyRange{linkmap.equal_range(src)};
        return depth + std::accumulate(keyRange.first, keyRange.second, 0, [&depth, this] (s64 a, auto const & link) {
            return a + nbOrbInt(link.second, depth+1);
        });
    }

    s64 nbLink() const 
    {
        return nbOrbInt("COM");
    }


    using Link = std::pair<std::string, std::string>;
    using LinkSet = std::multimap<std::string, std::string>;

    LinkSet linkmap;
};

int main()
{
    Tester t;
    t.expectTest(42, 42);

    t.expectTest(Orbit("COM)B").nbLink(), 1);
    t.expectTest(Orbit("COM)B\nB)C").nbLink(), 3);
    t.expectTest(Orbit("COM)B\nCOM)C").nbLink(), 2);
    t.expectTest(Orbit("COM)B\nCOM)C\nC)D").nbLink(), 4);
    t.expectTest(Orbit("COM)B\nCOM)C\nC)D\nD)E").nbLink(), 7);
    t.expectTest(Orbit("COM)B\nB)C\nC)D\nD)E\nE)F\nB)G\nG)H\nD)I\nE)J\nJ)K\nK)L").nbLink(), 42);
    t.expectTest(Orbit(fromFile("../dec06_1.txt")).nbLink(), 162816);

    t.expectTest(Orbit("COM)B\nB)C").findPath("B"), Orbit::Path{"COM"});
    t.expectTest(Orbit("COM)B\nB)C").findPath("C"), Orbit::Path{"COM", "B"});

    auto testExample = "COM)B\nB)C\nC)D\nD)E\nE)F\nB)G\nG)H\nD)I\nE)J\nJ)K\nK)L\nK)YOU\nI)SAN";
    t.expectTest(Orbit(testExample).findPath("YOU"), Orbit::Path{"COM", "B", "C", "D", "E", "J", "K"});
    t.expectTest(Orbit(testExample).findPath("SAN"), Orbit::Path{"COM", "B", "C", "D", "I"});

    t.expectTest(Orbit(testExample).findPath("YOU", "SAN"), Orbit::Path{"K", "J", "E", "I"});

    t.expectTest(Orbit(fromFile("../dec06_1.txt")).findPath("YOU", "SAN").size(), 0U);

    return 1;
}