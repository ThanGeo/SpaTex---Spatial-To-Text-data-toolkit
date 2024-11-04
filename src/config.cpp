#include "config.h"

namespace config
{
    DB_STATUS setup(ArgumentsStatement &argStmt) {
        DB_STATUS ret = DBERR_OK;
        Dataset R(argStmt.datasetR);
        Dataset S(argStmt.datasetS);
        // add datasets to config
        ret = g_config.datasetMetadata.addDataset(DATASET_R, R);
        if (ret != DBERR_OK) {
            logger::log_error(ret, "Failed while adding dataset R to config.");
            return ret;
        }
        ret = g_config.datasetMetadata.addDataset(DATASET_S, S);
        if (ret != DBERR_OK) {
            logger::log_error(ret, "Failed while adding dataset R to config.");
            return ret;
        }
        // check if self-join
        if (g_config.datasetMetadata.getDatasetR()->path.compare(g_config.datasetMetadata.getDatasetS()->path) == 0) {
            g_config.datasetMetadata.setSelfJoin(true);
            logger::log_success("Self-join enabled.");
        }
        // open output file
        ret = g_config.diskWriter.openOutputFilestream(argStmt.outputStmt.outputFilepath, argStmt.outputStmt.append);
        if (ret != DBERR_OK) {
            logger::log_error(ret, "Failed while opening output filestream.");
            return ret;
        }
        // set document output file
        g_config.diskWriter.setDocumentType(mapping::documentTypeTextToInt(argStmt.outputStmt.documentType));

        return ret;
    }
}
