# H.urna Lib - C++

H.urna Lib is a relatively small **open source project** that gives you the ability to transform
your algorithms into visualizations. A demonstration of some algorithms visualizations is available here:
[Online Demo - Hurna Explorer](https://demo.hurna.io)

It is implemented as a **cross-platform, easy to build, full headed library** containing some of the
most known **Algorithms and Data Structures** (cf. [Implementations](#implementations)).

It aims to **help to learn programming in C++, practice job interview coding tests** and
allows anyone to **build its own best quality software and
work-flow with no proprietary dependencies nor costs**.

**What's the point?**
- You will be able to visualize your own algorithms.
- You'll have a visual debugging tool.
- You'll have the platform to implement your own tests and data generations
- You'll have a visual tool to communicate on your algorithms
- You'll have already implemented algorithms that can run with your own data and strategies
- You'll have precise benchmark performance analysis
- You'll have an exercise platform easily usable by your students
- ...


Hurna Lib uses **best practices software development**
(cross-building, unit testing, automated documentation...); giving you a
**great caveat to start developing with the promise to conserve great quality standards**.
Please note that **all the possible dependencies included are open-source and free to use**.

## Practices and Guidelines

- **Automated, Customized, Documentation Generation**: CMake, Doxygen, Bootstrap-Doxygen
- **Automated Building, Cross Building, Portability**: CMake
- **Bug Tracking System, Code Reviews**: Git
- **Coding Style, Coding Standard**: _CppLint_
- **Code Coverage, Code Quality, Dashboarding, Monitoring**: CDash, CTest, GTest
- **Statical analysis**: _Cppcheck_
- **Proper Folders Structure - Consistency**
- **Testing, Unit Testing (UT), Test-Driven Development (TDD)**: CMake, GTest
- **Version Control System**: Git

## Tools
*Required:*
- [**CMake**](http://www.cmake.org):
The building relies on CMake, an Open-Source system that
allows to easily build cross-platform applications by generating native makefiles.
- [**Git**](https://git-scm.com):
Git is a free and open source distributed version control system designed to
handle everything from small to very large projects with speed and efficiency.

*Optionnal:*
- [**CDash**](https://www.cdash.org): Is a web dashboard that aggregates, analyzes, and displays summary and
detailed views of build and test results on a historical basis.
It builds organized builds by submitter, platform, architecture, compiler, and compiler setting.
- [**CppLint**](https://github.com/google/styleguide):
Every major open-source project has its own style guide: a set of conventions (sometimes arbitrary)
about how to write code for that project. It is much easier to understand a large codebase when
all the code in it is in a consistent style.
CppLint a C++ style checker following Google's C++ style guide.
- [**Cppcheck**](https://github.com/danmar/cppcheck):
Cppcheck is a static analysis tool for C/C++ code. It provides unique code analysis to
detect bugs and focuses on detecting undefined behaviour and dangerous coding constructs.
The goal is to detect only real errors in the code (i.e. have very few false positives).
- [**CTest**](https://gitlab.kitware.com/cmake/community/wikis/doc/ctest/Testing-With-CTest):
CTest is a testing tool distributed as a part of CMake. It can be used
to automate updating (using CVS for example), configuring, building,
testing, performing memory checking, performing coverage, and submitting
results to a CDash orcDart dashboard system.
- [**Doxygen**](http://www.stack.nl/~dimitri/doxygen):
Doxygen is the de facto standard tool for generating documentation from annotated C++ sources,
but it also supports other popular programming languages such as C, Objective-C, C#, PHP, Java, Python,
 IDL (Corba, Microsoft, and UNO/OpenOffice flavors), Fortran, VHDL, Tcl, and to some extent D.
- [**Doxygen-Bootstrapped**](https://github.com/Velron/doxygen-bootstrapped):
Customize doxygen output to use the twitter bootstrap framework.
- [**GTest**](https://github.com/google/googletest):
Google Test (GTest) is a cross-platform unit testing library for the C++ programming language.
It allows unit-testing of C sources as well as C++ with minimal modification.
The tests themselves could be run one at a time, or even be called to run all at once,
making the debugging process very flexible.


This toolset lower costs and reduce risk by identifying problems earlier:
it **improves work-flow, increase productivity, gives you agility and project health monitoring**.

## Quick Access

- [**Documentation**](http://wiki.hurna.io/hurna_lib/doc/index.html)
- [**Git Repository**](https://github.com/Hurna/Hurna-Lib)
- [**Kitware Dashboard**](https://my.cdash.org/index.php?project=Hurna+Lib)

### Related

- [**H.urna Website**](https://hurna.io/): The main website of H.urna organization.
- [**H.urna Explorer**](https://demo.hurna.io/): A free, online, visualization platform.
- [**H.urna Community**](https://community.hurna.io/): The community.
- [**H.urna Wiki**](https://wiki.hurna.io/): The main Wiki of H.urna organization.

# Getting Started
## Build using CMake-GUI:

    Run CMake-GUI
    Select source/target directory
    Click on Configure the project
    Click on Generate the Solution files
    Open the solution file (if MSVC) and build or use make command.

## Build using Command-Line (UNIX):

    mkdir Hurna-Lib-Build  (Create Build directory)
    cd Hurna-Lib-Build     (Open build director)
    ccmake ../Hurna-Lib    (Configure the make given Source repository)
                           (Press 'c' to configure, then 'g' to generate the solution)
    make                   (Make command, use -j parameter to use multi-core)

## Build using QT Creator:

    Open CMakeLists.txt
    Configure build directories and CMake options
    Build

# Doxygen Documentation
Use the CMake **BUILD_DOC** (default to true) option to automatically setup Documentation Generation.

# GTest - Unit Testing
Use the CMake **BUILD_TESTING** (default to true) option to automatically setup Unit Testing.
It allows to very quickly get set up with google test:

- Clone GTest via GIT
- Setup the Unit Testings
- Compile GTest at build time

Use the CMake **WITH_COVERAGE** (default to true) option to automatically setup Coverage Generation.
The minimal required coverage for this project is **95%**.

# Running Unit Tests (UTs) and Update Dashboards
You can whether use **CTest** or **manually** run the unit tests.

To run the set of tests in multi-core:

    cd Hurna-Lib-Build
    ctest -j4

To run the set and upload them to the dashboard:

    cd Hurna-Lib-Build
    ctest -D Experimental

To manually run the 'TestBasicBinary' Unit Test (using GTest):

    Hurna-Lib-Build/Modules/Search/Testing/Debug/TestBinary.exe  (Win)
    ./Hurna-Lib-Build/Modules/Search/Testing/Debug/TestBinary    (UNIX)

# <a name="implementations"></a>Visual Implementations
## Mazes Generators
- **Binary Tree ([Online Demo](https://demo.hurna.io/#path=maze/binary)):** Binary Tree Maze Generator is
one of the very rareful algorithms with the ability to generate a perfect maze without
keeping any state at all: it is a true memoryless Maze generation algorithm with no
limit to the size of Maze you can create. It can build the entire maze by looking at each cell independently.
This is basically the simplest and fastest algorithm possible.
- **Depth First Search (DFS) ([Online Demo](https://demo.hurna.io/#path=maze/dfs_generator)):**
Depth First Search (DFS) Maze Generator is a randomized version of the depth-first search traversal algorithm.
Implemented with a stack, this approach is one of the simplest ways to generate a maze.
- **Kruskal's ([Online Demo](https://demo.hurna.io/#path=maze/kruskals_generator)):**
Kruskal's Maze Generator is a randomized version of Kruskal’s algorithm: a method for producing a
minimal spanning tree for a weighted graph.
Kruskal's is interesting because it doesn't "grow" the Maze like a tree, but rather carves
passage segments all over the Maze at random, making it very fun to watch. Still,
it results in a perfect Maze in the end.
The counterpart is to require storage proportional to the size of the Maze,
along with the ability to enumerate each edge between cells in random order
(Using here a set of edges and taking them randomly).
- **Prim's ([Online Demo](https://demo.hurna.io/#path=maze/prims_generator)):**
Prim's Maze Generator is a randomized version of Prim's algorithm:
a method for producing a minimal spanning tree for a undirected weighted graph.
Prim's algorithm creates a tree by getting the adjacent cells and finding the best one to travel to next.
To Generate mazes using Prim's, we will instead take a random cell to travel to the next one.
- **Recursive Division ([Online Demo](https://demo.hurna.io/#path=maze/recursive_division_generator)):**
Recursive Division Maze Generator is the fastest algorithm without directional biases.
While Recurssive division really stands out with respect to parallelism,
this algorithm is particularly fascinating because of its fractal nature:
you could theoretically continue the process indefinitely at finer and
finer levels of detail (smaller and smaller scales).
This algorithm is somewhat similar to recursive backtracking, since they're both stack based,
except this focuses on walls instead of passages. As a Wall Builders generator,
the process begins with a large empty space (all cells are connected), and adds walls
(disconnect cells) until a maze results.
- **Sidewinder ([Online Demo](https://demo.hurna.io/#path=maze/sidewinder_generator)):**
Sidewinder Maze Generator is very similar to the Binary Tree algorithm, and only slightly more complicated.
Furthermore, the Sidewinder algorithm only needs to consider the current row,
and therefore can be used to generate infinitely large mazes (like the Binary Tree).
While binary tree mazes have two of its four sides being one long passage,
a Sidewinder mazes have just one long passage.

## Search
- **Binary Search ([Online Demo](https://demo.hurna.io/#path=search/binary)):** Iteratively proceed a dichotomous search, within a sorted sequence, on the first occurrence of the key.
- **K'th Order Statistics ([Online Demo](https://demo.hurna.io/#path=search/kth_element)):** Find the k'th smallest/biggest element.

## Sort
- **AggregateInplace ([Online Demo](https://demo.hurna.io/#path=sort/aggregate_in_place)):** Functor that proceeds a in place merge of two sequences of elements.
- **Bubble Sort ([Online Demo](https://demo.hurna.io/#path=sort/bubble_sort)):** Sometimes referred to as sinking sort: proceed an in-place bubble-sort on the elements.
- **Cocktail Sort ([Online Demo](https://demo.hurna.io/#path=sort/cocktail_sort)):** Variation of bubble sort. Optimize a bubble sort bubbling in both directions on each pass.
- **Comb Sort ([Online Demo](https://demo.hurna.io/#path=sort/comb_sort)):** Variation of bubble sort. The inner loop of bubble sort, which does the actual swap,
is modified such that gap between swapped elements goes down (for each iteration of outer loop) in steps of a "shrink factor" k: [ n/k, n/k2, n/k3, ..., 1 ].
- **MergeSort ([Online Demo](https://demo.hurna.io/#path=sort/merge_in_place)):** John von Neumann in 1945: Proceed merge-sort on the elements whether using an in-place strategy or using a buffer.
- **Partition-Exchange ([Online Demo](https://demo.hurna.io/#path=sort/partition)):** Proceed an in-place partitioning on the elements.
- **Quick Sort - Partition-Exchange Sort ([Online Demo](https://demo.hurna.io/#path=search/quick_sort)):** Proceed an in-place quick-sort on the elements.
