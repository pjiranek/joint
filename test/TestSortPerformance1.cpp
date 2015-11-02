//
// Created by Pavel Jiranek on 02/11/15.
//

#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <boost/iterator/counting_iterator.hpp>

#include "joint_iterator.hpp"

class TestSortPerformance1 : public ::testing::Test
{
    protected:
        typedef joint::iterator<std::vector<int>::iterator, std::vector<std::string>::iterator> joint_iterator;

        typedef joint_iterator::value_type value_type;
        typedef joint_iterator::reference  reference;

        typedef std::chrono::high_resolution_clock clock;

        std::vector<int>         numbers;
        std::vector<std::string> strings;

        joint_iterator begin;
        joint_iterator end;

        static size_t const size = 65536;

        virtual void SetUp()
        {
            numbers.reserve(size);
            strings.reserve(size);
            for (size_t i = 0; i < size; ++i)
            {
                numbers.push_back(i);
                strings.push_back("Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt "
                                          "ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud "
                                          "exercitation ullamco laboris nisi ut aliquid ex ea commodi consequat. "
                                          "Quis aute iure reprehenderit in voluptate velit esse cillum dolore eu "
                                          "fugiat nulla pariatur. Excepteur sint obcaecat cupiditat non proident, sunt "
                                          "in culpa qui officia deserunt mollit anim id est laborum.");
                strings.back() += strings.back();
                strings.back() += strings.back();
                strings.back() += strings.back();
            }

            std::default_random_engine generator(0);
            std::random_shuffle(numbers.begin(), numbers.end(), [&](size_t i) { return generator() % i; });

            begin = joint_iterator(std::make_tuple(numbers.begin(), strings.begin()));
            end   = joint_iterator(std::make_tuple(numbers.end(), strings.end()));
        }
};

TEST_F(TestSortPerformance1, JointIteratorDefaultComparator)
{
    auto t1 = clock::now();

    std::sort(begin, end);

    auto t2   = clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "Sort time: " << time << "ms" << std::endl;
    RecordProperty("SortTime", time);

    EXPECT_TRUE(std::is_sorted(numbers.begin(), numbers.end()));
}

TEST_F(TestSortPerformance1, JointIteratorCustomComparatorValueType)
{
    auto t1 = clock::now();

    std::sort(begin, end, [](value_type const & a, value_type const & b) { return a.get<0>() < b.get<0>(); });

    auto t2   = clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "Sort time: " << time << "ms" << std::endl;
    RecordProperty("SortTime", time);

    EXPECT_TRUE(std::is_sorted(numbers.begin(), numbers.end()));
}

TEST_F(TestSortPerformance1, JointIteratorCustomComparatorReference)
{
    auto t1 = clock::now();

    std::sort(begin, end, [](reference const & a, reference const & b) { return a.get<0>() < b.get<0>(); });

    auto t2   = clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "Sort time: " << time << "ms" << std::endl;
    RecordProperty("SortTime", time);

    EXPECT_TRUE(std::is_sorted(numbers.begin(), numbers.end()));
}

TEST_F(TestSortPerformance1, VectorOfStructures)
{
    auto t1 = clock::now();

    struct Aggregate
    {
        int         number;
        std::string string;
    };

    std::vector<Aggregate> aggregates;
    aggregates.reserve(size);

    for (size_t i = 0; i < size; ++i)
        aggregates.push_back(Aggregate{numbers[i], std::move(strings[i])});

    std::sort(aggregates.begin(), aggregates.end(),
              [](Aggregate const & a, Aggregate const & b) { return a.number < b.number; });

    for (size_t i = 0; i < size; ++i)
    {
        numbers[i] = aggregates[i].number;
        strings[i] = std::move(aggregates[i].string);
    }

    auto t2   = clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "Sort time: " << time << "ms" << std::endl;
    RecordProperty("SortTime", time);

    EXPECT_TRUE(std::is_sorted(numbers.begin(), numbers.end()));
}

TEST_F(TestSortPerformance1, PermutationVector)
{
    auto t1 = clock::now();

    std::vector<size_t> permutation(boost::counting_iterator<size_t>(0),
                                    boost::counting_iterator<size_t>(0) + size);

    std::sort(permutation.begin(), permutation.end(),
              [&](size_t a, size_t b) { return numbers[a] < numbers[b]; });

    std::vector<int>         numbers;
    std::vector<std::string> strings;
    numbers.reserve(size);
    strings.reserve(size);

    for (size_t i = 0; i < size; ++i)
    {
        numbers.push_back(this->numbers[permutation[i]]);
        strings.push_back(std::move(this->strings[permutation[i]]));
    }

    this->numbers = std::move(numbers);
    this->strings = std::move(strings);

    auto t2   = clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "Sort time: " << time << "ms" << std::endl;
    RecordProperty("SortTime", time);

    EXPECT_TRUE(std::is_sorted(numbers.begin(), numbers.end()));
}
