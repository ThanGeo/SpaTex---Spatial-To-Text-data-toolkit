#ifndef INDEX_CREATE_H
#define INDEX_CREATE_H

#include <fstream>

#include "def.h"
#include "containers.h"

namespace uniform_grid
{
    int getPartitionID(int i, int j, int partitionsPerDim);
    
    DB_STATUS create();

}

#endif