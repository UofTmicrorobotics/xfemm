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

   Date Modified: 2017
   By: Richard Crozier
       Johannes Zarl-Zierl
   Contact:
	    richard.crozier@yahoo.co.uk
       johannes@zarl-zierl.at

   Contributions by Johannes Zarl-Zierl were funded by
	Linz Center of Mechatronics GmbH (LCM)
*/
#include "PostProcessor.h"

#include <cstdlib>
#include <string>
#include <cstring>
#include <cstdio>
#include <cmath>
#include "femmcomplex.h"
#include "femmconstants.h"
#include "fparse.h"
#include "lua.h"
#include "lualib.h"


#ifndef _MSC_VER
#define _strnicmp strncasecmp
#ifndef SNPRINTF
#define SNPRINTF std::snprintf
#endif
#else
#ifndef SNPRINTF
#define SNPRINTF _snprintf
#endif
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//using namespace std;
using namespace femm;

// PostProcessor construction/destruction

double sqr(double x)
{
    return x*x;
}

/**
 * Constructor for the PostProcessor class.
 */
PostProcessor::PostProcessor()
{
    // set some default values for problem definition
    d_LineIntegralPoints = 400;
    Smooth = true;
    NumList = NULL;
    ConList = NULL;
    bHasMask = false;
    LengthConv = (double *)calloc(6,sizeof(double));
    LengthConv[0] = 0.0254;   //inches
    LengthConv[1] = 0.001;    //millimeters
    LengthConv[2] = 0.01;     //centimeters
    LengthConv[3] = 1.;       //meters
    LengthConv[4] = 2.54e-05; //mils
    LengthConv[5] = 1.e-06;   //micrometers

    for(int i=0; i<9; i++)
    {
        d_PlotBounds[i][0] = d_PlotBounds[i][1] =
                PlotBounds[i][0] = PlotBounds[i][1] = 0;
    }

    // initialise the warning message function pointer to
    // point to the PrintWarningMsg function
    WarnMessage = &PrintWarningMsg;
}

/**
 * Destructor for the PostProcessor class.
 */
PostProcessor::~PostProcessor()
{
    free(LengthConv);
    for(uint i=0; i<problem->meshnodes.size(); i++)
        if(ConList[i]!=NULL) free(ConList[i]);
    free(ConList);
    free(NumList);
}


// identical in FPProc and HPProc
int PostProcessor::InTriangle(double x, double y)
{
    static int k;
    int j,hi,lo,sz;
    double z;

    sz = problem->meshelems.size();

    if ((k < 0) || (k >= sz)) k = 0;

    // In most applications, the triangle we're looking
    // for is nearby the last one we found.  Since the elements
    // are ordered in a banded structure, we want to check the
    // elements nearby the last one selected first.
    if (InTriangleTest(x,y,k)) return k;

    // wasn't in the last searched triangle, so look through all the
    // elements
    hi = k;
    lo = k;

    for(j=0; j<sz; j+=2)
    {
        hi++;
        if (hi >= sz) hi = 0;
        lo--;
        if (lo < 0)   lo = sz - 1;

        CComplex hiCtr = problem->meshelems[hi]->ctr;
        z = (hiCtr.re - x) * (hiCtr.re - x) + (hiCtr.im - y) * (hiCtr.im - y);

        if (z <= problem->meshelems[hi]->rsqr)
        {
            if (InTriangleTest(x,y,hi))
            {
                k = hi;
                return k;
            }
        }

        CComplex loCtr = problem->meshelems[lo]->ctr;
        z = (loCtr.re-x)*(loCtr.re-x) + (loCtr.im-y)*(loCtr.im-y);

        if (z <= problem->meshelems[lo]->rsqr)
        {
            if (InTriangleTest(x,y,lo))
            {
                k = lo;
                return k;
            }
        }

    }

    return (-1);
}

// identical in fmesher, FPProc, and HPProc
int PostProcessor::ClosestNode(double x, double y)
{
    double d0,d1;

    if(problem->nodelist.size()==0) return -1;

    int idx=0;
    d0=problem->nodelist[0]->GetDistance(x,y);
    for(int i=0; i<(int)problem->nodelist.size(); i++)
    {
        d1=problem->nodelist[i]->GetDistance(x,y);
        if(d1<d0)
        {
            d0=d1;
            idx=i;
        }
    }

    return idx;
}

// FPProc and HPProc differ, but I'm not sure whether hpproc could just use this version instead
bool PostProcessor::InTriangleTest(double x, double y, int i)
{

    if ((i < 0) || (i >= int(problem->meshelems.size()))) return false;

    int j,k;
    double z;

    for (j=0; j<3; j++)
    {
        k = j + 1;

        if (k == 3) k = 0;

        int p_k = problem->meshelems[i]->p[k];
        int p_j = problem->meshelems[i]->p[j];
        // Case 1: p[k]>p[j]
        if (p_k > p_j)
        {
            z = (problem->meshnodes[p_k]->x - problem->meshnodes[p_j]->x) *
                    (y - problem->meshnodes[p_j]->y) -
                    (problem->meshnodes[p_k]->y - problem->meshnodes[p_j]->y) *
                    (x - problem->meshnodes[p_j]->x);

            if(z<0) return false;
        }
        //Case 2: p[k]<p[j]
        else
        {
            z = (problem->meshnodes[p_j]->x - problem->meshnodes[p_k]->x) *
                    (y - problem->meshnodes[p_k]->y) -
                    (problem->meshnodes[p_j]->y - problem->meshnodes[p_k]->y) *
                    (x - problem->meshnodes[p_k]->x);

            if (z > 0) return false;
        }
    }

    return true;
}

// identical in FPProc and HPProc
CComplex PostProcessor::Ctr(int i)
{
    CComplex c = 0;
    for(int j=0; j<3; j++)
    {
        int p_j = problem->meshelems[i]->p[j];
        CComplex p(problem->meshnodes[ p_j ]->x/3., problem->meshnodes[ p_j ]->y/3.);
        c+=p;
    }

    return c;
}

// identical in FPProc and HPProc
double PostProcessor::ElmArea(int i)
{
    int n[3];
    for(int j=0; j<3; j++) n[j]=problem->meshelems[i]->p[j];

    double b0=problem->meshnodes[n[1]]->y - problem->meshnodes[n[2]]->y;
    double b1=problem->meshnodes[n[2]]->y - problem->meshnodes[n[0]]->y;
    double c0=problem->meshnodes[n[2]]->x - problem->meshnodes[n[1]]->x;
    double c1=problem->meshnodes[n[0]]->x - problem->meshnodes[n[2]]->x;
    return (b0*c1-b1*c0)/2.;
}

// identical in FPProc and HPProc
double PostProcessor::ElmArea(femmsolver::CElement *elm)
{
    int n[3];
    for(int j=0; j<3; j++) n[j]=elm->p[j];

    double b0=problem->meshnodes[n[1]]->y - problem->meshnodes[n[2]]->y;
    double b1=problem->meshnodes[n[2]]->y - problem->meshnodes[n[0]]->y;
    double c0=problem->meshnodes[n[2]]->x - problem->meshnodes[n[1]]->x;
    double c1=problem->meshnodes[n[0]]->x - problem->meshnodes[n[2]]->x;
    return (b0*c1-b1*c0)/2.;
}


// identical in FPProc and HPProc
CComplex PostProcessor::HenrotteVector(int k)
{
    int n[3];
    double b[3],c[3];

    for(int i=0; i<3; i++)
    {
        n[i] = problem->meshelems[k]->p[i];
    }

    b[0]=problem->meshnodes[n[1]]->y - problem->meshnodes[n[2]]->y;
    b[1]=problem->meshnodes[n[2]]->y - problem->meshnodes[n[0]]->y;
    b[2]=problem->meshnodes[n[0]]->y - problem->meshnodes[n[1]]->y;
    c[0]=problem->meshnodes[n[2]]->x - problem->meshnodes[n[1]]->x;
    c[1]=problem->meshnodes[n[0]]->x - problem->meshnodes[n[2]]->x;
    c[2]=problem->meshnodes[n[1]]->x - problem->meshnodes[n[0]]->x;

    double da = (b[0] * c[1] - b[1] * c[0]);

    CComplex v = 0;
    for(int i=0; i<3; i++)
    {
        v -= problem->meshnodes[n[i]]->msk * (b[i] + I * c[i]) / (da * LengthConv[problem->LengthUnits]);  // grad
    }

    return v;
}


// identical in fmesher, FPProc and HPProc
int PostProcessor::ClosestArcSegment(double x, double y)
{
    if(problem->arclist.size()==0) return -1;

    double d0=ShortestDistanceFromArc(CComplex(x,y),*problem->arclist[0]);
    int idx=0;
    for(int i=0; i<(int)problem->arclist.size(); i++)
    {
        double d1=ShortestDistanceFromArc(CComplex(x,y),*problem->arclist[i]);
        if(d1<d0)
        {
            d0=d1;
            idx=i;
        }
    }

    return idx;
}

// identical in fmesher, FPProc and HPProc
void PostProcessor::GetCircle(CArcSegment &arc,CComplex &c, double &R)
{
    CComplex a0,a1,t;
    double d,tta;

    // construct the coordinates of the two points on the circle
    a0.Set(problem->nodelist[arc.n0]->x,problem->nodelist[arc.n0]->y);
    a1.Set(problem->nodelist[arc.n1]->x,problem->nodelist[arc.n1]->y);

    // calculate distance between arc endpoints
    d = abs(a1 - a0);

    // figure out what the radius of the circle is...

    // get unit vector pointing from a0 to a1
    t = (a1 - a0) / d;

    // convert swept angle from degrees to radians
    tta = arc.ArcLength * PI / 180.;

    // the radius is half the chord length divided by sin of
    // half the swept angle (finding the side length of a
    // triangle formed by the two points and the centre)
    R = d / (2.*sin(tta/2.));

    // center of the arc segment's circle
    c = a0 + (d/2. + I * sqrt(R*R - d*d / 4.)) * t;

}



// identical in fmesher, FPProc and HPProc
double PostProcessor::ShortestDistanceFromArc(CComplex p, CArcSegment &arc)
{
    double R,d,l,z;
    CComplex a0,a1,c,t;

    a0.Set(problem->nodelist[arc.n0]->x,problem->nodelist[arc.n0]->y);
    a1.Set(problem->nodelist[arc.n1]->x,problem->nodelist[arc.n1]->y);
    GetCircle(arc,c,R);
    d=abs(p-c);

    if(d==0) return R;

    t=(p-c)/d;
    l=abs(p-c-R*t);
    z=arg(t/(a0-c))*180/PI;
    if ((z>0) && (z<arc.ArcLength)) return l;

    z=abs(p-a0);
    l=abs(p-a1);
    if(z<l) return z;
    return l;
}

// identical in FPProc and HPProc
double PostProcessor::ShortestDistanceFromSegment(double p, double q, int segm)
{
    double t,x[3],y[3];

    int n0 = problem->linelist[segm]->n0;
    int n1 = problem->linelist[segm]->n1;
    x[0]=problem->nodelist[n0]->x;
    y[0]=problem->nodelist[n0]->y;
    x[1]=problem->nodelist[n1]->x;
    y[1]=problem->nodelist[n1]->y;

    t=((p-x[0])*(x[1]-x[0]) + (q-y[0])*(y[1]-y[0]))/
            ((x[1]-x[0])*(x[1]-x[0]) + (y[1]-y[0])*(y[1]-y[0]));

    if (t>1.) t=1.;
    if (t<0.) t=0.;

    x[2]=x[0]+t*(x[1]-x[0]);
    y[2]=y[0]+t*(y[1]-y[0]);

    return sqrt((p-x[2])*(p-x[2]) + (q-y[2])*(q-y[2]));
}


// identical in FPProc and HPProc
void PostProcessor::BendContour(double angle, double anglestep)
{
    if (angle==0) return;
    if (anglestep==0) anglestep=1;

    int k,n;
    double d,tta,dtta,R;
    CComplex c,a0,a1;

    // check to see if there are at least enough
    // points to have made one line;
    k = contour.size()-1;
    if (k<1) return;

    // restrict the angle of the contour to 180 degrees;
    if ((angle<-180.) || (angle>180.))
    {
        return;
    }
    n = (int) ceil(fabs(angle/anglestep));
    tta = angle*PI/180.;
    dtta = tta/((double) n);

    // pop last point off of the contour;
    a1 = contour[k];
    contour.erase(contour.begin()+k);
    a0 = contour[k-1];

    // compute location of arc center;
    // and radius of the circle that the
    // arc lives on.
    d = abs(a1-a0);
    R = d / ( 2. * sin(fabs(tta/2.)) );

    if(tta>0)
    {
        c = a0 + (R/d) * (a1-a0) * exp(I*(PI-tta)/2.);
    }
    else
    {
        c = a0 + (R/d) * (a1-a0) * exp(-I*(PI+tta)/2.);
    }

    // add the points on the contour
    for(k=1; k<=n; k++)
    {
        contour.push_back( c + (a0 - c) * exp(k * I * dtta) );
    }
}


// identical in FPProc and HPProc
void PostProcessor::FindBoundaryEdges()
{
    int i, j;
    static int plus1mod3[3] = {1, 2, 0};
    static int minus1mod3[3] = {2, 0, 1};

    // Init all elements' neigh to be unfinished.
    for(i = 0; i < (int)problem->meshelems.size(); i ++)
    {
        for(j = 0; j < 3; j ++)
            problem->meshelems[i]->n[j] = 0;
    }

    int orgi, desti;
    int ei, ni;
    bool done;

    // Loop all elements, to find and set there neighs.
    for(i = 0; i < (int)problem->meshelems.size(); i ++)
    {
        for(j = 0; j < 3; j ++)
        {
            if(problem->meshelems[i]->n[j] == 0)
            {
                // Get this edge's org and dest node index,
                orgi = problem->meshelems[i]->p[plus1mod3[j]];
                desti = problem->meshelems[i]->p[minus1mod3[j]];
                done = false;
                // Find this edge's neigh from the org node's list
                for(ni = 0; ni < NumList[orgi]; ni ++)
                {
                    // Find a Element around org node contained dest node of this edge.
                    ei = ConList[orgi][ni];
                    if (ei == i) continue; // Skip myself.
                    // Check this Element's 3 vert to see if there exist dest node.
                    if(problem->meshelems[ei]->p[0] == desti) {
                        done = true;
                        break;
                    } else if(problem->meshelems[ei]->p[1] == desti) {
                        done = true;
                        break;
                    } else if(problem->meshelems[ei]->p[2] == desti) {
                        done = true;
                        break;
                    }
                }
                if (!done) {
                    // This edge must be a Boundary Edge.
                    problem->meshelems[i]->n[j] = 1;
                }
            } // Finish One Edge
        } // End of One Element Loop
    } // End of Main Loop

}
