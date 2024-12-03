#include "index/create.h"

namespace uniform_grid
{
    int getPartitionID(int i, int j, int partitionsPerDim) {
        return (i + (j * partitionsPerDim));
    }

    /**
    @brief Calculates the intersecting partitions in the distribution grid for the given MBR.
     * @param[in] xMin, yMin, xMax, yMax MBR
     * @param[out] partitionIDs The partition IDs that intersect with the MBR.
     */
    static DB_STATUS getPartitionsForMBR(MBR &mbr, std::vector<int> &partitionIDs){
        int minPartitionX = (mbr.pMin.x - g_config.datasetMetadata.dataspaceMetadata.xMinGlobal) / (g_config.datasetMetadata.dataspaceMetadata.xExtent / (double) g_config.indexConfig.partitionsPerDim);
        int minPartitionY = (mbr.pMin.y - g_config.datasetMetadata.dataspaceMetadata.yMinGlobal) / (g_config.datasetMetadata.dataspaceMetadata.yExtent / (double) g_config.indexConfig.partitionsPerDim);
        int maxPartitionX = (mbr.pMax.x - g_config.datasetMetadata.dataspaceMetadata.xMinGlobal) / (g_config.datasetMetadata.dataspaceMetadata.xExtent / (double) g_config.indexConfig.partitionsPerDim);
        int maxPartitionY = (mbr.pMax.y - g_config.datasetMetadata.dataspaceMetadata.yMinGlobal) / (g_config.datasetMetadata.dataspaceMetadata.yExtent / (double) g_config.indexConfig.partitionsPerDim);
        
        int startPartitionID = getPartitionID(minPartitionX, minPartitionY, g_config.indexConfig.partitionsPerDim);
        int lastPartitionID = getPartitionID(maxPartitionX, maxPartitionY, g_config.indexConfig.partitionsPerDim);
        if (startPartitionID < 0 || startPartitionID > g_config.indexConfig.partitionsPerDim * g_config.indexConfig.partitionsPerDim -1) {
            logger::log_error(DBERR_INVALID_PARTITION, "Start partition ID calculated wrong");
            return DBERR_INVALID_PARTITION;
        }
        if (lastPartitionID < 0 || lastPartitionID > g_config.indexConfig.partitionsPerDim * g_config.indexConfig.partitionsPerDim -1) {
            logger::log_error(DBERR_INVALID_PARTITION, "Last partition ID calculated wrong: MBR(", mbr.pMin.x, mbr.pMin.y, mbr.pMax.x, mbr.pMax.y, ")");
            return DBERR_INVALID_PARTITION;
        }
        // create the distribution grid partition list for the object
        partitionIDs.reserve((maxPartitionX - minPartitionX) * (maxPartitionY - minPartitionY));
        for (int i=minPartitionX; i<=maxPartitionX; i++) {
            for (int j=minPartitionY; j<=maxPartitionY; j++) {
                int partitionID = getPartitionID(i, j, g_config.indexConfig.partitionsPerDim);
                partitionIDs.emplace_back(partitionID);
            }
        }
        return DBERR_OK;
    }

    static DB_STATUS calculateDataspaceBounds(Dataset* dataset) {
        DB_STATUS ret = DBERR_OK;
        // open file
        std::ifstream fin(dataset->path);
        if (!fin.is_open()) {
            logger::log_error(DBERR_FILE_OPEN, "Failed to open dataset path:", dataset->path);
            return DBERR_FILE_OPEN;
        }
        std::string line;
        double global_xMin = std::numeric_limits<int>::max();
        double global_yMin = std::numeric_limits<int>::max();
        double global_xMax = -std::numeric_limits<int>::max();
        double global_yMax = -std::numeric_limits<int>::max();
        size_t lineCounter = 0;
        size_t totalValidObjects = 0;

        // shape created
        std::string token;
        std::string wktData;
        // loop lines
        while (std::getline(fin, line)) {     
            // parse line
            std::stringstream ss(line);
            int currentCol = -1;
            // get the wkt data
            while (currentCol < dataset->wktColIdx) {
                std::getline(ss, token, '\t');
                currentCol++;
            }
            wktData = token;
            // get data type
            std::stringstream typess(wktData);
            std::getline(typess, token, '(');
            DataType datatype = mapping::dataTypeTextToInt(token);
            // create object
            Shape object;
            ret = shape_factory::createEmpty(datatype, object);
            if (ret != DBERR_OK) {
                // error creating shape
                logger::log_error(ret, "Failed while creating empty shape of data type", mapping::dataTypeIntToStr(datatype));
                return ret;
            }
            // set object from the WKT
            ret = object.setFromWKT(wktData);
            if (ret == DBERR_INVALID_GEOMETRY) {
                // this line is not the appropriate geometry type, so just ignore
                ret = DBERR_OK;
            } else if (ret != DBERR_OK) {
                // some other error occured, interrupt
                logger::log_error(ret, "Setting object shape from WKT failed.");
                return ret;
            } else {
                // valid object
                totalValidObjects += 1;
                // set the MBR
                object.setMBR();
                // set dataspace bounds
                global_xMin = std::min(global_xMin, object.mbr.pMin.x);
                global_yMin = std::min(global_yMin, object.mbr.pMin.y);
                global_xMax = std::max(global_xMax, object.mbr.pMax.x);
                global_yMax = std::max(global_yMax, object.mbr.pMax.y);
                
            }
            lineCounter += 1;
        }
        // update dataset dataspace
        dataset->dataspaceMetadata.set(global_xMin, global_yMin, global_xMax, global_yMax);
        // close file
        fin.close();
        
        return ret;
    }

    // todo: make parallel
    static DB_STATUS indexDataset(Dataset* dataset) {
        DB_STATUS ret = DBERR_OK;
        
        // open file
        std::ifstream fin(dataset->path);
        if (!fin.is_open()) {
            logger::log_error(DBERR_FILE_OPEN, "Failed to open dataset path:", dataset->path);
            return DBERR_FILE_OPEN;
        }
        std::string line;
        // count how many batches have been sent in total
        int partitionID;
        size_t lineCounter = 0;
        size_t totalValidObjects = 0;
        
        // shape created
        std::string token;
        std::string wktData;
        // loop lines (todo: first count lines, then make parallel load)
        while (std::getline(fin, line)) {
            // parse line
            std::stringstream ss(line);
            std::vector<std::string> tokens;
            ret = splitString(line, '\t', tokens);
            if (ret != DBERR_OK) {
                logger::log_error(ret, "Split data line failed.");
                return ret;
            }

            int currentCol = -1;
            // get the wkt data
            while (currentCol < dataset->wktColIdx) {
                std::getline(ss, token, '\t');
                currentCol++;
            }
            if (dataset->wktColIdx < tokens.size()) {
                wktData = tokens[dataset->wktColIdx];
            } else {
                logger::log_error(DBERR_INVALID_PARAMETER, "Invalid wkt column index for dataset. Value:", dataset->wktColIdx, "while the line has only", tokens.size(), "columns.");
                return DBERR_INVALID_PARAMETER;
            }
            // get data type
            std::string datatypeStr;
            std::stringstream typess(wktData);
            std::getline(typess, datatypeStr, '(');
            DataType datatype = mapping::dataTypeTextToInt(datatypeStr);
            // create object
            Shape object;
            ret = shape_factory::createEmpty(datatype, object);
            if (ret != DBERR_OK) {
                // error creating shape
                logger::log_error(ret, "Failed while creating empty shape of data type", mapping::dataTypeIntToStr(datatype));
                return ret;
            }
            // get the name of the entity
            if (dataset->nameColIdx < tokens.size()) {
                object.name = tokens[dataset->nameColIdx];
            } else {
                logger::log_error(DBERR_INVALID_PARAMETER, "Invalid wkt column index for dataset. Value:", dataset->wktColIdx, "while the line has only", tokens.size(), "columns.");
                return DBERR_INVALID_PARAMETER;
            }
            // add as object name the dataset type (if set) + object name
            if (dataset->description != "") {
                object.name = dataset->description + " " + object.name;
            }
            // get any other column to modify the name with
            if (dataset->otherColIdx != -1){
                if(dataset->otherColIdx < tokens.size()) {
                    int stateFP = std::stoi(tokens[dataset->otherColIdx]);
                    object.name += ", " + state::stateFpToStateName(stateFP);
                } else {
                    logger::log_error(DBERR_INVALID_PARAMETER, "Invalid other column index for dataset. Value:", dataset->otherColIdx, "while the line has only", tokens.size(), "columns.");
                    return DBERR_INVALID_PARAMETER;
                }
            }

            // set rec ID
            object.recID = lineCounter;
            // set object from the WKT
            ret = object.setFromWKT(wktData);
            if (ret == DBERR_INVALID_GEOMETRY) {
                // this line is not the appropriate geometry type, so just ignore
                ret = DBERR_OK;
            } else if (ret != DBERR_OK) {
                // some other error occured, interrupt
                return ret;
            } else {
                // valid object
                totalValidObjects += 1;
                // set the MBR
                object.setMBR();
                // calculate partitions
                std::vector<int> partitionIDs;
                ret = getPartitionsForMBR(object.mbr, partitionIDs);
                if (ret != DBERR_OK) {
                    return ret;
                }
                object.setPartitions(partitionIDs, partitionIDs.size());
                // add to index
                dataset->addObject(object);
            }
            lineCounter += 1;
        }
        // close file
        fin.close();

        return ret;
    }

    DB_STATUS create() {
        DB_STATUS ret = DBERR_OK;

        // calculate dataspace bounds
        ret = calculateDataspaceBounds(g_config.datasetMetadata.getDatasetR());
        if (ret != DBERR_OK) {
            logger::log_error(ret, "Failed calculating dataspace bounds for dataset", g_config.datasetMetadata.getDatasetR()->nickname);
            return ret;
        }
        ret = calculateDataspaceBounds(g_config.datasetMetadata.getDatasetS());
        if (ret != DBERR_OK) {
            logger::log_error(ret, "Failed calculating dataspace bounds for dataset", g_config.datasetMetadata.getDatasetS()->nickname);
            return ret;
        }
        g_config.datasetMetadata.updateDataspace();
        logger::log_success("Global dataspace bounds:", g_config.datasetMetadata.dataspaceMetadata.xMinGlobal, g_config.datasetMetadata.dataspaceMetadata.yMinGlobal, g_config.datasetMetadata.dataspaceMetadata.xMaxGlobal, g_config.datasetMetadata.dataspaceMetadata.yMaxGlobal);

        // index dataset R
        ret = uniform_grid::indexDataset(g_config.datasetMetadata.getDatasetR());
        if (ret != DBERR_OK) {
            logger::log_error(ret, "Failed while indexing dataset", g_config.datasetMetadata.getDatasetS()->nickname);
            return ret;
        }

        // index dataset S
        ret = uniform_grid::indexDataset(g_config.datasetMetadata.getDatasetS());
        if (ret != DBERR_OK) {
            logger::log_error(ret, "Failed while indexing dataset", g_config.datasetMetadata.getDatasetS()->nickname);
            return ret;
        }

        return ret;
    }
}
