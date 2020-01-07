#include <iostream>
#include "HexReader.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <iomanip>
#include <map>
#define BOOT_SECOR_SIZE 512
#define DIR_ENTRY_SIZE 32

#include "utils.h"

//std::string s = std::bitset< 64 >( 12345 ).to_string(); // string conversion
//std::cout << std::bitset< 64 >( 54321 ) << ' '; // direct output

int main() {
    std::map<uint8_t, std::string> attributeNames;
    attributeNames[0x01] = std::string{"Read only"};
    attributeNames[0x02] = std::string{"Hidden file"};
    attributeNames[0x04] = std::string{"System file"};
    attributeNames[0x08] = std::string{"Volume label"};
    attributeNames[0x0f] = std::string{"Long file name"};
    attributeNames[0x10] = std::string{"Directiry"};
    attributeNames[0x20] = std::string{"Archive"};

    int fd = open("/home/markiian/Downloads/hd0_just_FAT16_without_MBR.img", O_RDONLY);

    void * dataBootSector = mmap(NULL, BOOT_SECOR_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);
    RawData imageBootSector{dataBootSector, BOOT_SECOR_SIZE};
    FAT16::bootSector bootSector(imageBootSector);
    munmap(dataBootSector, BOOT_SECOR_SIZE);

    std::cout << "== " << bootSector.oem << " ==" << std::endl;
    std::cout << "Bytes per sector:      " << bootSector.bytesPerSector << std::endl;
    std::cout << "Sector per cluster:    " << bootSector.sectorPerCluster << std::endl;
    std::cout << "FAT copies:            " << bootSector.FATNum << std::endl;
    std::cout << "FAT size [sectors]:    " << bootSector.FATSize << std::endl;
    std::cout << "           [bytes]:    " << bootSector.FATSize * bootSector.bytesPerSector << std::endl;
    std::cout << "Root dir entries:      " << bootSector.rootDirCapacity << std::endl;
    std::cout << "Root dir size [bytes]: " << bootSector.rootDirCapacity * DIR_ENTRY_SIZE << std::endl;
    std::cout << "Reserved sectors:      " << bootSector.reservedArea << std::endl;
    std::cout << "Signature correct:     " << ((bootSector.signature == 0xAA55) ? "True" : "False") << std::endl;

    std::cout << std::endl << "FILE TREE" << std::endl;
    auto pageSize = sysconf(_SC_PAGE_SIZE);
    size_t desiredOffset = BOOT_SECOR_SIZE + bootSector.FATNum * bootSector.FATSize * bootSector.bytesPerSector;
    size_t validOffset = (desiredOffset / pageSize) * pageSize;
    size_t additionalOffset = desiredOffset - validOffset;
    void * dataRootDir = mmap(NULL,
                              bootSector.rootDirCapacity * DIR_ENTRY_SIZE + additionalOffset,
                              PROT_READ, MAP_PRIVATE, fd,
                              validOffset);
    RawData imageDir((char * )dataRootDir+additionalOffset, bootSector.rootDirCapacity * DIR_ENTRY_SIZE);

    for (int i = 0; i < bootSector.rootDirCapacity; ++i) {
        auto file = FAT16::file{imageDir.get<DIR_ENTRY_SIZE>(i * DIR_ENTRY_SIZE)};
        if (file.status == 0x00) {
            break;
        }
        printFileDescription(file);
    }

    munmap(dataRootDir, bootSector.rootDirCapacity * DIR_ENTRY_SIZE + additionalOffset);
    close(fd);
    return 0;
}
