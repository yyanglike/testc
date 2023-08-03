#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define GPT_HEADER_SIZE 92
#define GPT_ENTRY_SIZE 128
#define GPT_ENTRIES_OFFSET 2

struct GPTHeader {
    char signature[8];          // 8 bytes: GPT Header Signature ("EFI PART")
    unsigned int revision;      // 4 bytes: GPT Header Revision
    unsigned int header_size;   // 4 bytes: GPT Header Size in bytes
    unsigned int header_crc32;  // 4 bytes: CRC32 checksum of GPT Header
    unsigned int reserved;      // 4 bytes: Reserved; must be zero
    unsigned long long int current_lba;   // 8 bytes: LBA (Logical Block Address) of this GPT Header
    unsigned long long int backup_lba;    // 8 bytes: LBA of the Backup GPT Header
    unsigned long long int first_usable_lba;   // 8 bytes: LBA of the first usable block for partitions
    unsigned long long int last_usable_lba;    // 8 bytes: LBA of the last usable block for partitions
    unsigned char disk_guid[16];    // 16 bytes: Disk GUID (Globally Unique Identifier)
    unsigned long long int partition_entries_lba;  // 8 bytes: LBA of the start of partition entries (Partition Table)
    unsigned int num_partition_entries;    // 4 bytes: Number of partition entries (Partition Table size)
    unsigned int size_of_partition_entry;  // 4 bytes: Size of each partition entry
    unsigned int partition_entries_crc32;  // 4 bytes: CRC32 checksum of the Partition Table
};

struct GPTEntry {
    unsigned char type_guid[16];   // 16 bytes: Partition Type GUID
    unsigned char partition_guid[16];  // 16 bytes: Unique Partition GUID
    unsigned long long int starting_lba;    // 8 bytes: LBA of the start of the partition
    unsigned long long int ending_lba;  // 8 bytes: LBA of the last block of the partition (inclusive)
    unsigned long long int attributes; // 8 bytes: Partition attributes (e.g., bootable, read-only, hidden, etc.)
    unsigned short name[72];  // 72 bytes: UTF-16 encoded partition name (36 characters)
};


void print_guid(const unsigned char guid[16]) {
    printf("%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X%02X",
           guid[3], guid[2], guid[1], guid[0],
           guid[5], guid[4],
           guid[7], guid[6],
           guid[8], guid[9],
           guid[10], guid[11], guid[12], guid[13], guid[14], guid[15]);
}

void print_hex(const char* buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%02X", (unsigned char)buffer[i]);
    }
}


int main() {
    const char* disk_device = "/dev/sda"; // Change this to your disk device

    int disk_fd = open(disk_device, O_RDONLY);
    if (disk_fd == -1) {
        perror("Error opening disk device");
        return 1;
    }

    char gpt_header_buffer[GPT_HEADER_SIZE];
    struct GPTHeader* gpt_header;

    // Read GPT header from the disk
    if (pread(disk_fd, gpt_header_buffer, GPT_HEADER_SIZE, 512) != GPT_HEADER_SIZE) {
        perror("Error reading GPT header");
        close(disk_fd);
        return 1;
    }

    // Cast the buffer to the GPTHeader structure
    gpt_header = (struct GPTHeader*)gpt_header_buffer;

    // Print GPT header information
    printf("GPT Disk Signature: ");
    print_hex(gpt_header->signature, sizeof(gpt_header->signature));
    printf("\n");
    printf("GPT Header Revision: %u\n", gpt_header->revision);
    printf("GPT Header Size: %u bytes\n", gpt_header->header_size);
    printf("GPT Header CRC32: 0x%08X\n", gpt_header->header_crc32);
    printf("Reserved: %u\n", gpt_header->reserved);
    printf("Current LBA: %llu\n", gpt_header->current_lba);
    printf("Backup LBA: %llu\n", gpt_header->backup_lba);
    printf("First Usable LBA: %llu\n", gpt_header->first_usable_lba);
    printf("Last Usable LBA: %llu\n", gpt_header->last_usable_lba);
    printf("Disk GUID: ");
    print_guid(gpt_header->disk_guid);
    printf("\n");
    printf("Partition Entries LBA: %llu\n", gpt_header->partition_entries_lba);
    printf("Number of Partition Entries: %u\n", gpt_header->num_partition_entries);
    printf("Size of Partition Entry: %u bytes\n", gpt_header->size_of_partition_entry);
    printf("Partition Entries CRC32: 0x%08X\n", gpt_header->partition_entries_crc32);


    // // Check GPT signature to verify it's a GPT disk
    // if (strncmp(gpt_header->signature, "EFI PART", 8) != 0) {
    //     printf("Not a valid GPT disk.\n");
    //     close(disk_fd);
    //     return 1;
    // }

    // Print GPT header information
    printf("GPT Disk Signature: %s\n", gpt_header->signature);
    // Print other GPT header fields as needed

    // Read GPT entries (partition table)
    char gpt_entry_buffer[GPT_ENTRY_SIZE];
    struct GPTEntry* gpt_entries = (struct GPTEntry*)malloc(GPT_ENTRY_SIZE * gpt_header->num_partition_entries);

    if (pread(disk_fd, gpt_entries, GPT_ENTRY_SIZE * gpt_header->num_partition_entries, GPT_ENTRIES_OFFSET * 512) !=
        GPT_ENTRY_SIZE * gpt_header->num_partition_entries) {
        perror("Error reading GPT entries");
        free(gpt_entries);
        close(disk_fd);
        return 1;
    }



//    struct GPTEntry* gpt_entries = (struct GPTEntry*)gpt_entry_buffer;

    // for (int i = 0; i < gpt_header->num_partition_entries; i++) {
    //     printf("Partition %d\n", i + 1);
    //     printf("Partition Type GUID: ");
    //     print_guid(gpt_entries[i].type_guid);
    //     printf("\n");
    //     printf("Partition GUID: ");
    //     print_guid(gpt_entries[i].partition_guid);
    //     printf("\n");
    //     printf("Starting LBA: %llu\n", gpt_entries[i].starting_lba);
    //     printf("Ending LBA: %llu\n", gpt_entries[i].ending_lba);
    //     printf("Partition Attributes: 0x%016llX\n", gpt_entries[i].attributes);
    //     // Print the partition name (UTF-16 encoded)
    //     printf("Partition Name: ");
    //     for (int j = 0; j < 36; j++) {
    //         printf("%c", (char)gpt_entries[i].name[j]);
    //     }
    //     printf("\n");
    // }    

    // Free allocated memory and close the disk device
    free(gpt_entries);
    close(disk_fd);

    return 0;
}
