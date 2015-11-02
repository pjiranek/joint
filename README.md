Joint iterator
==============

Motivation
----------

In some applications, one would like to use generic algorithms (e.g., STL) on ranges defined by more than one
containers. For example, given two vectors

    std::vector<int> numbers = {5, 2, 4, 1, 3};
    std::vector<std::string> strings = {"five", "two", "four", "one", "three"};

one would like to sort the vector `numbers` and permute accordingly the vector `strings`.
Typical workarounds to solve this problem are as follows:

- Create a vector of `int-string` pairs, apply the sort to this vector with a suitable comparator, and stock the
  contents back to the original vectors.
- Create a vector of indices `0,...,N-1`, apply the sort to create a permutation vector, and apply the permutation to
  the original vectors.

Both approaches, however, require additional storage and data movements.

We try to circumvent this disadvantage by using a "joint" iterator, which gathers together multiple iterators
of the random access type. It works similarly to the `boost::zip_iterator`, which, however, is useful only for
read-only access to the containers and is not suitable for algorithms which modify the containers like sort.

Description
-----------

In order to sort the vectors above, we define a "joint range"

    auto begin = joint::make_joint(numbers.begin(), strings.begin());
    auto end   = joint::make_joint(numbers.end(),   numbers.end());

If we want to sort the vectors in the ascending order with respect to the integer vector, it can be done by

    typedef decltype(begin) joint_iterator;
    typedef joint_iterator::reference reference;
    std::sort(begin, end,
              [](reference const & a, reference const & b)
              { return a.get<0>() < b.get<0>(); });

How does it work
----------------

The iterators (defined in the class `joint::iterator`) are implemented as a tuple of iterators given by the arguments
to the class constructor or the helper function `join::make_joint()`. The iterators are required to be of the random
access type and the `joint::iterator` defines a random access type iterator as well. It defines all the required type
definitions and operators to increment, decrement, dereference, etc. For example, incrementing a `joint::iterator`
increments all the associated iterators. Note that `std::distance` (or, equivalently, computing the difference between
two `joint::iterator`s) computes the difference only on the first iterator (the distance between the remaining iterators
should be the same). The method `get<I>()` can be used to get access to the `I`th iterator.

Each iterator type should have an associated reference and value type.
We implement them using the wrapper classes `joint::reference_wrapper` and `joint::value_wrapper`, which are again
implemented as a tuple of "references" (in fact, the reference type is implemented as a tuple of pointers) and values
with their methods `get<I>()` to get the `I`th reference or value.

The trick to make the algorithms (like sort) to work is that the dereferencing operator `operator*()` of the
`joint::iterator` returns an instance of the `joint::reference_wrapper` and we implement a custom method `swap` for
swapping two reference wrappers. Actually, since `joint::iterator::operator*()` does not return an l-value, the `swap`
function must accept r-values instead. This puts some restrictions, e.g., onto the implementation of the move
constructor of the reference wrapper.

We define a default comparator which sorts ranges according to the first range in the ascending order (e.g., similarly
to the previous example). The problem is, however, that the default comparison must be defined on the value type.
Since `joint::iterator::operator*()` returns an r-value object, it might be tempting to make an implicit conversion
from the reference wrapper r-value to the value wrapper. This does not work (it would rip the guts of the object in the
container)! In order to make this properly, the values pointed to by the reference wrapper are *copied* to the value
wrapper, which might cause certain performance issues. Therefore, for the performance reasons, it is better to provide
algorithms with a comparator which takes directly the reference wrappers instead of value wrappers.

Performance
-----------

We compare four implementations of the sort of two vectors of the length `N` (a vector of `int` types and `T` types):

- `ALGO1`: `joint::iterator` with the comparator taking two instances of the value type:

        [](joint_iterator::value_type const & a, joint_iterator::value_type const & b)
        {
            return a.get<0>() < b.get<0>();
        }

- `ALGO2`: `joint::iterator` with the comparator taking two instances of the reference type (since the `reference` type
  is not an actual reference, we take a const reference for performance reasons):

        [](joint_iterator::reference const & a, joint_iterator::reference const & b)
        {
            return a.get<0>() < b.get<0>();
        }

- `ALGO3`: Using the vector of structures (first move the content of the two vectors to the vector of aggregate
  structures, apply a comparator to sort by the vector of integers, and then move the aggregates to the original
  vectors).

- `ALGO3`: Using the permutation vector (apply the sort to the permutation vector and apply it to the original vectors).

Two types of the second vector are considered:

- `T = std::string`, where each entry contains a several copies of "Lorem Ipsum" with `N=524288`.
- `T = long` with `N=1048576`.

The following test results were produced on my laptop:

<table>
    <tr>
        <td></td>
        <td><tt>ALGO1</tt></td>
        <td><tt>ALGO2</tt></td>
        <td><tt>ALGO3</tt></td>
        <td><tt>ALGO4</tt></td>
    </tr>
    <tr>
        <td><tt>T = std::string</tt></td>
        <td>6979ms</td>
        <td>127ms</td>
        <td>138ms</td>
        <td>107ms</td>
    </tr>
    <tr>
        <td><tt>T = long</tt></td>
        <td>120ms</td>
        <td>113ms</td>
        <td>122ms</td>
        <td>187ms</td>
    </tr>
</table>

For large data structures (containing, e.g., large strings), using the default comparison operator or the comparator
for the value types is not really a good idea since an object copies are created for each call to the comparator.
Nevertheless, the performance is quite similar in comparison with the implementations using additional data
(note that the data movements for `ALGO3` and `ALGO4` can be implemented efficiently using moves).

Disclaimer
----------

The code is probably not very clean but works :-)
