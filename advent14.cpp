#include "AdventUtil.h"
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <fmt/ranges.h>
#include <vector>


class Ing
{
    public:
    Ing() {}
    Ing(std::string const & s)
    {
        std::vector<std::string> values{absl::StrSplit(s, ' ', absl::SkipEmpty())};
        elem = values[1];
        nb = AdventStoi(values[0]);
    }
    
    std::string elem;
    s64 nb;
};

class Recipe
{
    public:
    Recipe(std::string const & s)
    {
        using namespace ranges;
        std::vector<std::string> values{absl::StrSplit(s, "=>")};
        std::string right = values[1];
        output = Ing(right);

        std::string left = values[0];
        std::vector<std::string> inputIng{absl::StrSplit(left, ',')};
        input = inputIng | view::transform([](auto const & s){return Ing{s};}) | to<std::vector<Ing>>;
    }

    Ing output;
    std::vector<Ing> input;
};

class Formulas
{
    public:
    Formulas(std::string const & s)
    {
        using namespace ranges;
        std::vector<std::string> values{absl::StrSplit(s, '\n')};
        recipes = values | view::transform([](auto const & s){return Recipe{s};}) | to<std::vector<Recipe>>;
    }

    class ElemOrder
    {
        public:
        bool operator()(std::string const & e1, std::string const & e2)
        {
            if (e1 == "ORE")
                return false;
            if (e2 == "ORE")
                return true;
            return e1 < e2;
        }
    };
    using Need = std::map<std::string, s64, ElemOrder>;

    s64 findNbFuelFor(s64 nbOre)
    {
        s64 curMin{0};
        s64 curMax{nbOre};
        while ((curMax - curMin) > 1)
        {
            s64 toTestNbFuel = (curMax+curMin)/2;
            auto resNbOre = getNbOre(toTestNbFuel);
            if (resNbOre == nbOre) 
                return toTestNbFuel;
            if (resNbOre < nbOre)
                curMin = toTestNbFuel;
            if (resNbOre > nbOre)
                curMax = toTestNbFuel;
        }
        return curMin;
    }

    s64 getNbOre(s64 nbFuel = 1) const
    {
        return gteNbOrePerFuelAndExtras(nbFuel).first;
    }

    std::pair<s64, Need> gteNbOrePerFuelAndExtras(s64 nbFuel) const
    {
        Need need{{"FUEL", nbFuel}};
        Need extras{};
        while (need.size() != 1 || need.begin()->first != "ORE")
        {
            std::string curElem = need.begin()->first;
            s64 nbNeed = need.begin()->second;
            need.erase(need.begin());

            if (extras[curElem])
            {
                s64 nbExtraToUse = std::min(nbNeed, extras[curElem]);
                nbNeed -= nbExtraToUse;
                extras[curElem] -= nbExtraToUse;
            }

            Recipe curRecipe = getRecipeFor(curElem);
            s64 nbRecipe = nbNeed/curRecipe.output.nb;
            if (nbNeed%curRecipe.output.nb != 0)
            {
                nbRecipe++;
                extras[curElem] += nbRecipe*curRecipe.output.nb - nbNeed;
            }

            for (auto const & i : curRecipe.input)
            {
                need[i.elem] += i.nb*nbRecipe;
            }
        }
        return {need.begin()->second, extras};
    }

    Recipe getRecipeFor(std::string const & e) const
    {
        return *ranges::find_if(recipes, [&e](Recipe const & r){
            return r.output.elem == e;
        });
    }

    std::vector<Recipe> recipes;
};

int main()
{
    Tester t;
    t.expectTest(42, 42);

    t.expectTest(Formulas(fromFile("../dec14_1.txt")).recipes.size(), 6U);
    t.expectTest(Formulas(fromFile("../dec14_1.txt")).getNbOre(), 31);
    t.expectTest(Formulas(fromFile("../dec14_2.txt")).getNbOre(), 165);
    t.expectTest(Formulas(fromFile("../dec14_3.txt")).getNbOre(), 13312);
    t.expectTest(Formulas(fromFile("../dec14_4.txt")).getNbOre(), 180697);
    t.expectTest(Formulas(fromFile("../dec14_5.txt")).getNbOre(), 2210736);

    t.expectTest(Formulas(fromFile("../dec14_fin.txt")).getNbOre(), 1590844);

    t.expectTest(Formulas(fromFile("../dec14_3.txt")).findNbFuelFor(1000000000000), 82892753);
    t.expectTest(Formulas(fromFile("../dec14_4.txt")).findNbFuelFor(1000000000000), 5586022);
    t.expectTest(Formulas(fromFile("../dec14_5.txt")).findNbFuelFor(1000000000000), 460664);
    t.expectTest(Formulas(fromFile("../dec14_fin.txt")).findNbFuelFor(1000000000000), 0);

    return 1;
}