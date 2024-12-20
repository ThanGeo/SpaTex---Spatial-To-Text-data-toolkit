#include "index/refinement.h"

namespace refinement
{
    /**
     * Boost geometry refinement for find relation
     */
    static inline bool compareDe9imChars(char character, char char_mask) {
        if (character != 'F' && char_mask == 'T') {
            // character is 0,1,2 and char_mask is T
            return true;
        } else if (character == 'F' && char_mask == 'F'){
            // both are F
            return true;
        } else {
            // no match
            return false;
        }
    }

    static inline bool compareMasks(std::string &de9imCode, char* maskCode) {
        for(int i=0; i<9; i++) {
            if (de9imCode[i] == '*' || maskCode[i] == '*' || compareDe9imChars(de9imCode[i], maskCode[i])){
                continue;
            } else {
                return false;
            }
        }
        return true;
    }

    static TopologyRelation refineDisjointInsideCoveredbyMeetIntersect(Shape* objR, Shape* objS) {
        // get the mask code
        std::string code = objR->createMaskCode(*objS);
        // disjoint
        if (compareMasks(code, disjointCode)) {
            return TR_DISJOINT;
        }
        // covered by
        if (compareMasks(code, coveredbyCode1) || compareMasks(code, coveredbyCode2) || 
            compareMasks(code, coveredbyCode3) || compareMasks(code, coveredbyCode4)) {
            // inside
            if (compareMasks(code, insideCode)) {
                return TR_INSIDE;
            }
            return TR_COVERED_BY;
        }
        // meet
        if (compareMasks(code, meetCode1) || compareMasks(code, meetCode2) || compareMasks(code, meetCode3)) {
            return TR_MEET;
        }
        // intersect
        return TR_INTERSECT;
    }

    static TopologyRelation refineDisjointContainsCoversMeetIntersect(Shape* objR, Shape* objS) {
        // get the mask code
        std::string code = objR->createMaskCode(*objS);
        // disjoint
        if (compareMasks(code, disjointCode)) {
            return TR_DISJOINT;
        }
        // covers
        if (compareMasks(code, coversCode1) || compareMasks(code, coversCode2) || 
            compareMasks(code, coversCode3) || compareMasks(code, coversCode4)) {
            // contains
            if (compareMasks(code, containsCode)) {
                return TR_CONTAINS;
            }
            return TR_COVERS;
        }
        // meet
        if (compareMasks(code, meetCode1) || compareMasks(code, meetCode2) || compareMasks(code, meetCode3)) {
            return TR_MEET;
        }
        // intersect
        return TR_INTERSECT;
    }

    static TopologyRelation refineEqualCoversCoveredbyTrueHitIntersect(Shape* objR, Shape* objS) {
        // get the mask code
        std::string code = objR->createMaskCode(*objS);
        // check equality first because it is a subset of covers and covered by
        if(compareMasks(code, equalCode)){
            return TR_EQUAL;
        }
        // covers
        if (compareMasks(code, coversCode1) || compareMasks(code, coversCode2) || 
            compareMasks(code, coversCode3) || compareMasks(code, coversCode4)) {
            // return TR_COVERS;
            // instead of covers, classify as contains for consistency with the DE-9IM
            return TR_CONTAINS;
        }
        // covered by
        if (compareMasks(code, coveredbyCode1) || compareMasks(code, coveredbyCode2) || 
            compareMasks(code, coveredbyCode3) || compareMasks(code, coveredbyCode4)) {
            // return TR_COVERED_BY;
            // instead of covers, classify as contains for consistency with the DE-9IM
            return TR_INSIDE;
        }
        // intersect
        return TR_INTERSECT;
    }

    static TopologyRelation refineDisjointMeetIntersect(Shape* objR, Shape* objS) {
        // get the mask code
        std::string code = objR->createMaskCode(*objS);   
        // disjoint
        if (compareMasks(code, disjointCode)) {
            return TR_DISJOINT;
        }
        // meet
        if (compareMasks(code, meetCode1) || compareMasks(code, meetCode2) || compareMasks(code, meetCode3)) {
            return TR_MEET;
        }
        // intersect
        return TR_INTERSECT;
    }

    DB_STATUS computeCardinalDirectionBetweenShapes(Shape* objR, Shape* objS, CardinalDirection &direction) {
        DB_STATUS ret = DBERR_OK;

        // compute centroids
        bg_point_xy centroidR = objR->getCentroid();
        bg_point_xy centroidS = objS->getCentroid();

        bg_linestring centroidVector;
        centroidVector.push_back(centroidR);
        centroidVector.push_back(centroidS);
        double dx = centroidR.x() - centroidS.x();
        double dy = centroidR.y() - centroidS.y();
        double angle = std::atan2(dy, dx) * 180.0 / M_PI;
        // Normalize the angle to be between 0 and 360
        if (angle < 0) {
            angle += 360.0;
        }
        // get direction based on angle degrees
        direction = getCardinalDirection(angle);

        return ret;
    }

    static DB_STATUS computeAreaText(Shape* objR, Shape* objS, TopologyRelation relation, std::string &areaText) {
        DB_STATUS ret = DBERR_OK;
        std::stringstream stream;
        switch (relation) {
            case TR_DISJOINT:
            case TR_MEET:
                // disjoint or meet, no common area
                areaText = "0";
                break;
            case TR_EQUAL:
                // ignore area for equality
                areaText = "0";
                break;
            case TR_CONTAINS:
            case TR_COVERS:
                // common area is the area of objS, since its being covered by R or is equal to S
                stream << std::fixed << std::setprecision(2) << objS->getArea();
                areaText = stream.str();
                break;
            case TR_INSIDE:
            case TR_COVERED_BY:
                // common area is the area of objR, since its being covered by S
                stream << std::fixed << std::setprecision(2) << objR->getArea();
                areaText = stream.str();
                break;
            case TR_INTERSECT:
                // actually compute the intersection area
                stream << std::fixed << std::setprecision(2) << objR->getIntersectionArea(*objS);
                areaText = stream.str();
                break;
            default:
                logger::log_error(DBERR_INVALID_PARAMETER, "Invalid topological relation with code:", relation);
                return DBERR_INVALID_PARAMETER;
        }

        return ret;
    }

    static DB_STATUS computeIntersection(Shape* objR, Shape* objS, TopologyRelation relation, std::string &intersectionText) {
        DB_STATUS ret = DBERR_OK;
        switch (relation) {
            case TR_DISJOINT:
            case TR_MEET:
                // disjoint or meet, no common area
                intersectionText = text_generator::generateAreaInSqkm(objR->name, objS->name, 0);
                break;
            case TR_CONTAINS:
            case TR_COVERS:
            case TR_EQUAL:
                // common area is the area of objS, since its being covered by R or is equal to S
                intersectionText = text_generator::generateAreaInSqkm(objR->name, objS->name, objS->getArea());
                break;
            case TR_INSIDE:
            case TR_COVERED_BY:
                // common area is the area of objR, since its being covered by S
                intersectionText = text_generator::generateAreaInSqkm(objR->name, objS->name, objR->getArea());
                break;
            case TR_INTERSECT:
                // actually compute the intersection area
                intersectionText = text_generator::generateAreaInSqkm(objR->name, objS->name, objR->getIntersectionArea(*objS));
                break;
            default:
                logger::log_error(DBERR_INVALID_PARAMETER, "Invalid topological relation with code:", relation);
                return DBERR_INVALID_PARAMETER;
        }

        return ret;
    }

    namespace sentences
    {
        DB_STATUS computeRelations(Shape* objR, Shape* objS, MBRRelationCase mbrRelationCase, std::string &relationText) {
            DB_STATUS ret = DBERR_OK;
            TopologyRelation relation = TR_INVALID;
            // switch based on MBR intersection case
            switch(mbrRelationCase) {
                case MBR_R_IN_S:
                    relation = refineDisjointInsideCoveredbyMeetIntersect(objR, objS);
                    break;
                case MBR_S_IN_R:
                    relation = refineDisjointContainsCoversMeetIntersect(objR, objS);
                    break;
                case MBR_EQUAL:
                    relation = refineEqualCoversCoveredbyTrueHitIntersect(objR, objS);
                    break;
                case MBR_INTERSECT:
                    relation = refineDisjointMeetIntersect(objR, objS);
                    break;
                case MBR_CROSS:
                    relation = TR_INTERSECT;
                    break;
                default:
                    logger::log_error(DBERR_INVALID_PARAMETER, "Invalid mbr relation case:", mbrRelationCase);
                    return DBERR_INVALID_PARAMETER;
            }

            // use refinement result to generate the topological relation
            relationText = text_generator::generateTopologicalRelation(objR->name, objS->name, relation);
            // special case, in adjacency also compute the cardinal direction if possible
            if (relationText != "" && (relation == TR_MEET || relation == TR_DISJOINT)) {
                CardinalDirection direction = CD_NONE;
                ret = computeCardinalDirectionBetweenShapes(objR, objS, direction);
                if (ret != DBERR_OK) {
                    logger::log_error(ret, "Error while computing the cardinal direction between objects with ids", objR->recID, "and", objS->recID);
                    return ret;
                }
                if (direction != CD_NONE) {
                    // append cardinal direction to the relation text
                    relationText += objR->name + " is " + mapping::cardinalDirectionIntToString(direction) + " of " + objS->name + ". ";
                }
            }
            // compute intersection
            std::string intersectionText = "";
            ret = computeIntersection(objR, objS, relation, intersectionText);
            if (ret != DBERR_OK) {
                logger::log_error(ret, "Error while computing the intersection area between objects with ids", objR->recID, "and", objS->recID);
                return ret;
            }
            // append intersection text
            relationText += intersectionText;

            return ret;
        }
    }

    namespace paragraphs
    {
        static DB_STATUS generateUncompressedRelationsText(Shape* objR, Shape* objS, TopologyRelation relation) {
            DB_STATUS ret = DBERR_OK;
            // generate the topological relation
            g_config.diskWriter.appendTextForEntity(objR->name, text_generator::generateTopologicalRelation(objR->name, objS->name, relation));
            if (!g_config.datasetMetadata.getSelfJoin()) {
                // if not a self join, get the reverse relation too
                g_config.diskWriter.appendTextForEntity(objS->name, text_generator::generateTopologicalRelation(objS->name, objR->name, getSwappedTopologyRelation(relation)));
            }
            // special case, in adjacency also compute the cardinal direction if possible
            if (relation == TR_MEET || relation == TR_DISJOINT) {
                CardinalDirection direction = CD_NONE;
                ret = computeCardinalDirectionBetweenShapes(objR, objS, direction);
                if (ret != DBERR_OK) {
                    logger::log_error(ret, "Error while computing the cardinal direction between objects with ids", objR->recID, "and", objS->recID);
                    return ret;
                }
                if (direction != CD_NONE) {
                    // append cardinal direction for the entities
                    g_config.diskWriter.appendTextForEntity(objR->name, objR->name + " is " + mapping::cardinalDirectionIntToString(direction) + " of " + objS->name + ". ");
                    if (!g_config.datasetMetadata.getSelfJoin()) {
                        // if not a self join, get the reverse relation too
                        g_config.diskWriter.appendTextForEntity(objS->name, objS->name + " is " + mapping::cardinalDirectionIntToString(getOppositeCardinalDirection(direction)) + " of " + objR->name + ". ");
                    }
                }
            }
            // compute intersection
            std::string intersectionText = "";
            ret = computeIntersection(objR, objS, relation, intersectionText);
            if (ret != DBERR_OK) {
                logger::log_error(ret, "Error while computing the intersection area between objects with ids", objR->recID, "and", objS->recID);
                return ret;
            }
            // append intersection text
            g_config.diskWriter.appendTextForEntity(objR->name, intersectionText);
            if (!g_config.datasetMetadata.getSelfJoin()) {
                // if not a self join, get the reverse relation too
                g_config.diskWriter.appendTextForEntity(objS->name, intersectionText);
            }
            return ret;
        }

        static DB_STATUS generateCompressedRelationsText(Shape* objR, Shape* objS, TopologyRelation relation) {
            DB_STATUS ret = DBERR_OK;
            CardinalDirection direction = CD_NONE;
            std::string intersectionText = "";

            // generate and append relations text for object R
            if (g_config.datasetMetadata.getSelfJoin() && relation == TR_EQUAL) {
                // ignore joining the same objects in self-joins
                return DBERR_OK;
            }

            // special case, in adjacency or disjointment also compute the cardinal direction if possible
            if (relation == TR_MEET || relation == TR_DISJOINT) {
                ret = computeCardinalDirectionBetweenShapes(objR, objS, direction);
                if (ret != DBERR_OK) {
                    logger::log_error(ret, "Error while computing the cardinal direction between objects with ids", objR->recID, "and", objS->recID);
                    return ret;
                }
            } else {
                // compute intersection area
                ret = computeAreaText(objR, objS, relation, intersectionText);
                if (ret != DBERR_OK) {
                    logger::log_error(ret, "Error while computing the intersection area between objects with ids", objR->recID, "and", objS->recID);
                    return ret;
                }
            }

            // get relations text
            std::string relationsText = text_generator::generateCombinedTopologicalRelation(objR->name, objS->name, relation, direction, intersectionText);
            g_config.diskWriter.appendTextForEntity(objR->name, relationsText);

            // if not a self-join, compute the reverse relation text for object S 
            if (!g_config.datasetMetadata.getSelfJoin()) {
                TopologyRelation reverseRelation = getSwappedTopologyRelation(relation);
                CardinalDirection reverseDirection = getOppositeCardinalDirection(direction);
                std::string reverseRelationText = text_generator::generateCombinedTopologicalRelation(objS->name, objR->name, reverseRelation, reverseDirection, intersectionText);
                g_config.diskWriter.appendTextForEntity(objS->name, reverseRelationText);
            }

            return ret;
        }

        DB_STATUS computeRelations(Shape* objR, Shape* objS, MBRRelationCase mbrRelationCase, DocumentType docType) {
            DB_STATUS ret = DBERR_OK;
            TopologyRelation relation = TR_INVALID;
            // switch based on MBR intersection case
            switch(mbrRelationCase) {
                case MBR_R_IN_S:
                    relation = refineDisjointInsideCoveredbyMeetIntersect(objR, objS);
                    break;
                case MBR_S_IN_R:
                    relation = refineDisjointContainsCoversMeetIntersect(objR, objS);
                    break;
                case MBR_EQUAL:
                    relation = refineEqualCoversCoveredbyTrueHitIntersect(objR, objS);
                    break;
                case MBR_INTERSECT:
                    relation = refineDisjointMeetIntersect(objR, objS);
                    break;
                case MBR_CROSS:
                    relation = TR_INTERSECT;
                    break;
                default:
                    logger::log_error(DBERR_INVALID_PARAMETER, "Invalid mbr relation case:", mbrRelationCase);
                    return DBERR_INVALID_PARAMETER;
            }

            // generate the topological relation
            if (docType == DOC_PARAGRAPHS){
                ret =  generateUncompressedRelationsText(objR, objS, relation);
                if (ret != DBERR_OK) {
                    logger::log_error(ret, "Failed when generated the uncompressed relations text.");
                    return ret;
                }    
            } else if (docType == DOC_PARAGRAPHS_COMPRESSED) {
                ret =  generateCompressedRelationsText(objR, objS, relation);
                if (ret != DBERR_OK) {
                    logger::log_error(ret, "Failed when generated the uncompressed relations text.");
                    return ret;
                }                
            } else {
                logger::log_error(DBERR_INVALID_PARAMETER, "Invalid document type option for generating relations text:", docType);
                return DBERR_INVALID_PARAMETER;
            }

            return ret;
        }

        
    }
}