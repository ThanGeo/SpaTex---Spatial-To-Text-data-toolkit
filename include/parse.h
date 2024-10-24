#ifndef PARSE_H
#define PARSE_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/exceptions.hpp>

#include "containers.h"
#include "utils.h"
#include "config.h"

namespace parse
{
    DB_STATUS parseArguments(int argc, char *argv[]);
}

#endif