//
// Created by Pavel Jiranek on 02/11/15.
//

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

class TestSortPerformance2 : public ::testing::Test
{
    protected:
        typedef joint::iterator<std::vector<int>::iterator, std::vector<long>::iterator> joint_iterator;

        typedef joint_iterator::value_type value_type;
        typedef joint_iterator::reference  reference;

        typedef std::chrono::high_resolution_clock clock;

        std::vector<int>         numbers;
        std::vector<long> longs;

        joint_iterator begin;
        joint_iterator end;

        static size_t const size = 1048576;

        virtual void SetUp()
        {
            numbers.reserve(size);
            longs.reserve(size);
            for (size_t i = 0; i < size; ++i)
            {
                numbers.push_back(i);
                longs.push_back(i);
            }

            std::default_random_engine generator(0);
            std::random_shuffle(numbers.begin(), numbers.end(), [&](size_t i) { return generator() % i; });

            begin = joint_iterator(std::make_tuple(numbers.begin(), longs.begin()));
            end   = joint_iterator(std::make_tuple(numbers.end(), longs.end()));
        }
};

TEST_F(TestSortPerformance2, JointIteratorDefaultComparator)
{
    auto t1 = clock::now();

    std::sort(begin, end);

    auto t2   = clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "Sort time: " << time << "ms" << std::endl;
    RecordProperty("SortTime", time);

    EXPECT_TRUE(std::is_sorted(numbers.begin(), numbers.end()));
}

TEST_F(TestSortPerformance2, JointIteratorCustomComparatorValueType)
{
    auto t1 = clock::now();

    std::sort(begin, end, [](value_type const & a, value_type const & b) { return a.get<0>() < b.get<0>(); });

    auto t2   = clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "Sort time: " << time << "ms" << std::endl;
    RecordProperty("SortTime", time);

    EXPECT_TRUE(std::is_sorted(numbers.begin(), numbers.end()));
}

TEST_F(TestSortPerformance2, JointIteratorCustomComparatorReference)
{
    auto t1 = clock::now();

    std::sort(begin, end, [](reference const & a, reference const & b) { return a.get<0>() < b.get<0>(); });

    auto t2   = clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "Sort time: " << time << "ms" << std::endl;
    RecordProperty("SortTime", time);

    EXPECT_TRUE(std::is_sorted(numbers.begin(), numbers.end()));
}

TEST_F(TestSortPerformance2, VectorOfStructures)
{
    auto t1 = clock::now();

    struct Aggregate
    {
        int         number;
        long        id;
    };

    std::vector<Aggregate> aggregates;
    aggregates.reserve(size);

    for (size_t i = 0; i < size; ++i)
        aggregates.push_back(Aggregate{numbers[i], longs[i]});

    std::sort(aggregates.begin(), aggregates.end(),
    [](Aggregate const & a, Aggregate const & b) { return a.number < b.number; });

    for (size_t i = 0; i < size; ++i)
    {
        numbers[i] = aggregates[i].number;
        longs[i] = aggregates[i].id;
    }

    auto t2   = clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "Sort time: " << time << "ms" << std::endl;
    RecordProperty("SortTime", time);

    EXPECT_TRUE(std::is_sorted(numbers.begin(), numbers.end()));
}

TEST_F(TestSortPerformance2, PermutationVector)
{
    auto t1 = clock::now();

    std::vector<size_t> permutation(boost::counting_iterator<size_t>(0),
                                    boost::counting_iterator<size_t>(0) + size);

    std::sort(permutation.begin(), permutation.end(),
              [&](size_t a, size_t b) { return numbers[a] < numbers[b]; });

    std::vector<int> numbers;
    std::vector<long> longs;
    numbers.reserve(size);
    longs.reserve(size);

    for (size_t i = 0; i < size; ++i)
    {
        numbers.push_back(this->numbers[permutation[i]]);
        longs.push_back(this->longs[permutation[i]]);
    }

    this->numbers = std::move(numbers);
    this->longs  = std::move(longs);

    auto t2   = clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cout << "Sort time: " << time << "ms" << std::endl;
    RecordProperty("SortTime", time);

    EXPECT_TRUE(std::is_sorted(numbers.begin(), numbers.end()));
}
