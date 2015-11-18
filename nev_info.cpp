#include <cstdlib>
#include <cstdio>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

#include "blackrock.h"
#include "shared.h"

std::ostream& operator<<(std::ostream& os, const NEVBasicHeader& header) {
    os << as_fixed(header.file_type) << " " << as_version(header.file_spec) << " created with '" << header.application << "' on " << header.time << endl;
    os << "  time resolution: " << header.global_time_resolution << " samples/second" << endl;
    os << "  waveform resolution: " << header.waveform_time_resolution << " samples/second" << endl;
    os << "  comment: '" << header.comment << "'" << endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const NEVNeuralWaveformExtensionData& data) {
    os << "  electrode id: " << data.id << " (" << (char) (data.bank + '@') << (int)  data.pin << ")" << endl;
    os << "  digitization factor: " << data.factor << " nV/LSB" << endl;
    os << "  energy threshold: ";
    if(data.threshold)
        os << data.threshold;
    else
        os << "none";
    os << endl;
    os << "  high threshold: " << data.high_threshold << " uV" << endl;
    os << "  low  threshold: " << data.low_threshold << " uV" << endl;
    os << "  unit count: ";
    if(data.units)
        os << (int) data.units;
    else
        os << "classification disabled";
    os << endl;
    os << "  spike waveform: " << data.width << " samples of " << (data.size ? data.size : 1) << " bytes" << endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const NEVNeuralLabelExtensionData& config) {
    os << "  electrode id: " << config.id << endl;
    os << "  label: " << config.label << endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const NEVNeuralFilterExtensionData& config) {
    os << "  electrode id: " << config.id << endl;
    os << "  high pass filter: " << config.highpass_corner << " MHz, " << config.highpass_order << ". order, " << (config.highpass_type ? "butterworth" : "none") << endl;
    os << "  low  pass filter: " << config.lowpass_corner << " MHz, " << config.lowpass_order << ". order, " << (config.lowpass_type ? "butterworth" : "none") << endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const NEVDigitalLabelExtensionData& config) {
    os << "  " << (config.mode ? "parallel" : "serial") << " port label: " << config.label << endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const NEVVideoSyncExtensionData& config) {
    os << "  source id: " << config.id << endl;
    os << "  name: " << config.name << endl;
    os << "  fps: " << config.rate << endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const NEVTrackableObjectExtensionData& config) {
    os << "  type: ";
    switch(config.type) {
        case 0:
            os << "undefined";
            break;
        case 1:
            os << "2D body tracking with markers";
            break;
        case 2:
            os << "2D body tracking with blobs";
            break;
        case 3:
            os << "3D body tracking with markers";
            break;
        case 4:
            os << "2D boundary tracking";
            break;
        default:
            os << "unknown";
    }
    os << endl;
    os << "  trackable id: " << config.id << endl;
    os << "  point count: " << config.count << endl;
    os << "  name: " << config.name << endl;
    return os;
}

int main( int argc, const char* argv[] )
{
    if(argc < 2) {
        cerr << "usage: " << argv[0] << " filename.nev" << endl;
        return EXIT_FAILURE;
    }

    FILE* nev_file = fopen (argv[1], "rb");

    if(!nev_file) {
        cerr << "Could not open file: " << argv[1] << endl;
        return EXIT_FAILURE;
    }

    // Read header
    NEVBasicHeader header;
    if(!readStruct<NEVBasicHeader>(&header, nev_file)) {
        perror ("Could not read basic header");
        fclose(nev_file);
        return EXIT_FAILURE;
    }

    cout << header << endl;

    for(int i = 0; i < header.extension_count; i++) {
        NEVExtensionHeader extension;
        if(!readStruct<NEVExtensionHeader>(&extension, nev_file)) {
            perror ("Could not read extension header");
            fclose(nev_file);
            return EXIT_FAILURE;
        }

        cout << "Extension Header '" << as_fixed(extension.id) << "':" << endl;

        if(!strncmp(extension.id, NEVArrayNameID, 8)){
            cout << "electrode array name: " << extension.data << endl;
        } else if(!strncmp(extension.id, NEVExtraCommentID, 8)){
            cout << "extra comment: " << extension.data << endl;
        } else if(!strncmp(extension.id, NEVContinuedCommentID, 8)){
            cout << "continued comment: " << extension.data << endl;
        } else if(!strncmp(extension.id, NEVMapFileID, 8)){
            cout << "map file name: " << extension.data << endl;
        } else if(!strncmp(extension.id, NEVNeuralWaveformID, 8)){
            cout << *reinterpret_cast<NEVNeuralWaveformExtensionData*>(extension.data) << endl;
        } else if(!strncmp(extension.id, NEVNeuralLabelID, 8)){
            cout << *reinterpret_cast<NEVNeuralLabelExtensionData*>(extension.data) << endl;
        } else if(!strncmp(extension.id, NEVNeuralFilterID, 8)){
            cout << *reinterpret_cast<NEVNeuralFilterExtensionData*>(extension.data) << endl;
        } else if(!strncmp(extension.id, NEVDigitalLabel, 8)){
            cout << *reinterpret_cast<NEVDigitalLabelExtensionData*>(extension.data) << endl;
        } else if(!strncmp(extension.id, NEVVideoSyncID, 8)){
            cout << *reinterpret_cast<NEVVideoSyncExtensionData*>(extension.data) << endl;
        } else if(!strncmp(extension.id, NEVTrackableObjectID, 8)){
            cout << *reinterpret_cast<NEVTrackableObjectExtensionData*>(extension.data) << endl;
        } else {
            cout << "Could not parse data: Unknown extension header." << endl << endl;
        }
    }
    fclose(nev_file);

    return EXIT_SUCCESS;
}
