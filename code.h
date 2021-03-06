#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define log2(x) (log10((double)(x))/log10(2.0))


void
calculate_minimum_redundancy(int *, int);

#define LIMIT 1000000                        /* maximum value for n */


#if 0
int
main(int argc, char **argv) {

    int *A;                              /* the array of freqs */
    int *B;                              /* a copy of A */
    int n;                               /* number of symbols */
    int totfreq;                         /* total frequency */
    int bits;                            /* total bits in codewords */
    double ent;                          /* entropy */

    int i;

    /* claim space for arrays */
    scanf("%d", &n);
    if (n <= 0 || n > LIMIT) {
        fprintf(stderr, "%s: n should be at least 0 and less than %d\n",
                argv[0], LIMIT);
        exit(1);
    }
    if ((A = (int *)malloc(n*sizeof(int))) == (int *)NULL
            || (B = (int *)malloc(n*sizeof(int))) == (int *)NULL) {
        fprintf(stderr, "%s: unable to allocate memory\n",
                argv[0]);
        exit(1);
    }

    /* read symbol frequencies */
    for (i=0; i<n; i++) {
        if (scanf("%d", A+i) == EOF) {
            n = i;
            break;
        }
    }

    /* calculate entropy, and check for sensible input values */
    totfreq = 0;
    for (i=0; i<n; i++) {
        totfreq += A[i];
        if (A[i] < 0 || (i>0 && A[i-1] > A[i])) {
            fprintf(stderr, "%s: %s %s\n",
                    argv[0],
                    "input frequencies must be non-negative",
                    "and non-decreasing");
            exit(1);
        }
    }
    if (totfreq <= 0) {
        fprintf(stderr, "%s: sum of frequencies must be positive\n",
                argv[0]);
        exit(1);
    }
    ent = 0.0;
    for (i=0; i<n; i++) {
        double prob = (double)A[i]/totfreq;
        ent += - prob * log2(prob);
    }


    /* make a copy of A into B so that average weighted codeword
     *            length can be calculated once the codeword lengths are known;
     *                       note that B is _not_ used by the length calculation process */
    for (i=0; i<n; i++) {
        B[i] = A[i];
    }

    /* now calculate the minimum-redundancy codeword lengths */
    calculate_minimum_redundancy(A, n);

    /* and evaluate average codeword length */
    bits = 0;
    for (i=0; i<n; i++) {
        bits += A[i] * B[i];
    }

    /*  and print the results */
    if (n <= 100) {
        for (i=0; i<n; i++) {
            printf("f_%02d = %4d, |c_%02d| = %2d\n",
                    i, B[i], i, A[i]);
        }
    }
    printf("entropy                 = %5.2f bits per symbol\n", ent);
    printf("minimum-redundancy code = %5.2f bits per symbol\n", 
            (double)bits/totfreq);
    if (ent > 0.0)
        printf("inefficiency            = %5.2f%%\n", 
                100*(double)bits/totfreq/ent - 100.0);

    free(A);
    free(B);
    return(0);
}

#endif

/*** Function to calculate in-place a minimum-redundancy code
 *      Parameters:
 *              A        array of n symbol frequencies, non-decreasing
 *                      n        number of symbols in A
 *                      */
void
calculate_minimum_redundancy(int A[], int n) {
    int root;                  /* next root node to be used */
    int leaf;                  /* next leaf to be used */
    int next;                  /* next value to be assigned */
    int avbl;                  /* number of available nodes */
    int used;                  /* number of internal nodes */
    int dpth;                  /* current depth of leaves */

    /* check for pathological cases */
    if (n==0) { return; }
    if (n==1) { A[0] = 0; return; }

    /* first pass, left to right, setting parent pointers */
    A[0] += A[1]; root = 0; leaf = 2;
    for (next=1; next < n-1; next++) {
        /* select first item for a pairing */
        if (leaf>=n || A[root]<A[leaf]) {
            A[next] = A[root]; A[root++] = next;
        } else
            A[next] = A[leaf++];

        /* add on the second item */
        if (leaf>=n || (root<next && A[root]<A[leaf])) {
            A[next] += A[root]; A[root++] = next;
        } else
            A[next] += A[leaf++];
    }

    /* second pass, right to left, setting internal depths */
    A[n-2] = 0;
    for (next=n-3; next>=0; next--)
        A[next] = A[A[next]]+1;

    /* third pass, right to left, setting leaf depths */
    avbl = 1; used = dpth = 0; root = n-2; next = n-1;
    while (avbl>0) {
        while (root>=0 && A[root]==dpth) {
            used++; root--;
        }
        while (avbl>used) {
            A[next--] = dpth; avbl--;
        }
        avbl = 2*used; dpth++; used = 0;
    }
}
