/****************************************************************************
 *
 * class CaeUnsCBAero
 *
 * Proprietary software product of Pointwise, Inc.
 * Copyright (c) 1995-2016 Pointwise, Inc.
 * All rights reserved.
 *
 ***************************************************************************/

#include "apiCAEP.h"
#include "apiCAEPUtils.h"
#include "apiGridModel.h"
#include "apiPWP.h"
#include "runtimeWrite.h"
#include "pwpPlatform.h"

#include "CaePlugin.h"
#include "CaeUnsGridModel.h"
#include "CaeUnsCBAero.h"

/*
NumNodes NumTris NumTets
NodeX_1
   ...
NodeX_NumNodes
NodeY_1
   ...
NodeY_NumNodes
NodeZ_1
   ...
NodeZ_NumNodes
Tri_1_NodeNdx1 Tri_1_NodeNdx2 Tri_1_NodeNdx3
   ...
Tri_NumTris_NodeNdx1 Tri_NumTris_NodeNdx2 Tri_NumTris_NodeNdx3
Tri_1_Flag1 Tri_1_Flag2
   ...
Tri_NumTris_Flag1 Tri_NumTris_Flag2
*/

//***************************************************************************
//***************************************************************************
//***************************************************************************

CaeUnsCBAero::CaeUnsCBAero(CAEP_RTITEM *pRti, PWGM_HGRIDMODEL
        model, const CAEP_WRITEINFO *pWriteInfo) :
    CaeUnsPlugin(pRti, model, pWriteInfo)
{
}

CaeUnsCBAero::~CaeUnsCBAero()
{
}

bool
CaeUnsCBAero::beginExport()
{
    //PWP_BOOL doDump;
    //model_.getAttribute("debugDump", doDump);
    //model_.getAttribute("quality", quality_);
    setProgressMajorSteps(5); // Xs, Ys, Zs, Tris, Flags
    return model_.appendEnumElementOrder(PWGM_ELEMORDER_VC);
}

PWP_BOOL
CaeUnsCBAero::write()
{
    return (writeHeader() && writeNodes() && writeTris() && writeFlags()) ?
        PWP_TRUE : ((rti_.opAborted = true), PWP_FALSE);
}

bool
CaeUnsCBAero::writeHeader()
{
    // NumNodes NumTris NumTets
    const PWP_UINT32 NumTets = 0;
    return rtFile_.isOpen() && rtFile_.write(model_.vertexCount(), " ") &&
        rtFile_.write(model_.elementCount(), " ") &&
        rtFile_.write(NumTets, "\n");
}

bool
CaeUnsCBAero::writeNodes()
{
    //NodeX_1
    //   ...
    //NodeX_NumNodes
    //NodeY_1
    //   ...
    //NodeY_NumNodes
    //NodeZ_1
    //   ...
    //NodeZ_NumNodes
    return writeNodeXYZs(PWGM_XYZ_X) && writeNodeXYZs(PWGM_XYZ_Y) &&
        writeNodeXYZs(PWGM_XYZ_Z);
}

bool
CaeUnsCBAero::writeNodeXYZs(const PWGM_ENUM_XYZ which)
{
    //NodeWHICH_1
    //   ...
    //NodeWHICH_NumNodes
    bool ret = progressBeginStep(model_.vertexCount());
    if (ret) {
        PWGM_XYZVAL val;
        CaeUnsVertex vert(model_);
        while (vert.isValid()) {
            if (!vert.xyzVal(which, val)) {
                ret = false;
                break;
            }
            if (!rtFile_.write(val, "\n")) {
                ret = false;
                break;
            }
            if (!progressIncrement()) {
                ret = false;
                break;
            }
            ++vert;
        }
    }
    return progressEndStep() && ret;
}


static const char ElemCntFmt[] = "%-10.10lu\n";

bool
CaeUnsCBAero::writeTris()
{
    //Tri_1_NodeNdx1 Tri_1_NodeNdx2 Tri_1_NodeNdx3
    //   ...
    //Tri_NumTris_NodeNdx1 Tri_NumTris_NodeNdx2 Tri_NumTris_NodeNdx3
    PwpFile fTaglist;
    PwpFile fTag;
    bool ret = progressBeginStep(model_.elementCount()) &&
        fTaglist.open(mkFileName(".Aero.taglist"), pwpWrite | pwpAscii);
    if (ret) {
        CaeUnsEnumElementData ed;
        CaeCondition cond;
        std::string condName;
        PWP_UINT32 blkId = PWP_UINT32_MAX;
        PWP_UINT32 vcElemCnt = 0; // running elem cnt for current cond
        CaeUnsElement elem(model_);
        while (elem.isValid()) {
            if (!elem.data(ed)) {
                ret = false;
                break;
            }
            // Are we encountering a new VC?
            if (ed.blockId() != blkId) {
                // new block, so cond may have changed
                blkId = ed.blockId();
                if (!CaeUnsBlock(model_, blkId).condition(cond)) {
                    ret = false;
                    break;
                }
                else if (condName != cond.name()) {
                    // new cond!
                    condName = cond.name();
                    if (!startCond(fTaglist, fTag, vcElemCnt, cond)) {
                        ret = false;
                        break;
                    }
                }
            }
            ++vcElemCnt;
            // 1-based element index
            if (!fTag.writef("%lu\n", (unsigned long)elem.index() + 1)) {
                ret = false;
                break;
            }
            if (!rtFile_.write(ed.indexAt(0), " ")) {
                ret = false;
                break;
            }
            if (!rtFile_.write(ed.indexAt(1), " ")) {
                ret = false;
                break;
            }
            if (!rtFile_.write(ed.indexAt(2), "\n")) {
                ret = false;
                break;
            }
            if (!progressIncrement()) {
                ret = false;
                break;
            }
            ++elem;
        }
        fTaglist.close();
    }
    return progressEndStep() && ret;
}

bool
CaeUnsCBAero::writeFlags()
{
    //Tri_1_Flag1 Tri_1_Flag2
    //   ...
    //Tri_NumTris_Flag1 Tri_NumTris_Flag2
    PWP_UINT32 elemCnt = model_.elementCount();
    bool ret = progressBeginStep(elemCnt);
    if (ret) {
        const PWP_UINT32 flag0 = 1;
        const PWP_UINT32 flag1 = 1;
        for (PWP_UINT32 ii = 0; ii < elemCnt; ++ii) {
            if (!rtFile_.write(flag0), " ") {
                ret = false;
                break;
            }
            if (!rtFile_.write(flag1, "\n")) {
                ret = false;
                break;
            }
            if (!progressIncrement()) {
                ret = false;
                break;
            }
        }
    }
    return progressEndStep() && ret;
}

bool
CaeUnsCBAero::startCond(PwpFile &fTaglist, PwpFile &fTag, PWP_UINT32 &elemCnt,
    const CaeCondition &cond)
{
    bool ret = true;
    if (fTag.isOpen()) {
        // strip filename from path sans extension
        std::string tagNm(fTag.getName());
        size_t pos = tagNm.find_last_of("/\\");
        if (std::string::npos != pos) {
            tagNm.erase(0, pos);
        }
        pos = tagNm.find_last_of(".");
        if (std::string::npos != pos) {
            tagNm.erase(pos);
        }
        // write actual elem count value AND write tag entry to taglist
        ret = fTag.rewind() &&
            fTag.writef(ElemCntFmt, (unsigned long)elemCnt) &&
            fTag.close() &&
            fTaglist.writef("%lu %s\n", (unsigned long)elemCnt, tagNm.c_str());
    }
    elemCnt = 0; // reset for next tag group

    if (ret) {
        // open new tag file
        std::string ext(".Aero_");
        ext += cond.name();
        // open tag file AND write elem count placeholder value
        ret = fTag.open(mkFileName(ext), pwpWrite | pwpAscii) &&
            fTag.writef(ElemCntFmt, (unsigned long)elemCnt);
    }
    return ret;
}

std::string
CaeUnsCBAero::mkFileName(const std::string &ext)
{
    if (baseFilename_.empty()) {
        baseFilename_ = exportDestination();
        const size_t pos = baseFilename_.rfind('.');
        if (std::string::npos != pos) {
            baseFilename_.erase(pos);
        }
    }
    return baseFilename_ + ext;
}

bool
CaeUnsCBAero::endExport()
{
    return !aborted();
}



//===========================================================================
// face streaming handlers - NOT USED
//===========================================================================

PWP_UINT32
CaeUnsCBAero::streamBegin(const PWGM_BEGINSTREAM_DATA &data)
{
    (void)&data; // stop unused warning
    //char msg[128];
    //sprintf(msg, "STREAM BEGIN: %lu", (unsigned long)data.totalNumFaces);
    //sendInfoMsg(msg);
    return 1;
}

PWP_UINT32
CaeUnsCBAero::streamFace(const PWGM_FACESTREAM_DATA &data)
{
    (void)&data; // stop unused warning
    //char msg[128];
    //sprintf(msg, "  STREAM FACE: %lu %lu", (unsigned long)data.owner.cellIndex,
    //    (unsigned long)data.face);
    //sendInfoMsg(msg);
    return 1;
}

PWP_UINT32
CaeUnsCBAero::streamEnd(const PWGM_ENDSTREAM_DATA &data)
{
    (void)&data; // stop unused warning
    //char msg[128];
    //sprintf(msg, "STREAM END: %s", (data.ok ? "true" : "false"));
    //sendInfoMsg(msg);
    return 1;
}


//===========================================================================
// called ONCE when plugin first loaded into memory
//===========================================================================

bool
CaeUnsCBAero::create(CAEP_RTITEM &rti)
{
    (void)rti.BCCnt; // silence unused arg warning
    bool ret = true;

    //-----------------------------------------------------------------------
    // BYTE ORDERING:
    //   Set the following flags to control the byte ordering options
    //   supported by the solver. If all flags are false, the plugin will use
    //   the platform's native byte ordering. Currently, Pointwise only runs on
    //   little endian, intel platforms. If the solver targeted by this plugin
    //   cannot import little endian files, you must set bigEndian to true and
    //   littleEndian to false.
    //-----------------------------------------------------------------------
    bool bigEndian = false;
    bool littleEndian = false;
    if (ret && (bigEndian || littleEndian)) {
        ret = allowByteOrders(rti, bigEndian, littleEndian);
    }

    //-----------------------------------------------------------------------
    // ELEMENT TOPOLOGY:
    //   Set the following flags to control the element topology options
    //   supported by the solver. If all flags are false, the allowed element
    //   topologies will be inferred from the supported element types. Unless
    //   this plugin has special needs, you should leave these all false.
    //-----------------------------------------------------------------------
    bool structured = false;
    bool unstructured = false;
    bool prismatic = false;
    if (ret && (structured || unstructured || prismatic)) {
        ret = allowElementTopologies(rti, structured, unstructured, prismatic);
    }

    // These attributes are for example only. You can publish any attribute
    // needed for your solver.
    // ret = ret &&
    //     publishUIntValueDef(rti, "iterations", 5, "Number of iterations", 0,
    //          2000) &&
    //     publishIntValueDef(rti, "magnitude", -5, "Signed int magnitude",
    //          -100, 100) &&
    //     publishRealValueDef(rti, "mach", 0.3, "Incoming flow velocity", 0.0,
    //          1000.0, 0.0, 50.0) &&
    //     publishRealValueDef(rti, "temperature", 77.5, "Ambient temperature",
    //          -5000.0, 5000.0, -100.0, 3000.0) &&
    //     publishEnumValueDef(rti, "temperature.units", "Fahrenheit",
    //          "Grid temperature units", "Fahrenheit|Celsius") &&
    //     publishEnumValueDef(rti, "units", "Inches", "Grid dimensional units",
    //          "Yards|Inches|Meters|Millimeters") &&
    //     publishStringValueDef(rti, "description", "", "Grid description") &&
    //     publishBoolValueDef(rti, "linear", false, "Grid is linear",
    //          "reject|accept");

    return ret;
}


//===========================================================================
// called ONCE just before plugin unloaded from memory
//===========================================================================

void
CaeUnsCBAero::destroy(CAEP_RTITEM &rti)
{
    (void)rti.BCCnt; // silence unused arg warning
}
