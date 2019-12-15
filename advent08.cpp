#include "AdventUtil.h"
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <fmt/ranges.h>
#include <vector>

template<class ForwardIt, class Getter>
ForwardIt min_element_on(ForwardIt first, ForwardIt last, Getter getter)
{
    if (first == last) 
        return last;
 
    ForwardIt smallest = first;
    auto curSmallest = getter(*first);
    ++first;
    for (; first != last; ++first) 
    {
        auto curValue = getter(*first);
        if (curValue < curSmallest) 
        {
            curSmallest = curValue;
            smallest = first;
        }
    }
    return smallest;
}

class Image
{
    public:
    class Layer
    {
        public:
        Layer(s64 w, s64 h, std::string const & s): width(w), height(h), data(s)
        {
        }
        char get(s64 x, s64 y) const
        {
            return data[y*width + x];
        }
        void set(s64 x, s64 y, char val)
        {
            data[y*width + x] = val;
        }

        s64 NbVal(char v) const 
        {
            return std::count(data.begin(), data.end(), v);
        }

        void Print() const
        {
            for (int y = 0; y < height; y++)
            {
                std::cout << '1';
                for (int x = 0; x < width; x++)
                {
                    std::cout << (get(x,y) == '0' ? '1' : ' ');
                }
                std::cout << '1';
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }

        s64 check1Val() const 
        {
            return NbVal('0');
        }
        s64 check2Val() const 
        {
            return NbVal('1')*NbVal('2');
        }

        s64 width;
        s64 height;
        std::string data;

    };

    Image(s64 w, s64 h, std::string const & s): width(w), height(h)
    {
        using namespace ranges;
        auto nbLayer = s.length()/(w*h);
        auto layerSize{w*h};
        layers = s | 
            views::chunk(layerSize) | 
            views::transform([w,h](std::string const & s1){ return Layer(w, h, s1);}) | 
            ranges::to<std::vector>();
    }
    Layer getLayer()
    {
        Layer finalLayer(layers.front());
        for (int x = 0; x < width; x ++)
        {
            for (int y = 0; y < height; y++)
            {
                finalLayer.set(x, y, get(x,y));
            }
        }
        return finalLayer;
    }

    s64 getMinLayerVal() const
    {
        auto minLayerIter = min_element_on(layers.begin(), layers.end(), [](Layer const & l){
            return l.check1Val();
        });
        return minLayerIter->check2Val();
    }

    char get(s64 x, s64 y)
    {
        for (auto const & l : layers)
        {
            if (l.get(x,y) != '2')
                return l.get(x,y);
        }
        return '2';
    }

    char get(s64 l, s64 x, s64 y)
    {
        return layers[l].get(x,y);
    }

    s64 width;
    s64 height;
    std::vector<Layer> layers;
};

int main()
{
    Tester t;
    t.expectTest(42, 42);

    t.expectTest(Image{3,2,"123456789012"}.layers.size(), 2U);
    t.expectTest(Image{3,2,"123456789012"}.get(0, 0, 0), '1');
    t.expectTest(Image{3,2,"123456789012"}.get(1, 0, 0), '7');
    t.expectTest(Image{3,2,"123456789012"}.getMinLayerVal(), 1);

    t.expectTest(Image{25,6,fromFile("../dec08_1.txt")}.getMinLayerVal(), 1064);

    auto tdata{"0222112222120000"};
    auto timage = Image{2, 2, tdata};
    t.expectTest(timage.get(0,0), '0');
    t.expectTest(timage.get(1,0), '1');
    t.expectTest(timage.get(1,0), '1');
    t.expectTest(timage.get(1,1), '0');

    t.expectTest(timage.getLayer().get(0,0), '0');
    t.expectTest(timage.getLayer().get(1,0), '1');
    t.expectTest(timage.getLayer().get(1,0), '1');
    t.expectTest(timage.getLayer().get(1,1), '0');

    Image{25,6,fromFile("../dec08_1.txt")}.getLayer().Print();
    return 1;
}