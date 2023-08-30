#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

#include "./generation.hpp"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "incorrect usage" << std::endl;
        std::cerr << "correct usage: hydro <input.hy>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    Tokenizer tokenizer(contents);
    std::vector<Token> tokens = tokenizer.tokenize();

    Parser parser(std::move(tokens));
    std::optional<NodeProg> prog = parser.parse_prog();

    if (!prog.has_value()) {
        std::cerr << "invalid program!" << std::endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(prog.value());
    {
        std::fstream file("out.s", std::ios::out);
        file << generator.gen_prog();
    }

    system("as -o out.o out.s");
    system("ld -macosx_version_min 11.0.0 -o out out.o "
           "-lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e "
           "_start -arch arm64");

    return EXIT_SUCCESS;
}
