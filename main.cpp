#include <iostream>
#include <cstdlib> // exit()
#include <vector> // vector
#include <list> // list
#include <utility> // pair
#include <chrono> // measuring time
#include "tsp.hpp"
#include "node.hpp"
#include "data.hpp"
#include "hungarian.hpp"

#define BEST_BOUND_SEARCH 1
#define BREADTH_FIRST_SEARCH 2
#define DEPTH_FIRST_SEARCH 3

int main (int argc, char **argv) {
	TSPInfo tsp_info;
	tsp_init(tsp_info, argc, argv);
	tsp_info.upper_bound = INFINITE;

	int choice = 0;
	while (choice < 1 || choice > 3) {
			std::cout << "Branch and Bound method for TSP" << std::endl;

		std::cout << "Choose a method of tree traversal:" << std::endl
			<< "1: BestBound" << std::endl
			<< "2: Breadth first" << std::endl
			<< "3: Depth first" << std::endl
			<< "> ";

		std::cin >> choice;
	}

	auto start = std::chrono::high_resolution_clock::now();

	Node root;
	node_calculate_solution(root, tsp_info);

	std::list<Node> tree;
	tree.push_back(root);

	Node best = root;

	while (tree.size()) {
		Node curr_node = tree.front();
		tree.pop_front();

		std::vector<int> &subtour = curr_node.subtours[curr_node.chosen_subtour];

		// creates and pushes children to tree, sorted by lowest lower_bound
		for (size_t i = 0; i < subtour.size() -1; ++i) {
			Node child;
			child.prohibited_edges = curr_node.prohibited_edges;

			// edge between nodes i and i+1
			std::pair<int, int> curr_edge (subtour[i], subtour[i +1]);

			child.prohibited_edges.push_back(curr_edge);

			node_calculate_solution(child, tsp_info);

			if (child.lower_bound > tsp_info.upper_bound) {
				// ignore node and all of its childs
				break;
			}

			// possible solution
			if (child.cut) {
				// if this solution cost is lower than the current upper bound
				// then we updated the upper bound and this solution is marked as best
				if (child.lower_bound < tsp_info.upper_bound) {
					best = child;
					tsp_info.upper_bound = child.lower_bound;
				}

				break;
			}

			switch (choice) {
			case BEST_BOUND_SEARCH: {
				// finding position to insert child
				std::list<Node>::iterator it;
				for (it = tree.begin(); it != tree.end(); ++it) {
					if (child.lower_bound < it->lower_bound) {
						break;
					}
				}

				tree.insert(it, child);

				break;
			}
			case BREADTH_FIRST_SEARCH: {
				tree.push_back(child);
				break;
			}
			// this is actually going for the rightmost child
			// and not the leftmost, easy to change, invert order of childs
			case DEPTH_FIRST_SEARCH:
				tree.push_front(child);
				break;
			}
		}
	}

	auto end = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	std::cout << "duration: " << duration.count() / (double) 1000000 << " seconds" << std::endl;

	std::cout << "Cost: " << tsp_info.upper_bound << std::endl;
	std::cout << "Solution: ";
	print_subtour(best.subtours[0]);

	// freeing matrixes allocated for tsp
	tsp_free(tsp_info);

	exit(EXIT_SUCCESS);
}
