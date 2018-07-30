/****************************************************************************
 *
 * class CaeUnsCBAero
 *
 * Proprietary software product of Pointwise, Inc.
 * Copyright (c) 1995-2016 Pointwise, Inc.
 * All rights reserved.
 *
 ***************************************************************************/

#ifndef _CAEUNSCBAERO_H_
#define _CAEUNSCBAERO_H_

#include "apiGridModel.h"
#include "apiPWP.h"

#include "CaePlugin.h"
#include "CaeUnsGridModel.h"


//***************************************************************************
//***************************************************************************
//***************************************************************************

class CaeUnsCBAero :
    public CaeUnsPlugin,
    public CaeFaceStreamHandler {

public:
    CaeUnsCBAero(CAEP_RTITEM *pRti, PWGM_HGRIDMODEL model,
        const CAEP_WRITEINFO *pWriteInfo);
    virtual ~CaeUnsCBAero();
    static bool create(CAEP_RTITEM &rti);
    static void destroy(CAEP_RTITEM &rti);

private:

    virtual bool        beginExport();
    virtual PWP_BOOL    write();
    virtual bool        endExport();

    // face streaming handlers
    virtual PWP_UINT32 streamBegin(const PWGM_BEGINSTREAM_DATA &data);
    virtual PWP_UINT32 streamFace(const PWGM_FACESTREAM_DATA &data);
    virtual PWP_UINT32 streamEnd(const PWGM_ENDSTREAM_DATA &data);

    bool    writeHeader();
    bool    writeNodes();
    bool    writeNodeXYZs(const PWGM_ENUM_XYZ which);
    bool    writeTris();
    bool    writeFlags();

private:
};

#endif // _CAEUNSCBAERO_H_
