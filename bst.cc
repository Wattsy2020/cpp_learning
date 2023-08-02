#include <vector>
#include <concepts>
#include <optional>
#include <assert.h>
#include <iterator>
#include <ranges>
#include <cmath>
#include <functional>
#include "itertools.h"
#include "testlib.h"

template <std::totally_ordered T>
class BinaryTree
{
public:
    BinaryTree() : items{} {};

    template <std::ranges::input_range Iter>
        requires std::same_as<std::ranges::range_value_t<Iter>, T>
    BinaryTree(const Iter items) : BinaryTree()
    {
        add(items.begin(), items.end());
    }

    BinaryTree(const std::initializer_list<T> items) : BinaryTree()
    {
        add(items.begin(), items.end());
    }

    void add(const T &item)
    {
        const size_t new_node_idx{find_node(item)};
        if (!node_exists(new_node_idx))
            add_node(new_node_idx, item);
    }

    template <std::input_iterator Iter>
        requires std::same_as<typename std::iterator_traits<Iter>::value_type, T>
    void add(Iter begin, const Iter end)
    {
        for (; begin != end; ++begin)
            add(*begin);
    }

    bool contains(const T &item)
    {
        return node_exists(find_node(item));
    }

    // technically not preorder because it includes gaps
    std::vector<std::optional<T>> preorder_traversal() const
    {
        return items;
    }

    std::vector<T> inorder_traversal() const
    {
        std::vector<T> result{};
        inorder_traversal(0, result);
        return result;
    }

    // O(1) calculation of height
    // uses the recurrence relation that Ri = 2*R(i-1) + 2, where Ri is the Rightmost node of level i (root is at level 0)
    // algebra gives Ri = 2^(i+1) - 2
    // gives i = log2(Ri + 2) - 1
    // so i = ceil(log2(index of last node in tree + 2)) - 1  , taking the ceiling since if even one node is in a level, the tree has that height
    long height() const
    {
        return std::lround(std::ceil(std::log2(items.size() + 1))) - 1;
    }

    // O(n) calculation of size (the number of nodes in the tree)
    long size() const { return inorder_traversal().size(); }

private:
    // items[0]: root
    // items[1]: left node of root
    // items[2]: right node of root
    // items[2*i + 1]: left node of i
    // items[2*i + 2]: right node of i
    std::vector<std::optional<T>> items;

    size_t get_left(const size_t idx) const { return 2 * idx + 1; }

    size_t get_right(const size_t idx) const { return 2 * idx + 2; }

    bool node_exists(const size_t idx) const
    {
        return idx < items.size() && items[idx].has_value();
    }

    // find the idx of the given item in the tree
    // if item doesn't exist in the tree, then return the index where it should be placed
    size_t find_node(const T &item) const
    {
        size_t current_node{0};
        while (node_exists(current_node))
        {
            const T current_val{items[current_node].value()};
            if (item < current_val)
                current_node = get_left(current_node);
            else if (item == current_val)
                return current_node;
            else
                current_node = get_right(current_node);
        }
        return current_node;
    }

    // add node to the tree, increasing the vector size if necessary
    void add_node(const size_t idx, const T item)
    {
        items.resize(std::max(items.size(), idx + 1));
        items[idx] = item;
    }

    void inorder_traversal(const size_t start_idx, std::vector<T> &output_vec) const
    {
        if (!node_exists(start_idx))
            return;

        inorder_traversal(get_left(start_idx), output_vec);
        output_vec.push_back(items[start_idx].value());
        inorder_traversal(get_right(start_idx), output_vec);
    }
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const BinaryTree<T> &set)
{
    os << set.preorder_traversal();
    return os;
}

void test_bst_add()
{
    BinaryTree<int> bst{};
    bst.add(2);
    bst.add(5);
    bst.add(0);
    std::vector<std::optional<int>> result{bst.preorder_traversal()};
    assert(result == (std::vector<std::optional<int>>{{2}, {0}, {5}}));
    bst.add(3);
    bst.add(1);
    std::vector<std::optional<int>> result2{bst.preorder_traversal()};
    assert(result2 == (std::vector<std::optional<int>>{{2}, {0}, {5}, {}, {1}, {3}}));
}

void test_bst_outstream()
{
    BinaryTree<int> bst{};
    bst.add(2);
    bst.add(5);
    bst.add(0);
    bst.add(3);
    bst.add(1);
    testlib::assert_outstream(bst, "[ 2 0 5 None 1 3 ]");
}

void test_bst_contains()
{
    std::vector<int> items{5, 2, 0, 9, 10, 4, 7};
    BinaryTree<int> bst(items);
    for (const int item : items)
        assert(bst.contains(item));
    assert(!bst.contains(1));
    bst.add(1);
    assert(bst.contains(1));
}

void test_bst_inorder_traversal()
{
    BinaryTree<int> tree1{3, 0, 6, 5, 7, 8};
    std::vector<int> result1{tree1.inorder_traversal()};
    assert(result1 == (std::vector<int>{0, 3, 5, 6, 7, 8}));

    BinaryTree<int> tree2{10, 1, 9, 2, 8, 3, 7, 4, 6, 5, 11, 15, 13, 14};
    std::vector<int> result2{tree2.inorder_traversal()};
    assert(result2 == (std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15}));
}

void test_bst_height()
{
    // height of an empty tree is undefined I guess assert((BinaryTree<int>{}).height() == 0);
    assert((BinaryTree{1}).height() == 0);
    assert((BinaryTree{3, 0, 5}).height() == 1);
    assert((BinaryTree{3, 0, 5, 6, 7, 8}).height() == 4);
    BinaryTree tree4{10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 11, 15, 13, 14};
    long height{tree4.height()};
    assert(height == 9);
    assert((BinaryTree{10, 1, 9, 2, 8, 3, 7, 4, 6, 5, 11, 15, 13, 14}).height() == 9);
}

void test_bst_size()
{
    assert((BinaryTree{1}).size() == 1);
    assert((BinaryTree{3, 0, 5}).size() == 3);
    assert((BinaryTree{3, 0, 5, 6, 7, 8}).size() == 6);
    assert((BinaryTree{10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 11, 15, 13, 14}).size() == 14);
    assert((BinaryTree{10, 1, 9, 2, 8, 3, 7, 4, 6, 5, 11, 15, 13, 14}).size() == 14);
}

int main()
{
    test_bst_add();
    test_bst_outstream();
    test_bst_contains();
    test_bst_inorder_traversal();
    test_bst_height();
    test_bst_size();
}