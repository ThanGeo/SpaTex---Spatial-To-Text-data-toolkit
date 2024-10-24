#ifndef INDEX_REFINEMENT_H
#define INDEX_REFINEMENT_H

#include "containers.h"

namespace refinement
{
    /**
     * TOPOLOGY RELATION DE-9IM CODES
    */
    static char insideCode[] = "T*F**F***";
    // char trueInsideCode[] = "T*F*FF***";
    static char coveredbyCode1[] = "T*F**F***";
    static char coveredbyCode2[] = "*TF**F***";
    static char coveredbyCode3[] = "**FT*F***";
    static char coveredbyCode4[] = "**F*TF***";
    static char containsCode[] = "T*****FF*";
    static char coversCode1[] = "T*****FF*";
    static char coversCode2[] = "*T****FF*";
    static char coversCode3[] = "***T**FF*";
    static char coversCode4[] = "****T*FF*";
    static char meetCode1[] = "FT*******"; 
    static char meetCode2[] = "F**T*****"; 
    static char meetCode3[] = "F***T****"; 
    static char equalCode[] = "T*F**FFF*"; 
    static char disjointCode[] = "FF*FF****";
    static char intersectCode1[] = "T********";
    static char intersectCode2[] = "*T*******";
    static char intersectCode3[] = "***T*****";
    static char intersectCode4[] = "****T****";

    //define topological masks for refinement
    // a inside b
    static boost::geometry::de9im::mask insideMask(insideCode); 
    // a contains b
    static boost::geometry::de9im::mask containsMask(containsCode); 
    // a covered by b
    static std::vector<boost::geometry::de9im::mask> coveredByMaskList = {
                    boost::geometry::de9im::mask(coveredbyCode1),
                    boost::geometry::de9im::mask(coveredbyCode2),
                    boost::geometry::de9im::mask(coveredbyCode3),
                    boost::geometry::de9im::mask(coveredbyCode4)};
    // a covers b
    static std::vector<boost::geometry::de9im::mask> coversMaskList = {
                    boost::geometry::de9im::mask(coversCode1),
                    boost::geometry::de9im::mask(coversCode2),
                    boost::geometry::de9im::mask(coversCode3),
                    boost::geometry::de9im::mask(coversCode4)};
    // a and b meet
    static boost::geometry::de9im::mask meetMask1(meetCode1); 
    static boost::geometry::de9im::mask meetMask2(meetCode2); 
    static boost::geometry::de9im::mask meetMask3(meetCode3); 
    // a and b are equal
    static boost::geometry::de9im::mask equalMask(equalCode); 
    // a and b are disjoint
    static boost::geometry::de9im::mask disjointMask(disjointCode); 
    // a overlaps b
    static std::vector<boost::geometry::de9im::mask> overlapMaskList = {
                    boost::geometry::de9im::mask(intersectCode1),
                    boost::geometry::de9im::mask(intersectCode2),
                    boost::geometry::de9im::mask(intersectCode3),
                    boost::geometry::de9im::mask(intersectCode4)};



    DB_STATUS computeRelationTexts(Shape* objR, Shape* objS, MBRRelationCase mbrRelationCase, std::string &relationText);



}


#endif
