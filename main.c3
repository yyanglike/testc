#include <stdio.h>
#include <stdlib.h>

typedef struct PartitionInfo {
    char device[20];
    unsigned int major;
    unsigned int minor;
    unsigned long long blocks;
    char name[20];
} PartitionInfo;

// Function to read partition information
PartitionInfo* read_partitions_info(int* num_partitions) {
    FILE* file = fopen("/proc/partitions", "r");
    if (file == NULL) {
        perror("Error opening /proc/partitions");
        return NULL;
    }

    int max_partitions = 100; // Set a maximum number of partitions to read
    PartitionInfo* partitions = (PartitionInfo*)malloc(max_partitions * sizeof(PartitionInfo));
    if (partitions == NULL) {
        perror("Memory allocation error");
        fclose(file);
        return NULL;
    }

    char buffer[256];
    int count = 0;

    // Skip the header line
    fgets(buffer, sizeof(buffer), file);

    while (fgets(buffer, sizeof(buffer), file)) {
        int res = sscanf(buffer, " %u %u %llu %s", &partitions[count].major, &partitions[count].minor,
                         &partitions[count].blocks, partitions[count].device);
        if (res == 4) {
            snprintf(partitions[count].name, sizeof(partitions[count].name), "/dev/%s",
                     partitions[count].device);
            count++;
            if (count >= max_partitions)
                break;
        }
    }

    fclose(file);
    *num_partitions = count;
    return partitions;
}

// Function to print partition information
void print_partitions_info(PartitionInfo* partitions, int num_partitions) {
    printf("%-8s %-8s %-10s %-10s %s\n", "Major", "Minor", "Blocks", "Device", "Name");
    for (int i = 0; i < num_partitions; i++) {
        printf("%-8u %-8u %-10llu %-10s %s\n", partitions[i].major, partitions[i].minor,
               partitions[i].blocks, partitions[i].device, partitions[i].name);
    }
}

// Function to release memory allocated for partition information
void free_partitions_info(PartitionInfo* partitions) {
    free(partitions);
}

int main() {
    int num_partitions = 0;
    PartitionInfo* partitions = read_partitions_info(&num_partitions);

    if (partitions != NULL) {
        printf("Total partitions found: %d\n", num_partitions);
        print_partitions_info(partitions, num_partitions);
        free_partitions_info(partitions);
    }

    return 0;
}
