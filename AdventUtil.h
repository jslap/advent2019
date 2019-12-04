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
template<>
struct formatter<BigInt>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const BigInt& i, FormatContext& ctx)
    {
        return format_to(ctx.begin(), "({}, {})", absl::Uint128High64(i), absl::Uint128Low64(i));
    }
};

} // namespace fmt

class Tester
{
public:
    Tester() = default;
    ~Tester() 
    {
        printResult();
    }
    int nbOk = 0;
    int nbFail = 0;
    int nbTotal = 0;
    template<class T1, class T2>
    void expectTest(const T1& got, const T2& expected)
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
        fmt::print("Done\n");
    }
};

std::string fromFile(const std::string &fileName)
{

    std::ifstream ifs(fileName);
    std::string srcStr((std::istreambuf_iterator<char>(ifs)),
                       (std::istreambuf_iterator<char>()));
    return srcStr;
}

int AdventStoi(std::string const & s)
{
    return std::stoi(s);
}