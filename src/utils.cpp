#include "utils.h"


std::string getFileExtension(const std::string& filePath) {
    size_t dotPos = filePath.find_last_of('.');
    
    // If no dot is found or it's at the end, return an empty string
    if (dotPos == std::string::npos || dotPos == filePath.length() - 1) {
        return "";
    }

    return filePath.substr(dotPos + 1); // Return the part after the dot
}

bool verifyFilepath(std::string filePath) {
    if (FILE *file = fopen(filePath.c_str(), "r")) {
        fclose(file);
        return true;
    }
    return false;
}

bool verifyDirectory(std::string directoryPath) {
    //check if APRIL dirs exists
    DIR* dir = opendir(directoryPath.c_str());
    if(dir) {
        /* Directory exists. */
        closedir(dir);
        return true;
    }else if(ENOENT == errno) {
        /* Directory does not exist. */
        return false;
    }else{
        /* opendir() failed for some other reason. */
        return false;
    }
}

/** @brief Converts a degree area into a sq km area */
double convertDegreesToSquareKilometers(double areaInDegrees, double lat) {
    // Earth's radius squared in square kilometers
    // double earthRadiusSquared = EARTH_RADIUS * EARTH_RADIUS;

    // Conversion factor from square degrees to square kilometers
    // 1 degree latitude = R * pi / 180 km
    // double conversionFactor = (DEG_TO_RAD * EARTH_RADIUS) * (DEG_TO_RAD * EARTH_RADIUS);
    // return areaInDegrees * conversionFactor;

    // test formula. The proper way would be to adjust each lon/lat coordinate while adding them to the area.
    // not just adjust the area in degrees
    double areaInSqKm = areaInDegrees * 111.32 * 111.32 * abs(cos(lat));
    // round to 2 decimal places
    return std::round(areaInSqKm * 100.0) / 100.0;
}



namespace mapping
{
    std::string dataTypeIntToStr(DataType val){
        switch(val) {
            case DT_POLYGON: return "POLYGON";
            case DT_RECTANGLE: return "RECTANGLE";
            case DT_POINT: return "POINT";
            case DT_LINESTRING: return "LINESTRING";
            case DT_MULTIPOLYGON: return "MULTIPOLYGON";
            default: return "";
        }
    }

    DataType dataTypeTextToInt(std::string str){
        if (str.compare("POLYGON") == 0) return DT_POLYGON;
        else if (str.compare("RECTANGLE") == 0) return DT_RECTANGLE;
        else if (str.compare("POINT") == 0) return DT_POINT;
        else if (str.compare("LINESTRING") == 0) return DT_LINESTRING;
        else if (str.compare("MULTIPOLYGON") == 0) return DT_MULTIPOLYGON;

        logger::log_error(DBERR_INVALID_DATATYPE, "Invalid data type matching for string:", str);
        return DT_INVALID;
    }

    FileFormat fileFormatTextToInt(std::string str) {
        if (str.compare("DAT") == 0) return FT_BINARY;
        else if (str.compare("CSV") == 0) return FT_CSV;
        else if (str.compare("WKT") == 0) return FT_WKT;
        else if (str.compare("TSV") == 0) return FT_WKT;    // special case, tsv file but the first column is the wkt data

        return FT_INVALID;
    }

    std::string cardinalDirectionIntToString(CardinalDirection val){
        switch(val) {
            case CD_NORTH: return "north";
            case CD_SOUTH: return "south";
            case CD_WEST: return "west";
            case CD_EAST: return "east";
            case CD_NORTHWEST: return "northwest";
            case CD_NORTHEAST: return "northeast";
            case CD_SOUTHWEST: return "southwest";
            case CD_SOUTHEAST: return "southeast";
            default: return "";
        }
    }

    std::string relationIntToStr(TopologyRelation relation) {
        switch(relation) {
            case TR_INTERSECT: return "intersects with";
            case TR_CONTAINS: return "contains";
            case TR_DISJOINT: return "is disjoint with";
            case TR_EQUAL: return "is equal with";
            case TR_COVERS: return "covers";
            case TR_MEET: return "is adjacent to";
            case TR_COVERED_BY: return "is covered by";
            case TR_INSIDE: return "is inside of";
            default: return "";
        }
    }

    std::string documentTypeIntToStr(DocumentType docType) {
        switch(docType) {
            case DOC_PARAGRAPHS: return "PARAGRAPHS";
            case DOC_PARAGRAPHS_COMPRESSED: return "PARAGRAPHS_COMPRESSED";
            case DOC_SENTENCES: return "SENTENCES";
            default: return "";
        }
    }

    DocumentType documentTypeTextToInt(std::string str) {
        if (str.compare("PARAGRAPHS") == 0) return DOC_PARAGRAPHS;
        else if (str.compare("SENTENCES") == 0) return DOC_SENTENCES;
        else if (str.compare("PARAGRAPHS_COMPRESSED") == 0) return DOC_PARAGRAPHS_COMPRESSED;

        return DOC_INVALID;
    }
}

namespace text_generator
{   
    std::string generateDirectionalRelation(std::string &entityNameR, std::string &entityNameS, CardinalDirection direction) {
        std::string directionText = mapping::cardinalDirectionIntToString(direction);
        if (directionText == "") {
            // don't generate a relation, empty direction
            return "";
        } else {
            return entityNameR + " is " + directionText + " of " + entityNameS;
        }
    }

    std::string generateTopologicalRelation(std::string &entityNameR, std::string &entityNameS, TopologyRelation relation) {
        std::string relationText = mapping::relationIntToStr(relation);
        if (relationText == "") {
            // don't generate a relation
            return "";
        } else {
            return entityNameR + " " + relationText + " " + entityNameS + ". ";
        }
    }

    std::string generateCombinedTopologicalRelation(std::string &entityNameR, std::string &entityNameS, TopologyRelation relation, CardinalDirection direction, std::string area) {
        std::string relationText = mapping::relationIntToStr(relation);
        std::string returnText = "";
        if (relationText == "") {
            // don't generate a relation
            return returnText;
        } else {
            returnText = entityNameR + " " + relationText;
            if (direction != CD_NONE) {
                std::string directionText = mapping::cardinalDirectionIntToString(direction);
                // generate direction text
                returnText += " and " + directionText + " of " + entityNameS;
            } else {
                returnText += " " + entityNameS;
            }

            if (area != "") {
                // append the area
                returnText += ", and they have " + area + " square km of area in common";
            }

            returnText += ". ";
        }

        return returnText;
    }

    std::string generateAreaInSqkm(std::string &entityNameR, std::string &entityNameS, double area) {
        if (area < EPS) {
            return "";
        } else {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(2) << area;
            return entityNameR + " and " + entityNameS + " have approximately " + stream.str() + " square kilometers of common area. ";
        }
    }
}



CardinalDirection getCardinalDirection(double angle) {
    if (angle >= 337.5 || angle < 22.5) {
        return CD_EAST;
    } else if (angle >= 22.5 && angle < 67.5) {
        return CD_NORTHEAST;
    } else if (angle >= 67.5 && angle < 112.5) {
        return CD_NORTH;
    } else if (angle >= 112.5 && angle < 157.5) {
        return CD_NORTHWEST;
    } else if (angle >= 157.5 && angle < 202.5) {
        return CD_WEST;
    } else if (angle >= 202.5 && angle < 247.5) {
        return CD_SOUTHWEST;
    } else if (angle >= 247.5 && angle < 292.5) {
        return CD_SOUTH;
    } else if (angle >= 292.5 && angle < 337.5) {
        return CD_SOUTHEAST;
    }
    return CD_NONE;
}

CardinalDirection getOppositeCardinalDirection(CardinalDirection direction) {
    switch (direction) {
        case CD_EAST:
            return CD_WEST;
        case CD_WEST:
            return CD_EAST;
        case CD_NORTH:
            return CD_SOUTH;
        case CD_SOUTH:
            return CD_NORTH;
        case CD_NORTHEAST:
            return CD_SOUTHWEST;
        case CD_NORTHWEST:
            return CD_SOUTHEAST;
        case CD_SOUTHEAST:
            return CD_NORTHWEST;
        case CD_SOUTHWEST:
            return CD_NORTHEAST;    
    }
    return CD_NONE;
}

TopologyRelation getSwappedTopologyRelation(TopologyRelation relation) {
    switch (relation) {
        case TR_INSIDE:
            return TR_CONTAINS;
        case TR_CONTAINS:
            return TR_INSIDE;
        case TR_COVERS:
            return TR_COVERED_BY;
        case TR_COVERED_BY:
            return TR_COVERS;
        case TR_MEET:
        case TR_EQUAL:
        case TR_INTERSECT:
        case TR_DISJOINT:
            return relation;
    }
    return TR_INVALID;
}
