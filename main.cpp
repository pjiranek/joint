#include <iostream>
#include <vector>

#include "joint_iterator.hpp"

int main()
{
    std::vector<int>         numbers = {4, 3, 2, 1, 0};
    std::vector<std::string> strings = {"four", "three", "two", "one", "zero"};

    auto begin = joint::make_joint(numbers.begin(), strings.begin());
    auto end   = joint::make_joint(numbers.end(), strings.end());

    for (auto iter = begin; iter != end; ++iter)
        std::cout << (* iter).get<0>() << " " << (* iter).get<1>() << "\n";
    std::cout << "\n";

    typedef decltype(begin)            joint_iterator;
    typedef joint_iterator::reference  reference;
    typedef joint_iterator::value_type value_type;

    std::sort(begin, end);

    std::cout << "Sorted by values:\n";
    for (auto iter = begin; iter != end; ++iter)
        std::cout << * iter.get<0>() << " " << * iter.get<1>() << "\n";
    std::cout << "\n";

    std::sort(begin, end,
              [](reference const a, reference const b) { return a.get<1>() < b.get<1>(); });

    std::cout << "Sorted by strings:\n";
    for (auto iter = begin; iter != end; ++iter)
        std::cout << * iter.get<0>() << " " << * iter.get<1>() << "\n";
    std::cout << "\n";

    return 0;
}
