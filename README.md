# sfo_cpp: a lightweight, templated submodular function maximization in C++

## Overview
A headers-only C++ library for submodular optimization (subset selection) problems on arbitrary data types.

This library implements a handful of basic algorithms for submodular function maximization.  In particular, they solve the problem:

 ```math
\begin{array}{cc} \underset{S\subseteq V}{\text{maximize}} & F(S) \\ \text{subject to} & S \in \mathcal{C}\end{array}
```

where $F:2^V\to\mathbb{R}$ is a submodular function and $V$ is a ground set of $n$ elements, and $\mathcal{C}\subseteq 2^V$ is a constraint set.  Submodular functions satisfy the inequality:

$$ F(S) + F(T) \geq F(S\cup T) + F(S\cap T), $$

for any $S, T \subseteq V$.  More intuitively, these functions exhibit the property of diminishing returns.

Monotone functions are functions that preserve the subset partial order on the power set $2^V$:

$$ A\subseteq B \quad \implies\quad F(A) \leq F(B) $$

For such functions, greedy algorithms are both efficient and provably near-optimal when the set $\mathcal{C}$ is some simple form of constraint, such as cardinality, knapsack, matroid, independence system, etc.

## Usage

### Building and testing
This library uses Bazel as its build system.  To compile, make sure you have [Bazel installed on your system](https://bazel.build/install) and run:
```bash
bazel build ...
```
which will build and install both the headers library and the tests into the `/build` directory.  If you would like to run one of the example scripts (such as `test_monotone_greedy` or `test_non_monotone_greedy`), use `bazel run` in the following way:
```bash
bazel run //tests:test_monotone_greedy
```

### Usage in other contexts
Basic usage follows four simple steps:

1) Define a  `std::unordered_set` of "ground set" elements to summarize.
2) Import and create the appropriate `GreedyAlgorithm` object from this library.
3) Give the `GreedyAlgorithm` object a reference to the ground set and `Constraint`s it must satisfy (if desired).
4) Call `GreedyAlgorithm.run_greedy(CostFunction)` on a `sfo_cpp::CostFunction` abstract base class.

The entire library is templated via some `typename E`, where  the instatiation of `E` defines the C++ objects that the algorithms will be summarizing.  The library accomplishes this by populating a `std::unordered_set<E*>` of _pointers_ to elements in $V$ rather than directly copying elements around.

### As a result, this library can summarize **_almost any C++ data type you want!_**

All you need to do is implement a `CostFunction` that evaluates how "good" a given summary is, and a `Constraint` that evaluates if a summary satisfies some constraint set or not, and hand everything over to the `GreedyAlgorithms` implemented here.

Once they are run, the algorithms will hold a variable `curr_set`, which is a STL `unordered_set<E*>` of pointers to the elements of the ground set (which is also a STL `unordered_set<E*>`).

#### Under the hood
To do this, the library defines a templated override of the comparison operator `<` for basic pairs `std::pair<E, double>`. The library uses this operator to interface with the STL `std::priority_queue` container and sort elements $j \in V$ by their marginal benefit as measured by $F$.

For convenience, the library includes a simple `Element` class, which the library will fall back to and instantiate if the template instantiation for `typename E` is not given.

## Cost function class
In `cost_function.hpp`, the library defines the templated (`typename E`) abstract base class `CostFunction` to represent the mathematical function $F:2^V\to\mathbb{R}$.

The `CostFunction` abstract base class only has one virtual method:
 - `operator()`: returns a C++ `double` corresponding to $F(S)$ when $S$ is a singleton (`E*` argument) or a subset (`std::unordered_set<E*>` argument) for any $S\subseteq V$.

A couple important example derived classes (specific cost functions) are implemented, such as the `Modular` and `SquareRootModular` cost functions.

In principle, however, one needs only to define an appropriate `CostFunction<E>` object with evaluation overloads to run the greedy algorithms on it.

## Constraint class
In `constraint.hpp`, the library defines the templated (`typename E`) abstract base class `Constraint` to represent the mathematical constraint $S\in \mathcal{C}$.

The set $\mathcal{C}$ could be all subsets with less than $B$ elements, all subsets whose knapsack cost is less than $B$, or any other general constraint.  Many of the implemented algorithms still retain guarantees even when the constraint set $\mathcal{C}$ is a matroid, knapsack, independence system, p-system, or some intersection of them.

The `Constraint<E>` abstract base class has two pure virtual functions:
- `test_membership`: returns a Boolean value stating if a singleton `<E*>` or subset `std::unordered_set<E*>` satisfy the constraint or not;
- `is_saturated`: returns a Boolean value stating if the singleton `<E*>` or subset `std::unordered_set<E*>` can have any elements added without violating the constraint. (Overriding this is optional, but it helps stop the algorithms faster)

To implement a constraint, you just have to override the `test_membership` functions.  A couple simple derived examples such as `Knapsack` and `Cardinality` constraints are implemented in `constraint.hpp`.


The `CostFunction` and `Constraint` objects are handed to one of the Algorithm objects, which implement the optimization routines to select a (provably near-optimal) subset of elements.


## Library of algorithms
* **Naive Greedy** (`GreedyAlgorithm`):
    * **Valid constraints**: Matroid, Knapsack
    * **Valid cost functions**: Monotone

    The naive greedy algorithm requires $\mathcal{O}(n)$ computations per iteration as it evaluates the marginal benefit of every possible element in $V$ and greedily selects the best element.  Since we select $B$ elements, this algorithm has $\mathcal{O}(Bn^2)$ complexity.  This algorithm, while simple, produces a subset $S\subseteq V$ such that $F(\hat{S}) \geq (1-\frac{1}{e})F(S^*)$, where $S^*$ is the global optimum (which is NP-Hard to compute) when $F$ is monotone and submodular.
    
    Reference [here.](https://link.springer.com/article/10.1007/BF01588971)

* **Lazy Greedy** (`LazyGreedy`).
    * **Valid constraints**: Matroid, Knapsack
    * **Valid cost functions**: Monotone

    The lazy greedy algorithm abuses the submodularity of $F$ to perform iterations in the order dictated by a _priority queue_.  While this still has complexity $\mathcal{O}(n)$ per iteration, in practice this method exhibits orders of magnitude speedup.  Because, in principle, the lazy greedy algorithm defaults to the naive greedy algorithm, this algorithm also comes with the same guarantee of $F(\hat{S})\geq (1-\frac{1}{e})F(S^*)$.

    Reference [here.](https://link.springer.com/chapter/10.1007/BFb0006528)

* **Stochastic Greedy** (`StochasticGreedy`)
    * **Valid constraints**: Cardinality
    * **Valid cost functions**: Monotone

    The stochastic greedy algorithm instead selects a uniform random subset of elements to greedily choose from each iteration.  For a given $\varepsilon \geq 0$, the algorithm samples $\frac{n}{B}\log\frac{1}{\varepsilon}$ elements each iteration and has an approximation guarantee of $F(\hat{S}) \geq (1-\frac{1}{e}-\varepsilon)F(S^*)$ in expectation.
    
    Reference [here.](https://arxiv.org/pdf/1409.7938.pdf)

* **"Lazier Than Lazy Greedy"** (`LazierThanLazyGreedy`)
    * **Valid constraints**: Cardinality
    * **Valid cost functions**: Monotone

    This algorithm is Stochastic Greedy, but also implements the priority queue for lazy evaluations, giving the same guarantee as Stochastic Greedy in expectation.

    Reference [here.](https://arxiv.org/pdf/1409.7938.pdf)

* **Bidirectional Greedy** (`BidirectionalGreedy`)
    * **Valid constraints**: None
    * **Valid cost functions**: Monotone, non-monotone

    This algorithm is only valid for **unconstrained problems** ($\mathcal{C} = V$), but returns a set $\hat{S}\subseteq V$ with $F(\hat{S}) \geq \frac{1}{3}F(S^{*})$ for _any_ submodular function $F$.  It also has a flag `randomized` that, if set to `true`, will run the randomized variant that returns a set with $F(\hat{S}) \geq\frac{1}{2}F(S^{*})$ guarantee in _expectation_.
    
    Reference [here.](https://theory.epfl.ch/moranfe/Publications/FOCS2012.pdf)

* **Approximate local search** (`ApxLocalSearch`)
    * **Valid constraints**: k-Matroid
    * **Valid cost functions**: Monotone, non-monotone

    **WIP**

    Reference [here.](https://arxiv.org/pdf/0902.0353.pdf)


Many algorithms, when asked to optimize the cost function with the `run_{ALG_NAME}()` call, may be handed a flag to instead run the cost-benefit algorithm instead.  In the cost-benefit algorithm, the benefit of each `Element` is divided by its additional cost according to the knapsack constraint.  As a result, this specific variant will only run when the `Constraint` that `GreedyAlgorithm` or `LazyGreedyAlgorithm` is provided with is of the specific _derived_ class `Knapsack`.

Quick testing scripts are given in `test_monotone_greedy.cpp` and `test_non_monotone_greedy.cpp`.

**Coming soon: non-monotone algorithms, pre-emption (streaming), semi-streaming, and more!**
