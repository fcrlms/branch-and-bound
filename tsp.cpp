#include "tsp.hpp"
#include "data.hpp"

void tsp_init (TSPInfo &tsp_info, int argc, char **argv) {
	Data *data = new Data(argc, argv[1]);
	data->readData();

	tsp_info.dimension = data->getDimension();

	tsp_info.cost_matrix = new double*[tsp_info.dimension];
	tsp_info.cost_copy = new double*[tsp_info.dimension];

	for (int i = 0; i < tsp_info.dimension; i++){
		tsp_info.cost_matrix[i] = new double[tsp_info.dimension];
		tsp_info.cost_copy[i] = new double[tsp_info.dimension];

		for (int j = 0; j < tsp_info.dimension; j++){
			tsp_info.cost_matrix[i][j] = data->getDistance(i, j);
			tsp_info.cost_copy[i][j] = tsp_info.cost_matrix[i][j];
		}
	}

    delete data;
}

void tsp_free (TSPInfo &tsp_info) {
    for (int i = 0; i < tsp_info.dimension; i++) {
		delete [] tsp_info.cost_matrix[i];
		delete [] tsp_info.cost_copy[i];
	}

	delete [] tsp_info.cost_matrix;
	delete [] tsp_info.cost_copy;
}
