#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

void parse_partition_info(const char *line, char *device_name, int *partition_number) {
    // Format of /proc/partitions:
    //   major minor  #blocks  name
    //   8        0   312571224 sda
    // int major,minor,blocks;
    // // sscanf(line, "%*u %*u %*u %s%d",major,minor,blocks, device_name, partition_number);
    // sscanf(line, "%*d %*d %*d %[^0-9]%d", device_name, partition_number);
    // printf("%s\t%d\n",device_name,partition_number);


    int major, minor, blocks;
    char full_name[128];

    // Read the values of major, minor, blocks, and the full_name.
    sscanf(line, "%d %d %d %s", &major, &minor, &blocks, full_name);

    // Extract the device name and partition number.
    int len = strlen(full_name);
    int i = len - 1;
    while (i >= 0 && full_name[i] >= '0' && full_name[i] <= '9') {
        i--;
    }
    if (i == len - 1) {
        // No partition number found, set it to -1
        *partition_number = -1;
    } else {
        *partition_number = atoi(&full_name[i + 1]);
    }

    // Copy the device name without the partition number
    strncpy(device_name, full_name, i + 1);
    device_name[i + 1] = '\0';
}

unsigned long long read_partition_start_sector(const char *device_name, int partition_number) {
    unsigned long long start_sector = 0;
    char path[MAX_LINE_LENGTH];
    FILE *fp;

    // Create the path to read start sector
    snprintf(path, sizeof(path), "/sys/block/%s/%s%d/start", device_name, device_name, partition_number);
    printf("%s\n",path);

    // Open the file to read start sector
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Error opening start sector file");
        exit(1);
    }

    // Read start sector value
    fscanf(fp, "%llu", &start_sector);

    // Close the file
    fclose(fp);

    return start_sector;
}

unsigned long long read_partition_size(const char *device_name, int partition_number) {
    unsigned long long size = 0;
    char path[MAX_LINE_LENGTH];
    FILE *fp;

    // Create the path to read partition size
    snprintf(path, sizeof(path), "/sys/block/%s/%s%d/size", device_name, device_name, partition_number);

    // Open the file to read partition size
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Error opening partition size file");
        exit(1);
    }

    // Read partition size value
    fscanf(fp, "%llu", &size);

    // Close the file
    fclose(fp);

    return size;
}

int main() {
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    char device_name[128];
    int partition_number;

    // Open /proc/partitions file
    fp = fopen("/proc/partitions", "r");
    if (fp == NULL) {
        perror("Error opening /proc/partitions");
        return 1;
    }

    // Skip the header line
    fgets(line, sizeof(line), fp);
    fgets(line, sizeof(line), fp);
    // fgets(line, sizeof(line), fp);

    // Read and print partition information
    while (fgets(line, sizeof(line), fp) != NULL) {
        // Skip empty lines or lines that don't contain partition information
        if (strlen(line) <= 1 || line[0] == '\n')
            continue;        
        parse_partition_info(line, device_name, &partition_number);
        if (partition_number == -1) {
            // Failed to parse partition info, skip this line
            continue;
        }            
        printf("Device: %s%d\n", device_name, partition_number);

        // Read and print start sector
        unsigned long long start_sector = read_partition_start_sector(device_name, partition_number);
        printf("Start Sector: %llu\n", start_sector);

        // Read and print partition size
        unsigned long long partition_size = read_partition_size(device_name, partition_number);
        printf("Partition Size: %llu sectors\n", partition_size);

        printf("\n");
    }

    // Close the file
    fclose(fp);

    return 0;
}
