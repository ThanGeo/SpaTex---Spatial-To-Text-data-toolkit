#include "index/filter.h"

namespace uniform_grid
{      
    static inline DB_STATUS relateMBRs(Shape* objR, Shape* objS, std::string &relationText) {
        DB_STATUS ret = DBERR_OK;
        // compute deltas
        double d_xmin = objR->mbr.pMin.x - objS->mbr.pMin.x;
        double d_ymin = objR->mbr.pMin.y - objS->mbr.pMin.y;
        double d_xmax = objR->mbr.pMax.x - objS->mbr.pMax.x;
        double d_ymax = objR->mbr.pMax.y - objS->mbr.pMax.y;

        // check for equality using an error margin because doubles
        if (abs(d_xmin) < EPS) {
            if (abs(d_xmax) < EPS) {
                if (abs(d_ymin) < EPS) {
                    if (abs(d_ymax) < EPS) {
                        // equal MBRs
                        ret = refinement::computeRelationTexts(objR, objS, MBR_EQUAL, relationText);
                        if (ret != DBERR_OK) {
                            logger::log_error(ret, "Forward for equal MBRs stopped with error.");
                        }
                        return ret;
                    }
                }
            }
        }
        // not equal MBRs, check other relations
        if (d_xmin <= 0) {
            if (d_xmax >= 0) {
                if (d_ymin <= 0) {
                    if (d_ymax >= 0) {
                        // MBR(s) inside MBR(r)
                        ret = refinement::computeRelationTexts(objR, objS, MBR_S_IN_R, relationText);
                        if (ret != DBERR_OK) {
                            logger::log_error(ret, "Forward for MBR(s) inside MBR(r) stopped with error.");
                        }
                        return ret;
                    }
                } else {
                    if (d_ymax < 0 && d_xmax > 0 && d_xmin < 0 && d_ymin < 0) {
                        // MBRs cross each other
                        ret = refinement::computeRelationTexts(objR, objS, MBR_CROSS, relationText);
                        if (ret != DBERR_OK) {
                            logger::log_error(ret, "Forward for MBRs cross stopped with error.");
                        }
                        return ret;
                    }
                }
            }
        } 
        if (d_xmin >= 0) {
            if (d_xmax <= 0) {
                if (d_ymin >= 0) {
                    if (d_ymax <= 0) {
                        // MBR(r) inside MBR(s)
                        ret = refinement::computeRelationTexts(objR, objS, MBR_R_IN_S, relationText);
                        if (ret != DBERR_OK) {
                            logger::log_error(ret, "Forward for MBR(r) inside MBR(s) stopped with error.");
                        }
                        return ret;
                    }
                } else {
                    if (d_ymax > 0 && d_xmax < 0 && d_xmin > 0 && d_ymin > 0) {
                        // MBRs cross each other
                        ret = refinement::computeRelationTexts(objR, objS, MBR_CROSS, relationText);
                        if (ret != DBERR_OK) {
                            logger::log_error(ret, "Forward for MBRs cross stopped with error.");
                        }
                        return ret;
                    }
                }
            }
        }
        // MBRs intersect generally
        ret = refinement::computeRelationTexts(objR, objS, MBR_INTERSECT, relationText);
        if (ret != DBERR_OK) {
            logger::log_error(ret, "Forward for MBRs intersect stopped with error.");
        }
        return ret;
    }
   
    static inline DB_STATUS relate(Shape* r, Shape* s, std::string &relationText) {
        DB_STATUS ret = DBERR_OK;
        if ((r->mbr.pMin.x > s->mbr.pMax.x) || (r->mbr.pMax.x < s->mbr.pMin.x)) {
            // disjoint, only compute cardinal direction
            CardinalDirection direction = CD_NONE;
            ret = refinement::computeCardinalDirectionBetweenShapes(r, s, direction);
            if (ret != DBERR_OK) {
                logger::log_error(ret, "Error while computing the cardinal direction between objects with ids", r->recID, "and", s->recID);
                return ret;
            }
            if (direction != CD_NONE) {
                // append cardinal direction to the relation text
                relationText = r->name + " is " + mapping::cardinalDirectionIntToString(direction) + " of " + s->name + ". ";
            }
            // logger::log_success("Generated relation for ids", r->recID, s->recID, ":", relationText);
        } else {
            // intersecting MBRs, relate more specifically
            ret = relateMBRs(r, s, relationText);
            if (ret != DBERR_OK) {
                return ret;
            }
        }
        return ret;
    }
    
    static inline DB_STATUS joinObjects(int tid, int partitionID, std::vector<Shape*>* objectsR, std::vector<Shape*>* objectsS) {
        DB_STATUS ret = DBERR_OK;
        std::string relationText = "";
        if (objectsR == nullptr || objectsS == nullptr) {
            return ret;
        }
        if (objectsR->size() == 0 || objectsS->size() == 0) {
            return ret;
        }
        auto r = objectsR->begin();
        auto lastR = objectsR->end();
        while (r != lastR) {
            auto s = objectsS->begin();
            auto lastS = objectsS->end();
            while (s != lastS) {
                // check if the objects' bottom left CMBR point is inside this partition
                // (reference point duplicate elimination)
                double cxmin = std::max((*r)->mbr.pMin.x, (*s)->mbr.pMin.x);
                double cymin = std::max((*r)->mbr.pMin.y, (*s)->mbr.pMin.y);
                int partitionX = (cxmin - g_config.datasetMetadata.dataspaceMetadata.xMinGlobal) / (g_config.datasetMetadata.dataspaceMetadata.xExtent / (double) g_config.indexConfig.partitionsPerDim);
                int partitionY = (cymin - g_config.datasetMetadata.dataspaceMetadata.yMinGlobal) / (g_config.datasetMetadata.dataspaceMetadata.yExtent / (double) g_config.indexConfig.partitionsPerDim);
                int cmbrPartitionID = getPartitionID(partitionX, partitionY, g_config.indexConfig.partitionsPerDim);
                if (cmbrPartitionID == partitionID) {
                    // relate objects
                    ret = relate(*r, *s, relationText);
                    if (ret != DBERR_OK) {
                        return ret;
                    }
                    // save the generated relation text in a buffer
                    g_config.diskWriter.addString(relationText, tid);
                }
                s++;
            }
            r++;
        }
        return ret;
    }
    
    DB_STATUS evaluate(Dataset* R, Dataset* S) {
        DB_STATUS ret = DBERR_OK;
        int tid = -1;
        // here the final results will be stored
        logger::log_task("Evaluating...");
        #pragma omp parallel num_threads(g_config.getNumThreads()) private(tid)
        {
            tid = omp_get_thread_num();
            DB_STATUS local_ret = DBERR_OK;
            // loop common partitions (todo: optimize to start from the dataset that has the fewer ones)
            #pragma omp for
            for (int i=0; i<R->uniformGridIndex.partitions.size(); i++) {
                // get partition ID and S container
                int partitionID = R->uniformGridIndex.partitions[i].partitionID;
                Partition* tlContainerS = S->uniformGridIndex.getPartition(partitionID);
                // if relation S has any objects for this partition (non-empty container)
                if (tlContainerS != nullptr) {
                    // common partition found
                    Partition* tlContainerR = &R->uniformGridIndex.partitions[i];
                    local_ret = joinObjects(tid, partitionID, tlContainerR->getContents(), tlContainerS->getContents());
                    if (local_ret != DBERR_OK) {
                        #pragma omp cancel for
                        ret = local_ret;
                        logger::log_error(ret, "Join failed for partition", partitionID);
                    }
                }
            }
        }
        // write header and rules (dont use this in multi-dataset runs as it will be written multiple times)
        // ret = g_config.diskWriter.writeFixedRules();
        // if (ret != DBERR_OK) {
        //     logger::log_error(ret, "Error writing header and rules to output.");
        //     return ret;
        // }
        // write buffers to disk
        ret = g_config.diskWriter.writeBuffers();
        if (ret != DBERR_OK) {
            logger::log_error(ret, "Error writing buffers to output.");
            return ret;
        }
        g_config.diskWriter.closeOutputFilestream();

        return ret;
    }
    
}

