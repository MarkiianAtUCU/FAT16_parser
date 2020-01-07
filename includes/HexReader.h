//
// Created by matsiuk on 06-Jan-20.
//

#ifndef FAT_HEXREADER_H
#define FAT_HEXREADER_H

#include <string>
#include <iostream>

class RawData {
    std::string filename;
    void *data_p;
    size_t length;

public:
//    RawData(const std::string &filename);

    RawData(void *data, size_t length) : data_p(data), length(length) {};

    template<size_t dataLength>
    RawData get(size_t offset) {
        if (offset + dataLength > length) {
            throw std::out_of_range{"offset + size > block length"};
        }
        return RawData{(char *) data_p + offset, dataLength};
    }


    template<class T>
    T as() {
        if (length != sizeof(T)) {
            std::cout << length << " " << sizeof(T) << std::endl;
            throw std::bad_cast();
        } else {
            return *((T *) data_p);
        }
    }

    void *data() {
        return data_p;
    }
};

template<>
std::string RawData::as<std::string>() {
    return std::string((char *) data_p, length);
}



#endif //FAT_HEXREADER_H
