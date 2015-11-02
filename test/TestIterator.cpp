//
// Created by Pavel Jiranek on 02/11/15.
//

#include <gtest/gtest.h>
#include <vector>
#include <string>

#include "joint_iterator.hpp"

class TestIterator : public ::testing::Test
{
    protected:
        typedef joint::iterator<std::vector<std::string>::iterator, std::vector<int>::iterator> joint_iterator;

        typedef joint_iterator::value_type value_type;
        typedef joint_iterator::reference  reference;

        std::vector<std::string> strings;
        std::vector<int>         numbers;

        joint_iterator begin;
        joint_iterator end;

        virtual void SetUp()
        {
            strings.push_back("one");
            strings.push_back("two");
            strings.push_back("three");
            strings.push_back("four");
            strings.push_back("five");
            strings.push_back("six");
            strings.push_back("seven");
            strings.push_back("eight");
            strings.push_back("nine");
            strings.push_back("ten");

            for (int i = 1; i <= 10; ++i) numbers.push_back(i);

            begin = joint_iterator(std::make_tuple(strings.begin(), numbers.begin()));
            end   = joint_iterator(std::make_tuple(strings.end(), numbers.end()));
        }
};

TEST_F(TestIterator, Distance)
{
    EXPECT_EQ(10, std::distance(begin, end));
    EXPECT_EQ(10, end - begin);
}

TEST_F(TestIterator, Operators)
{
    EXPECT_EQ("one", *begin.get<0>());
    EXPECT_EQ("one", *(begin++).get<0>());
    EXPECT_EQ("two", *begin.get<0>());

    EXPECT_EQ("ten", *(--end).get<0>());
    EXPECT_EQ("ten", *(end--).get<0>());
    EXPECT_EQ("nine", *end.get<0>());

    begin += 2;
    EXPECT_EQ("four", *begin.get<0>());

    end -= 3;
    EXPECT_EQ("six", *end.get<0>());

    EXPECT_EQ(2, end - begin);
}

TEST_F(TestIterator, Comparison)
{
    auto n  = strings.size();

    auto i1 = begin;
    auto j1 = strings.begin();
    for (int k1 = 0; k1 < n; ++k1)
    {
        auto i2 = begin;
        auto j2 = strings.begin();
        for (int k2 = 0; k2 < n; ++k2)
        {
            EXPECT_EQ(j1 == j2, i1 == i2);
            EXPECT_EQ(j1 != j2, i1 != i2);
            EXPECT_EQ(j1 < j2, i1 < i2);
            EXPECT_EQ(j1 > j2, i1 > i2);
            EXPECT_EQ(j1 <= j2, i1 <= i2);
            EXPECT_EQ(j1 >= j2, i1 >= i2);
            EXPECT_EQ(std::distance(j1, j2), std::distance(i1, i2));
            ++i2;
            ++j2;
        }
        ++i1;
        ++j1;
    }
}

TEST_F(TestIterator, Wrappers)
{
    reference r1 = *begin;
    EXPECT_EQ(1, r1.get<1>());
    EXPECT_EQ("one", r1.get<0>());

    r1.get<1>() = 10;
    EXPECT_EQ(10, *begin.get<1>());

    value_type v1 = *begin;
    EXPECT_EQ(10, v1.get<1>());
    v1.get<1>() = 1;
    EXPECT_EQ(1, v1.get<1>());
    EXPECT_EQ(10, r1.get<1>());
    EXPECT_EQ(10, *begin.get<1>());
}
