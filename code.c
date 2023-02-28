#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mt19937ar.h"


/*
|------------------------------------------------------------------------------|
|simPi                                                                         |
|use : Compute the value of PI with the Monte Carlo method.                    |
|                                                                              |
|input : nbPoints, number of points that will be created.                      |
|                                                                              |
|output : the estimated value of PI.                                           |
|------------------------------------------------------------------------------|
*/
double simPi(int nbPoints) {

    // Declaration
    double randNbX, randNbY, PI, progress;
    int inCircle = 0;

    // calculate the number of points that are less then the distance 1 from the centre of a circle
    for (int i = 0; i < nbPoints; i++) {
        
        // show to progress of this loop (more usefull when the number of points is huge because it help to know how much left for the function to finish)
        // happens every 10000 iteration
        if (i % 10000 == 0) {

            // transforme i to a percentage and prints it
            progress = i * 100.0 / nbPoints;
            printf("progress : %.2f %%\r", progress);
        }

        randNbX = 2.0 * genrand_real1() - 1.0;  // coordinate of x (between -1 and 1)
        randNbY = 2.0 * genrand_real1() - 1.0;  // coordinate of y (between -1 and 1)

        // increase inCircle by 1 everytime the point created is less then 1 distance from the centre of the cercle
        if((randNbX * randNbX + randNbY * randNbY) <= 1.0) {

            inCircle++;
        }
        
    }
    PI = 4.0 * inCircle / nbPoints;  // estimated value of PI
    printf("computing complited\nPi estimate: %.20f\n", PI);
    return PI;
}

// structure used to return multiple value in the function meanPI
typedef struct Returnvalues {
    double meanPi;  // mean of PI
    double* arrayPi;  // array containing estimated values of Pi
}returnPi;

/*
|------------------------------------------------------------------------------|
|meanPI                                                                        |
|use : find the mean of PI based on the values of PI found after               |
|       multiple simulations.                                                  |
|                                                                              |
|input : nbExpr, number of experiments.                                        |
|        nbPoints, number of points.                                           |
|output : sturture containing the mean PI and all the estimated values of      |
|         PI created.                                                          |
|------------------------------------------------------------------------------|
*/
returnPi meanPI(int nbExpr, int nbPoints) {

    // declaration
    returnPi dataPi;
    double meanPi = 0.0;
    double *arrayEstimation = (double*) malloc(nbExpr * sizeof(double));

    // if error while creating the array
    if (!arrayEstimation) {

        perror("error while creating an array");
        exit(1);
    }

    // create multiple simulatioon and store them in the array "arrayEstimation", also calculating the sum of thoses values of PI in meanPi
    for (int i = 0; i < nbExpr; i++) {

        printf("\nExperiment number %d / %d\n", i + 1, nbExpr);
        arrayEstimation[i] = simPi(nbPoints);
        meanPi += arrayEstimation[i];
    }

    // divising meanPi by number of experiments to get the mean value of PI
    meanPi /= nbExpr; 

    // printing results
    printf("\n\n    Experiments complited successfully\nthe mean PI is        : %.20f\n"
                                                       "the real PI is        : %.20f\n"
                                                       "the absolute error is : %.20f\n"
                                                       "the relative error is : %.20f\n",
                                                       meanPi,
                                                       M_PI,
                                                       fabs(meanPi - M_PI),
                                                       fabs(meanPi - M_PI) / M_PI);
    
    // storing variables for the return
    dataPi.meanPi = meanPi;
    dataPi.arrayPi = arrayEstimation;
    return dataPi;
}

/*
|------------------------------------------------------------------------------|
|getConfidenceRadius                                                           |
|use : calculate the confidence radius and the confidence interval.            |
|                                                                              |
|input : nbExpr, number of experiments.                                        |
|        nbPoints, number of points.                                           |
|------------------------------------------------------------------------------|
*/
void getConfidenceRadius(int nbExp, int nbPoints) {

    // declaration
    double stdDev = 0.0, meanPi, tValue, R;
    double* arrayEstimation;
    char lineData[14];
    int lineNb = 0, mult = 0, df = nbExp - 1, lineId;
    FILE* file;

    // creation a simulation and getting its meanPi and an array with all PI value estimated 
    returnPi dataPi = meanPI(nbExp, nbPoints);

    // if df is between 3à and 120 where values start jumping by 10 we round df to the closest number and change the value of mult
    if (df > 30 && df <= 120) {

        mult = 1;
        df = round(df / 10.0) * 10;
    }

    // if df is less then 120 -biggest value of df in the table we have in a file before it becomes considered as infinite- we get the line ID of the t value we need
    if (df <= 120) {
        lineId = df * (1 - mult) + mult * (30 + ((df - 30) / 10));
    }
    else { // we get the t value of inf which exist in the last line (line 40)
        lineId = 40;
    }

    // Open the file for reading
    file = fopen("t-table.txt", "r");

    if (file == NULL) {
        perror("Error: Unable to open the file t-table\n");
        exit(1);
    }
    
    //reading line by line the file until we get to the line we want then extracting the t value from it and then breaking the loop
    while (fgets(lineData, sizeof(lineData), file)) {

        lineNb++;
        if (lineNb == lineId) {

            tValue = atof(lineData + 6);
            break;
        }
    }

    
    // simplify the writing of the variables
    meanPi = dataPi.meanPi;
    arrayEstimation = dataPi.arrayPi;

    //calculating the value of the stdDev
    for (int i = 0; i < nbExp; i++) {

        stdDev += pow((arrayEstimation[i] - meanPi), 2);
    }

    stdDev /= nbExp - 1;

    //calculating the value of the confidence radius
    R = tValue * sqrt(stdDev / nbExp);

    printf("the confidence radius is : %.20f\nthe confidence interval at 99%% is : [%.20f, %.20f]\n", R, meanPi - R, meanPi + R);

    //freeing memory collocated
    free(dataPi.arrayPi);
    free(arrayEstimation);

    // Closing the file
    fclose(file);
}


int main() {

    printf("code is runing...\n");

    
    //simPi(1000);
    //simPi(1000000);
    //simPi(1000000000);

    //meanPI(40, 10000000);
    
    /*
    int nbExp;
    printf("what is the number of experiments? :");
    scanf("%d", &nbExp);
    getConfidenceRadius(nbExp, 1000000000);
    */

    getConfidenceRadius(40, 1000000000);
    return 0;
}