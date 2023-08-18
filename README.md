# SFO_CPP
A header-only library for combinatorial optimization (subset selection) problems using a generic object-oriented framework.

This library implements a handful of basic algorithms for submodular function maximization.  In particular, they solve the problem:

 ```math
\begin{array}{cc} \underset{S\subseteq V}{\text{maximize}} & F(S) \\ \text{subject to} & \vert S \vert \leq B\end{array}
```

where $F:2^V\to\mathbb{R}$ is a submodular function and $V$ is a ground set of $n$ elements.  Submodular functions satisfy the inequality:

$$ F(S) + F(T) \geq F(S\cup T) + F(S\cap T), $$

for any $S, T \subseteq V$.

More intuitively, these functions exhibit the property of diminishing returns.  For such functions, greedy algorithms are both efficient and provably near-optimal.

This uses an abstract base class `Element` (defined in `element.hpp`) to describe the elements $j\in V$ that we are selecting a subset of.  The `Element` class is built to naturally interface with the C++ standard library's `std::unordered_set` container to define subsets $S\subseteq V$.  Each element also has a `value`, which it uses to interface with the C++ standard libarary's `std::priority_queue` container, which is useful when sorting elements $j \in V$ by their value as measured by $F\left(\{j\}\right)$.

In `cost_function.hpp`, the library implements the abstract base class `CostFunction` to represent the mathematical function $F:2^V\to\mathbb{R}$, which contains virtual methods to overload the evaluation operator `()` on both single `Element` and `std::unordered_set<Element>` arguments.  A couple example derived classes (specific cost functions) are implemented, such as the `Modular` and `SquareRootModular` derived classes.

These two objects are then handed to the `GreedyAlgorithm` or `LazyGreedyAlgorithm` classes, implemented in `vanilla_greedy.hpp` and `lazy_greedy.hpp` respectively, which implement the optimization routines to select a (provably near-optimal) subset of elements.

Currently, only these two algorithms are implemented:

* Naive Greedy (`GreedyAlgorithm`):
    The naive greedy algorithm requires $\mathcal{O}(n)$ computations per iteration as it evaluates the marginal benefit of every possible element in $V$ and greedily selects the best element.  Since we select $B$ elements, this algorithm has $\mathcal{O}(Bn^2)$ complexity.  This algorithm, while simple, produces a subset $S\subseteq V$ such that $F(\hat{S}) \geq (1-\frac{1}{e})F(S^*)$, where $S^*$ is the global optimum (which is NP-Hard to compute) when $F$ is monotone and submodular.
* Lazy Greedy.
    The lazy greedy algorithm abuses the submodularity of $F$ to perform iterations in the order dictated by a _priority queue_.  While this still has complexity $\mathcal{O}(n)$ per iteration, in practice this method exhibits orders of magnitude speedup.  Because, in principle, the lazy greedy algorithm defaults to the naive greedy algorithm, this algorithm also comes with the same guarantee of $F(\hat{S})\geq (1-\frac{1}{e})F(S^*)$.

Both of these algorithms, when asked to optimize the cost function with the `run_{ALG_NAME}()` call, may be handed a flag to instead run the cost-benefit algorithm instead.  In the cost-benefit algorithm, the benefit of each `Element` is divided by its additional cost according to the knapsack constraint.  As a result, this specific variant will only run when the `Constraint` that `GreedyAlgorithm` or `LazyGreedyAlgorithm` is provided with is of the specific _derived_ class `Knapsack`.

A quick testing script is given in `test_greedy.cpp`.  A proper `CMake` configuration is coming soon, but for now you can simply compile this `test_greedy.cpp` directly and run the resulting executable.
