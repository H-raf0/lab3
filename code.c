#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mt19937ar.h"


/*misa7a dyak da2ira 3la dyal moraba3 = pi/4 hadak chi 3lach kn7ato no9at wst da2ira wo bra dyalha bach nsimiliw lmisa7a*/
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
    printf("computing complited\nPi estimate: %lf\n", PI);
    return PI;
}

// structure used to return multiple value in the function meanPI
typedef struct Returnvalues {
    double meanPi;  // mean of PI
    double* arrayPi;  // array containing estimated values of Pi
}returnPi;


returnPi meanPI(int nbExpr, int nbPoints) { //nb of experiments

    returnPi dataPi;
    double meanPi = 0.0;
    double *arrayEstimation = (double*) malloc(nbExpr * sizeof(double));

    // if error while creating the array
    if (!arrayEstimation) {

        perror("error while creating an array");
        exit(1);
    }

    for (int i = 0; i < nbExpr; i++) {

        printf("\nExperiment number %d / %d\n", i + 1, nbExpr);
        arrayEstimation[i] = simPi(nbPoints);
        meanPi += arrayEstimation[i];
    }

    meanPi /= nbExpr;

    printf("\n\n    Experiments complited successfully\nthe mean PI is        : %.20f\n"
                                                       "the real PI is        : %.20f\n"
                                                       "the absolute error is : %.20f\n"
                                                       "the relative error is : %.20f\n",
                                                       meanPi,
                                                       M_PI,
                                                       fabs(meanPi - M_PI),
                                                       fabs(meanPi - M_PI) / M_PI);
    
    dataPi.meanPi = meanPi;
    dataPi.arrayPi = arrayEstimation;
    return dataPi;
    //free(arrayEstimation);
}


void getConfidenceRadius(int nbExp, int nbPoints) {

    returnPi dataPi = meanPI(nbExp, nbPoints);
    double variance = 0.0, meanPi, tValue, R;
    double* arrayEstimation;
    char lineData[14];
    int lineNb = 0, mult = 0, df = nbExp - 1, lineId;
    FILE* file;

    if (df > 30 && df <= 120) {

        mult = 1;
        df = round(df / 10.0) * 10;
    }

    if (df <= 120) {
        lineId = df * (1 - mult) + mult * (30 + ((df - 30) / 10));
    }
    else {
        lineId = 40;
    }
    /*
    // Open the file for reading
    file = fopen("t-table.txt", "r");

    if (file == NULL) {
        perror("Error: Unable to open the file t-table\n");
        exit(1);
    }
    */
    errno_t err;

    // Open for read
    err = fopen_s(&file, "t-table.txt", "r");

    if (err){

        perror("Error: Unable to open the file t-table\n");
        exit(1);
    }

    while (fgets(lineData, sizeof(lineData), file)) {

        lineNb++;
        if (lineNb == lineId) {

            tValue = atof(lineData + 6);
            break;
        }
    }

    

    meanPi = dataPi.meanPi;
    arrayEstimation = dataPi.arrayPi;

    for (int i = 0; i < nbExp; i++) {

        variance += pow((arrayEstimation[i] - meanPi), 2);
    }

    variance /= nbExp - 1;

    R = tValue * sqrt(variance / nbExp);

    printf("the confidence radius is : %.20f\nthe confidence intervals at 99%% is : [%.20f, %.20f]\n", R, meanPi - R, meanPi + R);

    free(dataPi.arrayPi);
    // Close the file
    fclose(file);
}


int main() {

    printf("code is runing...\n");

    
    //simPi(1000);
    //simPi(1000000);
    //simPi(10000000);

    //meanPI(40, 10000000);

    /*int nbExp;
    printf("what is the number of experiments? :");
    scanf("%d", &nbExp);*/
    getConfidenceRadius(40, 10000000);

    return 0;
}