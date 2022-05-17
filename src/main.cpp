
#include <iostream>
#include <fstream>
#include <cstring>
#include <optional>

#include "TSFile.hpp"
#include "cxxopts.hpp"

cxxopts::ParseResult parse(int argc, const char *argv[])
{
    cxxopts::ParseResult result;
    cxxopts::Options options(argv[0], " - mpeg TS extractor");
    try
    {
        options
            .positional_help("[optional args]")
            .show_positional_help();

        options.add_options()("d,debug-headers", "Debug TS header processing.", cxxopts::value<bool>());
        options.add_options("Input / output file")("input-file", "Name of the input file (read only).", cxxopts::value<std::string>());
        result = options.parse(argc, argv);
    }
    catch (const cxxopts::OptionException &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }
    return result;
}

int main(int argc, const char *argv[])
{
    auto parameters = parse(argc, argv);
    if (!parameters.count("input-file"))
    {
        std::cout << "Mandatory parameter \"input-file\" is  missing." << std::endl;
        return -1;
    }

    Extractor::TS::TSFile ts(
        parameters["input-file"].as<std::string>(),
        parameters.count("debug-headers"));
    ts.Extract();

    return 0;
}
