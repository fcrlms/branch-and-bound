#include <cmath>

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

/**
 * Finds the smallest subtour of `root` and assigns it to
 * `root.chosen_subtour`
 *
 * In case of a tie, the subtour which the first node has
 * the lowest index gets chosen
 */
void find_lowest_subtour (Node &root) {
	int total_subtours = root->subtours.size();

	int eligible_subtour = 0;

	// searching for smallest subtour
	int lowest_size = root->subtours[eligible_subtour].size();
	for (int i = 1; i < total_subtours; ++i) {
		int current_size = root->subtours[i].size();

		if (current_size > lowest_size) {
			continue;
		} else if (current_size == lowest_size) {
			// if the current and eligible subtours have the same size
			// the subtour with the lowest index for the first node becomes
			// the eligible
			if (root->subtours[i][0] < root->subtours[eligible_subtour][0])
				eligible_subtour = i;
		} else {
			// the current subtour is the smallest
			eligible_subtour = i;
		}
	}

	root->chosen_subtour = eligible_subtour;
}

/*
If a node already analyzed all of its subtours we should remove it from the tree

Good criteria for order of operation is to choose the node with the lowest
lower_bound
(try to insert the lowest LB at the start of the list ???)

How to deal with a changing cost matrix?
- Track what edge costs were changed and revert them when necessary?
*/

std::list<Node> tree;

Node root;
// calculate solution using hungarian.cpp and update `root`

tree.push_back(root);
