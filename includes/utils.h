//
// Created by markiian on 07.01.20.
//

#ifndef FAT_UTILS_H
#define FAT_UTILS_H

#include "HexReader.h"
#include <bitset>


namespace FAT16 {
    struct bootSector {
        std::string oem;
        int bytesPerSector;
        int sectorPerCluster;
        int reservedArea;
        int FATNum;
        int FATSize;
        int rootDirCapacity;
        int signature;

        bootSector(RawData & image) {
            oem = image.get<8>(3).as<std::string>();
            bytesPerSector = image.get<2>(11).as<ushort>();
            sectorPerCluster = image.get<1>(13).as<uint8_t>();
            reservedArea = image.get<2>(14).as<ushort>();
            FATNum = image.get<1>(16).as<uint8_t>();
            FATSize = image.get<2>(22).as<ushort>();
            rootDirCapacity = image.get<2>(17).as<ushort>();
            signature = image.get<2>(510).as<ushort>();
        }
    };

    struct file {
        uint8_t status;
        std::string filename;
        std::string ext;
        uint8_t attr;
        uint8_t fileCreationTimeMillis;
        ushort fileCreationTimeHMS;
        ushort fileCreationDate;
        ushort fileAccessDate;
        ushort fileModifiedTimeHMS;
        ushort fileModifiedDate;
        ushort address;
        int size;

        file(RawData image) {
            status = image.get<1>(0).as<uint8_t >();
            filename = image.get<8>(0).as<std::string>();
            ext = image.get<3>(8).as<std::string>();
            attr = image.get<1>(11).as<uint8_t >();
            size = image.get<4>(28).as<int>();
            fileCreationTimeMillis = image.get<1>(13).as<uint8_t>();
            fileCreationTimeHMS = image.get<2>(14).as<ushort>();
            fileCreationDate = image.get<2>(16).as<ushort>();
            fileAccessDate = image.get<2>(18).as<ushort>();
            fileModifiedTimeHMS = image.get<2>(22).as<ushort>();
            fileModifiedDate = image.get<2>(24).as<ushort>();
            address = image.get<2>(26).as<ushort>();
        }
    };

}


std::string timeParse(std::bitset<16> b) {
    std::stringstream ss;

    int parsed = 0;
    for (int i = 11; i <= 15; ++i) {
        if (b[i]) {
            parsed += 1 << (i-11);
        }
    }
    ss << std::setfill('0') << std::setw(2) << parsed << ":";

    parsed = 0;
    for (int i = 5; i <= 10; ++i) {
        if (b[i]) {
            parsed += 1 << (i-5);
        }
    }
    ss << std::setfill('0') << std::setw(2) << parsed << ":";

    parsed = 0;
    for (int i = 0; i <= 4; ++i) {
        if (b[i]) {
            parsed += 1 << i;
        }
    }
    ss << std::setfill('0') << std::setw(2) << parsed*2;

    return ss.str();
}


std::string dateParse(std::bitset<16> b) {
    std::stringstream ss;

    int parsed = 0;
    for (int i = 0; i <= 4; ++i) {
        if (b[i]) {
            parsed += 1 << i;
        }
    }
    ss << parsed << ".";

    parsed = 0;
    for (int i = 5; i <= 8; ++i) {
        if (b[i]) {
            parsed += 1 << (i-5);
        }
    }
    ss << parsed << ".";

    parsed = 0;
    for (int i = 9; i <= 15; ++i) {
        if (b[i]) {
            parsed += 1 << (i - 9);
        }
    }
    ss << 1980+parsed;

    return ss.str();
}

std::string attributeParse(std::bitset<8> b, std::map<uint8_t, std::string> map) {
    std::stringstream ss;
    for ( auto & entry : map) {
//        std::cout << "\n===\n" << b << "\n" << std::bitset<8>(entry.first) << "\n===\n";
        auto attrBitset = std::bitset<8>(entry.first);
        if ((b & attrBitset).count() == attrBitset.count()) {
            ss << entry.second;
        } else {
            ss << "    ";
        }
    }

//    ss << std::endl<< b  << std::endl;
//    ss << std::endl<< (b & std::bitset<8>(0x01 )).count() << std::endl;

    return ss.str();
}

void printFileDescription(FAT16::file & file, std::map<uint8_t, std::string> map) {
    std::stringstream ss;
    ss << file.filename;
    if (file.size == 0) {
        ss << "        <dir>";
    } else {
        ss << "." << file.ext << "    " << std::setfill(' ') << std::setw(5) <<file.size ;
    }
    ss << "    " << std::setfill(' ') << std::setw(10) << dateParse(std::bitset<16>(file.fileModifiedDate)) << " ";
    ss << timeParse(std::bitset<16>(file.fileModifiedTimeHMS)) ;
    ss << "   "  << attributeParse( file.attr , map) << "    ";
    ss << std::setfill(' ') << std::setw(5) << file.address << "    "<< std::endl;

//    std::cout << std::bitset<16>(file.fileModifiedDate) << std::endl;
//    std::cout << bitsetPart<10,16>(std::bitset<16>(file.fileModifiedDate))<< std::endl;
    std::cout << ss.str();

}
#endif //FAT_UTILS_H
