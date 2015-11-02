//
// Created by Pavel Jiranek on 02/11/15.
//

#include <gtest/gtest.h>
#include <vector>
#include <random>
#include <algorithm>

#include "joint_iterator.hpp"

class A
{
    public:
        A()
                : m_i(0) { }

        A(int i)
                : m_i(i) { }

        ~A() { }

        A(A const & a)
        {
            m_i = a.m_i;
            ++numCopyConstructors;
        }

        A(A && a)
        {
            m_i = a.m_i;
            ++numMoveConstructors;
        }

        A & operator=(A const & a)
        {
            m_i = a.m_i;
            ++numCopyAssignments;
            return * this;
        }

        A & operator=(A && a)
        {
            m_i = a.m_i;
            ++numMoveAssignments;
            return * this;
        }

        int operator()() const { return m_i; }

    private:

        int m_i;

    public:

        static size_t numCopyConstructors;
        static size_t numCopyAssignments;
        static size_t numMoveConstructors;
        static size_t numMoveAssignments;

};

size_t A::numCopyConstructors = 0;
size_t A::numCopyAssignments  = 0;
size_t A::numMoveConstructors = 0;
size_t A::numMoveAssignments  = 0;

TEST(TestSortTrace, Run)
{
    std::vector<A> vector;

    std::default_random_engine         generator(0);
    std::uniform_int_distribution<int> distribution(0, 128);

    for (size_t i = 0; i < 1024; ++i)
    {
        int number = distribution(generator);
        vector.push_back(A(number));
    }

    auto                        begin = joint::make_joint(vector.begin());
    auto                        end   = joint::make_joint(vector.end());
    typedef decltype(begin)     iterator;
    typedef iterator::reference reference;

    A::numCopyConstructors = 0;
    A::numMoveConstructors = 0;
    A::numCopyAssignments  = 0;
    A::numMoveAssignments  = 0;

    std::sort(begin, end, [](reference const & a, reference const & b) { return a.get<0>()() < b.get<0>()(); });

    std::cout << A::numCopyConstructors << " copy constructors" << std::endl;
    std::cout << A::numMoveConstructors << " move constructors" << std::endl;
    std::cout << A::numCopyAssignments << " copy assignments" << std::endl;
    std::cout << A::numMoveAssignments << " move assignments" << std::endl;
}
