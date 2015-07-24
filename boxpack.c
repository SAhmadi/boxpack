//
//  boxpack.c
//  
//
//  @author Sirat Ahmadi
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define min(a, b) ( ((a) < (b)) ? (a) : (b) )
#define max(a, b) ( ((a) > (b)) ? (a) : (b) )

/* STRUCTURES */
typedef struct {
    size_t id;
    int capacity;
    
    size_t numberOfBoxes;
    int *boxSizes;

    char wasChecked;    // 1 == false && 0 == true
} Container;

/* ENUMERATIONS */
typedef enum {
    BEST_FIT, NEXT_FIT, FIRST_FIT, ALMOST_WORST_FIT
} FitTypes;

/* GLOBAL */
size_t numberOfContainers = 0;
size_t numberOfBoxData = 0;
FitTypes fitType = FIRST_FIT;
size_t lastSelectedContainerId = 0;

/* PROTOTYPES */
int inlineAddition(int number1, int number2);
int inlineSubtraction(int number1, int number2);

void openInputFile(char* pInputFilename, FILE **pInputFile);
void getCounts(FILE **pInputFile, Container **pContainers);

void processInputFile(FILE **pInputFile, Container *pContainers);
void setContainer(size_t currentContainerId, char *containerCapacity, Container *pContainers);
void fitBoxes(char *pLineSeperated, Container *pContainers);

/**
 * Main function.
 * Start of application
 * @param argc      Argument count
 * @param argv      Arguments
 */
int main(int argc, char** argv) {
    
    // check if correctly executed
    if (argc != 3) {
        fputs("Input and output file needed!", stderr);
        return 1;
    }
    
   inlineAddition(332, 23);
    
    // input and output files
//    char *pInputFilename = argv[1];
//    char *pOutputFilename = argv[2];
//    
//    /* PROCESS INPUT FILE */
//    FILE *pInputFile = NULL;
//    Container *pContainers = NULL;
//    
//    openInputFile(pInputFilename, &pInputFile);     // open input file
//    getCounts(&pInputFile, &pContainers);           // get Container count and Box data count
//    pContainers = malloc( sizeof(Container) * numberOfContainers);  // allocate memory for Containers
//    processInputFile(&pInputFile, pContainers);     // init Containers
//    fclose(pInputFile);                             // close input file
//    
//    for (int i = 0; i < numberOfContainers; i++) {
//        printf("%d: ", pContainers[i].id);
//        
//        for (int j = 0; j < pContainers[i].numberOfBoxes; j++) {
//            if (j == pContainers[i].numberOfBoxes - 1)
//                printf("%d\n", pContainers[i].boxSizes[j]);
//            else
//                printf("%d ", pContainers[i].boxSizes[j]);
//        }
//    }
//    
//    free(pContainers);
    return 0;
}

/**
 *
 */
int inlineAddition(int number1, int number2) {
    
    printf("%d + %d = ", number1, number2);
    
    __asm__ (
        "addl %%ebx, %%eax;"
        "movl %%eax, %%ecx;"
        : "=c" (number1)
        : "a" (number1), "b" (number2)
    );
    
    printf("%d\n", number1);
    return number1; // Sum is saved in number2
}

/**
 *
 */
int inlineSubtraction(int number1, int number2) {
    int ret;
    
//    __asm__(
//            "movl %0, %%eax;"
//            "movl %1, %%ebx;"
//            "addl %%ebx, %%eax;"
//            "movl %%eax, %%ecx;"
//            : "=g" (ret)
//            : "a" (number1), "b" (number2)
//            : "%eax", "%ebx", "%ecb"
//            );
//    
//    return ret; // Sum is saved in number2
    return 0;
}

/**
 *
 */
void openInputFile(char* pInputFilename, FILE **pInputFile) {
    
    // open input file
    *pInputFile = fopen(pInputFilename, "r");
    
    if (*pInputFile == NULL) {
        fputs("Error: Couldn't open input file!", stderr);
        exit(0);
    }
}

/**
 *
 */
void getCounts(FILE **pInputFile, Container **pContainers) {
    
    // read input file
    char *pLine = NULL;
    char *pTmpLine;
    char *pLineWithContainers = NULL;
    
    size_t lineLength = 0;
    char firstRow = 0;
    
    while ( (getline(&pLine, &lineLength, *pInputFile)) != -1 ) {
        if (firstRow == 0) {
            // count Containers
            pTmpLine = malloc(sizeof(char) * lineLength);    // so pLine doesnt get overidden
            strcpy(pTmpLine, pLine);
            
            pLineWithContainers = strtok(pTmpLine, " \n");
            numberOfContainers++;
            
            while (pLineWithContainers != NULL) {
                pLineWithContainers = strtok(NULL, " \n");
                numberOfContainers++;
            }
            numberOfContainers--;   // subtract one because last container equals \0
            
            firstRow++;  // Return when first line processed
        }
        else {
            // count Box Data
            pTmpLine = (char *) realloc(pTmpLine, sizeof(char) * lineLength);    // so pLine doesnt get overidden
            strcpy(pTmpLine, pLine);
            
            pLineWithContainers = strtok(pTmpLine, " \n");
            numberOfBoxData++;

            while (pLineWithContainers != NULL) {
                pLineWithContainers = strtok(NULL, " \n");
                numberOfBoxData++;
            }
            numberOfBoxData--;   // subtract one because last data equals \0
        }
    }
    rewind(*pInputFile);    // back to start for next method
    
    if (pLine) free(pLine);
    if (pTmpLine) free(pTmpLine);
}

/**
 *
 */
void processInputFile(FILE **pInputFile, Container *pContainers) {
    
    // read input file
    char *pLine = NULL;
    char *pTmpLine;
    char *pLineSeperated = NULL;
    
//    FitTypes fitType = FIRST_FIT;
    
    size_t lineLength = 0;
    size_t stringCounter = 0;
    size_t onRow = 1;
    
    while ( (getline(&pLine, &lineLength, *pInputFile)) != -1 ) {
        
        // init Containers
        if (onRow == 1) {
            // init Containers
            pTmpLine = malloc(sizeof(char) * lineLength);    // so pLine doesnt get overidden
            strcpy(pTmpLine, pLine);
            
            // set first Container manually
            pLineSeperated = strtok(pTmpLine, " \n");
            setContainer(stringCounter++, pLineSeperated, pContainers);
            
            // set rest of Containers
            while (pLineSeperated != NULL) {
                if (stringCounter >= numberOfContainers) break;
                
                pLineSeperated = strtok(NULL, " \n");
                setContainer(stringCounter++, pLineSeperated, pContainers);
            }
            
            onRow++;
            stringCounter = 0;
        }
        
        // process Containers and Box data
        else {
            pTmpLine = malloc(sizeof(char) * lineLength);    // so pLine doesnt get overidden
            strcpy(pTmpLine, pLine);
            pLineSeperated = strtok(pTmpLine, " ");
            
            fitBoxes(pLineSeperated, pContainers);
            stringCounter++;
            
            // set rest of Containers
            while (pLineSeperated != NULL) {
                if (stringCounter >= numberOfBoxData) break;
                
                pLineSeperated = strtok(NULL, " ");
                
                fitBoxes(pLineSeperated, pContainers);
                stringCounter++;
            }
        }
    }
    
    if (pLine) free(pLine);
    if (pTmpLine) free(pTmpLine);
}

/**
 *
 */
void setContainer(size_t currentContainerId, char *containerCapacity, Container *pContainers) {
    
    // Set Container
    pContainers[currentContainerId].id = currentContainerId;
    pContainers[currentContainerId].capacity = strtol(containerCapacity, NULL, 10);
    
    pContainers[currentContainerId].numberOfBoxes = 0;
    pContainers[currentContainerId].boxSizes = NULL;
    
    pContainers[currentContainerId].wasChecked = 1;
}

/**
 *
 */
void fitBoxes(char *pLineSeperated, Container *pContainers) {
    
    if (strcmp(pLineSeperated, "ff") == 0) fitType = FIRST_FIT;
    else if (strcmp(pLineSeperated, "bf") == 0) fitType = BEST_FIT;
    else if (strcmp(pLineSeperated, "nf") == 0) fitType = NEXT_FIT;
    else if (strcmp(pLineSeperated, "awf") == 0) fitType = ALMOST_WORST_FIT;
    
    if (strcmp(pLineSeperated, "ff") != 0 && strcmp(pLineSeperated, "bf") != 0
        && strcmp(pLineSeperated, "nf") != 0 && strcmp(pLineSeperated, "awf") != 0) {
        
        if (atoi(pLineSeperated) > -1) {
            
            // first fit is set
            if (fitType == FIRST_FIT) {
                char foundMatch = 1;
                for (int i = 0; i < numberOfContainers; i++) {
                    pContainers[i].wasChecked = 1;  // unset for NEXT_FIT
                    
                    if (pContainers[i].capacity >= atoi(pLineSeperated)) {
                        foundMatch = 0;
                        
                        pContainers[i].capacity -= atoi(pLineSeperated);
                        
                        pContainers[i].numberOfBoxes++;
                        pContainers[i].boxSizes = realloc(pContainers[i].boxSizes, sizeof(int) * pContainers[i].numberOfBoxes);
                        pContainers[i].boxSizes[pContainers[i].numberOfBoxes - 1] = atoi(pLineSeperated);
                        
                        lastSelectedContainerId = pContainers[i].id;
                        break;
                    }
                }

                if (foundMatch == 1) {
                    printf("validation failed\n");
                    exit(0);
                }
            }
            // best fit is set
            else if (fitType == BEST_FIT) {
                size_t selectedContainerCapacity;
                size_t selectedContainerId;
                
                for (int i = 0; i < numberOfContainers; i++) {
                    pContainers[i].wasChecked = 1;  // unset for NEXT_FIT
                    
                    if (pContainers[i].capacity - atoi(pLineSeperated) >= 0) {
                        if (!selectedContainerCapacity) {
                            selectedContainerId = pContainers[i].id;
                            selectedContainerCapacity = pContainers[i].capacity;
                        }
                        else if (selectedContainerCapacity == pContainers[i].capacity)
                            continue;
                        else {
                            selectedContainerCapacity = min(selectedContainerCapacity, pContainers[i].capacity);
                            
                            if (selectedContainerCapacity == pContainers[i].capacity)
                                selectedContainerId = pContainers[i].id;
                        }
                    }
                }
                
                if (pContainers[selectedContainerId].capacity - atoi(pLineSeperated) >= 0) {
                    pContainers[selectedContainerId].capacity -= atoi(pLineSeperated);
                    
                    pContainers[selectedContainerId].numberOfBoxes++;
                    pContainers[selectedContainerId].boxSizes = realloc(pContainers[selectedContainerId].boxSizes, sizeof(int) * pContainers[selectedContainerId].numberOfBoxes);
                    pContainers[selectedContainerId].boxSizes[pContainers[selectedContainerId].numberOfBoxes - 1] = atoi(pLineSeperated);
                    
                    lastSelectedContainerId = pContainers[selectedContainerId].id;
                }
                else {
                    printf("validation failed\n");
                    exit(0);
                }
            }
            // next fit is set
            else if (fitType == NEXT_FIT) {
                if (pContainers[lastSelectedContainerId].capacity >= atoi(pLineSeperated)) {
                    pContainers[lastSelectedContainerId].capacity -= atoi(pLineSeperated);

                    pContainers[lastSelectedContainerId].numberOfBoxes++;
                    pContainers[lastSelectedContainerId].boxSizes = realloc(pContainers[lastSelectedContainerId].boxSizes, sizeof(int) * pContainers[lastSelectedContainerId].numberOfBoxes);
                    pContainers[lastSelectedContainerId].boxSizes[pContainers[lastSelectedContainerId].numberOfBoxes - 1] = atoi(pLineSeperated);
                }
                else {
                    char allContainersChecked = 1;
                    while (pContainers[lastSelectedContainerId].capacity < atoi(pLineSeperated)) {
                        
                        for (int i = 0; i < numberOfContainers; i++) {
                            // if not all containers checked continue with search
                            if (pContainers[i].wasChecked == 1) {
                                allContainersChecked = 1;
                                break;
                            }
                            else allContainersChecked = 0;
                        }
                        
                        if (allContainersChecked == 1) {
                            lastSelectedContainerId = ((lastSelectedContainerId + 1) % numberOfContainers < numberOfContainers) ? (lastSelectedContainerId + 1) % numberOfContainers : 0;
                            pContainers[lastSelectedContainerId].wasChecked = 0;
                        }
                        else {
                            printf("validation failed\n");
                            exit(0);
                        }
                    }
                    
                    if (pContainers[lastSelectedContainerId].capacity - atoi(pLineSeperated) >= 0) {
                        pContainers[lastSelectedContainerId].capacity -= atoi(pLineSeperated);

                        pContainers[lastSelectedContainerId].numberOfBoxes++;
                        pContainers[lastSelectedContainerId].boxSizes = realloc(pContainers[lastSelectedContainerId].boxSizes, sizeof(int) * pContainers[lastSelectedContainerId].numberOfBoxes);
                        pContainers[lastSelectedContainerId].boxSizes[pContainers[lastSelectedContainerId].numberOfBoxes - 1] = atoi(pLineSeperated);
                    }
                }
            }
            // almost worst fit is set
            else if (fitType == ALMOST_WORST_FIT) {
                size_t maxContainerCapacity = pContainers[0].capacity;
                size_t idOfMaxContainerCapacity = pContainers[0].id;
                
                size_t secondMaxContainerCapacity = pContainers[0].id;
                size_t idOfSecondMaxContainerCapacity = pContainers[0].capacity;
                
                // Max Capacity
                for (int i = 0; i < numberOfContainers; i++) {
                    pContainers[i].wasChecked = 1;  // unset for NEXT_FIT
                
                    maxContainerCapacity = max(maxContainerCapacity, pContainers[i].capacity);
                    if (maxContainerCapacity == pContainers[i].capacity)
                        idOfMaxContainerCapacity = pContainers[i].id;
                }
                
                // Second max capacity
                for (int i = 0; i < numberOfContainers; i++) {
                    if (pContainers[i].id == idOfMaxContainerCapacity) continue;    // Skipt Container with max capacity
                    
                    secondMaxContainerCapacity = max(secondMaxContainerCapacity, pContainers[i].capacity);
                    if (secondMaxContainerCapacity == pContainers[i].capacity)
                        idOfSecondMaxContainerCapacity = pContainers[i].id;
                }
                
                if (pContainers[idOfSecondMaxContainerCapacity].capacity >= atoi(pLineSeperated)) {
                    lastSelectedContainerId = idOfSecondMaxContainerCapacity;
                    
                    pContainers[idOfSecondMaxContainerCapacity].capacity -= atoi(pLineSeperated);
                    
                    pContainers[idOfSecondMaxContainerCapacity].numberOfBoxes++;
                    pContainers[idOfSecondMaxContainerCapacity].boxSizes = realloc(pContainers[idOfSecondMaxContainerCapacity].boxSizes, sizeof(int) * pContainers[idOfSecondMaxContainerCapacity].numberOfBoxes);
                    pContainers[idOfSecondMaxContainerCapacity].boxSizes[pContainers[idOfSecondMaxContainerCapacity].numberOfBoxes - 1] = atoi(pLineSeperated);
                }
                else if (pContainers[idOfMaxContainerCapacity].capacity >= atoi(pLineSeperated)) {
                    lastSelectedContainerId = idOfMaxContainerCapacity;
                    
                    pContainers[idOfMaxContainerCapacity].capacity -= atoi(pLineSeperated);
                    
                    pContainers[idOfMaxContainerCapacity].numberOfBoxes++;
                    pContainers[idOfMaxContainerCapacity].boxSizes = realloc(pContainers[idOfMaxContainerCapacity].boxSizes, sizeof(int) * pContainers[idOfMaxContainerCapacity].numberOfBoxes);
                    pContainers[idOfMaxContainerCapacity].boxSizes[pContainers[idOfMaxContainerCapacity].numberOfBoxes - 1] = atoi(pLineSeperated);
                }
                else {
                    printf("validation failed\n");
                    exit(0);
                }
            }
        }
    }
}











