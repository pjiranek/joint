//
// Created by Pavel Jiranek on 29/10/15.
//

#ifndef JOINT_ITERATOR_HPP
#define JOINT_ITERATOR_HPP

#include <tuple>
#include <iterator>
#include <utility>
#include <type_traits>

#include <iostream>

namespace joint
{

    namespace detail
    {

        // Stuff to check if the iterator(s) are have the random access tag.
        template<typename Iterator>
        struct is_random_access_iterator
                : std::integral_constant<bool,
                                         std::is_same<typename std::iterator_traits<Iterator>::iterator_category,
                                                      std::random_access_iterator_tag>::value>
        {
        };

        template<typename... Iterators>
        struct assert_random_access { };

        template<typename Iterator>
        struct assert_random_access<Iterator>
        {
            static_assert(is_random_access_iterator<Iterator>::value, "Iterator is not random access.");
        };

        template<typename Iterator, typename... Iterators>
        struct assert_random_access<Iterator, Iterators...> : public assert_random_access<Iterators...> { };

        // This stuff is to implement "for-eachers" for tuples.
        // It was "stolen" and subsequently customized using a post on stackoverflow.com
        // (google "std tuple for each" for the original post).
        template<size_t... Is>
        struct sequence { };

        template<size_t N, size_t... Is>
        struct generate_sequence : generate_sequence<N - 1, N - 1, Is...> { };

        template<size_t... Is>
        struct generate_sequence<0, Is...> : sequence<Is...> { };

        // "For-each" function for one tuple with arguments.
        template<typename T, typename F, typename... Args, size_t... Is>
        void for_each_one_tuple_impl(T & t, F f, sequence<Is...>, Args... args)
        {
            auto l = {(f(std::get<Is>(t), args...), 0)...};
        };

        template<typename... Ts, typename F, typename... Args>
        void for_each_one_tuple(std::tuple<Ts...> & t, F f, Args... args)
        {
            for_each_one_tuple_impl(t, f, generate_sequence<sizeof...(Ts)>(), args...);
        };

        template<typename T1, typename T2, typename F, size_t... Is>
        void for_each_two_tuples_rhs_nonconst_lvalue_impl(T1 & t1, T2 & t2, F f, sequence<Is...>)
        {
            auto l = {(f(std::get<Is>(t1), std::get<Is>(t2)), 0)...};
        };

        template<typename... Ts1, typename... Ts2, typename F>
        void for_each_two_tuples_rhs_nonconst_lvalue(std::tuple<Ts1...> & t1, std::tuple<Ts2...> & t2, F f)
        {
            for_each_two_tuples_rhs_nonconst_lvalue_impl(t1, t2, f, generate_sequence<sizeof...(Ts1)>());
        };

        template<typename T1, typename T2, typename F, size_t... Is>
        void for_each_two_tuples_rhs_const_lvalue_impl(T1 & t1, T2 const & t2, F f, sequence<Is...>)
        {
            auto l = {(f(std::get<Is>(t1), std::get<Is>(t2)), 0)...};
        };

        template<typename... Ts1, typename... Ts2, typename F>
        void for_each_two_tuples_rhs_const_lvalue(std::tuple<Ts1...> & t1, std::tuple<Ts2...> const & t2, F f)
        {
            for_each_two_tuples_rhs_const_lvalue_impl(t1, t2, f, generate_sequence<sizeof...(Ts1)>());
        };

        template<typename T1, typename T2, typename F, size_t... Is>
        void for_each_two_tuples_rhs_rvalue_impl(T1 & t1, T2 && t2, F f, sequence<Is...>)
        {
            auto l = {(f(std::get<Is>(t1), std::get<Is>(t2)), 0)...};
        };

        template<typename... Ts1, typename... Ts2, typename F>
        void for_each_two_tuples_rhs_rvalue(std::tuple<Ts1...> & t1, std::tuple<Ts2...> && t2, F f)
        {
            for_each_two_tuples_rhs_rvalue_impl(t1, t2, f, generate_sequence<sizeof...(Ts1)>());
        };


        // Functors passed to the "for-eachers".
        // =====================================

        // Swap the values pointed to by two pointers.
        struct pointer_content_swapper
        {
            template<typename P> void operator()(P * p1, P * p2) noexcept
            {
                using namespace std;

                swap(* p1, * p2);
            }
        };

        // Advance an iterator by n.
        struct iterator_advancer
        {
            template<typename I> void operator()(I & i, typename I::difference_type n) { i += n; }
        };

        // Copy pointers.
        struct copy_pointers
        {
            // Since we want to make this work with iterators, we first dereference the right-hand side pointers
            // and then take the address again.
            template<typename P, typename I> void operator()(P *& p, I i) { p = &* i; }
        };

        // Copy pointers.
        struct copy_pointer_values
        {
            // Simple copy of the content of p2 to the object pointed to by p1 using the copy assignment.
            template<typename P> void operator()(P * p1, P const * p2) { * p1 = * p2; }
        };

        // Copy pointers.
        struct move_pointer_values
        {
            // Simple move of the content of p2 to the object pointed to by p1 using the move assignment.
            template<typename P> void operator()(P * p1, P * p2) { * p1 = std::move(* p2); }
        };

        // Make pointers to values.
        struct make_pointers_to_values
        {
            template<typename T> void operator()(T *& p, T & v) { p = & v; }
        };

        // Copy values to pointers.
        struct copy_values_to_pointers
        {
            template<typename T> void operator()(T * p, T const & v) { * p = v; }
        };

        // Move values to pointers.
        struct move_values_to_pointers
        {
            template<typename T> void operator()(T * p, T & v) { * p = std::move(v); }
        };

        //! Copy values from pointers.
        struct copy_values_from_pointers
        {
            template<typename T, typename I> void operator()(T & v, I i) { v = * i; }
        };

        //! Move values from pointers.
        struct move_values_from_pointers
        {
            template<typename T> void operator()(T & v1, T * v2) { v1 = std::move(* v2); }
        };

    }

    // Forward declarations.
    template<typename...> class value_wrapper;

    template<typename...> class reference_wrapper;

    template<typename... Is> void swap(reference_wrapper<Is...> a, reference_wrapper<Is...> b) noexcept;

    //! A wrapper class serving as a reference type for `iterator<Iterators...>`.
    //!
    //! Since I don't know how to make a tuple of references from a tuple of values/pointers/..., the reference wrapper
    //! is implemented as a tuple of *pointers*.
    template<typename... Iterators>
    class reference_wrapper
    {
        public:

            //! Create a reference on the references provided by the iterators.
            //!
            //! The references point to the data referenced by the iterators.
            reference_wrapper(std::tuple<Iterators...> & iterators)
            {
                detail::for_each_two_tuples_rhs_nonconst_lvalue(m_pointers, iterators,
                                                                detail::copy_pointers());
            }

            //! Copy constructor initializes the references to point to the same values.
            //!
            //! The references point to the same values as the right-hand side similarly to the ordinary reference
            //! initialization.
            reference_wrapper(reference_wrapper<Iterators...> const &) = default;

            //! Copy assignment copies the content of other references into the data pointed to by "this" references.
            //!
            //! Note that the copy assignment is different from the copy constructor. Instead of making the reference
            //! point to the same data as the data given by the right-hand side, the copy assignment copies the values
            //! similarly to the typical assignment of ordinary references.
            reference_wrapper<Iterators...> operator=(reference_wrapper<Iterators...> const & refs)
            {
                detail::for_each_two_tuples_rhs_const_lvalue(m_pointers, refs.m_pointers,
                                                             detail::copy_pointer_values());
                return * this;
            }

            //! Move constructor initializes the references to point to the same values.
            //!
            //! This is the default behavior of the move constructor.
            reference_wrapper(reference_wrapper<Iterators...> &&) = default;

            //! Move assignment moves the content of other references into the data pointed to by "this" references.
            reference_wrapper<Iterators...> operator=(reference_wrapper<Iterators...> && refs)
            {
                // NOTE This cannot be implemented using moves; otherwise, a simple std::copy() fails!
                detail::for_each_two_tuples_rhs_const_lvalue(m_pointers, refs.m_pointers,
                                                             detail::copy_pointer_values());
                return * this;
            }

            //! Create references on the values.
            reference_wrapper(value_wrapper<Iterators...> & vals);

            //! Copy the content of the values to the references.
            reference_wrapper<Iterators...> & operator=(value_wrapper<Iterators...> const & vals);

            //! Move the content of the values to the references.
            reference_wrapper<Iterators...> & operator=(value_wrapper<Iterators...> && vals);

            //! Get the I-th reference.
            template<size_t I>
            typename std::tuple_element<I, std::tuple<Iterators...>>::type::reference get()
            {
                return * std::get<I>(m_pointers);
            };

            //! Get the I-th reference.
            template<size_t I>
            typename std::tuple_element<I, std::tuple<Iterators...>>::type::reference const get() const
            {
                return * std::get<I>(m_pointers);
            };

        private:

            std::tuple<typename std::iterator_traits<Iterators>::pointer...> m_pointers;

            template<typename... Is> friend void swap(reference_wrapper<Is...> a, reference_wrapper<Is...> b) noexcept;

            template<typename...> friend class value_wrapper;
    };

    //! Swap two reference wrappers.
    //!
    //! This method does not swap the two objects but instead swaps the contents in a componentwise fashion
    //! and is the reason why our approach works. The arguments cannot be taken by reference since the dereference
    //! operator of the joint iterator does not return an l-value reference!
    template<typename... Iterators>
    void swap(reference_wrapper<Iterators...> a, reference_wrapper<Iterators...> b) noexcept
    {
        detail::for_each_two_tuples_rhs_nonconst_lvalue(a.m_pointers, b.m_pointers,
                                                        detail::pointer_content_swapper());
    }

    //! A wrapper class serving as a value type for `iterator<Iterators...>`.
    template<typename... Iterators>
    class value_wrapper
    {
        public:

            //! Create values from the tuple of iterators.
            value_wrapper(std::tuple<Iterators...> const & iterators)
            {
                detail::for_each_two_tuples_rhs_const_lvalue(m_values, iterators,
                                                             detail::copy_values_from_pointers());
            }

            //! Create values from a tuple of values.
            value_wrapper(std::tuple<typename std::iterator_traits<Iterators>::value_type...> const & values)
                    : m_values(values) { }

            //! Create values from a tuple of values.
            value_wrapper(std::tuple<typename std::iterator_traits<Iterators>::value_type...> && values)
                    : m_values(values) { }

            //! Copy values from a tuple of values.
            value_wrapper<Iterators...> &
            operator=(std::tuple<typename std::iterator_traits<Iterators>::value_type...> const & values)
            {
                m_values = values;
                return * this;
            }

            //! Copy values from a tuple of values.
            value_wrapper<Iterators...> &
            operator=(std::tuple<typename std::iterator_traits<Iterators>::value_type...> && values)
            {
                m_values = values;
                return * this;
            }

            //! Default copy constructor.
            value_wrapper(value_wrapper<Iterators...> const &) = default;
            //! Default copy assignment.
            value_wrapper<Iterators...> & operator=(value_wrapper<Iterators...> const &) = default;
            //! Default move constructor.
            value_wrapper(value_wrapper<Iterators...> &&) = default;
            //! Default move assignment.
            value_wrapper<Iterators...> & operator=(value_wrapper<Iterators...> &&) = default;

            //! Create values from references (copy content).
            value_wrapper(reference_wrapper<Iterators...> const & refs);
            //! Copy assign values from references.
            value_wrapper<Iterators...> & operator=(reference_wrapper<Iterators...> const & refs);
            //! Move assign values from references.
            value_wrapper<Iterators...> & operator=(reference_wrapper<Iterators...> && refs);

            // This method should serve as a conversion of the value to a const reference.
            //! This should convert a constant value to constant reference.
            operator reference_wrapper<Iterators...> const() const;

            //! Get the I-th value.
            template<size_t I>
            typename std::tuple_element<I, std::tuple<Iterators...>>::type::value_type & get()
            {
                return std::get<I>(m_values);
            };

            //! Get the I-th value.
            template<size_t I>
            typename std::tuple_element<I, std::tuple<Iterators...>>::type::value_type const & get() const
            {
                return std::get<I>(m_values);
            };

        private:
            std::tuple<typename std::iterator_traits<Iterators>::value_type...> m_values;

            template<typename...> friend class reference_wrapper;
    };

    // Reference wrapper implementations.

    template<typename... Iterators>
    reference_wrapper<Iterators...>::reference_wrapper(value_wrapper<Iterators...> & vals)
    {
        detail::for_each_two_tuples_rhs_nonconst_lvalue(m_pointers, vals.m_values,
                                                        detail::make_pointers_to_values());
    }

    template<typename... Iterators>
    reference_wrapper<Iterators...> &
    reference_wrapper<Iterators...>::operator=(value_wrapper<Iterators...> const & vals)
    {
        detail::for_each_two_tuples_rhs_const_lvalue(this->m_pointers, vals.m_values,
                                                     detail::copy_values_to_pointers());
        return * this;
    }

    template<typename... Iterators>
    reference_wrapper<Iterators...> &
    reference_wrapper<Iterators...>::operator=(value_wrapper<Iterators...> && vals)
    {
        // This seems to be like the only place where we can safely do a move!
        detail::for_each_two_tuples_rhs_rvalue(this->m_pointers, std::move(vals.m_values),
                                               detail::move_values_to_pointers());
        return * this;
    }

    // Value wrapper implementations.

    template<typename... Iterators>
    value_wrapper<Iterators...>::value_wrapper(reference_wrapper<Iterators...> const & refs)
    {
        detail::for_each_two_tuples_rhs_const_lvalue(m_values, refs.m_pointers,
                                                     detail::copy_values_from_pointers());
    }

    template<typename... Iterators>
    value_wrapper<Iterators...> &
    value_wrapper<Iterators...>::operator=(reference_wrapper<Iterators...> const & refs)
    {
        detail::for_each_two_tuples_rhs_const_lvalue(m_values, refs.m_pointers,
                                                     detail::copy_values_from_pointers());
        return * this;
    }

    template<typename... Iterators>
    value_wrapper<Iterators...> &
    value_wrapper<Iterators...>::operator=(reference_wrapper<Iterators...> && refs)
    {
        detail::for_each_two_tuples_rhs_const_lvalue(m_values, refs.m_pointers,
                                                     detail::copy_values_from_pointers());
        return * this;
    }

    template<typename... Iterators>
    value_wrapper<Iterators...>::operator reference_wrapper<Iterators...> const() const
    {
        return reference_wrapper<Iterators...>(const_cast<value_wrapper<Iterators...> &>(* this));
    }

    //! Joint iterator.
    template<typename Iterator, typename... Iterators>
    class iterator
    {
        public:
            typedef typename Iterator::difference_type        difference_type;
            typedef std::random_access_iterator_tag           iterator_category;
            typedef reference_wrapper<Iterator, Iterators...> reference;
            typedef value_wrapper<Iterator, Iterators...>     value_type;
            typedef iterator<Iterator, Iterators...>          pointer;
        public:

            //! Default constructor.
            iterator()
                    : m_iterators() { }

            //! Create a joint iterator given a list of iterators.
            iterator(std::tuple<Iterator, Iterators...> iterators)
                    : m_iterators(iterators) { }

            //! Prefix increment (increment each iterator).
            iterator<Iterator, Iterators...> & operator++()
            {
                detail::for_each_one_tuple(m_iterators, detail::iterator_advancer(), 1);
                return * this;
            };

            //! Prefix decrement (decrement each iterator).
            iterator<Iterator, Iterators...> & operator--()
            {
                detail::for_each_one_tuple(m_iterators, detail::iterator_advancer(), -1);
                return * this;
            };

            //! Postfix increment (increment each iterator).
            iterator<Iterator, Iterators...> operator++(int)
            {
                auto copy = * this;
                this->operator++();
                return copy;
            };

            //! Postfix decrement (decrement each iterator).
            iterator<Iterator, Iterators...> operator--(int)
            {
                auto copy = * this;
                this->operator--();
                return copy;
            };

            //! Forward advance iterator by `n` (advance each iterator).
            iterator<Iterator, Iterators...> & operator+=(difference_type n)
            {
                detail::for_each_one_tuple(m_iterators, detail::iterator_advancer(), n);
                return * this;
            };

            //! Backward advance iterator by `n` (advance each iterator).
            iterator<Iterator, Iterators...> & operator-=(difference_type n)
            {
                detail::for_each_one_tuple(m_iterators, detail::iterator_advancer(), -n);
                return * this;
            };

            //! Forward advance iterator by `n` (advance each iterator).
            iterator<Iterator, Iterators...> operator+(difference_type n)
            {
                auto copy = * this;
                copy.operator+=(n);
                return copy;
            };

            //! Backward advance iterator by `n` (advance each iterator).
            iterator<Iterator, Iterators...> operator-(difference_type n)
            {
                auto copy = * this;
                copy.operator-=(n);
                return copy;
            };

            //! Return a reference wrapper associated with this iterator.
            reference operator*() { return reference(this->m_iterators); }

            //! Get the pointer (not very useful, just returns this object).
            pointer operator->() { return * this; }

            //! Get the I-th iterator.
            template<size_t I>
            typename std::tuple_element<I, std::tuple<Iterator, Iterators...>>::type get() const
            {
                return std::get<I>(m_iterators);
            };
        private:
            std::tuple<Iterator, Iterators...>                   m_iterators;
            // This does nothing, just checks that all iterators are random access.
            detail::assert_random_access<Iterator, Iterators...> assert_random_access;
    };

    //! Compare two iterators. The comparison is based on the first iterator only.
    template<typename... Iterators>
    bool operator==(iterator<Iterators...> const & i1, iterator<Iterators...> const & i2)
    {
        return i1.template get<0>() == i2.template get<0>();
    };

    //! Compare two iterators. The comparison is based on the first iterator only.
    template<typename... Iterators>
    bool operator!=(iterator<Iterators...> const & i1, iterator<Iterators...> const & i2)
    {
        return i1.template get<0>() != i2.template get<0>();
    };

    //! Compare two iterators. The comparison is based on the first iterator only.
    template<typename... Iterators>
    bool operator<(iterator<Iterators...> const & i1, iterator<Iterators...> const & i2)
    {
        return i1.template get<0>() < i2.template get<0>();
    };

    //! Compare two iterators. The comparison is based on the first iterator only.
    template<typename... Iterators>
    bool operator>(iterator<Iterators...> const & i1, iterator<Iterators...> const & i2)
    {
        return i1.template get<0>() > i2.template get<0>();
    };

    //! Compare two iterators. The comparison is based on the first iterator only.
    template<typename... Iterators>
    bool operator<=(iterator<Iterators...> const & i1, iterator<Iterators...> const & i2)
    {
        return i1.template get<0>() <= i2.template get<0>();
    };

    //! Compare two iterators. The comparison is based on the first iterator only.
    template<typename... Iterators>
    bool operator>=(iterator<Iterators...> const & i1, iterator<Iterators...> const & i2)
    {
        return i1.template get<0>() >= i2.template get<0>();
    };

    //! Compute the difference of two iterators. The difference is based on the first iterator only.
    template<typename Iterator, typename... Iterators>
    typename Iterator::difference_type operator-(iterator<Iterator, Iterators...> const & i1,
                                                 iterator<Iterator, Iterators...> const & i2)
    {
        return i1.template get<0>() - i2.template get<0>();
    };

    //! Make a joint iterator from a list of iterators.
    template<typename... Iterators>
    iterator<Iterators...> make_joint(Iterators... iterators)
    {
        return iterator<Iterators...>(std::make_tuple(iterators...));
    }

    //! Default comparison operator for values. It considers only the values of the first iterator.
    template<typename... Iterators>
    bool operator<(value_wrapper<Iterators...> const & a,
                   value_wrapper<Iterators...> const & b)
    {
        return a.template get<0>() < b.template get<0>();
    }

    // NOTE It does not work for reference wrappers. Why???

} // namespace joint

#endif //JOINT_ITERATOR_HPP
