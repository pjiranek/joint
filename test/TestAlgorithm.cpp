//
// Created by Pavel Jiranek on 02/11/15.
//

#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <algorithm>

#include "joint_iterator.hpp"

// We do not really test here "joint" iterators but rather a simple vector (of strings) wrapper in the joint iterator.
// If it works properly for a vector of strings, it works probably also for other data types and multiple vectors.

class TestAlgorithm : public ::testing::Test
{
    protected:
        typedef joint::iterator<std::vector<std::string>::iterator> joint_iterator;

        typedef joint_iterator::value_type value_type;
        typedef joint_iterator::reference  reference;

        std::vector<std::string> createSorted(std::size_t size)
        {
            auto vector = createUnsorted(size);

            // I hope I can count on STL sort here. :-)
            std::sort(vector.begin(), vector.end());

            return vector;
        }

        std::vector<std::string> createUnsorted(std::size_t size)
        {
            static std::vector<std::string> const nouns   = {"Cat", "Mouse", "Dog", "Elephant"};
            static std::vector<std::string> const verbs   = {"eats", "plays with", "does not collaborate with"};
            static std::vector<std::string> const objects = {"cats", "mice", "dogs", "elephants"};

            std::vector<std::string> vector;

            std::default_random_engine         generator(0);
            std::uniform_int_distribution<int> distribution(0, 1024);

            for (size_t i = 0; i < size; ++i)
            {
                std::string sentence = nouns[distribution(generator) % nouns.size()] + " "
                                       + verbs[distribution(generator) % verbs.size()] + " "
                                       + objects[distribution(generator) % objects.size()] + ".";

                vector.push_back(sentence);
            }

            return vector;
        }

        template<typename Iterator> joint::iterator<Iterator> make_joint(Iterator iterator)
        {
            return joint::make_joint(iterator);
        }

};

TEST_F(TestAlgorithm, Copy)
{
    auto vector_original = createUnsorted(16);

    auto vector_source = vector_original;
    auto vector_target = std::vector<std::string>(vector_source.size());

    auto first1 = make_joint(vector_source.begin());
    auto last1  = make_joint(vector_source.end());
    auto first2 = make_joint(vector_target.begin());

    std::copy(first1, last1, first2);

    EXPECT_EQ(vector_original, vector_source);
    EXPECT_EQ(vector_original, vector_target);
}

TEST_F(TestAlgorithm, CopyN)
{
    auto vector_original = createUnsorted(16);

    auto vector_source = vector_original;
    auto vector_target = std::vector<std::string>(vector_source.size() / 2);

    auto first1 = make_joint(vector_source.begin());
    auto first2 = make_joint(vector_target.begin());

    std::copy_n(first1, vector_source.size() / 2, first2);

    EXPECT_EQ(vector_original, vector_source);
    EXPECT_TRUE(std::equal(vector_source.begin(),
                           vector_source.begin() + vector_source.size() / 2,
                           vector_target.begin()));
}

TEST_F(TestAlgorithm, CopyBackward)
{
    auto vector_original = createUnsorted(16);

    auto vector_source = vector_original;
    auto vector_target = std::vector<std::string>(vector_source.size());

    auto first1 = make_joint(vector_source.begin());
    auto last1  = make_joint(vector_source.end());
    auto last2 = make_joint(vector_target.end());

    std::copy_backward(first1, last1, last2);

    EXPECT_EQ(vector_original, vector_source);
    EXPECT_EQ(vector_source, vector_target);
}

TEST_F(TestAlgorithm, Sort)
{
    auto vector_original = createUnsorted(128);
    auto vector = vector_original;

    auto first = make_joint(vector.begin());
    auto last  = make_joint(vector.end());

    std::sort(first, last);

    EXPECT_TRUE(std::is_sorted(vector.begin(), vector.end()));

    std::sort(vector_original.begin(), vector_original.end());
    EXPECT_EQ(vector_original, vector);
}

TEST_F(TestAlgorithm, StableSort)
{
    auto vector_original = createUnsorted(128);
    auto vector = vector_original;

    auto first = make_joint(vector.begin());
    auto last  = make_joint(vector.end());

    std::stable_sort(first, last);

    EXPECT_TRUE(std::is_sorted(vector.begin(), vector.end()));

    std::sort(vector_original.begin(), vector_original.end());
    EXPECT_EQ(vector_original, vector);
}

TEST_F(TestAlgorithm, PartialSort)
{

    auto vector = createUnsorted(128);

    auto first = make_joint(vector.begin());
    auto last  = make_joint(vector.end());

    std::stable_sort(first, last);

    EXPECT_TRUE(std::is_sorted(vector.begin(), vector.end()));
}
