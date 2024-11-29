#ifndef DEF_H
#define DEF_H

#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <math.h>
#include <omp.h>

#define RED "\e[0;31m"
#define GREEN "\e[0;32m"
#define YELLOW "\e[0;33m"
#define BLUE "\e[0;34m"
#define PURPLE "\e[0;35m"
#define ORANGE "\e[38;5;208m"
#define NC "\e[0m"

#define EPS 1e-08   // error margin
#define EARTH_RADIUS 6371    // Earth's radius in kilometers
extern const double DEG_TO_RAD;

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/box.hpp>

/** @typedef bg_point_xy @brief boost geometry point definition. */
typedef boost::geometry::model::d2::point_xy<double> bg_point_xy;
/** @typedef bg_linestring @brief boost geometry linestring definition. */
typedef boost::geometry::model::linestring<bg_point_xy> bg_linestring;
/** @typedef bg_rectangle @brief boost geometry rectangle definition. */
typedef boost::geometry::model::box<bg_point_xy> bg_rectangle;
/** @typedef bg_polygon @brief boost geometry polygon definition. */
typedef boost::geometry::model::polygon<bg_point_xy> bg_polygon;
/** @typedef bg_multi_polygon @brief boost geometry multipolygon definition. */
typedef boost::geometry::model::multi_polygon<bg_polygon> bg_multi_polygon;

#define DBBASE 100000
/** @brief Flags/states for status reporting. 
 * 
 * All functions should return a DB_STATUS value. After each call, there should always be a check whether 
 * the returned value is DBERR_OK. If not, the error should propagate and the program should terminate safely.
 * 
 */
typedef enum DB_STATUS {
    DBERR_OK = DBBASE + 0,
    DB_FIN,

    DBERR_INVALID_ARGS = DBBASE + 1000,
    DBERR_INVALID_OPERATION = DBBASE + 1001,
    DBERR_INVALID_GEOMETRY = DBBASE + 1002,
    DBERR_OUT_OF_BOUNDS = DBBASE + 1003,
    DBERR_INVALID_KEY = DBBASE + 1004,
    DBERR_INVALID_PARAMETER = DBBASE + 1005,
    DBERR_INVALID_DATATYPE = DBBASE + 1006,
    DBERR_INVALID_FILEPATH = DBBASE + 1007,
    DBERR_FILE_OPEN = DBBASE + 1008,
    DBERR_INI_ERROR = DBBASE + 1009,
    DBERR_INVALID_PARTITION = DBBASE + 1010,
    DBERR_FILE_WRITE = DBBASE + 1011,
    DBERR_INVALID_DOC_TYPE = DBBASE + 1012,
} DB_STATUS;

/** @enum FileFormat @brief Data file types. */
enum FileFormat {
    FT_INVALID,
    FT_BINARY,
    FT_CSV,
    FT_WKT,
};

/** @enum DataType @brief Spatial data types. */
enum DataType{
    DT_INVALID,
    DT_POINT,
    DT_LINESTRING,
    DT_RECTANGLE,
    DT_POLYGON,
    DT_MULTIPOLYGON,
};

enum DatasetIndex {
    DATASET_R,
    DATASET_S,
};

enum MBRRelationCase {
    MBR_R_IN_S,
    MBR_S_IN_R,
    MBR_EQUAL,
    MBR_CROSS,
    MBR_INTERSECT,
};

enum CardinalDirection {
    CD_NORTH,
    CD_SOUTH,
    CD_EAST,
    CD_WEST,
    CD_NORTHWEST,
    CD_NORTHEAST,
    CD_SOUTHWEST,
    CD_SOUTHEAST,
    CD_NONE = 777,
};

enum TopologyRelation {
    TR_DISJOINT,
    TR_EQUAL,
    TR_INSIDE,
    TR_CONTAINS,
    TR_MEET,
    TR_COVERS,
    TR_COVERED_BY,
    TR_INTERSECT,
    TR_INVALID = 777,
};

enum DocumentType {
    DOC_SENTENCES,
    DOC_PARAGRAPHS,
    DOC_PARAGRAPHS_COMPRESSED,
    DOC_INVALID = 777,
};

#endif