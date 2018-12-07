//Donald MacPhail
//unzip, decompression algorithm

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

//Read 1 byte, then another, then combine.
bool readChunk(std::istream &is, unsigned int &code) {
    char c;
    if(!is.get(c)) {
        return false;
    }
    code = c & 0xff;
    if(!is.get(c)) {
        return false;
    }
    code |= (c & 0xff) << 8;
    if(code == EOF_CODE) {
        return false;
    } else {
        return true;
    }
}

//decompress file
//unpacks dictionary to read codes (keys) and send strings (values) to ostream
void decompress(std::istream &is, std::ostream &os) {
    // Build the dictionary.
    std::unordered_map<unsigned int, std::string> dictionary(SHRT_MAX);

    //initialize dictionary
    for ( unsigned int i = 0 ; i < 256 ; i++ ) {
        dictionary[i] = std::string(1,i);
    }

    //read from stream, find the code, print the value string
    unsigned int code;
    unsigned int nextCode = 257;
    std::string prev;
    while(readChunk(is, code)) {
        if(dictionary.find(code) == dictionary.end()) {
            dictionary[code] = prev + prev[0];
        }
        os << dictionary[code];
        if(prev.size() && nextCode <= SHRT_MAX) {
            dictionary[nextCode++] = prev + dictionary[code][0];
        }
        prev = dictionary[code];
    }
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

    //TODO: find a better way to do this
    //hacky file extension meddling
    std::string base = argv[1];
    base.erase(base.end()-4, base.end());
    char cstr[base.size() + 1];
    strcpy(cstr, base.c_str());
    std::ofstream output_file(cstr, std::ios_base::binary);

    if (!output_file.is_open()) {
        printf("%s .zip could not be opened.\n", argv[1]);
        return EXIT_FAILURE;
    }

    try {
        input_file.exceptions(std::ios_base::badbit);
        output_file.exceptions(std::ios_base::badbit | std::ios_base::failbit);
        decompress(input_file, output_file);
    } catch (const std::ios_base::failure &f) {
        printf(f.what() + '.', false);
        return EXIT_FAILURE;
    } catch (const std::exception &e) {
        printf(e.what() + '.', false);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
