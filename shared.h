#include <sstream>
#include <iomanip>

// Read struct from file and return false if failed.
template <typename T>
inline bool readStruct(T* pointer, FILE* file) {
    size_t bytesToRead = sizeof(T);
    size_t bytesRead = fread(pointer, 1, bytesToRead, file);
    return (bytesToRead == bytesRead);
}

// Print array of fixed length properly.
template <unsigned size>
inline std::string as_fixed(const char (&array)[size]) {
    std::stringstream ss;
    for(size_t i = 0; i < size; i++)
        ss << array[i];
    return ss.str();
};


std::string as_version(uint16_t i) {
    std::stringstream ss;
    ss << 'v' << (i >> 7) << '.' << (i & 0xFF);
    return ss.str();
}

// Print windows system time struct
inline std::ostream& operator<<(std::ostream& os, const WindowsSystemTime& time) {
    os << std::setfill('0') << time.year
       << '-' << std::setw(2) << time.month
       << '-' << std::setw(2) << time.day
       << ' ' << std::setw(2) << time.hour
       << ':' << std::setw(2) << time.minute
       << ':' << std::setw(2) << time.second
       << '.' << std::setw(3) << time.milliseconds;
    return os;
};
