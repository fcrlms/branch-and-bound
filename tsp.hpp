#ifndef TSP_INFO_H
#define TSP_INFO_H

typedef struct s_tsp_info {
	int dimension;

	double **cost_matrix;
	double **cost_copy;

	/**
	 * The upper bound is defined as the cost of a valid TSP solution
	 * Possible way of determining it: find a viable solution using a
	 * heuristic and set its cost as the upper bound
	 *
	 * If the program has produced a valid solution and its cost
	 * is lower than the upper bound we may updated it to prevent
	 * further unecessary calculations
	 */
	double upper_bound;
} TSPInfo;

void tsp_init (TSPInfo &tsp_info, int argc, char **argv);

void tsp_free (TSPInfo &tsp_info);

#endif
