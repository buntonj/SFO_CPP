# SFO_CPP
A header-only library for combinatorial optimization (subset selection) problems using a generic object-oriented framework.

This library implements a handful of basic algorithms for submodular function maximization.  In particular, they solve the problem:

 ```math
\begin{array}{cc} \underset{S\subseteq V}{\text{maximize}} & F(S) \\ \text{subject to} & \vert S \in \mathcal{C}\end{array}
```

where $F:2^V\to\mathbb{R}$ is a submodular function and $V$ is a ground set of $n$ elements, and $\mathcal{C}\subseteq 2^V$ is a constraint set.  Submodular functions satisfy the inequality:

$$ F(S) + F(T) \geq F(S\cup T) + F(S\cap T), $$

for any $S, T \subseteq V$.

More intuitively, these functions exhibit the property of diminishing returns.  For such functions, greedy algorithms are both efficient and provably near-optimal when the set $\mathcal{C}$ is some simple form of constraint, such as cardinality, knapsack, matroid, independence system, etc.

The algorithms are defined as template classes for some `typename E`, where the instatiation with `E` defines the C++ class of objects that the algorithms will be summarizing.  It also implements a templated override of the comparison operator `<` for `std::pair<E, double>` which is used to interface with the C++ standard libarary's `std::priority_queue` container and sorting elements $j \in V$ by their marginal benefit as measured by $F$.  For convenience, the library also provides a simple `Element` class, which the library will fall back to and instantiate if the template instantiation for `typename E` is not given.

In `cost_function.hpp`, the library implements the templated (`typename E`) abstract base class `CostFunction` to represent the mathematical function $F:2^V\to\mathbb{R}$, which contains virtual methods to overload the evaluation operator `()` on both single `E` and `std::unordered_set<E>` arguments.  A couple example derived classes (specific cost functions) are implemented, such as the `Modular` and `SquareRootModular` derived classes.

In `constraint.hpp`, the library implements the templated (`typename E`) abstract base class `Constraint` to represent the constraint $S\in \mathcal{C}$.  The set $\mathcal{C}$ could be all subsets with less than $B$ elements, all subsets whose knapsack cost is less than $B$, or any other general constraint.  Many of the implemented algorithms still retain guarantees even when the constraint set $\mathcal{C}$ is a matroid, knapsack, independence system, p-system, or some intersection of them.

These two objects are then handed to an optimizer class, implemented in the parent folder, which implement the optimization routines to select a (provably near-optimal) subset of elements.

Currently, only these two algorithms are implemented:

* Naive Greedy (`GreedyAlgorithm`):
    The naive greedy algorithm requires $\mathcal{O}(n)$ computations per iteration as it evaluates the marginal benefit of every possible element in $V$ and greedily selects the best element.  Since we select $B$ elements, this algorithm has $\mathcal{O}(Bn^2)$ complexity.  This algorithm, while simple, produces a subset $S\subseteq V$ such that $F(\hat{S}) \geq (1-\frac{1}{e})F(S^*)$, where $S^*$ is the global optimum (which is NP-Hard to compute) when $F$ is monotone and submodular.
* Lazy Greedy.
    The lazy greedy algorithm abuses the submodularity of $F$ to perform iterations in the order dictated by a _priority queue_.  While this still has complexity $\mathcal{O}(n)$ per iteration, in practice this method exhibits orders of magnitude speedup.  Because, in principle, the lazy greedy algorithm defaults to the naive greedy algorithm, this algorithm also comes with the same guarantee of $F(\hat{S})\geq (1-\frac{1}{e})F(S^*)$.
* Stochastic Greedy
    The stochastic greedy algorithm instead selects a uniform random subset of elements to greedily choose from each iteration.  For a given $\varepsilon \geq 0$, the algorithm samples $\frac{n}{B}\log\frac{1}{\varepsilon}$ elements each iteration and has an approximation guarantee of $F(\hat{S}) \geq (1-\frac{1}{e}-\varepsilon)F(S^*)$ in expectation.
* "Lazier Than Lazy Greedy"
    This algorithm is Stochastic Greedy, but also implements the priority queue for lazy evaluations, giving the same guarantee as Stochastic Greedy in expectation.


Both of these algorithms, when asked to optimize the cost function with the `run_{ALG_NAME}()` call, may be handed a flag to instead run the cost-benefit algorithm instead.  In the cost-benefit algorithm, the benefit of each `Element` is divided by its additional cost according to the knapsack constraint.  As a result, this specific variant will only run when the `Constraint` that `GreedyAlgorithm` or `LazyGreedyAlgorithm` is provided with is of the specific _derived_ class `Knapsack`.

A quick testing script is given in `test_greedy.cpp`.  A proper `CMake` configuration is coming soon, but for now you can simply compile this `test_greedy.cpp` directly and run the resulting executable.
