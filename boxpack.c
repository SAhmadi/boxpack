//Sirat,Ahmadi
//boxpack.c

#define _GNU_SOURCE     // so we can use getline() function later on 
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define min(a, b) ( ((a) < (b)) ? (a) : (b) )   // min-function
#define max(a, b) ( ((a) > (b)) ? (a) : (b) )   // max-function

/* STRUCTURES */
typedef struct
{
    int id;
    int capacity;
    
    int numberOfBoxes;
    int *boxSizes;

    char wasChecked;    // 1 == false && 0 == true
} Container;

/* ENUMERATIONS */
typedef enum
{
    BEST_FIT, NEXT_FIT, FIRST_FIT, ALMOST_WORST_FIT
} FitTypes;

/* GLOBAL */
size_t numberOfContainers = 0;
size_t numberOfBoxData = 0;
size_t numberOfOnlyBoxSizes = 0;
size_t lastSelectedContainerId = 0;
FitTypes fitType = FIRST_FIT;

/* PROTOTYPES */
int inlineAddition(int number1, int number2);
int inlineSubtraction(int number1, int number2);

void openInputFile(char* pInputFilename, FILE **pInputFile);
void getCounts(FILE **pInputFile);

Container* processInputFile(FILE **pInputFile, Container *pContainers);
Container* fitBoxes(char *pLineSeperated, Container *pContainers);

void writeToOutputFile(char* pOutputFilename, FILE **pOutputFile, Container *pContainers);

/**
 * Main function.
 * Start of application
 * @param argc      Argument count
 * @param argv      Arguments
 * @return Application successfully finished.
 */
int main(int argc, char** argv)
{
    // check if correctly executed
    if (argc != 3)
    {
        fputs("Input and output file needed!", stderr);
        return 1;
    }
    
    // input and output files
    char *pInputFilename = argv[1];
    char *pOutputFilename = argv[2];
    
    /* PROCESS INPUT FILE */
    FILE *pInputFile = NULL;    
    openInputFile(pInputFilename, &pInputFile);     // open input file
    getCounts(&pInputFile);                         // get Container count and Box data count
    
    // allocate memory for Containers
    Container *pContainers = malloc( sizeof(Container) * numberOfContainers);  
    
    int i;
    for (i = 0; i < numberOfContainers; i++)
    {
        pContainers[i].numberOfBoxes = 0;
        pContainers[i].boxSizes = malloc(sizeof(int) * numberOfOnlyBoxSizes);  // allocate enough memory so no realloc is needed
    }

    pContainers = processInputFile(&pInputFile, pContainers);       // init Containers
    
    if (pContainers == NULL)
    {
        for (i = 0; i < numberOfContainers; i++)
        {
            free(pContainers[i].boxSizes);
        }
    
        free(pContainers);
        return 0; 
    }
    
    fclose(pInputFile);                                             // close input file
    
    // Output
    FILE *pOutputFile = NULL;
    writeToOutputFile(pOutputFilename, &pOutputFile, pContainers);
    fclose(pOutputFile);   // close output file
    
    for (i = 0; i < numberOfContainers; i++)
    {
        free(pContainers[i].boxSizes);
    }
    
    free(pContainers);
    return 0;
}

/**
 * Addition of two integers in Assembly.
 * @param summand1      First summand
 * @param summand2      Second summand
 * @return Sum of two integers.
 */
int inlineAddition(int summand1, int summand2)
{
    __asm__(
        "addl %%ebx, %%eax;"
        "movl %%eax, %%ecx;"
        : "=c" (summand1)
        : "a" (summand1), "b" (summand2)
    );
    
    return summand1; // Sum is saved in old var
}

/**
 * Subtraction of two integers in Assembly.
 * @param minuend           Minuend
 * @param subtrahend        Subtrahend
 * @return Difference of two integers.
 */
int inlineSubtraction(int minuend, int subtrahend)
{
    __asm__(
        "subl %%ebx, %%eax;"
        "movl %%eax, %%ecx;"
        : "=c" (minuend)
        : "a" (minuend), "b" (subtrahend)
    );
    
    return minuend; // Difference is saved in old var
}

/**
 * Write Container data in output file.
 * @param pOutputFilename       Filename
 * @param pOutputFile           Output file
 * @param pContainers           Containers
 */
void writeToOutputFile(char* pOutputFilename, FILE **pOutputFile, Container *pContainers)
{
    // open output file
    *pOutputFile = fopen(pOutputFilename, "w");
    
    if (*pOutputFile == NULL)
    {
        fputs("Error: Couldn't open output file!\n", stderr);
        exit(1);
    }
    
    int i;
    for (i = 0; i < numberOfContainers; i++)
    {
        fprintf(*pOutputFile, "%d: ", pContainers[i].id);
        
        if (pContainers[i].numberOfBoxes == 0)
        {
            if (i == numberOfContainers - 1) 
            {
                fprintf(*pOutputFile, "0");
                continue;
            }
            else 
            {
                fprintf(*pOutputFile, "0\n");
                continue;
            }
        }
        
        int j;
        for (j = 0; j < pContainers[i].numberOfBoxes; j++)
        {
            if (j == pContainers[i].numberOfBoxes - 1) fprintf(*pOutputFile, "%d\n", pContainers[i].boxSizes[j]);
            else fprintf(*pOutputFile, "%d ", pContainers[i].boxSizes[j]);
        }
    }
}

/**
 * Open input file.
 * @param pInputFilename        Filename
 * @param pInputFile            Input file
 */
void openInputFile(char* pInputFilename, FILE **pInputFile)
{
    // open input file
    *pInputFile = fopen(pInputFilename, "r");
    
    if (*pInputFile == NULL)
    {
        fputs("Error: Couldn't open input file!\n", stderr);
        exit(1);
    }
}

/**
 * Get container count and box data.
 * @param pInputFile        Input file
 */
void getCounts(FILE **pInputFile)
{
    // read input file
    char *pLine = NULL;
    char *pTmpLine;
    char *pLineWithContainers = NULL;
    size_t lineLength = 0;
    
    char rowCounter = 0;
    
    while ((getline(&pLine, &lineLength, *pInputFile)) != -1)
    {
        if (rowCounter == 2)
        {
            fputs("Error: Invalid row count! Only 2 rows allowed.\n", stderr);
            exit(1);
        }
        
        // count Containers
        if (rowCounter == 0)
        {
            pTmpLine = malloc(sizeof(char) * lineLength);    // so pLine doesnt get overidden
            strcpy(pTmpLine, pLine);
            
            pLineWithContainers = strtok(pTmpLine, " \n");

            int i;
            for (i = 0; i < strlen(pLineWithContainers); i++)
            {
                if (!isdigit(pLineWithContainers[i]))
                {
                    fputs("Error: Invalid Container size! Only Integers allowed.\n", stderr);
                    exit(1);
                }
            }

            numberOfContainers = inlineAddition(numberOfContainers, 1);
            
            while (pLineWithContainers != NULL)
            {
                pLineWithContainers = strtok(NULL, " \n");
                
                if (pLineWithContainers != NULL)
                {   
                    numberOfContainers = inlineAddition(numberOfContainers, 1);

                    int i;
                    for (i = 0; i < strlen(pLineWithContainers); i++)
                    {
                        if (!isdigit(pLineWithContainers[i]))
                        {
                            fputs("Error: Invalid container size! Only Integers allowed.\n", stderr);
                            exit(1);
                        }
                    }
                }
            }
                        
            rowCounter++;  // Return when first line processed
            free(pTmpLine);
        }
        // countBox Data
        else
        {
            pTmpLine = malloc(sizeof(char) * lineLength);    // so pLine doesnt get overidden
            strcpy(pTmpLine, pLine);
            
            pLineWithContainers = strtok(pTmpLine, " \n");
            numberOfBoxData = inlineAddition(numberOfBoxData, 1);

            // check for valid input
            int i;
            for (i = 0; i < strlen(pLineWithContainers); i++)
            {
                if (!isdigit(pLineWithContainers[i]))
                {
                    if (strcmp(pLineWithContainers, "ff") != 0 && strcmp(pLineWithContainers, "bf") != 0
                        && strcmp(pLineWithContainers, "nf") != 0 && strcmp(pLineWithContainers, "awf") != 0)
                    {
                        fputs("Error: Invalid box data! Only Integers or supported fit-types allowed.\n", stderr);
                        exit(1);
                    }
                }
            }
            
            if (strcmp(pLineWithContainers, "ff") != 0 && strcmp(pLineWithContainers, "bf") != 0
                && strcmp(pLineWithContainers, "nf") != 0 && strcmp(pLineWithContainers, "awf") != 0) numberOfOnlyBoxSizes = inlineAddition(numberOfOnlyBoxSizes, 1);
            
            while (pLineWithContainers != NULL)
            {
                pLineWithContainers = strtok(NULL, " \n");
            
                numberOfBoxData = inlineAddition(numberOfBoxData, 1);
                if (pLineWithContainers != NULL)
                {                    
                    if (strcmp(pLineWithContainers, "ff") != 0 && strcmp(pLineWithContainers, "bf") != 0
                        && strcmp(pLineWithContainers, "nf") != 0 && strcmp(pLineWithContainers, "awf") != 0) numberOfOnlyBoxSizes = inlineAddition(numberOfOnlyBoxSizes, 1);
                }
            }
            numberOfBoxData = inlineSubtraction(numberOfBoxData, 1);    // subtract one because last data equals \0
            
            rowCounter++;
        }
    }    
    
    rewind(*pInputFile);    // back to start for next method
    
    free(pLine);
    free(pTmpLine);
    free(pLineWithContainers);
}

/**
 * Processing input file
 * @param pInputFile        Input file
 * @param pContainers       Containers
 * @return All containers.
 */
Container* processInputFile(FILE **pInputFile, Container *pContainers)
{
    // read input file
    char *pLine = NULL;
    char *pTmpLine;
    char *pLineSeperated = NULL;
    size_t lineLength = 0;
    
    size_t stringCounter = 0;
    char firstRow = 0;
    
    while ((getline(&pLine, &lineLength, *pInputFile)) != -1)
    {
        // init Containers
        if (firstRow == 0)
        {
            pTmpLine = (char *) malloc(sizeof(char) * lineLength);    // so pLine doesnt get overidden
            strcpy(pTmpLine, pLine);
            
            // set first Container manually
            pLineSeperated = strtok(pTmpLine, " \n");
            
            // Set Container
            pContainers[stringCounter].id = stringCounter;
            pContainers[stringCounter].capacity = strtol(pLineSeperated, NULL, 10);
            //pContainers[stringCounter].numberOfBoxes = 0;
            //pContainers[stringCounter].boxSizes = malloc(sizeof(int) * numberOfOnlyBoxSizes);  // allocate enough memory so no realloc is needed
            pContainers[stringCounter].wasChecked = 1;
            
            stringCounter = inlineAddition(stringCounter, 1);
            
            // set rest of Containers
            while (pLineSeperated != NULL)
            {
                if (stringCounter >= numberOfContainers) break;
                
                pLineSeperated = strtok(NULL, " \n");
                
                pContainers[stringCounter].id = stringCounter;
                pContainers[stringCounter].capacity = strtol(pLineSeperated, NULL, 10);
                //pContainers[stringCounter].numberOfBoxes = 0;
                //pContainers[stringCounter].boxSizes = malloc(sizeof(int) * numberOfOnlyBoxSizes);  // allocate enough memory so no realloc is needed
                pContainers[stringCounter].wasChecked = 1;
                    
                stringCounter = inlineAddition(stringCounter, 1);
            }
            
            firstRow++;
            stringCounter = 0;
            free(pTmpLine);
        }
        // process Containers and Box data
        else
        {
            pTmpLine = malloc(sizeof(char) * lineLength);    // so pLine doesnt get overidden
            strcpy(pTmpLine, pLine);
            
            pLineSeperated = strtok(pTmpLine, " ");
            
            pContainers = fitBoxes(pLineSeperated, pContainers);
            if (pContainers == NULL) return NULL;
            
            stringCounter = inlineAddition(stringCounter, 1);
            
            // set rest of Containers
            while (pLineSeperated != NULL)
            {
                if (stringCounter >= numberOfBoxData) break;
                
                pLineSeperated = strtok(NULL, " ");
                
                pContainers = fitBoxes(pLineSeperated, pContainers);
                if (pContainers == NULL) return NULL;
                stringCounter = inlineAddition(stringCounter, 1);
            }
        }
    }
    
    free(pLine);
    free(pTmpLine);
    //free(pLineSeperated);
    
    return pContainers;
}

/**
 * Sorting boxes into containers.
 * @param pLineSeperated    Box data
 * @param pContainers       Containers
 * @return All containers.
 */
Container* fitBoxes(char *pLineSeperated, Container *pContainers)
{    
    if (strcmp(pLineSeperated, "ff") == 0) fitType = FIRST_FIT;
    else if (strcmp(pLineSeperated, "bf") == 0) fitType = BEST_FIT;
    else if (strcmp(pLineSeperated, "nf") == 0) fitType = NEXT_FIT;
    else if (strcmp(pLineSeperated, "awf") == 0) fitType = ALMOST_WORST_FIT;
    
    if (strcmp(pLineSeperated, "ff") != 0 && strcmp(pLineSeperated, "bf") != 0
        && strcmp(pLineSeperated, "nf") != 0 && strcmp(pLineSeperated, "awf") != 0)
    {
        if (atoi(pLineSeperated) > -1)
        {
            // first fit is set
            if (fitType == FIRST_FIT)
            {
                char foundMatch = 1;
                
                int i;
                for (i = 0; i < numberOfContainers; i++)
                {
                    pContainers[i].wasChecked = 1;  // unset for NEXT_FIT
                    
                    if (pContainers[i].capacity >= atoi(pLineSeperated))
                    {
                        foundMatch = 0;
                        
                        pContainers[i].capacity = inlineSubtraction(pContainers[i].capacity, atoi(pLineSeperated));
                        pContainers[i].numberOfBoxes = inlineAddition(pContainers[i].numberOfBoxes, 1);
                        pContainers[i].boxSizes[pContainers[i].numberOfBoxes - 1] = atoi(pLineSeperated);
                        
                        lastSelectedContainerId = pContainers[i].id;
                        break;
                    }
                }

                if (foundMatch == 1)
                {
                    printf("validation failed\n");
                    return NULL;
                    //exit(1);
                }
            }
            // best fit is set
            else if (fitType == BEST_FIT)
            {
                char firstIteration = 0;
                size_t selectedContainerCapacity;
                size_t selectedContainerId;
                
                int i;
                for (i = 0; i < numberOfContainers; i++)
                {
                    pContainers[i].wasChecked = 1;  // unset for NEXT_FIT
                    
                    if (pContainers[i].capacity - atoi(pLineSeperated) >= 0)
                    {
                        if (firstIteration == 0)
                        {
                            selectedContainerId = pContainers[i].id;
                            selectedContainerCapacity = pContainers[i].capacity;
                            firstIteration++;
                        }
                        else if (selectedContainerCapacity == pContainers[i].capacity)
                            continue;
                        else
                        {
                            selectedContainerCapacity = min(selectedContainerCapacity, pContainers[i].capacity);
                            if (selectedContainerCapacity == pContainers[i].capacity) selectedContainerId = pContainers[i].id;
                        }
                    }
                }
                
                if (pContainers[selectedContainerId].capacity - atoi(pLineSeperated) >= 0)
                {
                    pContainers[selectedContainerId].capacity = inlineSubtraction(pContainers[selectedContainerId].capacity, atoi(pLineSeperated));
                    pContainers[selectedContainerId].numberOfBoxes++;
                    pContainers[selectedContainerId].boxSizes[pContainers[selectedContainerId].numberOfBoxes - 1] = atoi(pLineSeperated);
                    
                    lastSelectedContainerId = pContainers[selectedContainerId].id;
                }
                else
                {
                    printf("validation failed\n");
                    return NULL;
                    //exit(1);
                }
            }
            // next fit is set
            else if (fitType == NEXT_FIT)
            {
                if (pContainers[lastSelectedContainerId].capacity >= atoi(pLineSeperated))
                {
                    pContainers[lastSelectedContainerId].capacity = inlineSubtraction(pContainers[lastSelectedContainerId].capacity, atoi(pLineSeperated));
                    pContainers[lastSelectedContainerId].numberOfBoxes++;
                    pContainers[lastSelectedContainerId].boxSizes[pContainers[lastSelectedContainerId].numberOfBoxes - 1] = atoi(pLineSeperated);
                }
                else
                {
                    char allContainersChecked = 1;
                    while (pContainers[lastSelectedContainerId].capacity < atoi(pLineSeperated))
                    {
                        int i;
                        for (i = 0; i < numberOfContainers; i++)
                        {
                            // if not all containers checked continue with search
                            if (pContainers[i].wasChecked == 1) { allContainersChecked = 1; break; }
                            else allContainersChecked = 0;
                        }
                        
                        if (allContainersChecked == 1)
                        {
                            lastSelectedContainerId = ((lastSelectedContainerId + 1) % numberOfContainers < numberOfContainers) ? (lastSelectedContainerId + 1) % numberOfContainers : 0;
                            pContainers[lastSelectedContainerId].wasChecked = 0;
                        }
                        else
                        {
                            printf("validation failed\n");
                            return NULL;
                            //exit(1);
                        }
                    }
                    
                    if (pContainers[lastSelectedContainerId].capacity - atoi(pLineSeperated) >= 0)
                    {
                        pContainers[lastSelectedContainerId].capacity = inlineSubtraction(pContainers[lastSelectedContainerId].capacity, atoi(pLineSeperated));
                        pContainers[lastSelectedContainerId].numberOfBoxes++;
                        pContainers[lastSelectedContainerId].boxSizes[pContainers[lastSelectedContainerId].numberOfBoxes - 1] = atoi(pLineSeperated);
                    }
                }
            }
            // almost worst fit is set
            else if (fitType == ALMOST_WORST_FIT)
            {
                size_t maxContainerCapacity = pContainers[0].capacity;
                size_t idOfMaxContainerCapacity = pContainers[0].id;
                
                size_t secondMaxContainerCapacity = pContainers[0].id;
                size_t idOfSecondMaxContainerCapacity = pContainers[0].capacity;
                
                // Max Capacity
                int i;
                for (i = 0; i < numberOfContainers; i++)
                {
                    pContainers[i].wasChecked = 1;  // unset for NEXT_FIT
                
                    maxContainerCapacity = max(maxContainerCapacity, pContainers[i].capacity);
                    if (maxContainerCapacity == pContainers[i].capacity)
                        idOfMaxContainerCapacity = pContainers[i].id;
                }
                
                // Second max capacity
                for (i = 0; i < numberOfContainers; i++)
                {
                    if (pContainers[i].id == idOfMaxContainerCapacity) continue;    // Skipt Container with max capacity
                    
                    secondMaxContainerCapacity = max(secondMaxContainerCapacity, pContainers[i].capacity);
                    if (secondMaxContainerCapacity == pContainers[i].capacity)
                        idOfSecondMaxContainerCapacity = pContainers[i].id;
                }
                
                if (pContainers[idOfSecondMaxContainerCapacity].capacity >= atoi(pLineSeperated))
                {
                    lastSelectedContainerId = idOfSecondMaxContainerCapacity;
                    
                    pContainers[idOfSecondMaxContainerCapacity].capacity = inlineSubtraction(pContainers[idOfSecondMaxContainerCapacity].capacity, atoi(pLineSeperated));
                    pContainers[idOfSecondMaxContainerCapacity].numberOfBoxes++;
                    pContainers[idOfSecondMaxContainerCapacity].boxSizes[pContainers[idOfSecondMaxContainerCapacity].numberOfBoxes - 1] = atoi(pLineSeperated);
                }
                else if (pContainers[idOfMaxContainerCapacity].capacity >= atoi(pLineSeperated))
                {
                    lastSelectedContainerId = idOfMaxContainerCapacity;
                    
                    pContainers[idOfMaxContainerCapacity].capacity = inlineSubtraction(pContainers[idOfMaxContainerCapacity].capacity, atoi(pLineSeperated));
                    pContainers[idOfMaxContainerCapacity].numberOfBoxes++;
                    pContainers[idOfMaxContainerCapacity].boxSizes[pContainers[idOfMaxContainerCapacity].numberOfBoxes - 1] = atoi(pLineSeperated);
                }
                else
                {
                    printf("validation failed\n");
                    return NULL;
                    //exit(1);
                }
            }
        }
        else printf("Warning: Boxsize %s is negative and will be ignored in sorting process!\n", pLineSeperated);
    }
    
    return pContainers;
}











