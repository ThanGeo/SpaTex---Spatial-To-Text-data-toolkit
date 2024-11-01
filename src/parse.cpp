#include "parse.h"

// property tree var
static boost::property_tree::ptree dataset_config_pt;

static DB_STATUS verifyDatasetStatement(DatasetStatement &datasetStmt) {    
    // verify filepath
    if (!verifyFilepath(datasetStmt.path)) {
        logger::log_error(DBERR_INVALID_FILEPATH, "File is missing at path:", datasetStmt.path);
        return DBERR_INVALID_FILEPATH;
    }
    
    std::string extension = getFileExtension(datasetStmt.path);
    
    // make upper case
    std::transform(extension.begin(), extension.end(), extension.begin(), ::toupper);

    // get file format code
    datasetStmt.fileFormat = mapping::fileFormatTextToInt(extension);
    if (datasetStmt.fileFormat == DT_INVALID) {
        logger::log_error(DBERR_INVALID_ARGS, "Invalid file format/extension:", datasetStmt.path);
        return DBERR_INVALID_ARGS;
    }

    return DBERR_OK;
}

static DB_STATUS verifyOutputSetupFilepath(OutputStatement &outputStmt) {
    DB_STATUS ret = DBERR_OK;
    if (outputStmt.append) {
        // append to existing file, verify filepath
        if (!verifyFilepath(outputStmt.outputFilepath)) {
            // filepath does not exist, create
            std::ofstream output(outputStmt.outputFilepath, std::ofstream::out);
            if (!output.is_open()) {
                logger::log_error(DBERR_FILE_OPEN, "Error opening output file:", outputStmt.outputFilepath);
                return DBERR_FILE_OPEN;
            }
            output.close();
        }
    }
    return ret;
}

static DB_STATUS verifyArguments(ArgumentsStatement &argsStmt) {
    DB_STATUS ret = DBERR_OK;
    if (!argsStmt.datasetR.set || !argsStmt.datasetS.set) {
        logger::log_error(DBERR_INVALID_ARGS, "Two datasets must be set. Use both -R and -S arguments.");
        return DBERR_INVALID_ARGS;
    }
    // R
    ret = verifyDatasetStatement(argsStmt.datasetR);
    if (ret != DBERR_OK) {
        logger::log_error(ret, "Failed while verifying dataset R.");
        return ret;
    }
    // S
    ret = verifyDatasetStatement(argsStmt.datasetS);
    if (ret != DBERR_OK) {
        logger::log_error(ret, "Failed while verifying dataset S.");
        return ret;
    }
    // output
    ret = verifyOutputSetupFilepath(argsStmt.outputStmt);
    if (ret != DBERR_OK) {
        logger::log_error(ret, "Failed while verifying output filepath.");
        return ret;
    }

    return ret;
}

static DB_STATUS loadMetadata(DatasetStatement &stmt) {
    try {
        stmt.path = dataset_config_pt.get<std::string>(stmt.nickname+".path");
    }
    catch (const std::exception &e){
        logger::log_task(e.what());
        logger::log_error(DBERR_INI_ERROR, "'path' invalid or missing parameter from datasets.ini configuration file for dataset", stmt.nickname);
        return DBERR_INI_ERROR;
    }

    try {
        stmt.fileFormat = mapping::fileFormatTextToInt(dataset_config_pt.get<std::string>(stmt.nickname+".filetype"));
    }
    catch(const std::exception& e) {
        logger::log_task(e.what());
        logger::log_error(DBERR_INI_ERROR, "'filetype' invalid or missing parameter from datasets.ini configuration file for dataset", stmt.nickname);
        return DBERR_INI_ERROR;
    }

    try {
        stmt.description = dataset_config_pt.get<std::string>(stmt.nickname+".description");
    }
    catch(const std::exception& e) {
        logger::log_task(e.what());
        logger::log_error(DBERR_INI_ERROR, "'description' invalid or missing parameter from datasets.ini configuration file for dataset", stmt.nickname);
        return DBERR_INI_ERROR;
    }

    try {
        stmt.wktColIdx = dataset_config_pt.get<int>(stmt.nickname+".wktcolidx");
    }
    catch(const std::exception& e) {
        logger::log_task(e.what());
        logger::log_error(DBERR_INI_ERROR, "'wktcolidx' invalid or missing parameter from datasets.ini configuration file for dataset", stmt.nickname);
        return DBERR_INI_ERROR;
    }

    try {
        stmt.nameColIdx = dataset_config_pt.get<int>(stmt.nickname+".namecolidx");
    }
    catch(const std::exception& e) {
        logger::log_task(e.what());
        logger::log_error(DBERR_INI_ERROR, "'namecolidx' invalid or missing parameter from datasets.ini configuration file for dataset", stmt.nickname);
        return DBERR_INI_ERROR;
    }
    
    return DBERR_OK;
}

namespace parse
{
    DB_STATUS parseArguments(int argc, char *argv[]) {
        char c;
        DB_STATUS ret = DBERR_OK;
        ArgumentsStatement argsStmt;
        
        // open the config file parser
        boost::property_tree::ini_parser::read_ini(g_config.dirPaths.datasetsConfigPath, dataset_config_pt);

        // after config file has been loaded, parse cmd arguments and overwrite any selected options
        while ((c = getopt(argc, argv, "R:S:p:t:ao:?")) != -1)
        {
            switch (c)
            {
                case 'R':
                    // Dataset R path
                    argsStmt.datasetR.set = true;
                    argsStmt.datasetR.nickname = std::string(optarg);
                    argsStmt.datasetR.key = std::string(optarg) + "_R";
                    break;
                case 'S':
                    // Dataset S path
                    argsStmt.datasetS.set = true;
                    argsStmt.datasetS.nickname = std::string(optarg);
                    argsStmt.datasetS.key = std::string(optarg) + "_S";
                    break;
                case 'p':
                    g_config.indexConfig.partitionsPerDim = atoi(optarg);
                    break;
                case 't':
                    g_config.setNumThreads(atoi(optarg));
                    break;
                case 'a':
                    // append output to file
                    argsStmt.outputStmt.append = true;
                    break;
                case 'o':
                    // output filepath
                    argsStmt.outputStmt.outputFilepath = std::string(optarg);
                    break;
                default:
                    logger::log_error(DBERR_INVALID_ARGS, "Unkown argument:", c);
                    return DBERR_INVALID_ARGS;
            }
        }

        // load metadata from datasets.ini
        ret = loadMetadata(argsStmt.datasetR);
        if (ret != DBERR_OK) {
            return ret;
        }
        ret = loadMetadata(argsStmt.datasetS);
        if (ret != DBERR_OK) {
            return ret;
        }

        // verify arguments
        ret = verifyArguments(argsStmt);
        if (ret != DBERR_OK) {
            logger::log_error(ret, "Argument verification failed.");
            return ret;
        } 

        // setup the config based on the arguments
        ret = config::setup(argsStmt);
        if (ret != DBERR_OK) {
            logger::log_error(ret, "Configuration setup failed.");
            return ret;
        } 

        return DBERR_OK;
    }
}

