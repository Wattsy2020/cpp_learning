#include <vector>
#include <concepts>
#include <optional>
#include <assert.h>
#include <iterator>
#include <ranges>
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
        const int new_node_idx{find_node(item)};
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

    std::vector<std::optional<T>> preorder_traversal() const
    {
        return items;
    }

private:
    // items[0]: root
    // items[1]: left node of root
    // items[2]: right node of root
    // items[2*i + 1]: left node of i
    // items[2*i + 2]: right node of i
    std::vector<std::optional<T>> items;

    bool node_exists(const int idx) const
    {
        return idx < items.size() && items[idx].has_value();
    }

    // find the idx of the given item in the tree
    // if item doesn't exist in the tree, then return the index where it should be placed
    int find_node(const T &item) const
    {
        int current_node{0};
        while (node_exists(current_node))
        {
            const T current_val{items[current_node].value()};
            if (item < current_val)
                current_node = 2 * current_node + 1;
            else if (item == current_val)
                return current_node;
            else
                current_node = 2 * current_node + 2;
        }
        return current_node;
    }

    // add node to the tree, increasing the vector size if necessary
    void add_node(const int idx, const T item)
    {
        while (idx >= items.size())
            items.push_back(std::nullopt);
        items[idx] = item;
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

int main()
{
    test_bst_add();
    test_bst_outstream();
}