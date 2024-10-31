#include "include/utils.h"
#include "include/parse.h"
#include "include/index/create.h"
#include "include/index/filter.h"


int main(int argc, char *argv[]) {
    clock_t timer;
    DB_STATUS ret = DBERR_OK;

    // parse user input
    ret = parse::parseArguments(argc, argv);
    if (ret != DBERR_OK) {
        logger::log_error(ret, "Parsing arguments failed.");
        return ret;
    }

    // load data to index
    ret = uniform_grid::create();
    if (ret != DBERR_OK) {
        logger::log_error(ret, "Index creation failed.");
        return ret;
    }

    logger::log_success("Dataset", g_config.datasetMetadata.getDatasetR()->nickname, "loaded", g_config.datasetMetadata.getDatasetR()->totalObjects,"objects");
    logger::log_success("Dataset", g_config.datasetMetadata.getDatasetS()->nickname, "loaded", g_config.datasetMetadata.getDatasetS()->totalObjects,"objects");
    
    g_config.datasetMetadata.getDatasetR()->printPartitionStatistics();
    g_config.datasetMetadata.getDatasetS()->printPartitionStatistics();
    // g_config.datasetMetadata.getDatasetR()->printPartitionContents(383318);
    // g_config.datasetMetadata.getDatasetS()->printPartitionContents(383318);

    // write the objects MBRs (todo)

    // evaluate
    timer = clock();
    ret = uniform_grid::evaluate(g_config.datasetMetadata.getDatasetR(), g_config.datasetMetadata.getDatasetS());
    if (ret != DBERR_OK) {
        logger::log_error(ret, "Evaluation failed.");
        return ret;
    }
    logger::log_success("Evaluation finished in", (clock()-timer) / (double)(CLOCKS_PER_SEC), "seconds");

    // print write buffers
    // g_config.diskWriter.printBufferSizes();

    return 0;
}