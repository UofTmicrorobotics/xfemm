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

   Additional changes:
   Copyright 2016-2017 Johannes Zarl-Zierl <johannes.zarl-zierl@jku.at>
   Contributions by Johannes Zarl-Zierl were funded by Linz Center of
   Mechatronics GmbH (LCM)
*/

#ifndef FMESHER_H
#define FMESHER_H

#include "nosebl.h"
#include "femmcomplex.h"
#include "IntPoint.h"

#include "CArcSegment.h"
#include "CBlockLabel.h"
#include "CBoundaryProp.h"
#include "CCircuit.h"
#include "CNode.h"
#include "CPointProp.h"
#include "CSegment.h"
#include "femmenums.h"
#include "FemmProblem.h"

#include <memory>
#include <vector>
#include <string>

#ifndef LineFraction
#define LineFraction 500.0
#endif

// Default mesh size is the diagonal of the geometry's
// bounding box divided by BoundingBoxFraction
#ifndef BoundingBoxFraction
#define BoundingBoxFraction 100.0
#endif

// forward definition
struct triangulateio;

namespace fmesher
{

// FMesher Class

class FMesher
{

protected:


// Attributes
public:

    enum loaderrors { F_FILE_OK, F_FILE_UNKNOWN_TYPE, F_FILE_NOT_OPENED, F_FILE_MALFORMED};

    explicit FMesher();
    explicit FMesher(std::string);
    explicit FMesher(std::shared_ptr<femm::FemmProblem> p);

    femm::EditMode d_EditMode;
    std::shared_ptr<femm::FemmProblem> problem;
	bool    Verbose;

	std::string BinDir;

	// vectors containing the mesh information
    std::vector< std::unique_ptr<femm::IntPoint> >      meshline;
    std::vector< std::unique_ptr<femm::IntPoint> >      greymeshline;
    std::vector< std::unique_ptr<femm::CNode> >	meshnode;

    // used to echo start of input file to output
    std::vector< std::string > probdescstrings;

// Operations
public:

    static femm::FileType GetFileType(std::string PathName);
    bool SaveFEMFile(std::string PathName); ///< \deprecated
    bool WriteTriangulationFiles(const struct triangulateio &out, std::string Pathname);

    //void downstr(char *s);

    // Core functions
	bool LoadMesh(std::string PathName);
	int DoNonPeriodicBCTriangulation(std::string PathName);
	int DoPeriodicBCTriangulation(std::string PathName);
    //bool OldOnOpenDocument(LPCTSTR lpszPathName);
	bool HasPeriodicBC();

    // pointer to function to call when issuing warning messages
    void (*WarnMessage)(const char*);

    // pointer to function to use for triangle to issue warning messages
    int (*TriMessage)(const char * format, ...);

    /**
     * @brief Rotate copies of the selected objects of the selected type.
     * Each copy is rotated by the given amount compared to the previous copy.
     * @param c center
     * @param dt (incremental) angle in degrees
     * @param ncopies number of copies
     * @param selector
     */
    void RotateCopy(CComplex c, double dt, int ncopies, femm::EditMode selector);
    /**
     * @brief Rotate the selected objects of the selected type.
     * @param c center
     * @param t angle
     * @param selector
     */
    void RotateMove(CComplex c, double t, femm::EditMode selector);

    /**
     * @brief Scale the selected objects relative to a base point
     * @param bx base-point x
     * @param by base-point y
     * @param sf scale factor
     * @param selector
     */
    void ScaleMove(double bx, double by, double sf, femm::EditMode selector);

    /**
     * @brief Translate copies of the selected objects of the selected type.
     * Each copy is translated by the given amount compared to the previous copy.
     * @param incx offset
     * @param incy offset
     * @param ncopies number of copies
     * @param selector
     */
    void TranslateCopy(double incx, double incy, int ncopies, femm::EditMode selector);
    /**
     * @brief Translate the selected objects of the selected type.
     * @param dx
     * @param dy
     * @param selector
     */
    void TranslateMove(double dx, double dy, femm::EditMode selector);

private:

    virtual bool Initialize(femm::FileType t);
	void addFileStr (char * q);

};

} // namespace femm

#endif
