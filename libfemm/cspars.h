/*
   This code is a modified version of an algorithm
   forming part of the software program Finite
   Element Method Magnetics (FEMM), authored by
   David Meeker. The original software code is
   subject to the Aladdin Free Public Licence
   version 8, November 18, 1999. For more information
   on FEMM see www.femm.info. This modified version
   is not endorsed in any way by the original
   authors of FEMM.

   This software has been modified to use the C++
   standard template libraries and remove all Microsoft (TM)
   MFC dependent code to allow easier reuse across
   multiple operating system platforms.

   Date Modified: 2011 - 11 - 10
   By: Richard Crozier
   Contact: richard.crozier@yahoo.co.uk
*/

#ifndef CSPARS_H
#define CSPARS_H

class complexd_tEntry
{
public:

    complexd_t x;				// value stored in the entry
    int c;					// column that the entry lives in
    complexd_tEntry *next;	// pointer to next entry in the row;
    complexd_tEntry();

private:
};

class CBigComplexLinProb
{
public:

    // data members

    complexd_t *P; 				// search direction
    complexd_t *U;
    complexd_t *R; 				// residual
    complexd_t *V;
    complexd_t *Z;
    complexd_t *b;				// RHS of linear equation
    complexd_t *uu;
    complexd_t *vv;

    complexd_tEntry **M;			// pointer to list of matrix entries;
    complexd_tEntry **Mh;			// Hermitian matrix arising from N-R algorithm;
    complexd_tEntry **Ma;			// Antihermitian matrix arising from N-R algorithm;
    complexd_tEntry **Ms;			// Additional complex-symmetric matrix arising from N-R algorithm;
    int n;						// dimensions of the matrix;
    int bdw;					// optional bandwidth parameter;
    int bNewton;				// Flag which denotes whether or not there are entries in Mh or Ms;
    int NumNodes;
    double Precision;
    double Lambda;			// relaxation factor;

    // member functions

    CBigComplexLinProb();				// constructor
    ~CBigComplexLinProb();				// destructor
    int Create(int d, int bw, int nodes);	// initialize the problem
    void Put(complexd_t v, int p, int q, int k=0); // use to create/set entries in the matrix
    complexd_t Get(int p, int q, int k=0);
    void AddTo(complexd_t v, int p, int q);
    void MultA(complexd_t *X, complexd_t *Y, int k=0);
    void MultConjA(complexd_t *X, complexd_t *Y, int k=0);
    complexd_t Dot(complexd_t *x, complexd_t *y);
    complexd_t ConjDot(complexd_t *x, complexd_t *y);
    void SetValue(int i, complexd_t x);
    void Periodicity(int i, int j);
    void AntiPeriodicity(int i, int j);
    void Wipe();
    void MultPC(complexd_t *X, complexd_t *Y);
    void MultAPPA(complexd_t *X, complexd_t *Y);


    // flag==false initializes solution to zero
    // flag==true  starts from solution of previous call
    int PBCGSolveMod(int flag,bool verbose=false);	// Precondition Biconjugate Gradient
    int PCGSQStart();
    int PBCGSolve(int flag);
    int BiCGSTAB(int flag);
    int KludgeSolve(int flag);

//		CFknDlg *TheView;

private:

};

#endif
