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
        johannes.zarl-zierl@jku.at

   Contributions by Johannes Zarl-Zierl were funded by
   Linz Center of Mechatronics GmbH (LCM)
*/
// fpproc.h : interface of the FPProc class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef FPPROC_H
#define FPPROC_H

#include "CMPointVals.h"

#include "lua.h"
#include "luadebug.h"
#include "complex_type.h"
#include "femmenums.h"
#include "fparse.h"
#include "CArcSegment.h"
#include "CBlockLabel.h"
#include "CBoundaryProp.h"
#include "CCircuit.h"
#include "CPostProcMElement.h"
#include "CAirGapElement.h"
#include "CMaterialProp.h"
#include "CMeshNode.h"
#include "CNode.h"
#include "CPointProp.h"
#include "CSegment.h"
#include "PostProcessor.h"

#include <vector>

//#ifndef PLANAR
//#define PLANAR 0
//#endif
//
//#ifndef AXISYMMETRIC
//#define AXISYMMETRIC 1
//#endif

using std::string;

/**
 * @brief The EditMode determines which objects are affected by an edit or selection command.
 */
enum class FPProcError {
    /// \brief An air gap boundary of a given name was not found
    AGENameNotFound = 0,
    /// \brief Operate on line segments
    AGENoHarmonics = 1,
    /// \brief Operate on block labels
    AGENegativeHarmonicRequested = 2,
    /// \brief Operate on arc segments
    AGERequestedHarmonicTooLarge = 3,
    /// \brief An invalid value
    NoError
};

class FPProc : public femm::PProcIface
{

// Attributes
public:

    FPProc();
    virtual ~FPProc();

    // General problem attributes
    double  Frequency;
    double  Depth;
    double  Precision;
    int     LengthUnits;
    double *LengthConv;
    femm::ProblemType problemType;
    int    Coords;
    string  ProblemNote;
    bool    Smooth;
    bool    bMultiplyDefinedLabels;
    int	    WeightingScheme;
    double  extRo,extRi,extZo;
    int     NumAirGapElems;
    std::string PrevSoln;
    int     PrevType;


    double  A_High, A_Low;
    double  A_lb, A_ub;

    double  PlotBounds[9][2];
    double  d_PlotBounds[9][2];
//    double  Br_High,Br_Low;
//    double  Bi_High,Bi_Low;
    double  B_High, B_Low;
    double  H_High;
//    double  B_lb, B_ub;
//    double  Jr_High,Jr_Low;
//    double  Ji_High,Ji_Low;
//    double  J_High, J_Low;

    // Some default behaviors
    int  d_LineIntegralPoints;
    bool d_ShiftH;
    bool bHasMask;
    int bIncremental;

    // lists of nodes, segments, and block labels
    std::vector< femm::CNode >        nodelist;
    std::vector< femm::CSegment >     linelist;
    std::vector< femm::CArcSegment >  arclist;
    std::vector< femm::CMBlockLabel > blocklist;

    // vectors containing the mesh information
    std::vector< femmsolver::CMMeshNode > meshnode;
    std::vector< femmpostproc::CPostProcMElement >  meshelem;
    std::vector< femmsolver::CAirGapElement >   agelist;

    // List of elements connected to each node;
    int *NumList;
    int **ConList;

    // lists of properties
    std::vector< femm::CMMaterialProp > blockproplist;
    std::vector< femm::CMBoundaryProp > lineproplist;
    std::vector< femm::CMPointProp >    nodeproplist;
    std::vector< femm::CMCircuit >      circproplist;

    // list of points in a user-defined contour;
    std::vector< complexd_t > contour;

    // stuff that PTLOC needs
    std::vector< femmsolver::CMMeshNode >  *pmeshnode;
    std::vector< femmpostproc::CPostProcMElement >   *pmeshelem;

//    TriEdge recenttri;
//    int samples;
//    unsigned long randomseed;
//    TriEdge bdrylinkhead[512];
//    int numberofbdrylink;

    // member functions
    int InTriangle(double x, double y) const;
    bool InTriangleTest(double x, double y, int i) const;
    bool GetPointValues(double x, double y, CMPointVals &u);
    bool GetPointValues(double x, double y, int k, CMPointVals &u);
    // void GetLineValues(CXYPlot &p, int PlotType, int npoints);
    // void GetGapValues(CXYPlot &p, int PlotType, int npoints, int myAGE);
    void GetElementB(femmpostproc::CPostProcMElement &elm);
    void FindBoundaryEdges();
    complexd_t Ctr(int i) const;
    double ElmArea(int i) const;
    double ElmArea(femmpostproc::CPostProcMElement *elm) const;
    double ElmVolume(int i) const;
    //double ElmVolume(CElement *elm);
    void GetPointB(const double x, const double y, complexd_t &B1, complexd_t &B2, const femmpostproc::CPostProcMElement &elm);
    void GetNodalB(complexd_t *b1, complexd_t *b2,femmpostproc::CPostProcMElement &elm);
    /**
     * @brief Compute the block integral over selected blocks.
     *
     * ## Available block integrals
     * Note: Some integrals require that you set up a mesh element mask using MakeMask before calling.
     *
     * Number |MakeMask required| Integral
     * -------|-----------------|---------
     *  0     | no  | A * J
     *  1     | no  | A
     *  2     | no  | Magnetic field energy
     *  3     | no  | Hysteresis and/or lamination losses
     *  4     | no  | Resistive losses
     *  5     | no  | Block cross-section area
     *  6     | no  | Total losses
     *  7     | no  | Total current
     *  8     | no  | Integral of Bx (or Br ) over block
     *  9     | no  | Integral of By (or Bz ) over block
     *  10    | no  | Block volume
     *  11    | no  | x (or r) part of steady-state Lorentz force
     *  12    | no  | y (or z) part of steady-state Lorentz force
     *  13    | no  | x (or r) part of 2× Lorentz force
     *  14    | no  | y (or z) part of 2× Lorentz force
     *  15    | no  | Steady-state Lorentz torque
     *  16    | no  | 2× component of Lorentz torque
     *  17    | no  | Magnetic field coenergy
     *  18    | yes | x (or r) part of steady-state weighted stress tensor force
     *  19    | yes | y (or z) part of steady-state weighted stress tensor force
     *  20    | yes | x (or r) part of 2× weighted stress tensor force
     *  21    | yes | y (or z) part of 2× weighted stress tensor force
     *  22    | yes | Steady-state weighted stress tensor torque
     *  23    | yes | 2× component of weighted stress tensor torque
     *  24    | no  | R2 (i.e. moment of inertia / density)
     *  25    | no  | x (or r) part of 1× weighted stress tensor force
     *  26    | no  | y (or z) part of 1× weighted stress tensor force
     *  27    | no  | 1× component of weighted stress tensor torque
     *  28    | no  | x (or r) part of 1× Lorentz force
     *  29    | no  | y (or z) part of 1× Lorentz force
     *  30    | no  | 1× component of Lorentz torque
     *
     * @param inttype The identifier of the block integral.
     * @return the requested block integral
     */
    complexd_t BlockIntegral(const int inttype);
    void LineIntegral(int inttype, complexd_t *z);

    int ClosestNode(const double x, const double y) const;
    int ClosestArcSegment(double x, double y) const;
    void GetCircle(const femm::CArcSegment &asegm,complexd_t &c, double &R) const;
    double ShortestDistanceFromArc(const complexd_t p, const femm::CArcSegment &arc) const;
    double ShortestDistanceFromSegment(double p, double q, int segm) const;

    complexd_t GetJA(int k,complexd_t *J,complexd_t *A) const;
    complexd_t PlnInt(double a, complexd_t *u, complexd_t *v) const;
    complexd_t AxiInt(double a, complexd_t *u, complexd_t *v,double *r) const;
    bool ScanPreferences();
    void BendContour(double angle, double anglestep);

    complexd_t HenrotteVector(int k) const;
    bool IsKosher(int k) const;
    double AECF(int k) const;
    void GetFillFactor(int lbl);

    // pointer to function to call when issuing warning messages
    int (*WarnMessage)(const char*, ...);
//	void MsgBox(const char* message);

    complexd_t GetStrandedVoltageDrop(int lbl) const;
    complexd_t GetVoltageDrop(int circnum) const;
    complexd_t GetFluxLinkage(int circnum) const;
    complexd_t GetStrandedLinkage(int lbl) const;
    complexd_t GetSolidAxisymmetricLinkage(int lbl) const;
    complexd_t GetParallelLinkage(int numcirc) const;
    complexd_t GetParallelLinkageAlt(int numcirc) const;
    void GetMu(complexd_t b1, complexd_t b2,complexd_t &mu1, complexd_t &mu2, int i);
    void GetMu(double b1, double b2, double &mu1, double &mu2, int i);
    void GetMagnetization(int n, complexd_t &M1, complexd_t &M2);
    void GetH(double b1, double b2, double &h1, double &h2, int k);
    void GetH(complexd_t b1, complexd_t b2, complexd_t &h1, complexd_t &h2, int k);
    int numElements() const override;
    int numNodes() const override;
    FPProcError getGapHarmonics(const std::string myBdryName, const int n, complexd_t &acc, complexd_t &acs, complexd_t &brc, complexd_t &brs, complexd_t &btc, complexd_t &bts) const;
    bool AGEBoundNumFromName(const std::string myBdryName, int &n) const;
    FPProcError numGapHarmonics(const std::string myBdryName, int &nh) const;
    FPProcError getAGEflux(const std::string myBdryName, const double angle, complexd_t &br, complexd_t &bt) const;
    FPProcError getGapA(const std::string myBdryName, double tta, complexd_t &ac) const;
    FPProcError gapTimeAvgStoredEnergyIntegral(const std::string myBdryName, complexd_t &W) const;
    FPProcError gapIncrementalForceIntegral(const std::string myBdryName, complexd_t &fx, complexd_t &fy) const;
    FPProcError gapIncrementalTorqueIntegral(const std::string myBdryName, complexd_t &tq) const;
    FPProcError gap2XForceIntegral(const std::string myBdryName, complexd_t &fx, complexd_t &fy) const;
    FPProcError gapDCForceIntegral(const std::string myBdryName, complexd_t &fx, complexd_t &fy) const;
    FPProcError gap2XTorqueIntegral(const std::string myBdryName, complexd_t &tq) const;
    FPProcError gapDCTorqueIntegral(const std::string myBdryName, double &tq) const;



    void ClearDocument();
    bool NewDocument();
//     virtual void Serialize(CArchive& ar);
    bool OpenDocument(std::string lpszPathName) override;
    bool MakeMask();
    //bool LoadMeshNodesFromSolution(bool loadA, FILE* fp);
    //bool LoadMeshElementsFromSolution(FILE* fp);
    //bool LoadPBCFromSolution(FILE* fp);
    //bool LoadAGEsFromSolution(FILE* fp);

// Implementation
public:
    // lua extensions
    bool luafired;

private:

    char warnBuf [1028];

//#ifdef _DEBUG
    //virtual void AssertValid() const;
    //virtual void Dump(CDumpContext& dc) const;
//#endif

};
#endif
