#ifndef NODE_H
#define NODE_H

#include <vector> // vector
#include <utility> // pair
#include "tsp.hpp"

typedef struct s_node Node;

struct s_node {
	/**
	 * Subtours of this solution
	 */
	std::vector< std::vector<int> > subtours;

	/**
	 * This solutions cost
	 */
	double lower_bound;

	/**
	 * Smallest subtour of subtours, if there is more than one,
	 * the subtour with the first node of lowest index is chosen
	 */
	int chosen_subtour;

	/**
	 * List of prohibited edges of the chosen_subtour
	 * (connections between nodes)
	 *
	 * It can be enforce by setting Cij in the distance matrix
	 * to infinity
	 */
	std::vector< std::pair<int, int> > prohibited_edges;

	/**
	 * If this node with its set of prohibited nodes produces
	 * a valid solution (only one subtour) then we will 'cut' it,
	 * i.e. we won't branch it further
	 */
	bool cut;
};

void node_calculate_solution (Node &node, TSPInfo &tsp_info);

void print_subtour (std::vector<int> &subtour);
void print_subtours (Node &node);
void print_node (Node &node);

#endif
