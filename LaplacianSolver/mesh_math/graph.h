#ifndef GRAPH_H
#define GRAPH_H

#include <set>
#include <stack>
#include <vector>
#include <utility>
#include <algorithm>

/**
 * Unordered graph to keep mesh connectivity information
 */

namespace data_structs {

template<typename label>
class graph
{
	using edge_type = std::pair<label, label>;

    using label_list = std::set<label>;
    using adjacency_list = std::vector<label_list>;

    adjacency_list adjacency_list_;

public:

    /**
     * Creates an empty graph
     */
    graph(){}

    /**
     * Resets the connectivity array
     */
    void clear(){ adjacency_list_.clear(); }

    /**
     * Adds new connection into the unordered graph,
     * Note: node labels go in ascending order
     */
    void add_connection(label i, label j)
    {
        label max = std::max(i, j);
        label min = std::min(i, j);

        if(max >= adjacency_list_.size()) adjacency_list_.resize(max + 1);
		adjacency_list_[max].insert(min);
		adjacency_list_[min].insert(max);
    }

    /**
     * Get neighbour of the node
     */
    inline const label_list& get_node_neighbour(label node_label) const
    { return adjacency_list_[node_label]; }

    /**
     * Get number of nodes
     */
    inline size_t size() const { return adjacency_list_.size(); }

    /**
     * Returns a total number of connections
     */
    size_t connections_number() const
    {
        size_t connestions = 0;
        for(const label_list& ll : adjacency_list_)
            connestions += ll.size();
        return connestions/2;
    }

    /**
     * Iterates over unique connections passing corresponding labels into the observer functor
     */
    template<typename Observer>
    void iterate_over_unique_connections(Observer observer) const
    {
        for(size_t i = 0; i != adjacency_list_.size(); ++i)
        {
            label_list::const_oterator 
				first = adjacency_list_[i].lower_bound(i),
				last  = adjacency_list_[i];
            for(; first != last; ++first) observer(i, *first);
        }
    }

    /**
     * Graph deep search itterative from label into fully connected graph as a mesh should be
     */
    template<typename Obs, typename Pred>
	void dfs_iterative(label start, Pred pred, Obs obs) const
	{
		std::vector<bool> visited(adjacency_list_.size(), false);
		std::stack<label> dfs_stack;
		dfs_stack.push(start);

		while (!dfs_stack.empty())
		{
			label current_label = dfs_stack.top(); dfs_stack.pop();

			if (!visited[current_label] && pred(current_label))
			{
				observer(current_label);
				visited[current_label] = true;
				for (label l : get_node_neighbour(current_label)) dfs_stack.push(l);
			}
		}
	}
};

}

#endif // GRAPH_H
