#include <cstdlib>
#include <cstdio>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

#include "blackrock.h"
#include "shared.h"

std::ostream& operator<<(std::ostream& os, const NSXBasicHeader& header) {
    os << as_fixed(header.file_type) << " " << as_version(header.file_spec) << " '" <<  header.label << "' (" << header.time << ")" << endl;
    os << "Comment: '" << header.comment << "'" << endl;
    os << 30000.0 / header.sampling_period << " sample/sec and " << header.time_resolution << " counts/sec for " << header.channel_count << " channels" << endl;
    os << "Data starts at " << header.header_size << endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const NSXExtensionHeader& header) {
     os << as_fixed(header.type) << " of '" << header.label << "' connected to " << (char) (header.bank + '@') << (int)  header.pin << " (id " << header.id << ')' << endl;
     os << header.min_analog_value << " - " << header.max_analog_value << ' ' << header.unit << " (" << header.min_digital_value << " - " << header.max_digital_value << ')' << endl;
     os << "high pass filter: " << header.highpass_corner << " MHz, " << header.highpass_order << ". order, " << (header.highpass_type ? "butterworth" : "none") << endl;
     os << "low pass filter: " << header.lowpass_corner << " MHz, " << header.lowpass_order << ". order, " << (header.lowpass_type ? "butterworth" : "none") << endl;
     return os;
}

std::ostream& operator<<(std::ostream& os, const NSXDataHeader& header) {
    os << "type " << (int) header.header << " of length " << header.length << " starting at " << header.timestamp << " counts" << endl;
    return os;
}

int main( int argc, const char* argv[] )
{
    if(argc < 2) {
        cerr << "usage: " << argv[0] << " filename.nsx" << endl;
        return EXIT_FAILURE;
    }

    FILE* nsx_file = fopen(argv[1], "rb");

    if(!nsx_file) {
        cerr << "Could not open file: " << argv[1] << endl;
        return EXIT_FAILURE;
    }

    // Read header
    NSXBasicHeader header;
    if(!readStruct<NSXBasicHeader>(&header, nsx_file)) {
        perror("Could not read basic header");
        fclose(nsx_file);
        return EXIT_FAILURE;
    }

    cout << header << endl;

    for(int i = 0; i < header.channel_count; i++) {
        NSXExtensionHeader extension;
        if(!readStruct<NSXExtensionHeader>(&extension, nsx_file)) {
            perror("Could not read extension header");
            fclose(nsx_file);
            return EXIT_FAILURE;
        }
        cout << extension << endl;
    }

    // ToDo: Check data offsets, check if paused, etc.

    fclose(nsx_file);

    return EXIT_SUCCESS;
}
