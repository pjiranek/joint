//
// Created by Pavel Jiranek on 02/11/15.
//

#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <sstream>
#include <random>

#include "joint_iterator.hpp"

template<typename T> std::string to_string(T const & v)
{
    std::stringstream s;
    s << v;
    return s.str();
}

class TestAlgorithm : public ::testing::Test
{
    protected:
        typedef joint::iterator<std::vector<int>::iterator, std::vector<std::string>::iterator> joint_iterator;

        typedef joint_iterator::value_type value_type;
        typedef joint_iterator::reference  reference;

        std::vector<int>         numbers;
        std::vector<std::string> strings;

        static size_t const size      = 128;
        static int const    randomMax = 64;

        joint_iterator begin;
        joint_iterator end;

        void createSortedVectors()
        {
            numbers.clear();
            strings.clear();

            std::default_random_engine         generator(0);
            std::uniform_int_distribution<int> distribution(0, randomMax);

            for (size_t i = 0; i < size; ++i)
            {
                int number = distribution(generator);
                numbers.push_back(number);
            }

            std::sort(numbers.begin(), numbers.end());
            for (size_t i = 0; i < size; ++i)
            {
                strings.push_back(to_string(numbers[i]));
            }

            begin = joint::make_joint(numbers.begin(), strings.begin());
            end   = joint::make_joint(numbers.end(), strings.end());
        }

        void createRandomVectors()
        {
            numbers.clear();
            strings.clear();

            std::default_random_engine         generator(0);
            std::uniform_int_distribution<int> distribution(0, randomMax);

            for (size_t i = 0; i < size; ++i)
            {
                int number = distribution(generator);
                numbers.push_back(number);
                strings.push_back(to_string(number));
            }

            begin = joint::make_joint(numbers.begin(), strings.begin());
            end   = joint::make_joint(numbers.end(), strings.end());
        }

        // Test if the string representation of the numbers are consistent.
        void testVectorsAreEqual() const;
        void testVectorsAreEqual(std::vector<int> const & numbers,
                                 std::vector<std::string> const & strings) const;
        void testVectorsAreEqual(std::vector<int> const & numbers,
                                 std::vector<std::string> const & strings,
                                 size_t n) const;

};

void TestAlgorithm::testVectorsAreEqual() const
{
    testVectorsAreEqual(this->numbers, this->strings);
}

void TestAlgorithm::testVectorsAreEqual(std::vector<int> const & numbers,
                                        std::vector<std::string> const & strings) const
{
    testVectorsAreEqual(numbers, strings, numbers.size());
}

void TestAlgorithm::testVectorsAreEqual(std::vector<int> const & numbers,
                                        std::vector<std::string> const & strings,
                                        size_t n) const
{
    std::vector<std::string> test_strings;

    for (size_t i = 0; i < n; ++i)
        test_strings.push_back(to_string(numbers[i]));

    ASSERT_EQ(test_strings, strings);
}

TEST_F(TestAlgorithm, Copy)
{
    createRandomVectors();

    std::vector<int>         numbers(size);
    std::vector<std::string> strings(size);

    auto begin = joint::make_joint(numbers.begin(), strings.begin());

    std::copy(this->begin, this->end, begin);
    EXPECT_EQ(this->numbers, numbers);
    EXPECT_EQ(this->strings, strings);
    testVectorsAreEqual(numbers, strings);
}

TEST_F(TestAlgorithm, CopyN)
{
    createRandomVectors();

    std::vector<int>         numbers(size / 2);
    std::vector<std::string> strings(size / 2);

    auto begin = joint::make_joint(numbers.begin(), strings.begin());

    std::copy_n(this->begin, size / 2, begin);

    EXPECT_TRUE(std::equal(this->numbers.begin(), this->numbers.begin() + size / 2, numbers.begin()));
    testVectorsAreEqual(numbers, strings);
}

TEST_F(TestAlgorithm, CopyBackward)
{
    createRandomVectors();

    std::vector<int>         numbers(size);
    std::vector<std::string> strings(size);

    auto begin = std::reverse_iterator<joint_iterator>(joint::make_joint(numbers.begin(), strings.begin()));

    std::copy_backward(this->begin, this->end, begin);

    EXPECT_TRUE(std::equal(this->numbers.begin(), this->numbers.end(), numbers.rbegin()));
    testVectorsAreEqual(numbers, strings);
}

TEST_F(TestAlgorithm, Sort)
{
    auto comparator = [](reference const & a, reference const & b) { return a.get<0>() < b.get<0>(); };

    createRandomVectors();

    std::sort(begin, end, comparator);

    EXPECT_TRUE(std::is_sorted(numbers.begin(), numbers.end()));
    testVectorsAreEqual();
}

TEST_F(TestAlgorithm, StableSort)
{
    auto comparator = [](reference const & a, reference const & b) { return a.get<0>() < b.get<0>(); };

    createRandomVectors();

    std::stable_sort(begin, end, comparator);

    EXPECT_TRUE(std::is_sorted(numbers.begin(), numbers.end()));
    testVectorsAreEqual();
}

TEST_F(TestAlgorithm, PartialSort)
{
    auto comparator = [](reference const & a, reference const & b) { return a.get<0>() < b.get<0>(); };

    createRandomVectors();

    std::partial_sort(begin, begin + size / 2, end, comparator);

    EXPECT_TRUE(std::is_sorted(numbers.begin(), numbers.begin() + size / 2));
    testVectorsAreEqual();
}

TEST_F(TestAlgorithm, PartialSortCopy)
{
    auto comparator = [](reference const & a, reference const & b) { return a.get<0>() < b.get<0>(); };

    createRandomVectors();

    std::vector<int>         numbers2(size / 2);
    std::vector<std::string> strings2(size / 2);

    auto begin2 = joint::make_joint(numbers2.begin(), strings2.begin());
    auto end2   = joint::make_joint(numbers2.end(), strings2.end());

    std::partial_sort_copy(begin, end, begin2, end2, comparator);

    EXPECT_TRUE(std::is_sorted(numbers2.begin(), numbers2.end()));
    testVectorsAreEqual();
    testVectorsAreEqual(numbers2, strings2);
}

TEST_F(TestAlgorithm, Merge)
{
    createSortedVectors();
    auto numbers1 = numbers;
    auto strings1 = strings;
    auto begin1   = joint::make_joint(numbers1.begin(), strings1.begin());
    auto end1     = joint::make_joint(numbers1.end(), strings1.end());

    createSortedVectors();
    auto numbers2 = numbers;
    auto strings2 = strings;
    auto begin2   = joint::make_joint(numbers2.begin(), strings2.begin());
    auto end2     = joint::make_joint(numbers2.end(), strings2.end());

    this->numbers.resize(numbers1.size() + numbers2.size());
    this->strings.resize(strings1.size() + strings2.size());
    begin = joint::make_joint(numbers.begin(), strings.begin());

    std::merge(begin1, end1, begin2, end2, begin);

    testVectorsAreEqual();

    std::vector<int> numbers(numbers1.size() + numbers2.size());
    std::merge(numbers1.begin(), numbers1.end(), numbers2.begin(), numbers2.end(), numbers.begin());

    EXPECT_EQ(numbers, this->numbers);
}

TEST_F(TestAlgorithm, NthElement)
{
    auto comparator = [](reference const & a, reference const & b) { return a.get<0>() < b.get<0>(); };

    createSortedVectors();
    auto numbers = this->numbers;

    std::nth_element(begin, begin + size / 2, end);

    testVectorsAreEqual();

    std::nth_element(numbers.begin(), numbers.begin() + size / 2, numbers.end());
    EXPECT_EQ(numbers[size / 2], this->numbers[size / 2]);
}
