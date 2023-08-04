# SFO_CPP
A library for combinatorial optimization (subset selection) problems using a generic object-oriented framework.

This library implements a handful of basic algorithms for submodular function maximization.  In particular, they solve the problem:

 ```math
\begin{array}{cc} \underset{S\subseteq V}{\text{maximize}} & F(S) \\ \text{subject to} & \vert S \vert \leq B\end{array}
```

where $F:2^V\to\mathbb{R}$ is a submodular function and $V$ is a ground set of $n$ elements.  Submodular functions satisfy the inequality:

$$ F(S) + F(T) \geq F(S\cup T) + F(S\cap T), $$

for any $S, T \subseteq V$.

More intuitively, these functions exhibit the property of diminishing returns.  For such functions, greedy algorithms are both efficient and provably near-optimal.

This library implements various versions of the greedy algorithm for these functions, using a generic `Element`, `CostFunction`, and `Greedy` classes that are appropriate overloaded to leverage the C++ standard library's `unordered_set` and `queue` data structures.  Currently, two algorithms are implemented:

* Naive Greedy:
    The naive greedy algorithm requires $\mathcal{O}(n)$ computations per iteration as it evaluates the marginal benefit of every possible element in $V$ and greedily selects the best element.  Since we select $B$ elements, this algorithm has $\mathcal{O}(Bn^2)$ complexity.  This algorithm, while simple, produces a subset $S\subseteq V$ such that $F(\hat{S}) \geq (1-\frac{1}{e})F(S^*)$, where $S^*$ is the global optimum (which is NP-Hard to compute) when $F$ is monotone and submodular.
* Lazy Greedy.
    The lazy greedy algorithm abuses the submodularity of $F$ to perform iterations in the order dictated by a _priority queue_.  While this still has complexity $\mathcal{O}(n)$ per iteration, in practice this method exhibits orders of magnitude speedup.  Because, in principle, the lazy greedy algorithm defaults to the naive greedy algorithm, this algorithm also comes with the same guarantee of $F(\hat{S})\geq (1-\frac{1}{e})F(S^*)$.

TO DO:
* Lazier than lazy greedy.
* Algorithms for non-monotone submodular maximization.
* Algorithms for _streaming_ submodular function maximization.
