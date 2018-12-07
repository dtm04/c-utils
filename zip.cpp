//file compression algorithm
//Donald MacPhail

#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <istream>
#include <iterator>
#include <climits>
#include <unordered_map>
#include <ostream>
#include <stdexcept>
#include <string>

const unsigned int EOF_CODE = 256;

void writeChunk(std::ostream &os, unsigned int i) {
    os.put(i & 0xff);
    os.put((i>>8) & 0xff);
}

// Compress file into dictionary<string-keys, code-values>
// Write data to outputstream
void compress(std::istream &is, std::ostream &os) {
    // Build the dictionary.
    std::unordered_map<std::string, unsigned int> dictionary(SHRT_MAX);

    //initialize dictionary
    for ( unsigned int i = 0 ; i < 256 ; i++ ) {
        dictionary[std::string(1,i)] = i;
    }

    //read stream, set codes
    unsigned int nextCode = 257;
    std::string s;
    char c;
    while(is.get(c)) {
        s = s + c;
        if(dictionary.find(s) == dictionary.end()) {
            if(nextCode <= SHRT_MAX) {
                dictionary[s] = nextCode++;
            }
            //s.pop_back();
            s.erase(s.size()-1);
            writeChunk(os, dictionary[s]);
            //os << dictionary[s] << '\n';
            s = c;
        }
    }
    if(s.size()) {
        writeChunk(os, dictionary[s]);
    }
    writeChunk(os, EOF_CODE);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Wrong number of arguments.\n");
        return EXIT_FAILURE;
    }

    std::ifstream input_file(argv[1], std::ios_base::binary);

    if (!input_file.is_open()) {
        printf("%s could not be opened.\n", argv[1]);
        return EXIT_FAILURE;
    }

    //TODO: this section can be improved, it's not very robust but low on time
    std::string base = argv[1];
    base.append(".zip");
    char cstr[base.size() + 1];
    strcpy(cstr, base.c_str());
    std::ofstream output_file(cstr, std::ios_base::binary);

    if (!output_file.is_open()) {
        printf("%s .zip could not be opened.\n", argv[1]);
        return EXIT_FAILURE;
    }

    //google helped with this fancy exception handling
    try {
        input_file.exceptions(std::ios_base::badbit);
        output_file.exceptions(std::ios_base::badbit | std::ios_base::failbit);

        compress(input_file, output_file);
    } catch (const std::ios_base::failure &f) {
        printf(f.what() + '.', false);
        return EXIT_FAILURE;
    }
    catch (const std::exception &e) {
        printf(e.what() + '.', false);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}
