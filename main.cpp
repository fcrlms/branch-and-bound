#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <list>
#include "data.hpp"
#include "hungarian.hpp"

/**
 * The upper bound is defined as the cost of a valid TSP solution
 * Possible way of determining it: find a viable solution using a
 * heuristic and set its cost as the upper bound
 *
 * If the program has produced a valid solution and its cost
 * is lower than the upper bound we may updated it to prevent
 * further unecessary calculations
 */
double upper_bound = HUGE_VAL;

typedef struct s_node Node;

struct s_node {
	/**
	 * Subtours of this solution
	 */
	std::vector<std::vector<int>> subtours;

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
	std::vector<std::pair<int, int>> prohibited_edges;

	/**
	 * If this node with its set of prohibited nodes produces
	 * a valid solution (only one subtour) then we will 'cut' it,
	 * i.e. we won't branch it further
	 */
	bool cut;
};

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

/**
 * Finds the smallest subtour of `root` and assigns it to
 * `root.chosen_subtour`
 *
 * In case of a tie, the subtour which the first node has
 * the lowest index gets chosen
 */
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
 * Turns assingment matrix into a list of subtours
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
				if (assignment_matrix[i][current_node] == 1) {
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

/*
If a node already analyzed all of its subtours we should remove it from the tree

Good criteria for order of operation is to choose the node with the lowest
lower_bound
(try to insert the lowest LB at the start of the list ???)

How to deal with a changing cost matrix?
- Track what edge costs were changed and revert them when necessary?
*/

int main (int argc, char **argv) {
	Data *data = new Data(argc, argv[1]);
	data->readData();

	int dimension = data->getDimension();

	double **cost = new double*[dimension];
	for (int i = 0; i < dimension; i++){
		cost[i] = new double[dimension];
		for (int j = 0; j < dimension; j++){
			cost[i][j] = data->getDistance(i, j);
		}
	}


	hungarian_problem_t p;
	hungarian_init(&p, cost, dimension, dimension, HUNGARIAN_MODE_MINIMIZE_COST); // Carregando o problema


	double obj_value = hungarian_solve(&p);
	std::cout << "Obj. value: " << obj_value << std::endl;

	std::cout << "Assignment" << std::endl;
	hungarian_print_assignment(&p);

	Node root;
	root.lower_bound = obj_value;

	get_subtours_from_matrix(root, p.assignment, dimension);

	root.cut = root.subtours.size() == 1;

	std::cout << "Subtours:" << std::endl;
	print_subtours(root);
	std::cout << "----" << std::endl;

	find_lowest_subtour(root);
	std::cout << "Chosen subtour: ";
	print_subtour(root.subtours[root.chosen_subtour]);

	std::list<Node> tree;

	tree.push_back(root);

	hungarian_free(&p);
	for (int i = 0; i < dimension; i++) delete [] cost[i];
	delete [] cost;
	delete data;

	exit(EXIT_SUCCESS);
}
