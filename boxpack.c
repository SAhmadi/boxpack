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
    long id;
    long capacity;
    
    long numberOfBoxes;
    long *boxSizes;

    char wasChecked;    // 1 == false && 0 == true
} Container;

/* ENUMERATIONS */
typedef enum
{
    BEST_FIT, NEXT_FIT, FIRST_FIT, ALMOST_WORST_FIT
} FitTypes;

/* GLOBAL */
long numberOfContainers = 0;
long numberOfBoxData = 0;
long numberOfOnlyBoxSizes = 0;
long lastSelectedContainerId = 0;
FitTypes fitType = FIRST_FIT;

/* PROTOTYPES */
int inlineAddition(int summand1, int summand2);
int inlineSubtraction(int minuend, int subtrahend);

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
        fputs("Input and output files needed!", stderr);
        return 1;
    }
    
    // input and output files
    char *pInputFilename = argv[1];
    char *pOutputFilename = argv[2];
    
    /* PROCESS INPUT FILE */
    FILE *pInputFile = NULL;  
      
    // open input file
    pInputFile = fopen(pInputFilename, "r");
    if (pInputFile == NULL)
    {
        fputs("Error: Couldn't open input file!\n", stderr);
        return 1;
    }
    
    // get Container count and Box data count
    getCounts(&pInputFile);
    
    // allocate memory for Containers
    Container *pContainers = malloc(sizeof(Container) * numberOfContainers);
    
    // set boxSize property of Containers
    size_t i;
    for (i = 0; i < numberOfContainers; i++)
    {
        pContainers[i].id = i;
        pContainers[i].capacity = 0;
        pContainers[i].wasChecked = 1; // = false
        pContainers[i].numberOfBoxes = 0;
        pContainers[i].boxSizes = malloc(sizeof(int) * numberOfOnlyBoxSizes);  // allocate enough memory so no realloc is needed
    }
    pContainers = processInputFile(&pInputFile, pContainers);       // init Containers
    if (pContainers == NULL) 
    {
        printf("main received NULL\n");
        fclose(pInputFile);

        free(pContainers);
        printf("app termination\n");
        exit(1);
    }
   
    // close input file
    fclose(pInputFile);
    
    // Output
    FILE *pOutputFile = NULL;
    writeToOutputFile(pOutputFilename, &pOutputFile, pContainers);
    fclose(pOutputFile);   // close output file
    
    // free Containers
    for (i = 0; i < numberOfContainers; i++)
    {
        free(pContainers[i].boxSizes);
    }
    
    free(pContainers);
    return 0;
}

/**
 * Addition of two integers in Assembler-Language.
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
    
    return summand1; // Sum is saved in old variable
}

/**
 * Subtraction of two integers in Assembler-Language.
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
    
    return minuend; // Difference is saved in old variable
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
    
    size_t i;
    for (i = 0; i < numberOfContainers; i++)
    {
        // print ID
        fprintf(*pOutputFile, "%ld: ", pContainers[i].id);
        
        // print "0" for no boxes
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
        
        // print all containing boxes
        int j;
        for (j = 0; j < pContainers[i].numberOfBoxes; j++)
        {
            if (j == pContainers[i].numberOfBoxes - 1)
                fprintf(*pOutputFile, "%ld\n", pContainers[i].boxSizes[j]);
            else
                fprintf(*pOutputFile, "%ld ", pContainers[i].boxSizes[j]);
        }
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
    char *pLineSeperated = NULL;
    size_t lineLength = 0;
    
    char rowCounter = 0;
    
    // Iterate through lines
    while ((getline(&pLine, &lineLength, *pInputFile)) != -1)
    {
        if (rowCounter == 2)
        {
            fputs("Error: Invalid row count! Only 2 rows allowed.\n", stderr);
            free(pLine);
            free(pTmpLine);
            fclose(*pInputFile);
            exit(1);
        }
        
        // count Containers in first row
        if (rowCounter == 0)
        {
            pTmpLine = malloc(sizeof(char) * lineLength);    // so pLine doesnt get overidden by strcpy
            strcpy(pTmpLine, pLine);
            
            // seperate line and iterate through FIRST element
            // needs to be handled manually because of strtok!
            pLineSeperated = strtok(pTmpLine, " \n");   
            size_t i;
            for (i = 0; i < strlen(pLineSeperated); i++)
            {
                if (!isdigit(pLineSeperated[i]))
                {
                    fputs("Error: Invalid Container size! Only Integers allowed.\n", stderr);
                    free(pTmpLine);
                    free(pLine);
                    fclose(*pInputFile);
                    exit(1);
                }
            }

            // increment number of containers
            numberOfContainers = inlineAddition(numberOfContainers, 1);
            
            // sperate line and iterate through REST elements
            while (pLineSeperated != NULL)
            {
                pLineSeperated = strtok(NULL, " \n");
                
                if (pLineSeperated != NULL)
                {   
                    numberOfContainers = inlineAddition(numberOfContainers, 1);

                    size_t j;
                    for (j = 0; j < strlen(pLineSeperated); j++)
                    {
                        if (!isdigit(pLineSeperated[j]))
                        {
                            fputs("Error: Invalid container size! Only Integers allowed.\n", stderr);
                            free(pTmpLine);
                            free(pLine);
                            fclose(*pInputFile);
                            exit(1);
                        }
                    }
                }
            }
                        
            rowCounter++;  // Return when first line processed
            free(pTmpLine);
        }
        // count Box-Data in second line
        else
        {
            pTmpLine = malloc(sizeof(char) * lineLength);    // so pLine doesnt get overidden by strcpy
            strcpy(pTmpLine, pLine);
            
            // seperate line to get FIRST element
            // needs to be handled manually because of strtok!
            pLineSeperated = strtok(pTmpLine, " \n");

            // increment number of box data
            numberOfBoxData = inlineAddition(numberOfBoxData, 1);

            // check for valid input
            size_t i;
            for (i = 0; i < strlen(pLineSeperated); i++)
            {             
                if (!isdigit(pLineSeperated[i]))
                {
                    if (strcmp(pLineSeperated, "ff") != 0 && strcmp(pLineSeperated, "bf") != 0
                        && strcmp(pLineSeperated, "nf") != 0 && strcmp(pLineSeperated, "awf") != 0)
                    {
                        fputs("Error: Invalid fit-type! Only supported fit-types allowed.\n", stderr);
                        free(pTmpLine);
                        free(pLine);
                        fclose(*pInputFile);
                        exit(1);
                    }
                }
            }
            
            // check for valid box-size
            if (strcmp(pLineSeperated, "ff") != 0 && strcmp(pLineSeperated, "bf") != 0
                        && strcmp(pLineSeperated, "nf") != 0 && strcmp(pLineSeperated, "awf") != 0
                        && strtol(pLineSeperated, NULL, 10) < 0) 
            {
                fputs("Error: Invalid box size! Only Integers > 0 allowed.\n", stderr);
                free(pTmpLine);
                free(pLine);
                fclose(*pInputFile);
                exit(1);
            }
            else
                numberOfOnlyBoxSizes = inlineAddition(numberOfOnlyBoxSizes, 1);
            
            // seperate line and iterate through REST elements
            while (pLineSeperated != NULL)
            {
                pLineSeperated = strtok(NULL, " \n");

                // increment number of box-data and only-box-sizes
                numberOfBoxData = inlineAddition(numberOfBoxData, 1);
                if (pLineSeperated != NULL)
                {                    
                    // check for valid box-size
                    size_t i;
                    for (i = 0; i < strlen(pLineSeperated); i++)
                    {             
                        if (!isdigit(pLineSeperated[i]))
                        {
                            if (strcmp(pLineSeperated, "ff") != 0 && strcmp(pLineSeperated, "bf") != 0
                                && strcmp(pLineSeperated, "nf") != 0 && strcmp(pLineSeperated, "awf") != 0)
                            {
                                fputs("Error: Invalid fit-type! Only supported fit-types allowed.\n", stderr);
                                free(pTmpLine);
                                free(pLine);
                                fclose(*pInputFile);
                                exit(1);
                            }
                        }
                    }

                    if (strcmp(pLineSeperated, "ff") != 0 && strcmp(pLineSeperated, "bf") != 0
                                && strcmp(pLineSeperated, "nf") != 0 && strcmp(pLineSeperated, "awf") != 0
                                && strtol(pLineSeperated, NULL, 10) < 0) 
                    {
                        fputs("Error: Invalid box size! Only Integers > 0 allowed.\n", stderr);
                        free(pTmpLine);
                        free(pLine); 
                        fclose(*pInputFile);
                        exit(1);
                    }
                    else
                        numberOfOnlyBoxSizes = inlineAddition(numberOfOnlyBoxSizes, 1);
                }
            }
            numberOfBoxData = inlineSubtraction(numberOfBoxData, 1);    // subtract one because last data equals \0
            
            rowCounter++;
        }
    }    
    
    rewind(*pInputFile);    // back to start for next method
    
    free(pTmpLine);
    free(pLine); 
    free(pLineSeperated);
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
            
            pContainers[stringCounter].id = stringCounter;
            pContainers[stringCounter].capacity = strtol(pLineSeperated, NULL, 10);
            pContainers[stringCounter].wasChecked = 1;
            
            // increment counter
            stringCounter = inlineAddition(stringCounter, 1);
            
            // set rest of Containers
            while (pLineSeperated != NULL)
            {
                if (stringCounter >= numberOfContainers) break;
                
                pLineSeperated = strtok(NULL, " \n");
                
                pContainers[stringCounter].id = stringCounter;
                pContainers[stringCounter].capacity = strtol(pLineSeperated, NULL, 10);
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
            if (pContainers == NULL) 
            {
                printf("processInput received NULL\n");
                free(pLine);
                free(pContainers);
                free(pTmpLine);
                printf("processInput send NULL\n");
                return NULL;
            }

            stringCounter = inlineAddition(stringCounter, 1);
            
            // set rest of Containers
            while (pLineSeperated != NULL)
            {
                if (stringCounter >= numberOfBoxData) break;
                
                pLineSeperated = strtok(NULL, " ");
                pContainers = fitBoxes(pLineSeperated, pContainers);
                if (pContainers == NULL) 
                {
                    printf("processInput received NULL\n");
                    free(pLine);
                    free(pContainers);
                    free(pTmpLine);
                    printf("processInput send NULL\n");
                    return NULL;
                }
                
                stringCounter = inlineAddition(stringCounter, 1);
            }
            
            free(pTmpLine);
        }
    }
    
    free(pLine);
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
        if (strtol(pLineSeperated, NULL, 10) > -1)
        {
            // first fit is set
            if (fitType == FIRST_FIT)
            {
                char foundMatch = 1;    // 1 == false, 0 == true
                
                size_t i;
                for (i = 0; i < numberOfContainers; i++)
                {
                    pContainers[i].wasChecked = 1;  // unset for NEXT_FIT
                    
                    // successfull fit
                    if (pContainers[i].capacity >= strtol(pLineSeperated, NULL, 10))
                    {
                        foundMatch = 0;
                        
                        pContainers[i].capacity = inlineSubtraction(pContainers[i].capacity, strtol(pLineSeperated, NULL, 10));
                        pContainers[i].numberOfBoxes = inlineAddition(pContainers[i].numberOfBoxes, 1);
                        pContainers[i].boxSizes[pContainers[i].numberOfBoxes - 1] = strtol(pLineSeperated, NULL, 10);
                        
                        // save selectionID for NEXT-FIT
                        lastSelectedContainerId = pContainers[i].id;
                        break;
                    }
                }

                // failed fit
                if (foundMatch == 1)
                {
                    printf("validation failed\n");

                    size_t i;
                    for (i = 0; i < numberOfContainers; ++i)
                    {
                        free(pContainers[i].boxSizes);
                    }
                    free(pContainers);
                    printf("return NULL from FF\n");
                    return NULL;
                }
            }
            // best fit is set
            else if (fitType == BEST_FIT)
            {
                char firstIteration = 0;
                int selectedContainerCapacity = pContainers[0].capacity;
                int selectedContainerId = pContainers[0].id;
                
                size_t i;
                for (i = 0; i < numberOfContainers; i++)
                {
                    pContainers[i].wasChecked = 1;  // unset for NEXT_FIT
                    
                    if (pContainers[i].capacity - strtol(pLineSeperated, NULL, 10) >= 0)
                    {
                        if (firstIteration == 0)    // init selector variables in first iteration
                        {
                            selectedContainerId = pContainers[i].id;
                            selectedContainerCapacity = pContainers[i].capacity;
                            firstIteration++;
                        }
                        else if (selectedContainerCapacity == pContainers[i].capacity)  // if capacity is equal choose current selection
                            continue;
                        else    // choose the smaller capacity
                        {
                            selectedContainerCapacity = min(selectedContainerCapacity, pContainers[i].capacity);
                            if (selectedContainerCapacity == pContainers[i].capacity) 
                                selectedContainerId = pContainers[i].id;
                        }
                    }
                }
                
                // successfull fit
                if (pContainers[selectedContainerId].capacity - strtol(pLineSeperated, NULL, 10) >= 0)
                {
                    pContainers[selectedContainerId].capacity = inlineSubtraction(pContainers[selectedContainerId].capacity, strtol(pLineSeperated, NULL, 10));
                    pContainers[selectedContainerId].numberOfBoxes++;
                    pContainers[selectedContainerId].boxSizes[pContainers[selectedContainerId].numberOfBoxes - 1] = strtol(pLineSeperated, NULL, 10);
                    
                    // save selectionID for NEXT-FIT
                    lastSelectedContainerId = pContainers[selectedContainerId].id;
                }
                // failed fit
                else
                {
                    printf("validation failed\n");

                    size_t i;
                    for (i = 0; i < numberOfContainers; ++i)
                    {
                        free(pContainers[i].boxSizes);
                    }
                    free(pContainers);
                    printf("return NULL from BF\n");
                    return NULL;
                }
            }
            // next fit is set
            else if (fitType == NEXT_FIT)
            {
                // next fit is already selected
                if (pContainers[lastSelectedContainerId].capacity >= strtol(pLineSeperated, NULL, 10))
                {
                    pContainers[lastSelectedContainerId].capacity = inlineSubtraction(pContainers[lastSelectedContainerId].capacity, strtol(pLineSeperated, NULL, 10));
                    pContainers[lastSelectedContainerId].numberOfBoxes++;
                    pContainers[lastSelectedContainerId].boxSizes[pContainers[lastSelectedContainerId].numberOfBoxes - 1] = strtol(pLineSeperated, NULL, 10);
                }
                else
                {
                    char allContainersChecked = 1;
                    while (pContainers[lastSelectedContainerId].capacity < strtol(pLineSeperated, NULL, 10))
                    {
                        size_t i;
                        for (i = 0; i < numberOfContainers; i++)
                        {
                            // if not all containers checked continue with search
                            if (pContainers[i].wasChecked == 1) { allContainersChecked = 1; break; }
                            else allContainersChecked = 0;
                        }
                        
                        // search new nect fit
                        if (allContainersChecked == 1)
                        {
                            lastSelectedContainerId = ((lastSelectedContainerId + 1) % numberOfContainers < numberOfContainers) ? (lastSelectedContainerId + 1) % numberOfContainers : 0;
                            pContainers[lastSelectedContainerId].wasChecked = 0;
                        }
                        // failed fit
                        else
                        {
                            printf("validation failed\n");

                            size_t i;
                            for (i = 0; i < numberOfContainers; ++i)
                            {
                                free(pContainers[i].boxSizes);
                            }
                            free(pContainers);
                            printf("return NULL from NF\n");
                            return NULL;
                        }
                    }
                    
                    // successfull fit
                    if (pContainers[lastSelectedContainerId].capacity - strtol(pLineSeperated, NULL, 10) >= 0)
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
                int maxContainerCapacity = 0;
                int idOfMaxContainerCapacity = 0;
                
                int secondMaxContainerCapacity = 0;
                int idOfSecondMaxContainerCapacity = 0;
                
                // Max Capacity
                size_t i;
                for (i = 0; i < numberOfContainers; i++)
                {
                    pContainers[i].wasChecked = 1;  // unset for NEXT_FIT
                
                    maxContainerCapacity = max(maxContainerCapacity, pContainers[i].capacity);
                    if (maxContainerCapacity == pContainers[i].capacity)
                    {
                        if (pContainers[idOfMaxContainerCapacity].capacity == inlineSubtraction(pContainers[idOfMaxContainerCapacity].capacity, strtol(pLineSeperated, NULL, 10)))
                            idOfMaxContainerCapacity = min(idOfMaxContainerCapacity, pContainers[i].id);
                        else
                            idOfMaxContainerCapacity = pContainers[i].id;
                    }
                }
                
                // Second max capacity
                for (i = 0; i < numberOfContainers; i++)
                {
                    if (pContainers[i].id == idOfMaxContainerCapacity) continue;    // Skip Container with max capacity
                    
                    secondMaxContainerCapacity = max(secondMaxContainerCapacity, pContainers[i].capacity);
                    if (secondMaxContainerCapacity == pContainers[i].capacity)
                    {
                        if (pContainers[idOfSecondMaxContainerCapacity].capacity == inlineSubtraction(pContainers[idOfSecondMaxContainerCapacity].capacity, strtol(pLineSeperated, NULL, 10)))
                            idOfSecondMaxContainerCapacity = min(idOfSecondMaxContainerCapacity, pContainers[i].id);
                        else
                            idOfSecondMaxContainerCapacity = pContainers[i].id;
                    }
                }

                // store box in second-max-container
                if (pContainers[idOfSecondMaxContainerCapacity].capacity >= strtol(pLineSeperated, NULL, 10))
                {
                    // save current selectionID for NEXT-FIT
                    lastSelectedContainerId = idOfSecondMaxContainerCapacity;
                    
                    pContainers[idOfSecondMaxContainerCapacity].capacity = inlineSubtraction(pContainers[idOfSecondMaxContainerCapacity].capacity, strtol(pLineSeperated, NULL, 10));
                    pContainers[idOfSecondMaxContainerCapacity].numberOfBoxes++;
                    pContainers[idOfSecondMaxContainerCapacity].boxSizes[pContainers[idOfSecondMaxContainerCapacity].numberOfBoxes - 1] = strtol(pLineSeperated, NULL, 10);
                }
                // store box in max-container 
                else if (pContainers[idOfMaxContainerCapacity].capacity >= strtol(pLineSeperated, NULL, 10))
                {
                    lastSelectedContainerId = idOfMaxContainerCapacity;
                    
                    pContainers[idOfMaxContainerCapacity].capacity = inlineSubtraction(pContainers[idOfMaxContainerCapacity].capacity, strtol(pLineSeperated, NULL, 10));
                    pContainers[idOfMaxContainerCapacity].numberOfBoxes++;
                    pContainers[idOfMaxContainerCapacity].boxSizes[pContainers[idOfMaxContainerCapacity].numberOfBoxes - 1] = strtol(pLineSeperated, NULL, 10);
                }
                // failed fit
                else
                {
                    printf("validation failed\n");
                    
                    size_t i;
                    for (i = 0; i < numberOfContainers; ++i)
                    {
                        free(pContainers[i].boxSizes);
                    }
                    free(pContainers);
                    printf("return NULL from AWF\n");
                    return NULL;
                }
            }
        }
    }
    
    return pContainers;
}











