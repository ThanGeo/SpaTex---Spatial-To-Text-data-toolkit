#include "containers.h"

Config g_config;

Dataset::Dataset(DatasetStatement &stmt){
    this->path = stmt.path;
    this->dataType = stmt.dataType;
    this->fileFormat = stmt.fileFormat;
    this->nickname = stmt.nickname;
    this->description = stmt.description;
    this->wktColIdx = stmt.wktColIdx;
    this->nameColIdx = stmt.nameColIdx;
}

Shape* Dataset::getObject(size_t recID) {
    auto it = objects.find(recID);
    if (it == objects.end()) {
        return nullptr;
    }
    return &it->second;
}

DB_STATUS Dataset::addObject(Shape &object) {
    // add object to the objects map
    objects[object.recID] = object;
    // get object reference
    Shape* objectRef = this->getObject(object.recID);
    if (objectRef == nullptr) {
        logger::log_error(DBERR_INVALID_KEY, "Object with id", object.recID, "does not exist in the object map.");
        return DBERR_INVALID_KEY;
    }
    // insert reference to partition index
    for (int i=0; i<object.getPartitionCount(); i++) {
        this->uniformGridIndex.addObject(object.getPartitionID(i), objectRef);
    }
    // keep the ID in the list
    objectIDs.push_back(object.recID);
    totalObjects += 1;

    return DBERR_OK;
}

void Dataset::printObjectsGeometries() {
    for (auto &it : objectIDs) {
        printf("MBR: (%f,%f),(%f,%f)\n", this->objects[it].mbr.pMin.x, this->objects[it].mbr.pMin.y, this->objects[it].mbr.pMax.x, this->objects[it].mbr.pMax.y);
        this->objects[it].printGeometry();
    }
}

void Dataset::printPartitions() {
    for (auto it : this->uniformGridIndex.partitions) {
        printf("Partition %d\n", it.partitionID);
        // todo: print contents too
    }
}

void Dataset::printPartitionContents(int partitionID) {
    printf("Partition %d contents:\n", partitionID);
    Partition* partition = uniformGridIndex.getPartition(partitionID);
    if (partition != nullptr) {
        for(auto &it: *partition->getContents()) {
            this->getObject(it->recID)->printGeometry();
        }
        printf("\n");
    } else {
        printf("<null>\n");
    }

}

void Dataset::printPartitionStatistics() {
    size_t totalObjectsInPartitions = 0;
    size_t totalPartitions = this->uniformGridIndex.partitions.size();
    for (auto &partition : this->uniformGridIndex.partitions) {
        totalObjectsInPartitions += partition.getContents()->size();
    }

    logger::log_success("Dataset", this->nickname, "non-empty partitions:", totalPartitions, "with avg objects per partition:", totalObjectsInPartitions / (double) totalPartitions);
}

Dataset* DatasetMetadata::getDatasetByNickname(std::string &nickname) {
    auto it = datasets.find(nickname);
    if (it == datasets.end()) {
        return nullptr;
    }        
    return &it->second;
}

DataspaceMetadata::DataspaceMetadata() {
    xMinGlobal = std::numeric_limits<int>::max();
    yMinGlobal = std::numeric_limits<int>::max();
    xMaxGlobal = -std::numeric_limits<int>::max();
    yMaxGlobal = -std::numeric_limits<int>::max();
}

void DataspaceMetadata::set(double xMinGlobal, double yMinGlobal, double xMaxGlobal, double yMaxGlobal) {
    this->xMinGlobal = xMinGlobal - EPS;
    this->yMinGlobal = yMinGlobal - EPS;
    this->xMaxGlobal = xMaxGlobal + EPS;
    this->yMaxGlobal = yMaxGlobal + EPS;
    this->xExtent = this->xMaxGlobal - this->xMinGlobal;
    this->yExtent = this->yMaxGlobal - this->yMinGlobal;
    this->maxExtent = std::max(this->xExtent, this->yExtent);
    // printf("-------------------------\n");
    // printf("Dataspace bounds: (%f,%f),(%f,%f)\n", this->xMinGlobal, this->yMinGlobal, this->xMaxGlobal, this->yMaxGlobal);
    // printf("xExtent: %f, yExtent: %f\n", this->xExtent, this->yExtent);
    // printf("Max extent: %f\n", this->maxExtent);
    // printf("-------------------------\n");
}

void DataspaceMetadata::clear() {
    xMinGlobal = 0;
    yMinGlobal = 0;
    xMaxGlobal = 0;
    yMaxGlobal = 0;
    xExtent = 0;
    yExtent = 0;
}

int DatasetMetadata::getNumberOfDatasets() {
    return numberOfDatasets;
}

void DatasetMetadata::clear() {
    numberOfDatasets = 0;
    R = nullptr;
    S = nullptr;
    datasets.clear();
    dataspaceMetadata.clear();
}

Dataset* DatasetMetadata::getDatasetR() {
    return R;
}

Dataset* DatasetMetadata::getDatasetS() {
    return S;
}

Dataset* DatasetMetadata::getDatasetByIdx(DatasetIndex datasetIndex) {
    switch (datasetIndex) {
        case DATASET_R:
            return R;
        case DATASET_S:
            return S;
    }
    logger::log_error(DBERR_INVALID_PARAMETER, "Invalid dataset index:", datasetIndex);
    return nullptr;
}
/**
@brief adds a Dataset to the configuration's dataset metadata
 * @warning it has to be an empty dataset BUT its nickname needs to be set
 */
DB_STATUS DatasetMetadata::addDataset(DatasetIndex datasetIdx, Dataset &dataset) {
    // add to datasets struct
    datasets[dataset.nickname] = dataset;
    switch (datasetIdx) {
        case DATASET_R:
            // R is being added
            R = &datasets[dataset.nickname];
            break;
        case DATASET_S:
            // S is being added
            S = &datasets[dataset.nickname];
            break;
        default:
            logger::log_error(DBERR_INVALID_PARAMETER, "Invalid dataset index. Use only DATASET_R or DATASET_S.");
            break;
    }
    numberOfDatasets++;
    return DBERR_OK;
}

void DatasetMetadata::updateDataspace() {
    // find the bounds that enclose both datasets
    for (auto &it: datasets) {
        dataspaceMetadata.xMinGlobal = std::min(dataspaceMetadata.xMinGlobal, it.second.dataspaceMetadata.xMinGlobal);
        dataspaceMetadata.yMinGlobal = std::min(dataspaceMetadata.yMinGlobal, it.second.dataspaceMetadata.yMinGlobal);
        dataspaceMetadata.xMaxGlobal = std::max(dataspaceMetadata.xMaxGlobal, it.second.dataspaceMetadata.xMaxGlobal);
        dataspaceMetadata.yMaxGlobal = std::max(dataspaceMetadata.yMaxGlobal, it.second.dataspaceMetadata.yMaxGlobal);
    }
    dataspaceMetadata.xExtent = dataspaceMetadata.xMaxGlobal - dataspaceMetadata.xMinGlobal;
    dataspaceMetadata.yExtent = dataspaceMetadata.yMaxGlobal - dataspaceMetadata.yMinGlobal;
    // set as both datasets' bounds
    for (auto &it: datasets) {
        it.second.dataspaceMetadata = dataspaceMetadata;
    }
}


Partition* UniformGridIndex::getOrCreatePartition(int partitionID) {
    // return &it->second;
    auto it = partitionMap.find(partitionID);
    if (it == partitionMap.end()) {
        // new partition
        Partition partition(partitionID);
        partitions.push_back(partition);
        size_t newIndex = partitions.size() - 1;
        partitionMap[partitionID] = newIndex;
        return &partitions[newIndex];
    }
    // existing partition
    return &partitions[it->second];
}

void UniformGridIndex::addObject(int partitionID, Shape* objectRef) {
    auto it = partitionMap.find(partitionID);
    if (it == partitionMap.end()) {
        // new partition
        Partition partition(partitionID);
        // add object reference
        partition.addObject(objectRef);
        // save partition
        partitions.push_back(partition);
        int newIndex = partitions.size() - 1;
        partitionMap[partitionID] = newIndex;
    } else {
        // existing partition
        partitions[it->second].addObject(objectRef);
    }
}

Partition* UniformGridIndex::getPartition(int partitionID) {
    auto it = partitionMap.find(partitionID);
    if (it == partitionMap.end()) {
        // does not exist
        return nullptr;
    } 
    // exists
    return &partitions[it->second];
}

std::vector<Shape*>* Partition::getContents() {
    return &contents;
}

void Partition::addObject(Shape *objectRef) {
    contents.emplace_back(objectRef);
}

namespace shape_factory
{
    // Create empty shapes
    Shape createEmptyPointShape() {
        return Shape(PointWrapper());
    }

    Shape createEmptyPolygonShape() {
        return Shape(PolygonWrapper());
    }

    Shape createEmptyLineStringShape() {
        return Shape(LineStringWrapper());
    }

    Shape createEmptyRectangleShape() {
        return Shape(RectangleWrapper());
    }

    DB_STATUS createEmpty(DataType dataType, Shape &object) {
        switch (dataType) {
            case DT_POINT:
                object = createEmptyPointShape();
                break;
            case DT_LINESTRING:
                object = createEmptyLineStringShape();
                break;
            case DT_RECTANGLE:
                object = createEmptyRectangleShape();
                break;
            case DT_POLYGON:
                object = createEmptyPolygonShape();
                break;
            default:
                logger::log_error(DBERR_INVALID_DATATYPE, "Invalid datatype in factory method:", dataType);
                return DBERR_INVALID_DATATYPE;
        }
        return DBERR_OK;
    }
}
