#include <iostream>
#include <vector> // vector
#include <list> // list
#include <utility> // pair
#include "data.hpp" // INFINITE
#include "tsp.hpp"
#include "node.hpp"
#include "hungarian.hpp"

void print_subtour (std::vector<int> &subtour) {
	int len = subtour.size();

	int j = 0;
	while (j < len -1) {
		std::cout << subtour[j++] << ", ";
	}

	std::cout << subtour[j] << std::endl;
}

void print_subtours (Node &node) {
	for (unsigned long i = 0; i < node.subtours.size(); ++i) {
		print_subtour(node.subtours[i]);
	}
}

void print_node (Node &node) {
	std::cout << "== Node ==" << std::endl;

	std::cout << "Prohibited edges: ";
	for (unsigned long i = 0; i < node.prohibited_edges.size(); ++i) {
		std::cout << "("
			<< node.prohibited_edges[i].first << ","
			<< node.prohibited_edges[i].second << ")" << " ,";
	}
	std::cout << std::endl;

	std::cout << "Cut? " << node.cut << std::endl;
	std::cout << "Lower bound: " << node.lower_bound << std::endl;

	std::cout << "Chosen subtour: ";
	print_subtour(node.subtours[node.chosen_subtour]);
	std::cout << "Subtours: " << std::endl;
	print_subtours(node);

	std::cout << "== end node ==" << std::endl;
}

void find_lowest_subtour (Node &root) {
	int total_subtours = root.subtours.size();

	int eligible_subtour = 0;
	int lowest_size = root.subtours[eligible_subtour].size();

	// searching for smallest subtour
	for (int i = 1; i < total_subtours; ++i) {
		int current_size = root.subtours[i].size();

		if (current_size > lowest_size) {
			continue;
		} else if (current_size == lowest_size) {
			// if the current and eligible subtours have the same size
			// the subtour with the lowest index for the first node becomes
			// the eligible
			if (root.subtours[i][0] < root.subtours[eligible_subtour][0])
				eligible_subtour = i;
		} else {
			// the current subtour is the smallest
			eligible_subtour = i;
			lowest_size = current_size;
		}
	}

	root.chosen_subtour = eligible_subtour;
}

/**
 * Uses the assignment matrix to create vectors that represent
 * the subtours from the hungarian solution
 */
void get_subtours_from_matrix (Node &node, int **assignment_matrix, int dimension) {
	std::vector<bool> was_visited;

	for (int i = 0; i < dimension; ++i) {
		was_visited.push_back(false);
	}

	for (int n = 0; n < dimension; ++n) {
		if (was_visited[n])
			continue;

		int subtour_start = n;
		int current_node = n;
		std::vector<int> current_subtour;

		do {
			// insert the current node in the subtour and
			// mark it as visited
			current_subtour.push_back(current_node +1);
			was_visited[current_node] = true;

			// the current node is connect to another single node
			// that we jump to
			for (int i = 0; i < dimension; ++i) {
				// search on current_node row
				if (assignment_matrix[current_node][i] == 1) {
					current_node = i;
					break;
				}
			}

		// when the node we jump to is the starting node
		// we have completed the subtour
		} while (current_node != subtour_start);

		current_subtour.push_back(subtour_start +1);
		node.subtours.push_back(current_subtour);
	}
}

void node_calculate_solution (Node &node, TSPInfo &tsp_info) {
	// all prohibited edges have their cost set to infinity
	for (unsigned long k = 0; k < node.prohibited_edges.size(); ++k) {
		int i = node.prohibited_edges[k].first -1;
		int j = node.prohibited_edges[k].second -1;

		tsp_info.cost_copy[i][j] = INFINITE;
	}

	hungarian_problem_t new_problem;
	hungarian_init(&new_problem, tsp_info.cost_copy,
        tsp_info.dimension, tsp_info.dimension, HUNGARIAN_MODE_MINIMIZE_COST);

	// the hungarian is called with the copy of the cost matrix we've changed
	node.lower_bound = hungarian_solve(&new_problem);

	// setting the subtours of our node
	get_subtours_from_matrix(node, new_problem.assignment, tsp_info.dimension);

	node.cut = node.subtours.size() == 1;

	// setting the chosen subtour
	find_lowest_subtour(node);

	// reverting changes made to the copy matrix
	for (unsigned long k = 0; k < node.prohibited_edges.size(); ++k) {
		int i = node.prohibited_edges[k].first -1;
		int j = node.prohibited_edges[k].second -1;

		tsp_info.cost_copy[i][j] = tsp_info.cost_matrix[i][j];
	}

	hungarian_free(&new_problem);
}
