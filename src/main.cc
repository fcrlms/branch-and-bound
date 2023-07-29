#include <iostream>
#include <cstdlib> // exit()
#include <vector> // vector
#include <list> // list
#include <utility> // pair
#include <chrono> // measuring time
#include <queue> // priority_queue
#include "tsp.h"
#include "node.h"
#include "data.h"
#include "hungarian.h"

#define BEST_BOUND_SEARCH 1
#define BREADTH_FIRST_SEARCH 2
#define DEPTH_FIRST_SEARCH 3

#define TESTS_TO_RUN 1

// Custom compare for priority_queue
class Compare
{
	public:
	    bool operator() (Node &a, Node &b) {
		    return a.lower_bound > b.lower_bound;
		}
};

void search_best (TSPInfo &tsp_info);
void search_breadth (TSPInfo &tsp_info);
void search_depth (TSPInfo &tsp_info);

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

	double durations[TESTS_TO_RUN];
	double costs[TESTS_TO_RUN];

	for (int i = 0; i < TESTS_TO_RUN; ++i) {
		// resetting upper bound
		tsp_info.upper_bound = INFINITE;

		auto start = std::chrono::high_resolution_clock::now();

		switch (choice) {
		case BEST_BOUND_SEARCH:
			search_best(tsp_info);
			break;
		case BREADTH_FIRST_SEARCH:
			search_breadth(tsp_info);
			break;
		case DEPTH_FIRST_SEARCH:
			search_depth(tsp_info);
			break;
		}

		auto end = std::chrono::high_resolution_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

		costs[i] = tsp_info.upper_bound;
		durations[i] = duration.count();
	}

	double duration = 0;
	double cost = 0;
	for (int i = 0; i < TESTS_TO_RUN; ++i) {
		duration += durations[i];
		cost += costs[i];
	}
	duration /= TESTS_TO_RUN;
	cost /= TESTS_TO_RUN;

	std::cout << "duration: " << duration / 1000000 << " seconds" << std::endl;

	std::cout << "Cost: " << cost << std::endl;

	tsp_free(tsp_info);

	exit(EXIT_SUCCESS);
}

void search_best (TSPInfo &tsp_info) {
	Node root;
	node_calculate_solution(root, tsp_info);

	priority_queue<Node, std::vector<Node>, Compare> tree;
	tree.push(root);

	Node best = root;

	while (tree.size()) {
		Node curr_node = tree.top();
		tree.pop();

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

			tree.push(child);
		}
	}
}

void search_breadth (TSPInfo &tsp_info) {
	Node root;
	node_calculate_solution(root, tsp_info);

	std::list<Node> tree;
	tree.push_back(root);

	Node best = root;

	while (tree.size()) {
		Node curr_node = tree.front();
		tree.pop_front();

		std::vector<int> &subtour = curr_node.subtours[curr_node.chosen_subtour];

		// creates and pushes children to tree
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

			tree.push_back(child);
		}
	}
}

void search_depth (TSPInfo &tsp_info) {
	Node root;
	node_calculate_solution(root, tsp_info);

	std::list<Node> tree;
	tree.push_back(root);

	Node best = root;

	while (tree.size()) {
		Node curr_node = tree.front();
		tree.pop_front();

		std::vector<int> &subtour = curr_node.subtours[curr_node.chosen_subtour];

		// creates and pushes children to tree in inverse order
		// when we acess the tree with front() we will do
		// a depth first search
		for (size_t i = subtour.size() -1; i > 0; --i) {
			Node child;
			child.prohibited_edges = curr_node.prohibited_edges;

			// edge between nodes i-1 and i
			std::pair<int, int> curr_edge (subtour[i -1], subtour[i]);

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

			tree.push_front(child);
		}
	}
}
