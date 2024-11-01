#ifndef CONTAINERS_H
#define CONTAINERS_H

#include <variant>
#include <any>
#include <fstream>

#include "def.h"
#include "utils.h"

struct DatasetStatement
{
    bool set = false;
    std::string path = "";
    FileFormat fileFormat = FT_INVALID;
    std::string nickname = "";
    std::string key = "";
    std::string description = "";
    int wktColIdx = -1;
    int nameColIdx = -1;
};

struct OutputStatement
{
    bool append = false;
    std::string outputFilepath;
    std::string documentType;
};

struct ArgumentsStatement
{
    DatasetStatement datasetR;
    DatasetStatement datasetS;
    OutputStatement outputStmt;
};

/**
 * @brief Struct for 2-dimension points with double coordinates x and y (lon, lat).
 */
struct Point {
    double x, y;
    Point(double xVal, double yVal) : x(xVal), y(yVal) {}
};

/**
 * @brief Struct for Minimum Bounding Rectangles (MBR).
 * 
 * It holds the MBR's bottom-left (pMin) and top-right (pMax) points.
 */
struct MBR {
    Point pMin;
    Point pMax;

    MBR() : pMin(Point(std::numeric_limits<int>::max(), std::numeric_limits<int>::max())), pMax(Point(-std::numeric_limits<int>::max(), -std::numeric_limits<int>::max())) {}
};

/**
 * @brief Wrapper class for the Geometry objects.
 * 
 * Contains only the definitions of the functions so that the derived geometry classes can inherit and overload them.
 * @warning Never should anyone define an object of this type and try to utilize it (undefined behaviour)
 */
template<typename GeometryType>
struct GeometryWrapper {
public:
    /**
    @brief Template field geometry will be set by the derived classes to the appropriate Boost Geometry type 
     * that represents the the derived class.
     */
    GeometryType geometry;

    GeometryWrapper() {}
    explicit GeometryWrapper(const GeometryType& geom) : geometry(geom) {}

    /** @brief Sets the boost geometry object. */
    void setGeometry(const GeometryType& geom) {
        geometry = geom;
    }

    bg_rectangle getBoostEnvelope(bg_rectangle &envelope) {
        logger::log_error(DBERR_INVALID_OPERATION, "Geometry wrapper can be accessed directly for operation: getBoostEnvelope");
        bg_rectangle empty;
        return empty;
    }

    DB_STATUS setFromWKT(std::string &wktText) {
        logger::log_error(DBERR_INVALID_OPERATION, "Geometry wrapper can be accessed directly for operation: setFromWKT");
        return DBERR_INVALID_OPERATION;
    }

    void reset() {
        logger::log_error(DBERR_INVALID_OPERATION, "Geometry wrapper can be accessed directly for operation: reset");
    }

    void getCentroid() {
        logger::log_error(DBERR_INVALID_OPERATION, "Geometry wrapper can be accessed directly for operation: getCentroid");
    }

    void addPoint(const double x, const double y) {
        logger::log_error(DBERR_INVALID_OPERATION, "Geometry wrapper can be accessed directly for operation: addPoint");
    }

    bool pipTest(const bg_point_xy &point) const {
        logger::log_error(DBERR_INVALID_OPERATION, "Geometry wrapper can be accessed directly for operation: pipTest");
        return false;
    }

    void printGeometry() {
        logger::log_error(DBERR_INVALID_OPERATION, "Geometry wrapper can be accessed directly for operation: printGeometry");
    }

    void correctGeometry() {
        logger::log_error(DBERR_INVALID_OPERATION, "Geometry wrapper can be accessed directly for operation: correctGeometry");
    }

    double getArea() {
        logger::log_error(DBERR_INVALID_OPERATION, "Geometry wrapper can be accessed directly for operation: getArea");
    }

    template<typename OtherGeometryType>
    double getIntersectionArea(const GeometryWrapper<OtherGeometryType> &other) const {
        logger::log_error(DBERR_INVALID_OPERATION, "Geometry wrapper can be accessed directly for operation: getIntersectionArea");
        return 0.0f;
    }

    void modifyBoostPointByIndex(int index, double x, double y) {
        logger::log_error(DBERR_INVALID_OPERATION, "Geometry wrapper can be accessed directly for operation: modifyBoostPointByIndex");
    }

    const std::vector<bg_point_xy>* getReferenceToPoints() const {
        logger::log_error(DBERR_INVALID_OPERATION, "Geometry wrapper can be accessed directly for operation: getReferenceToPoints.");
        return nullptr;
    }

    int getVertexCount() const {
        logger::log_error(DBERR_INVALID_OPERATION, "Geometry wrapper can be accessed directly for operation: getVertexCount.");
        return 0;
    }

    template<typename OtherGeometryType>
    std::string createMaskCode(const GeometryWrapper<OtherGeometryType> &other) const {
        logger::log_error(DBERR_INVALID_OPERATION, "createMaskCode unsupported for the invoked shapes.");
        return "";
    }

    template<typename OtherBoostGeometryObj>
    bool intersects(const OtherBoostGeometryObj &other) const {
        return boost::geometry::intersects(geometry, other.geometry);
    }

    template<typename OtherBoostGeometryObj>
    bool disjoint(const OtherBoostGeometryObj &other) const {
        return boost::geometry::disjoint(geometry, other.geometry);
    }

    template<typename OtherBoostGeometryObj>
    bool inside(const OtherBoostGeometryObj &other) const {
        logger::log_error(DBERR_INVALID_OPERATION, "intersects predicate unsupported for the invoked shapes.");
        return false;
    }

    template<typename OtherBoostGeometryObj>
    bool coveredBy(const OtherBoostGeometryObj &other) const {
        logger::log_error(DBERR_INVALID_OPERATION, "covered bt predicate unsupported for the invoked shapes.");
        return false;
    }

    template<typename OtherBoostGeometryObj>
    bool contains(const OtherBoostGeometryObj &other) const {
        logger::log_error(DBERR_INVALID_OPERATION, "contains predicate unsupported for the invoked shapes.");
        return false;
    }

    template<typename OtherBoostGeometryObj>
    bool covers(const OtherBoostGeometryObj &other) const {
        logger::log_error(DBERR_INVALID_OPERATION, "covers predicate unsupported for the invoked shapes.");
        return false;
    }

    template<typename OtherBoostGeometryObj>
    bool meets(const OtherBoostGeometryObj &other) const {
        logger::log_error(DBERR_INVALID_OPERATION, "meets predicate unsupported for the invoked shapes.");
        return false;
    }

    template<typename OtherBoostGeometryObj>
    bool equals(const OtherBoostGeometryObj &other) const {
        logger::log_error(DBERR_INVALID_OPERATION, "meets predicate unsupported for the invoked shapes.");
        return false;
    }
};

/**
@brief Point geometry derived struct. 
 */
template<>
struct GeometryWrapper<bg_point_xy> {
public:
    bg_point_xy geometry;
    GeometryWrapper(){}
    GeometryWrapper(const bg_point_xy &geom) : geometry(geom) {}

    /** @brief Replaces the geometry (point) with a new one. */
    void addPoint(const double x, const double y) {
        // For points, simply replace the existing point
        geometry = bg_point_xy(x, y);
    }

    void getBoostEnvelope(bg_rectangle &envelope) {
        boost::geometry::envelope(geometry, envelope);
    }

    void correctGeometry() {
        boost::geometry::correct(geometry);
    }

    double getArea() {
        return 0.0f;
    }

    double getIntersectionArea(const GeometryWrapper<bg_polygon>& other) const {return 0.0f;}
    double getIntersectionArea(const GeometryWrapper<bg_point_xy>& other) const {return 0.0f;}
    double getIntersectionArea(const GeometryWrapper<bg_linestring>& other) const {return 0.0f;}
    double getIntersectionArea(const GeometryWrapper<bg_rectangle>& other) const {return 0.0f;}
    double getIntersectionArea(const GeometryWrapper<bg_multi_polygon>& other) const {return 0.0f;}

    void reset() {
        boost::geometry::clear(geometry);
    }

    bg_point_xy getCentroid() {
        return this->geometry;
    }

    DB_STATUS setFromWKT(std::string &wktText) {
        // check if it is correct
        if (wktText.find("POINT") == std::string::npos) {
            // it is not a polygon WKT, ignore
            // logger::log_warning("WKT text passed into set from WKT is not a point:", wktText);
            return DBERR_INVALID_GEOMETRY;
        }
        // special case, it might be multi
        if (wktText.find("MULTIPOINT") != std::string::npos) {
            // it is a multipoint wkt, ignore
            // logger::log_warning("WKT text passed into set from WKT is a multipoint:", wktText);
            return DBERR_INVALID_GEOMETRY;
        }
        // load
        boost::geometry::read_wkt(wktText, geometry);
        // correct
        correctGeometry();
        // check if valid
        std::string reason;
        if (!boost::geometry::is_valid(geometry,reason)) {
            // invalid geometry, reset and return error
            reset();
            // logger::log_warning("Point geometry is invalid:", wktText, "Reason:", reason);
            return DBERR_INVALID_GEOMETRY;
        }
        return DBERR_OK;
    }

    bool pipTest(const bg_point_xy &point) const {
        return false;
    }

    void printGeometry() {
        printf("(%f,%f)\n", geometry.x(), geometry.y());
    }

    void modifyBoostPointByIndex(int index, double x, double y) {
        if (index != 0) {
            logger::log_error(DBERR_INVALID_OPERATION, "Ignoring non-zero index for point shape, modifying the point anyway.");
        }
        geometry = bg_point_xy(x, y);
    }

    const std::vector<bg_point_xy>* getReferenceToPoints() const {
        logger::log_error(DBERR_INVALID_OPERATION, "Can't return reference to points on Point shape.");
        return nullptr;
    }

    int getVertexCount() const {
        return 1;
    }

    /**
     * queries
     */

    std::string createMaskCode(const GeometryWrapper<bg_polygon>& other) const;
    std::string createMaskCode(const GeometryWrapper<bg_point_xy>& other) const {return "";}
    std::string createMaskCode(const GeometryWrapper<bg_linestring>& other) const {return "";}
    std::string createMaskCode(const GeometryWrapper<bg_rectangle>& other) const {return "";}
    std::string createMaskCode(const GeometryWrapper<bg_multi_polygon>& other) const;

    template<typename OtherBoostGeometryObj>
    bool intersects(const OtherBoostGeometryObj &other) const {
        return boost::geometry::intersects(geometry, other.geometry);
    }

    template<typename OtherBoostGeometryObj>
    bool disjoint(const OtherBoostGeometryObj &other) const {
        return boost::geometry::disjoint(geometry, other.geometry);
    }

    bool inside(const GeometryWrapper<bg_polygon>& other) const;
    bool inside(const GeometryWrapper<bg_multi_polygon>& other) const;
    bool inside(const GeometryWrapper<bg_linestring>& other) const;
    bool inside(const GeometryWrapper<bg_rectangle>& other) const;
    bool inside(const GeometryWrapper<bg_point_xy>& other) const {
        return boost::geometry::within(geometry, other.geometry);
    }

    bool contains(const GeometryWrapper<bg_point_xy>& other) const {return false;}
    bool contains(const GeometryWrapper<bg_polygon>& other) const {return false;}
    bool contains(const GeometryWrapper<bg_multi_polygon>& other) const {return false;}
    bool contains(const GeometryWrapper<bg_linestring>& other) const {return false;}
    bool contains(const GeometryWrapper<bg_rectangle>& other) const {return false;}

    bool meets(const GeometryWrapper<bg_polygon>& other) const;
    bool meets(const GeometryWrapper<bg_multi_polygon>& other) const;
    bool meets(const GeometryWrapper<bg_linestring>& other) const;
    bool meets(const GeometryWrapper<bg_rectangle>& other) const {return false;}
    bool meets(const GeometryWrapper<bg_point_xy>& other) const {
        return boost::geometry::touches(geometry, other.geometry);
    }

    bool equals(const GeometryWrapper<bg_linestring>& other) const {return false;}
    bool equals(const GeometryWrapper<bg_polygon>& other) const {return false;}
    bool equals(const GeometryWrapper<bg_multi_polygon>& other) const {return false;}
    bool equals(const GeometryWrapper<bg_rectangle>& other) const {return false;}
    bool equals(const GeometryWrapper<bg_point_xy>& other) const {
        return boost::geometry::equals(geometry, other.geometry);
    }
};


// rectangle
template<>
struct GeometryWrapper<bg_rectangle> {
public:
    bg_rectangle geometry;
    std::vector<bg_point_xy> vertices;
    GeometryWrapper(){}
    GeometryWrapper(const bg_rectangle &geom) : geometry(geom) {}

    void addPoint(const double x, const double y) {
        bg_point_xy point(x, y);
        if (geometry.min_corner().x() == 0 && geometry.min_corner().y() == 0) {
            // no points yet, add min point
            geometry.min_corner() = point;
        } else if (geometry.max_corner().x() == 0 && geometry.max_corner().y() == 0) {
            // one point exists, add max point
            geometry.max_corner() = point;
        } else {
            // both points already exist, error
            logger::log_error(DBERR_INVALID_OPERATION, "Cannot add more than two points to a rectangle");
        }
        vertices.emplace_back(point);
    }

    void getBoostEnvelope(bg_rectangle &envelope) {
        envelope = geometry;
    }

    DB_STATUS setFromWKT(std::string &wktText) {
        // check if it is correct
        if (wktText.find("BOX") == std::string::npos) {
            // it is not a rectangle WKT, ignore
            // logger::log_warning("WKT text passed into set from WKT is not a rectangle (box):", wktText);
            return DBERR_INVALID_GEOMETRY;
        }
        // load
        boost::geometry::read_wkt(wktText, geometry);
        // correct
        correctGeometry();
        // check if valid
        std::string reason;
        if (!boost::geometry::is_valid(geometry,reason)) {
            // invalid geometry, reset and return error
            reset();
            // logger::log_warning("Rectangle geometry is invalid:", wktText, "Reason:", reason);
            return DBERR_INVALID_GEOMETRY;
        }
        return DBERR_OK;
    }

    void correctGeometry() {
        boost::geometry::correct(geometry);
    }

    template<typename OtherGeometryType>
    double getIntersectionArea(const GeometryWrapper<OtherGeometryType> &other) const {
        logger::log_error(DBERR_INVALID_OPERATION, "Operation getIntersectionArea not supported for rectangle shapes.");
        return -1.0f;
    }

    bool pipTest(const bg_point_xy &point) const {
        return boost::geometry::within(point, geometry);
    }

    void reset() {
        boost::geometry::clear(geometry);
        vertices.clear();
    }

    bg_point_xy getCentroid() {
        bg_point_xy centroid;
        boost::geometry::centroid(this->geometry, centroid);
        return centroid;
    }

    double getArea() {
        double degreeArea = boost::geometry::area(geometry);
        return convertDegreesToSquareKilometers(degreeArea, getCentroid().y());
    }

    void printGeometry() {
        printf("(%f,%f),(%f,%f),(%f,%f),(%f,%f)\n", geometry.min_corner().x(), geometry.min_corner().y(),
        geometry.max_corner().x(), geometry.min_corner().y(),
        geometry.max_corner().x(), geometry.max_corner().y(),
        geometry.min_corner().x(), geometry.max_corner().y());
    }

    void modifyBoostPointByIndex(int index, double x, double y) {
        if (index == 0) {
            geometry.min_corner() = bg_point_xy(x, y);
        } else if (index == 1) {
            geometry.max_corner() = bg_point_xy(x, y);
        } else {
            logger::log_error(DBERR_OUT_OF_BOUNDS, "Rectangle point index out of bounds for modifyBoostPointByIndex:", index);
        }
        vertices[index] = bg_point_xy(x, y);
    }

    const std::vector<bg_point_xy>* getReferenceToPoints() const {
        return &vertices;
    }

    int getVertexCount() const {
        return 2;
    }

    /**
     * queries
     */
    
    template<typename OtherGeometryType>
    std::string createMaskCode(const GeometryWrapper<OtherGeometryType> &other) const {
        logger::log_error(DBERR_INVALID_OPERATION, "createMaskCode unsupported for the invoked shapes.");
        return "";
    }

    template<typename OtherBoostGeometryObj>
    bool intersects(const OtherBoostGeometryObj &other) const {
        return boost::geometry::intersects(geometry, other.geometry);
    }

    template<typename OtherBoostGeometryObj>
    bool disjoint(const OtherBoostGeometryObj &other) const {
        return boost::geometry::disjoint(geometry, other.geometry);
    }

    bool inside(const GeometryWrapper<bg_point_xy> &other) const {return false;}
    bool inside(const GeometryWrapper<bg_linestring> &other) const {return false;}
    bool inside(const GeometryWrapper<bg_polygon> &other) const {return false;}
    bool inside(const GeometryWrapper<bg_multi_polygon> &other) const {return false;}
    bool inside(const GeometryWrapper<bg_rectangle> &other) const {
        return boost::geometry::within(geometry, other.geometry);
    }

    bool contains(const GeometryWrapper<bg_linestring> &other) const {return false;}
    bool contains(const GeometryWrapper<bg_polygon> &other) const {return false;}
    bool contains(const GeometryWrapper<bg_multi_polygon> &other) const {return false;}
    bool contains(const GeometryWrapper<bg_rectangle> &other) const {
        return boost::geometry::within(other.geometry, geometry);
    }
    bool contains(const GeometryWrapper<bg_point_xy> &other) const {
        return boost::geometry::within(other.geometry, geometry);
    }

    bool meets(const GeometryWrapper<bg_point_xy> &other) const {return false;}
    bool meets(const GeometryWrapper<bg_linestring> &other) const {return false;}
    bool meets(const GeometryWrapper<bg_polygon> &other) const {return false;}
    bool meets(const GeometryWrapper<bg_multi_polygon> &other) const {return false;}
    bool meets(const GeometryWrapper<bg_rectangle> &other) const {return false;}

    bool equals(const GeometryWrapper<bg_point_xy>& other) const {return false;}
    bool equals(const GeometryWrapper<bg_linestring>& other) const {return false;}
    bool equals(const GeometryWrapper<bg_multi_polygon>& other) const {return false;}
    bool equals(const GeometryWrapper<bg_polygon>& other) const;
    bool equals(const GeometryWrapper<bg_rectangle>& other) const {
        return boost::geometry::equals(geometry, other.geometry);
    }
};

// linestring
template<>
struct GeometryWrapper<bg_linestring> {
public:
    bg_linestring geometry;
    GeometryWrapper(){}
    GeometryWrapper(const bg_linestring &geom) : geometry(geom) {}

    void addPoint(const double x, const double y) {
        bg_point_xy point(x, y);
        geometry.push_back(point);
    }

    void getBoostEnvelope(bg_rectangle &envelope) {
        boost::geometry::envelope(geometry, envelope);
    }

    void correctGeometry() {
        boost::geometry::correct(geometry);
    }

    double getArea() {
        return 0.0f;
    }

    double getIntersectionArea(const GeometryWrapper<bg_point_xy> &other) const {return 0.0f;}
    double getIntersectionArea(const GeometryWrapper<bg_linestring> &other) const {return 0.0f;}
    double getIntersectionArea(const GeometryWrapper<bg_rectangle> &other) const {return 0.0f;}
    double getIntersectionArea(const GeometryWrapper<bg_polygon> &other) const;
    double getIntersectionArea(const GeometryWrapper<bg_multi_polygon> &other) const;

    bool pipTest(const bg_point_xy &point) const {
        return false;
    }

    DB_STATUS setFromWKT(std::string &wktText) {
        // check if it is correct
        if (wktText.find("LINESTRING") == std::string::npos) {
            // it is not a polygon WKT, ignore
            // logger::log_warning("WKT text passed into set from WKT is not a linestring:", wktText);
            return DBERR_INVALID_GEOMETRY;
        }
        // special case, it might be multi
        if (wktText.find("MULTILINESTRING") != std::string::npos) {
            // it is a multilinestring wkt, ignore
            // logger::log_warning("WKT text passed into set from WKT is a multilinestring:", wktText);
            return DBERR_INVALID_GEOMETRY;
        }
        // load
        boost::geometry::read_wkt(wktText, geometry);
        // correct
        correctGeometry();
        // check if valid
        std::string reason;
        if (!boost::geometry::is_valid(geometry,reason)) {
            // invalid geometry, reset and return error
            reset();
            // logger::log_warning("Linestring geometry is invalid:", wktText, "Reason:", reason);
            return DBERR_INVALID_GEOMETRY;
        }
        return DBERR_OK;
    }

    void printGeometry() {
        for(auto &it: geometry) {
            printf("(%f,%f),", it.x(), it.y());
        }
        printf("\n");
    }

    void reset() {
        boost::geometry::clear(geometry);
    }

    bg_point_xy getCentroid() {
        bg_point_xy centroid;
        boost::geometry::centroid(this->geometry, centroid);
        return centroid;
    }

    void modifyBoostPointByIndex(int index, double x, double y) {
        if (index < geometry.size()) {
            geometry[index] = bg_point_xy(x, y);
        } else {
            logger::log_error(DBERR_OUT_OF_BOUNDS, "Linestring point index out of bounds for modifyBoostPointByIndex:", index);
        }
    }

    const std::vector<bg_point_xy>* getReferenceToPoints() const {
        return &geometry;
    }

    int getVertexCount() const {
        return geometry.size();
    }

    // topology
    // declaration
    std::string createMaskCode(const GeometryWrapper<bg_polygon>& other) const;
    std::string createMaskCode(const GeometryWrapper<bg_multi_polygon>& other) const;
    std::string createMaskCode(const GeometryWrapper<bg_point_xy>& other) const {return "";}
    std::string createMaskCode(const GeometryWrapper<bg_rectangle>& other) const {return "";}
    // definitions
    std::string createMaskCode(const GeometryWrapper<bg_linestring>& other) const {
        boost::geometry::de9im::matrix matrix = boost::geometry::relation(geometry, other.geometry);
        return matrix.str();
    }
    
    template<typename OtherBoostGeometryObj>
    bool intersects(const OtherBoostGeometryObj &other) const {
        return boost::geometry::intersects(geometry, other.geometry);
    }

    template<typename OtherBoostGeometryObj>
    bool disjoint(const OtherBoostGeometryObj &other) const {
        return boost::geometry::disjoint(geometry, other.geometry);
    }

    bool inside(const GeometryWrapper<bg_point_xy> &other) const {return false;}
    bool inside(const GeometryWrapper<bg_polygon> &other) const;
    bool inside(const GeometryWrapper<bg_multi_polygon> &other) const;
    bool inside(const GeometryWrapper<bg_rectangle> &other) const {return false;}
    bool inside(const GeometryWrapper<bg_linestring> &other) const {
        return boost::geometry::within(geometry, other.geometry);
    }

    bool contains(const GeometryWrapper<bg_point_xy> &other) const {return false;}
    bool contains(const GeometryWrapper<bg_polygon> &other) const {return false;}
    bool contains(const GeometryWrapper<bg_multi_polygon> &other) const {return false;}
    bool contains(const GeometryWrapper<bg_rectangle> &other) const {return false;}
    bool contains(const GeometryWrapper<bg_linestring> &other) const {return false;}

    bool meets(const GeometryWrapper<bg_polygon>& other) const;
    bool meets(const GeometryWrapper<bg_multi_polygon>& other) const;
    bool meets(const GeometryWrapper<bg_rectangle>& other) const {return false;}
    bool meets(const GeometryWrapper<bg_point_xy>& other) const {
        return boost::geometry::touches(geometry, other.geometry);
    }
    bool meets(const GeometryWrapper<bg_linestring>& other) const {
        return boost::geometry::touches(geometry, other.geometry);
    }

    bool equals(const GeometryWrapper<bg_point_xy>& other) const {return false;}
    bool equals(const GeometryWrapper<bg_polygon>& other) const {return false;}
    bool equals(const GeometryWrapper<bg_multi_polygon>& other) const {return false;}
    bool equals(const GeometryWrapper<bg_rectangle>& other) const {return false;}
    bool equals(const GeometryWrapper<bg_linestring>& other) const {
        return boost::geometry::equals(geometry, other.geometry);
    }
};

// polygon
template<>
struct GeometryWrapper<bg_polygon> {
public:
    bg_polygon geometry;
    GeometryWrapper(){}
    GeometryWrapper(const bg_polygon &geom) : geometry(geom) {}

    void addPoint(const double x, const double y) {
        bg_point_xy point(x, y);
        boost::geometry::append(geometry.outer(), point);
    }

    void correctGeometry() {
        boost::geometry::correct(geometry);
    }

    void printGeometry() {
        for(auto &it: geometry.outer()) {
            printf("(%f,%f),", it.x(), it.y());
        }
        printf("\n");
    }

    void getBoostEnvelope(bg_rectangle &envelope) {
        boost::geometry::envelope(geometry, envelope);
    }

    DB_STATUS setFromWKT(std::string &wktText) {
        // check if it is correct
        if (wktText.find("POLYGON") == std::string::npos) {
            // it is not a polygon WKT, ignore
            // logger::log_warning("WKT text passed into set point from WKT is not a polygon:", wktText);
            return DBERR_INVALID_GEOMETRY;
        }
        // special case, it might be multipolygon
        if (wktText.find("MULTIPOLYGON") != std::string::npos) {
            // it is a multipolygon wkt, ignore
            // logger::log_warning("WKT text passed into set polygon from WKT is a multipolygon:", wktText);
            return DBERR_INVALID_GEOMETRY;
        }
        // load
        boost::geometry::read_wkt(wktText, geometry);
        // correct
        correctGeometry();
        // check if valid
        std::string reason;
        if (!boost::geometry::is_valid(geometry,reason)) {
            // invalid geometry, reset and return error
            reset();
            // logger::log_warning("Polygon geometry is invalid:", wktText, "Reason:", reason);
            return DBERR_INVALID_GEOMETRY;
        }
        return DBERR_OK;
    }

    void reset() {
        boost::geometry::clear(geometry);
    }

    bg_point_xy getCentroid() const {
        bg_point_xy centroid;
        boost::geometry::centroid(this->geometry, centroid);
        return centroid;
    }

    double getIntersectionArea(const GeometryWrapper<bg_linestring>& other) const {return 0.0f;}
    double getIntersectionArea(const GeometryWrapper<bg_point_xy>& other) const {return 0.0f;}
    double getIntersectionArea(const GeometryWrapper<bg_multi_polygon>& other) const;
    double getIntersectionArea(const GeometryWrapper<bg_rectangle>& other) const {
        std::vector<bg_polygon> output;
        boost::geometry::intersection(geometry, other.geometry, output);

        double degreeArea = 0;
        for (auto &it : output) {
            degreeArea += boost::geometry::area(it);
        }
        return convertDegreesToSquareKilometers(degreeArea, getCentroid().y());
    }

    double getIntersectionArea(const GeometryWrapper<bg_polygon>& other) const {
        std::vector<bg_polygon> output;
        boost::geometry::intersection(geometry, other.geometry, output);

        double degreeArea = 0;
        for (auto &it : output) {
            degreeArea += boost::geometry::area(it);
        }
        return convertDegreesToSquareKilometers(degreeArea, getCentroid().y());
    }


    double getArea() {
        double degreeArea = boost::geometry::area(geometry);
        return convertDegreesToSquareKilometers(degreeArea, getCentroid().y());
    }

    void modifyBoostPointByIndex(int index, double x, double y) {
        if (index < geometry.outer().size()) {
            geometry.outer()[index] = bg_point_xy(x, y);
        } else {
            logger::log_error(DBERR_OUT_OF_BOUNDS, "Polygon point index out of bounds for modifyBoostPointByIndex:", index);
        }
    }

    const std::vector<bg_point_xy>* getReferenceToPoints() const {
        return &geometry.outer();
    }

    int getVertexCount() const {
        return geometry.outer().size();
    }

    // APRIL
    bool pipTest(const bg_point_xy &point) const {
        return boost::geometry::within(point, geometry);
    }

    // topology definitions
    std::string createMaskCode(const GeometryWrapper<bg_rectangle>& other) const {return "";};
    std::string createMaskCode(const GeometryWrapper<bg_multi_polygon>& other) const;
    std::string createMaskCode(const GeometryWrapper<bg_polygon>& other) const {
        boost::geometry::de9im::matrix matrix = boost::geometry::relation(geometry, other.geometry);
        return matrix.str();
    }
    std::string createMaskCode(const GeometryWrapper<bg_linestring>& other) const {
        boost::geometry::de9im::matrix matrix = boost::geometry::relation(geometry, other.geometry);
        return matrix.str();
    }
    std::string createMaskCode(const GeometryWrapper<bg_point_xy>& other) const {
        boost::geometry::de9im::matrix matrix = boost::geometry::relation(geometry, other.geometry);
        return matrix.str();
    }

    template<typename OtherBoostGeometryObj>
    bool intersects(const OtherBoostGeometryObj &other) const {
        return boost::geometry::intersects(geometry, other.geometry);
    }

    template<typename OtherBoostGeometryObj>
    bool disjoint(const OtherBoostGeometryObj &other) const {
        return boost::geometry::disjoint(geometry, other.geometry);
    }

    bool inside(const GeometryWrapper<bg_point_xy>& other) const {return false;}
    bool inside(const GeometryWrapper<bg_linestring>& other) const {return false;}
    bool inside(const GeometryWrapper<bg_rectangle>& other) const {return false;}
    bool inside(const GeometryWrapper<bg_multi_polygon>& other) const;
    bool inside(const GeometryWrapper<bg_polygon>& other) const {
        return boost::geometry::within(geometry, other.geometry);
    }

    bool contains(const GeometryWrapper<bg_rectangle>& other) const {return false;}
    bool contains(const GeometryWrapper<bg_multi_polygon>& other) const;
    bool contains(const GeometryWrapper<bg_polygon>& other) const {
        return boost::geometry::within(other.geometry, geometry);
    }
    bool contains(const GeometryWrapper<bg_point_xy>& other) const {
        return boost::geometry::within(other.geometry, geometry);
    }
    bool contains(const GeometryWrapper<bg_linestring>& other) const {
        return boost::geometry::within(other.geometry, geometry);
    }

    bool meets(const GeometryWrapper<bg_rectangle>& other) const {return false;}
    bool meets(const GeometryWrapper<bg_multi_polygon>& other) const;
    bool meets(const GeometryWrapper<bg_polygon>& other) const {
        return boost::geometry::touches(geometry, other.geometry);
    }
    bool meets(const GeometryWrapper<bg_point_xy>& other) const {
        return boost::geometry::touches(geometry, other.geometry);
    }
    bool meets(const GeometryWrapper<bg_linestring>& other) const {
        return boost::geometry::touches(geometry, other.geometry);
    }

    bool equals(const GeometryWrapper<bg_point_xy>& other) const {return false;}
    bool equals(const GeometryWrapper<bg_linestring>& other) const {return false;}
    bool equals(const GeometryWrapper<bg_multi_polygon>& other) const;
    bool equals(const GeometryWrapper<bg_polygon>& other) const {
        return boost::geometry::equals(geometry, other.geometry);
    }
    bool equals(const GeometryWrapper<bg_rectangle>& other) const {
        return boost::geometry::equals(geometry, other.geometry);
    }
};

// multi_polygon
template<>
struct GeometryWrapper<bg_multi_polygon> {
public:
    bg_multi_polygon geometry;
    GeometryWrapper(){}
    GeometryWrapper(const bg_multi_polygon &geom) : geometry(geom) {}

    void addPoint(const double x, const double y) {
        logger::log_error(DBERR_INVALID_OPERATION, "Method 'addPoint()' not supported for multi polygon shape.");
    }

    void correctGeometry() {
        boost::geometry::correct(geometry);
    }

    void printGeometry() {
        std::cout << "MultiPolygon WKT: " << boost::geometry::wkt(geometry) << std::endl;
    }

    void getBoostEnvelope(bg_rectangle &envelope) {
        boost::geometry::envelope(geometry, envelope);
    }

    DB_STATUS setFromWKT(std::string &wktText) {
        // check if it is correct
        if (wktText.find("MULTIPOLYGON") == std::string::npos) {
            // it is not a multipolygon WKT, ignore
            // logger::log_warning("WKT text passed into set point from WKT is not a polygon:", wktText);
            return DBERR_INVALID_GEOMETRY;
        }
        // load
        boost::geometry::read_wkt(wktText, geometry);
        // correct
        correctGeometry();
        // check if valid
        std::string reason;
        if (!boost::geometry::is_valid(geometry,reason)) {
            // invalid geometry, reset and return error
            reset();
            // logger::log_warning("Polygon geometry is invalid:", wktText, "Reason:", reason);
            return DBERR_INVALID_GEOMETRY;
        }
        return DBERR_OK;
    }

    void reset() {
        boost::geometry::clear(geometry);
    }

    bg_point_xy getCentroid() const {
        bg_point_xy centroid;
        boost::geometry::centroid(this->geometry, centroid);
        return centroid;
    }

    double getIntersectionArea(const GeometryWrapper<bg_linestring>& other) const {return 0.0f;}
    double getIntersectionArea(const GeometryWrapper<bg_point_xy>& other) const {return 0.0f;}
    double getIntersectionArea(const GeometryWrapper<bg_rectangle>& other) const {
        std::vector<bg_polygon> output;
        boost::geometry::intersection(geometry, other.geometry, output);

        double degreeArea = 0;
        for (auto &it : output) {
            degreeArea += boost::geometry::area(it);
        }
        return convertDegreesToSquareKilometers(degreeArea, getCentroid().y());
    }

    double getIntersectionArea(const GeometryWrapper<bg_polygon>& other) const {
        std::vector<bg_polygon> output;
        boost::geometry::intersection(geometry, other.geometry, output);

        double degreeArea = 0;
        for (auto &it : output) {
            degreeArea += boost::geometry::area(it);
        }
        return convertDegreesToSquareKilometers(degreeArea, getCentroid().y());
    }

    double getIntersectionArea(const GeometryWrapper<bg_multi_polygon>& other) const {
        std::vector<bg_polygon> output;
        boost::geometry::intersection(geometry, other.geometry, output);

        double degreeArea = 0;
        for (auto &it : output) {
            degreeArea += boost::geometry::area(it);
        }
        return convertDegreesToSquareKilometers(degreeArea, getCentroid().y());
    }

    double getArea() {
        double degreeArea = boost::geometry::area(geometry);
        return convertDegreesToSquareKilometers(degreeArea, getCentroid().y());
    }

    void modifyBoostPointByIndex(int index, double x, double y) {
        logger::log_error(DBERR_INVALID_OPERATION, "Method 'modifyBoostPointByIndex()' not supported for multi polygon shape.");
    }

    const std::vector<bg_point_xy>* getReferenceToPoints() const {
        logger::log_error(DBERR_INVALID_OPERATION, "Method 'getReferenceToPoints()' not supported for multi polygon shape.");
        return nullptr;
    }

    int getVertexCount() const {
        logger::log_error(DBERR_INVALID_OPERATION, "Method 'getVertexCount()' not supported for multi polygon shape.");
        return -1;
    }

    // APRIL
    bool pipTest(const bg_point_xy &point) const {
        return boost::geometry::within(point, geometry);
    }

    // topology definitions
    std::string createMaskCode(const GeometryWrapper<bg_rectangle>& other) const {return "";};
    std::string createMaskCode(const GeometryWrapper<bg_polygon>& other) const {
        boost::geometry::de9im::matrix matrix = boost::geometry::relation(geometry, other.geometry);
        return matrix.str();
    }
    std::string createMaskCode(const GeometryWrapper<bg_linestring>& other) const {
        boost::geometry::de9im::matrix matrix = boost::geometry::relation(geometry, other.geometry);
        return matrix.str();
    }
    std::string createMaskCode(const GeometryWrapper<bg_point_xy>& other) const {
        boost::geometry::de9im::matrix matrix = boost::geometry::relation(geometry, other.geometry);
        return matrix.str();
    }
    std::string createMaskCode(const GeometryWrapper<bg_multi_polygon>& other) const {
        boost::geometry::de9im::matrix matrix = boost::geometry::relation(geometry, other.geometry);
        return matrix.str();
    }

    template<typename OtherBoostGeometryObj>
    bool intersects(const OtherBoostGeometryObj &other) const {
        return boost::geometry::intersects(geometry, other.geometry);
    }

    template<typename OtherBoostGeometryObj>
    bool disjoint(const OtherBoostGeometryObj &other) const {
        return boost::geometry::disjoint(geometry, other.geometry);
    }

    bool inside(const GeometryWrapper<bg_point_xy>& other) const {return false;}
    bool inside(const GeometryWrapper<bg_linestring>& other) const {return false;}
    bool inside(const GeometryWrapper<bg_rectangle>& other) const {return false;}
    bool inside(const GeometryWrapper<bg_polygon>& other) const {
        return boost::geometry::within(geometry, other.geometry);
    }
    bool inside(const GeometryWrapper<bg_multi_polygon>& other) const {
        return boost::geometry::within(geometry, other.geometry);
    }

    bool contains(const GeometryWrapper<bg_rectangle>& other) const {return false;}
    bool contains(const GeometryWrapper<bg_polygon>& other) const {
        return boost::geometry::within(other.geometry, geometry);
    }
    bool contains(const GeometryWrapper<bg_point_xy>& other) const {
        return boost::geometry::within(other.geometry, geometry);
    }
    bool contains(const GeometryWrapper<bg_linestring>& other) const {
        return boost::geometry::within(other.geometry, geometry);
    }
    bool contains(const GeometryWrapper<bg_multi_polygon>& other) const {
        return boost::geometry::within(other.geometry, geometry);
    }

    bool meets(const GeometryWrapper<bg_rectangle>& other) const {return false;}
    bool meets(const GeometryWrapper<bg_polygon>& other) const {
        return boost::geometry::touches(geometry, other.geometry);
    }
    bool meets(const GeometryWrapper<bg_point_xy>& other) const {
        return boost::geometry::touches(geometry, other.geometry);
    }
    bool meets(const GeometryWrapper<bg_linestring>& other) const {
        return boost::geometry::touches(geometry, other.geometry);
    }
    bool meets(const GeometryWrapper<bg_multi_polygon>& other) const {
        return boost::geometry::touches(geometry, other.geometry);
    }

    bool equals(const GeometryWrapper<bg_point_xy>& other) const {return false;}
    bool equals(const GeometryWrapper<bg_linestring>& other) const {return false;}
    bool equals(const GeometryWrapper<bg_rectangle>& other) const {return false;}
    bool equals(const GeometryWrapper<bg_polygon>& other) const {
        return boost::geometry::equals(geometry, other.geometry);
    }
    bool equals(const GeometryWrapper<bg_multi_polygon>& other) const {
        return boost::geometry::equals(geometry, other.geometry);
    }
};

/** Define all forward declared member functions for the wrappers. 
 * They had to be forward-declared and later defined because of dependencies.
 */
/** @brief Overloaded method for creating the DE-9IM mask code for Linestring-Polygon cases.*/
inline std::string GeometryWrapper<bg_linestring>::createMaskCode(const GeometryWrapper<bg_polygon>& other) const {
    boost::geometry::de9im::matrix matrix = boost::geometry::relation(geometry, other.geometry);
    return matrix.str();
}
inline std::string GeometryWrapper<bg_linestring>::createMaskCode(const GeometryWrapper<bg_multi_polygon>& other) const {
    boost::geometry::de9im::matrix matrix = boost::geometry::relation(geometry, other.geometry);
    return matrix.str();
}
inline double GeometryWrapper<bg_linestring>::getIntersectionArea(const GeometryWrapper<bg_polygon> &other) const {return 0.0f;}
inline double GeometryWrapper<bg_linestring>::getIntersectionArea(const GeometryWrapper<bg_multi_polygon> &other) const {return 0.0f;}

/** @brief Overloaded method for the 'inside' relate predicate query for Linestring-Polygon cases.*/
inline bool GeometryWrapper<bg_linestring>::inside(const GeometryWrapper<bg_polygon> &other) const {
    return boost::geometry::within(geometry, other.geometry);
}
/** @brief Overloaded method for the 'inside' relate predicate query for Linestring-MultiPolygon cases.*/
inline bool GeometryWrapper<bg_linestring>::inside(const GeometryWrapper<bg_multi_polygon> &other) const {
    return boost::geometry::within(geometry, other.geometry);
}
/** @brief Overloaded method for the 'meets' relate predicate query for Linestring-Polygon cases.*/
inline bool GeometryWrapper<bg_linestring>::meets(const GeometryWrapper<bg_polygon> &other) const {
    return boost::geometry::touches(geometry, other.geometry);
}
/** @brief Overloaded method for the 'meets' relate predicate query for Linestring-MultiPolygon cases.*/
inline bool GeometryWrapper<bg_linestring>::meets(const GeometryWrapper<bg_multi_polygon> &other) const {
    return boost::geometry::touches(geometry, other.geometry);
}

/** @brief Overloaded method for creating the DE-9IM mask code for Point-Polygon cases.*/
inline std::string GeometryWrapper<bg_point_xy>::createMaskCode(const GeometryWrapper<bg_polygon>& other) const  {
    boost::geometry::de9im::matrix matrix = boost::geometry::relation(geometry, other.geometry);
    return matrix.str();
}
/** @brief Overloaded method for creating the DE-9IM mask code for Point-MultiPolygon cases.*/
inline std::string GeometryWrapper<bg_point_xy>::createMaskCode(const GeometryWrapper<bg_multi_polygon>& other) const  {
    boost::geometry::de9im::matrix matrix = boost::geometry::relation(geometry, other.geometry);
    return matrix.str();
}
/** @brief Overloaded method for the 'inside' relate predicate query for Point-Linestring cases.*/
inline bool GeometryWrapper<bg_point_xy>::inside(const GeometryWrapper<bg_linestring> &other) const {
    return boost::geometry::within(geometry, other.geometry);
}
/** @brief Overloaded method for the 'inside' relate predicate query for Point-Rectangle cases.*/
inline bool GeometryWrapper<bg_point_xy>::inside(const GeometryWrapper<bg_rectangle> &other) const {
    return boost::geometry::within(geometry, other.geometry);
}
/** @brief Overloaded method for the 'inside' relate predicate query for Point-Polygon cases.*/
inline bool GeometryWrapper<bg_point_xy>::inside(const GeometryWrapper<bg_polygon> &other) const {
    return boost::geometry::within(geometry, other.geometry);
}
/** @brief Overloaded method for the 'inside' relate predicate query for Point-MultiPolygon cases.*/
inline bool GeometryWrapper<bg_point_xy>::inside(const GeometryWrapper<bg_multi_polygon> &other) const {
    return boost::geometry::within(geometry, other.geometry);
}
/** @brief Overloaded method for the 'meets' relate predicate query for Point-Linestring cases.*/
inline bool GeometryWrapper<bg_point_xy>::meets(const GeometryWrapper<bg_linestring> &other) const {
    return boost::geometry::touches(geometry, other.geometry);
}
/** @brief Overloaded method for the 'meets' relate predicate query for Point-Polygon cases.*/
inline bool GeometryWrapper<bg_point_xy>::meets(const GeometryWrapper<bg_polygon> &other) const {
    return boost::geometry::touches(geometry, other.geometry);
}
/** @brief Overloaded method for the 'meets' relate predicate query for Point-MultiPolygon cases.*/
inline bool GeometryWrapper<bg_point_xy>::meets(const GeometryWrapper<bg_multi_polygon> &other) const {
    return boost::geometry::touches(geometry, other.geometry);
}

/** @brief Overloaded method for the 'equals' relate predicate query for Rectangle-Polygon cases.*/
inline bool GeometryWrapper<bg_rectangle>::equals(const GeometryWrapper<bg_polygon>& other) const {
    return boost::geometry::equals(geometry, other.geometry);
}

inline double GeometryWrapper<bg_polygon>::getIntersectionArea(const GeometryWrapper<bg_multi_polygon>& other) const {
    std::vector<bg_polygon> output;
    boost::geometry::intersection(geometry, other.geometry, output);

    double degreeArea = 0;
    for (auto &it : output) {
        degreeArea += boost::geometry::area(it);
    }
    return convertDegreesToSquareKilometers(degreeArea, getCentroid().y());
}
inline std::string GeometryWrapper<bg_polygon>::createMaskCode(const GeometryWrapper<bg_multi_polygon>& other) const {
    boost::geometry::de9im::matrix matrix = boost::geometry::relation(geometry, other.geometry);
    return matrix.str();
}
inline bool GeometryWrapper<bg_polygon>::inside(const GeometryWrapper<bg_multi_polygon>& other) const {
    return boost::geometry::within(geometry, other.geometry);
}
inline bool GeometryWrapper<bg_polygon>::contains(const GeometryWrapper<bg_multi_polygon>& other) const {
    return boost::geometry::within(other.geometry, geometry);
}
inline bool GeometryWrapper<bg_polygon>::meets(const GeometryWrapper<bg_multi_polygon>& other) const {
    return boost::geometry::touches(geometry, other.geometry);
}
inline bool GeometryWrapper<bg_polygon>::equals(const GeometryWrapper<bg_multi_polygon>& other) const {
    return boost::geometry::equals(geometry, other.geometry);
}


/** @typedef PointWrapper @brief type definition for the point wrapper*/
using PointWrapper = GeometryWrapper<bg_point_xy>;
/** @typedef PolygonWrapper @brief type definition for the polygon wrapper*/
using PolygonWrapper = GeometryWrapper<bg_polygon>;
/** @typedef LineStringWrapper @brief type definition for the linestring wrapper*/
using LineStringWrapper = GeometryWrapper<bg_linestring>;
/** @typedef RectangleWrapper @brief type definition for the rectangle wrapper*/
using RectangleWrapper = GeometryWrapper<bg_rectangle>;
/** @typedef RectangleWrapper @brief type definition for the rectangle wrapper*/
using MultiPolygonWrapper = GeometryWrapper<bg_multi_polygon>;

/** @typedef ShapeVariant @brief All the allowed Shape variants (geometry wrappers). */
using ShapeVariant = std::variant<PointWrapper, PolygonWrapper, LineStringWrapper, RectangleWrapper, MultiPolygonWrapper>;

/**
 * @brief A spatial object. Could be point, linestring, rectangle or polygon, as specified by its 'dataType' field.
 * 
 * @details All geometry wrappers are not meant to be visible to the user. Always use this struct for loading, querying or otherwise handling data.
 * Extensions to the struct's methods require explicit definitions for the geometry types in the derived geometry structs.
 */
struct Shape {
private:
    /**
    @brief The geometry variant of the Shape object. Access to the object's boost geometry parent field is done through variant
     * method definitions that utilize this field.
     * @warning Direct access is not encouraged. See the member method definitions for more.
     */
    ShapeVariant shape;
    std::vector<int> partitions;
    int partitionCount;
    double perc = 0.85;
    double xExtentPerc = 0;
    double yExtentPerc = 0;
public:
    /** @brief the object's ID, as read by the data file. */
    size_t recID;
    /** @brief the shape's data type. */
    DataType type;
    /** @brief the object's MBR. */
    MBR mbr;
    /** @brief the entity's name */
    std::string name;
    /** @brief Default empty Shape constructor. */
    Shape() {}

    /** @brief Default empty expicit type Shape constructor. */
    template<typename T>
    explicit Shape(T geom, DataType datatype) {
        shape = geom;
        type = datatype;
    }

    // Method to get the name/type of the shape variant
    std::string getShapeType() const {
        // Use std::visit to handle each possible type in the variant
        return std::visit([](const auto& shape) -> std::string {
            using T = std::decay_t<decltype(shape)>; // Get the underlying type
            if constexpr (std::is_same_v<T, PointWrapper>)
                return "PointWrapper";
            else if constexpr (std::is_same_v<T, PolygonWrapper>)
                return "PolygonWrapper";
            else if constexpr (std::is_same_v<T, LineStringWrapper>)
                return "LineStringWrapper";
            else if constexpr (std::is_same_v<T, RectangleWrapper>)
                return "RectangleWrapper";
            else
                return "Unknown type";
        }, shape);
    }

    /** @brief Returns the partition ID for partition number 'partitionIndex' in the partitions list.
     * @param partitionIndex indicates the offset (index) of the requested partition from [0, partitionCount-1].
     */
    inline int getPartitionID(int partitionIndex) {
        return partitions[partitionIndex];
    }

    /** @brief Sets the object's partitions (partitionCount in total) to the given list of partitions.
     * @param newPartitions Contains consecutive double values that represent pairs <partitionID, classType>. 
     * Has size partitionCount.
     * @param partitionCount The total number of partitions represented by the list.
     */
    inline void setPartitions(std::vector<int> &newPartitions, int partitionCount) {
        this->partitionCount = partitionCount;
        this->partitions = newPartitions;
    }

    /** @brief Initializes the list of partitions based on the input partition ids. */
    inline void initPartitions(std::vector<int> &partitionIDs) {
        partitionCount = partitionIDs.size();
        partitions.reserve(partitionCount);
        for (auto &it: partitionIDs) {
            partitions.push_back(it);
        }
    }

    inline std::vector<int>* getPartitionIDs() {
        return &this->partitions;
    }

    inline int getPartitionCount() {
        return partitionCount;
    }

    /** @brief Sets the shape's mbr. If the max values are larger than the min values, it fixes this by swapping them. */
    inline void setMBR(double xMin, double yMin, double xMax, double yMax) {
        mbr.pMin.x = std::min(xMin, xMax);
        mbr.pMin.y = std::min(yMin, yMax);
        mbr.pMax.x = std::max(xMin, xMax);
        mbr.pMax.y = std::max(yMin, yMax);
        this->xExtentPerc = (mbr.pMax.x - mbr.pMin.x) * this->perc;
        this->yExtentPerc = (mbr.pMax.y - mbr.pMin.y) * this->perc;
    }

    /** @brief Sets the MBR from the object's boost geometry envelope. */
    inline void setMBR() {
        bg_rectangle envelope;
        // get envelope from boost
        std::visit([&](auto&& arg) {
            arg.getBoostEnvelope(envelope);
        }, shape);
        // set mbr
        mbr.pMin.x = envelope.min_corner().x();
        mbr.pMin.y = envelope.min_corner().y();
        mbr.pMax.x = envelope.max_corner().x();
        mbr.pMax.y = envelope.max_corner().y();
        this->xExtentPerc = (mbr.pMax.x - mbr.pMin.x) * this->perc;
        this->yExtentPerc = (mbr.pMax.y - mbr.pMin.y) * this->perc;
    }

    /** @brief Resets the MBR points */
    inline void resetMBR() {
        mbr.pMin.x = std::numeric_limits<int>::max();
        mbr.pMin.y = std::numeric_limits<int>::max();
        mbr.pMax.x = -std::numeric_limits<int>::max();
        mbr.pMax.y = -std::numeric_limits<int>::max();
        this->xExtentPerc = 0;
        this->yExtentPerc = 0;
    }

    inline double getXExtentPerc() {
        return this->xExtentPerc;
    }

    inline double getYExtentPerc() {
        return this->yExtentPerc;
    }

    /** @brief Resets the shape object to empty */
    void resetPoints() {
        std::visit([](auto&& arg) {
            arg.reset();
        }, shape);
    }

    /** @brief Returns the centroid of the shape */
    bg_point_xy getCentroid() {
        return std::visit([](auto&& arg) {
            return arg.getCentroid();
        }, shape);
    }

    /** @brief Resets the boost geometry object. */
    void reset() {
        recID = 0;
        resetMBR();
        partitions.clear();
        partitionCount = 0;
        resetPoints();
        name = "";
    }

    /** @brief Adds a point to the boost geometry (see derived method definitions). */
    void addPoint(const double x, const double y) {
        // then in boost object
        std::visit([&x, &y](auto&& arg) {
            arg.addPoint(x, y);
        }, shape);
    }

    /** @brief Corrects the geometry object based on the boost geometry standards. */
    void correctGeometry() {
        std::visit([](auto&& arg) {
            arg.correctGeometry();
        }, shape);
    }

    /** @brief Returns the shape's area in sq km */
    double getArea() {
        return std::visit([](auto&& arg) {
            return arg.getArea();
        }, shape);
    } 

    double getIntersectionArea(const Shape &other) const {
        return std::visit([&other](auto&& arg) -> double {
            return std::visit([&arg](auto&& otherArg) -> double {
                return arg.getIntersectionArea(otherArg);
            }, other.shape);
        }, shape);
    }

    /** @brief Sets the shape's boost geometry points and MBR to the new list. */
    void setPoints(std::vector<double> &coords) {
        resetPoints();
        resetMBR();
        for (int i=0; i<coords.size(); i+=2) {
            addPoint(coords[i], coords[i+1]);
            mbr.pMin.x = std::min(mbr.pMin.x, coords[i]);
            mbr.pMin.y = std::min(mbr.pMin.y, coords[i+1]);
            mbr.pMax.x = std::max(mbr.pMax.x, coords[i]);
            mbr.pMax.y = std::max(mbr.pMax.y, coords[i+1]);
        }
        correctGeometry();
    }

    DB_STATUS setFromWKT(std::string &wktText) {
        return std::visit([&wktText](auto&& arg) {
            return arg.setFromWKT(wktText);
        }, shape);
    }

    /** @brief Modifies the point specified by 'index' with the new values x,y. (see derived method definitions) 
     * @param index The position of the point to modify in the geometry object.
     * @param x The new x value.
     * @param y The new y value.
    */
    void modifyBoostPointByIndex(int index, double x, double y) {
        std::visit([index, &x, &y](auto&& arg) {
            arg.modifyBoostPointByIndex(index, x, y);
        }, shape);
    }

    /** @brief Prints the geometry. */
    void printGeometry() {
        printf("id: %zu\n", recID);
        std::visit([](auto&& arg) {
            arg.printGeometry();
        }, shape);
    }

    /** @brief Returns a reference to the point list of the geometry. */
    const std::vector<bg_point_xy>* getReferenceToPoints() {
        return std::visit([](auto&& arg) -> const std::vector<bg_point_xy>* {
            return arg.getReferenceToPoints();
        }, shape);
    }

    /** @brief Returns the point count of the geometry. */
    int getVertexCount() {
        return std::visit([](auto&& arg) -> int {
            return arg.getVertexCount();
        }, shape);
    }

    /** @brief Performs a point-in-polygon test with the given point (see derived method definitions). */
    bool pipTest(const bg_point_xy& point) const {
        return std::visit([&point](auto&& arg) -> bool {
            return arg.pipTest(point);
        }, shape);
    }

    /** @brief Generates and returns the DE-9IM mask of this geometry (as R) with the input geometry (as S) 
     * @warning Not all geometry type combinations are supported (see data type support).
    */
    std::string createMaskCode(const Shape &other) const {
        return std::visit([&other](auto&& arg) -> std::string {
            return std::visit([&arg](auto&& otherArg) -> std::string {
                return arg.createMaskCode(otherArg);
            }, other.shape);
        }, shape);
    }

    /** @brief Returns true whether the input geometry intersects (border or area) with this geometry. False otherwise. 
     * @warning Not all geometry type combinations are supported (see data type support).
    */
    template<typename OtherBoostGeometryObj>
    bool intersects(const OtherBoostGeometryObj &other) const {
        return std::visit([&other](auto&& arg) -> bool {
            return std::visit([&arg](auto&& otherArg) -> bool {
                return arg.intersects(otherArg);
            }, other.shape);
        }, shape);
    }

    /** @brief Returns true whether the input geometry is disjoint (no common points) with this geometry. False otherwise. 
     * @warning Not all geometry type combinations are supported (see data type support).
    */
    template<typename OtherBoostGeometryObj>
    bool disjoint(const OtherBoostGeometryObj &other) const {
        return std::visit([&other](auto&& arg) -> bool {
            return std::visit([&arg](auto&& otherArg) -> bool {
                return arg.disjoint(otherArg);
            }, other.shape);
        }, shape);
    }

    /** @brief Returns true whether the geometry is completely inside (no inside-border common points) the input geometry. False otherwise. 
     * @warning Not all geometry type combinations are supported (see data type support).
    */
    template<typename OtherBoostGeometryObj>
    bool inside(const OtherBoostGeometryObj &other) const {
        return std::visit([&other](auto&& arg) -> bool {
            return std::visit([&arg](auto&& otherArg) -> bool {
                return arg.inside(otherArg);
            }, other.shape);
        }, shape);
    }

    /** @brief Returns true whether the geometry is covered by (inside-border common points are allowed) the input geometry. False otherwise. 
     * @warning Not all geometry type combinations are supported (see data type support).
    */
    template<typename OtherBoostGeometryObj>
    bool coveredBy(const OtherBoostGeometryObj &other) const {
        return std::visit([&other](auto&& arg) -> bool {
            return std::visit([&arg](auto&& otherArg) -> bool {
                return arg.inside(otherArg);
            }, other.shape);
        }, shape);
    }

    /** @brief Returns true whether the geometry completely contains (reverse of inside) the input geometry. False otherwise. 
     * @warning Not all geometry type combinations are supported (see data type support).
    */
    template<typename OtherBoostGeometryObj>
    bool contains(const OtherBoostGeometryObj &other) const {
        return std::visit([&other](auto&& arg) -> bool {
            return std::visit([&arg](auto&& otherArg) -> bool {
                return arg.contains(otherArg);
            }, other.shape);
        }, shape);
    }

    /** @brief Returns true whether the geometry covers (reverse of covered by) the input geometry. False otherwise. 
     * @warning Not all geometry type combinations are supported (see data type support).
    */
    template<typename OtherBoostGeometryObj>
    bool covers(const OtherBoostGeometryObj &other) const {
        return std::visit([&other](auto&& arg) -> bool {
            return std::visit([&arg](auto&& otherArg) -> bool {
                return arg.contains(otherArg);
            }, other.shape);
        }, shape);
    }

    /** @brief Returns true whether the geometry meets (touches) the input geometry (their insides do not have common points, but their borders do). 
     * False otherwise. 
     * @warning Not all geometry type combinations are supported (see data type support).
    */
    template<typename OtherBoostGeometryObj>
    bool meets(const OtherBoostGeometryObj &other) const {
        return std::visit([&other](auto&& arg) -> bool {
            return std::visit([&arg](auto&& otherArg) -> bool {
                return arg.meets(otherArg);
            }, other.shape);
        }, shape);
    }

    /** @brief Returns true whether the geometry is spatially equal the input geometry. False otherwise. 
     * @warning Not all geometry type combinations are supported (see data type support).
    */
    template<typename OtherBoostGeometryObj>
    bool equals(const OtherBoostGeometryObj &other) const {
        return std::visit([&other](auto&& arg) -> bool {
            return std::visit([&arg](auto&& otherArg) -> bool {
                return arg.equals(otherArg);
            }, other.shape);
        }, shape);
    }

    // void printVariantType() {
    //     boost::typeindex::type_id<T?>
    // }
};

/** @namespace shape_factory
@brief contains the factory methods for generating geometry wrapper derived objects.
 */
namespace shape_factory
{
    // Create empty shapes
    Shape createEmptyPointShape();
    Shape createEmptyPolygonShape();
    Shape createEmptyLineStringShape();
    Shape createEmptyRectangleShape();
    Shape createEmptyMultiPolygonShape();

    /** @brief creates an empty shape object of the specified data type.
     * @param dataType the requested data type of the Shape object
     * @param object the return value where the empty object will be stored
     * @return DB_STATUS returns the status value of the operation
    */
    DB_STATUS createEmpty(DataType dataType, Shape &object);
}

/** @brief All dataspace related metadata, filled in after loading the dataset(s).
 * @param xMinGlobal, yMinGlobal, xMaxGlobal, yMaxGlobal The dataspace's borders (MBR).
 * @param xExtent, yExtent, maxExtent The dataspace's extent.
 */
struct DataspaceMetadata {
    double xMinGlobal, yMinGlobal, xMaxGlobal, yMaxGlobal;  // global bounds based on dataset bounds
    double xExtent, yExtent, maxExtent;
    bool boundsSet = false;

    DataspaceMetadata();
    void set(double xMinGlobal, double yMinGlobal, double xMaxGlobal, double yMaxGlobal);
    void clear();
};

/** @brief Holds all necessary partition information. 
 * @param partitionID The partition's ID in the grid.
 * @param classIndex Fixed 4 position vector, one for each two-layer index class.
 */
struct Partition {
    int partitionID;
    /** @brief Contains the list of object pointers (Shape*) for this partition */
    std::vector<Shape*> contents;
    /**
    @brief Default constructor that defines the 4-position vector. Two-layer index classes: A, B, C, D
     */
    Partition(int id){
        partitionID = id;
    }

    /** @brief Returns a reference to the partition's contents */
    std::vector<Shape*>* getContents();
    void addObject(Shape *objectRef);
};

/** @brief Holds all uniform grid index information.
 * @param partitions A vector containing each individual non-empty partition.
 * @param partitionMap A map that holds the positions of each partition (by ID) in the 'partitions' vector.
 */
struct UniformGridIndex {
    std::vector<Partition> partitions;
    std::unordered_map<int, size_t> partitionMap;
private:
    /** @brief Compares to Shapes by MBR bottom-left point y. */
    static bool compareByY(const Shape* a, const Shape* b) {
        return a->mbr.pMin.y < b->mbr.pMin.y;
    }
public:
    /** @brief Returns or creates a new partition with the given ID. */
    Partition* getOrCreatePartition(int partitionID);
    /** 
    @brief Adds an object ref to a partition with partitionID
     * @param[in] partitionID The partition's ID to add the object to.
     * @param[out] objectRef The returned object reference.
     */
    void addObject(int partitionID, Shape* objectRef);
    /**
    @brief Returns the partition reference to this partition ID
     */
    Partition* getPartition(int partitionID);
};


/**
 * @brief All dataset related information.
 */
struct Dataset{
    FileFormat fileFormat;
    std::string path;
    // derived from the path
    std::string datasetName;
    // as given by arguments and specified by datasets.ini config file
    std::string nickname;
    std::string key;
    std::string description;
    int wktColIdx = -1;
    int nameColIdx = -1;
    // holds the dataset's dataspace metadata (MBR, extent)
    DataspaceMetadata dataspaceMetadata;
    // unique object count
    size_t totalObjects = 0;
    std::vector<size_t> objectIDs;
    std::unordered_map<size_t, Shape> objects;
    UniformGridIndex uniformGridIndex;

    Dataset(){}
    Dataset(DatasetStatement &stmt);

    /** @brief Adds a Shape object into the two layer index and the reference map. 
     * @note Calculates the partitions and the object's classes in them. */
    DB_STATUS addObject(Shape &object);

    /** @brief Returns a reference to the object with the given ID. */
    Shape* getObject(size_t recID);


    void printObjectsGeometries();
    void printObjectsPartitions();
    void printPartitions();
    void printPartitionStatistics();
    void printPartitionContents(int partitionID);
};


/** @brief Holds the dataset(s) related metadata in the configuration.
 */
struct DatasetMetadata {
private:
    Dataset* R;
    Dataset* S;
    int numberOfDatasets;

public:
    std::unordered_map<std::string,Dataset> datasets;
    DataspaceMetadata dataspaceMetadata;
    
    Dataset* getDatasetByNickname(std::string &nickname);

    int getNumberOfDatasets();

    void clear();

    Dataset* getDatasetR();

    Dataset* getDatasetS();

    Dataset* getDatasetByIdx(DatasetIndex datasetIndex);

    /**
    @brief adds a Dataset to the configuration's dataset metadata
     * @warning it has to be an empty dataset BUT its nickname needs to be set
     */
    DB_STATUS addDataset(DatasetIndex datasetIdx, Dataset &dataset);

    void updateDataspace();
};

struct DirectoryPaths {
    const std::string datasetsConfigPath = "../datasets.ini";
};

struct IndexConfig {
    int partitionsPerDim = 10000;
};

/** @brief Parallel buffered disk writer for the relations texts */
struct DiskWriter {
private:
    std::vector<std::string> buffers;
    size_t buffer_limit = 8192;    // in bytes (be careful of string::max_size)
    std::ofstream output;
    DocumentType docType = DOC_SENTENCES;
    // for paragraph document type
    std::unordered_map<std::string, std::string> entityRelationMap;
public:
    DiskWriter(int numThreads) {
        buffers.resize(numThreads);
    }
    void addString(std::string &str, int tid);
    DB_STATUS writeBuffers();
    DB_STATUS writeFixedRules();
    void printBufferSizes();

    DB_STATUS openOutputFilestream(std::string &filepath, bool append);

    void closeOutputFilestream();

    void setDocumentType(DocumentType docType);
    DocumentType getDocumentType();

    void appendTextForEntity(std::string entityKey, std::string text);
    
};

/** @brief The main configuration struct. Holds all necessary configuration options.
 */
struct Config {
private:
    // int NUM_THREADS = omp_get_max_threads();
    int NUM_THREADS = 1;    // default: single threaded
public:
    DatasetMetadata datasetMetadata;
    DirectoryPaths dirPaths;
    IndexConfig indexConfig;
    DiskWriter diskWriter = DiskWriter(NUM_THREADS);

    void setNumThreads(int numThreads) {
        NUM_THREADS = numThreads;
        diskWriter = DiskWriter(NUM_THREADS);
    }

    int getNumThreads() {
        return NUM_THREADS;
    }
};

extern Config g_config;

#endif