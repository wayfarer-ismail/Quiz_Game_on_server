//
// Created by ismail on 08/04/2023.
//

#include <stdio.h>
#include <stdlib.h>
#include "QuizDB.h"

int main(int argc, char** argv)
{
    int q, numq = sizeof(QuizQ)/sizeof(QuizQ[0]);
    for (q = 0; q < numq; q++)
    {
        printf("Q. %s\n", QuizQ[q]);
        printf("A. %s\n", QuizA[q]);
    }

    exit(EXIT_SUCCESS);
}