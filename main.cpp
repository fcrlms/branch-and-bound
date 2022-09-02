#include <iostream>
#include <cstdlib> // exit()
#include <vector> // vector
#include <list> // list
#include <utility> // pair
#include "tsp.hpp"
#include "node.hpp"
#include "data.hpp"
#include "hungarian.hpp"

int main (int argc, char **argv) {
	TSPInfo tsp_info;
	tsp_init(tsp_info, argc, argv);

	// change later
	tsp_info.upper_bound = INFINITE;

	Node root;
	node_calculate_solution(root, tsp_info);

	std::list<Node> tree;
	tree.push_back(root);

	Node sol = root;

	while (tree.size()) {
		Node curr_node = tree.front();
		tree.pop_front();

		if (curr_node.lower_bound > tsp_info.upper_bound) {
			// ignore node and all of its childs
			continue;
		}

		if (curr_node.cut) {
			// treat as solution

			if (curr_node.lower_bound < tsp_info.upper_bound) {
				sol = curr_node;
				tsp_info.upper_bound = curr_node.lower_bound;
			}

			continue;
		}

		std::vector<int> &subtour = curr_node.subtours[curr_node.chosen_subtour];

		// creates and pushes children to tree, sorted by lowest lower_bound
		for (unsigned long i = 0; i < subtour.size() -1; ++i) {
			Node child;
			child.prohibited_edges = curr_node.prohibited_edges;

			// edge between nodes i and i+1
			std::pair<int, int> curr_edge (subtour[i], subtour[i +1]);

			child.prohibited_edges.push_back(curr_edge);

			node_calculate_solution(child, tsp_info);

			// finding position to insert child
			std::list<Node>::iterator it;
			for (it = tree.begin(); it != tree.end(); ++it) {
				if (child.lower_bound < it->lower_bound) {
					break;
				}
			}

			tree.insert(it, child);
		}
	}

	// freeing matrixes allocated for tsp
	tsp_free(tsp_info);

	std::cout << "Solution: " << tsp_info.upper_bound << std::endl;
	print_node(sol);

	exit(EXIT_SUCCESS);
}
