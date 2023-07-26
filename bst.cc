#include <vector>
#include <concepts>
#include <optional>
#include <assert.h>
#include "itertools.h"
#include "testlib.h"

template <std::totally_ordered T>
class BinaryTree
{
public:
    BinaryTree() : items{} {};

    void add(const T &item)
    {
        int current_node{0};
        while (node_exists(current_node))
        {
            const T current_val{items[current_node].value()};
            if (item < current_val)
                current_node = 2 * current_node + 1;
            else if (item == current_val)
                return;
            else
                current_node = 2 * current_node + 2;
        }
        add_node(current_node, item);
    }

    std::vector<std::optional<T>> preorder_traversal() const { return items; }

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

int main()
{
    test_bst_add();
    test_bst_outstream();
}