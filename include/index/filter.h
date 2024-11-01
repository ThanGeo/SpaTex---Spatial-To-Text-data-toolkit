#ifndef INDEX_FILTER_H
#define INDEX_FILTER_H

#include "containers.h"
#include "refinement.h"
#include "index/create.h"

namespace uniform_grid
{
    namespace sentences
    {
        DB_STATUS evaluate(Dataset* R, Dataset* S);
    }

    namespace paragraphs
    {
        DB_STATUS evaluate(Dataset* R, Dataset* S);
    }
}

#endif