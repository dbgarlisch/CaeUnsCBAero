/****************************************************************************
 *
 * Pointwise Plugin utility functions
 *
 ***************************************************************************/

#ifndef _RTCAEPINITITEMS_H_
#define _RTCAEPINITITEMS_H_

/*.................................................
    initialize caepRtItem[0]
*/
#define ID_CaeUnsCBAero  20  // YOU MAY NEED TO CHANGE THIS TO AVOID CONFLICTS
{
    //== CAEP_FORMATINFO FormatInfo
    {   PWP_SITE_GROUPNAME,         // const char *group
        "CBAero",                   // const char *name
        MAKEGUID(ID_CaeUnsCBAero),  // PWP_UINT32 id

        PWP_FILEDEST_FILENAME,      // CAEP_ENUM_FILEDEST fileDest

        PWP_FALSE,              // PWP_BOOL allowedExportConditionsOnly
        PWP_TRUE,               // PWP_BOOL allowedVolumeConditions

        PWP_TRUE,               // PWP_BOOL allowedFileFormatASCII
        PWP_FALSE,              // PWP_BOOL allowedFileFormatBinary
        PWP_FALSE,              // PWP_BOOL allowedFileFormatUnformatted

        PWP_FALSE,              // PWP_BOOL allowedDataPrecisionSingle
        PWP_TRUE,               // PWP_BOOL allowedDataPrecisionDouble

        PWP_TRUE,               // PWP_BOOL allowedDimension2D
        PWP_FALSE               // PWP_BOOL allowedDimension3D
    },

    &pwpRtItem[1],  /* PWU_RTITEM* */

    //== CAEP_BCINFO*    pBCInfo;    -- array of format BCs or NULL
    //== PWP_UINT32      BCCnt;      -- # format BCs
    nullptr, //CaeUnsCBAeroBCInfo,      // CAEP_BCINFO*
    0, //ARRAYSIZE(CaeUnsCBAeroBCInfo), // PWP_UINT32 BCCnt

    //== CAEP_VCINFO*    pVCInfo;    -- array of format VCs or NULL
    //== PWP_UINT32      VCCnt;      -- # format VCs
    nullptr, //CaeUnsCBAeroVCInfo,       // CAEP_VCINFO* pVCInfo
    0, //ARRAYSIZE(CaeUnsCBAeroVCInfo),  // PWP_UINT32 VCCnt

    //== const char**    pFileExt;   -- array of valid file extensions
    //== PWP_UINT32      ExtCnt;     -- # valid file extensions
    CaeUnsCBAeroFileExt,            // const char **pFileExt
    ARRAYSIZE(CaeUnsCBAeroFileExt), // PWP_UINT32 ExtCnt

    //== PWP_BOOL  elemType[PWGM_ELEMTYPE_SIZE]; -- un/supported elem
    {   PWP_TRUE,              // elemType[PWGM_ELEMTYPE_BAR]
        PWP_FALSE,             // elemType[PWGM_ELEMTYPE_HEX]
        PWP_FALSE,             // elemType[PWGM_ELEMTYPE_QUAD]
        PWP_TRUE,              // elemType[PWGM_ELEMTYPE_TRI]
        PWP_FALSE,             // elemType[PWGM_ELEMTYPE_TET]
        PWP_FALSE,             // elemType[PWGM_ELEMTYPE_WEDGE]
        PWP_FALSE,             // elemType[PWGM_ELEMTYPE_PYRAMID]
        PWP_TRUE },            // elemType[PWGM_ELEMTYPE_POINT]

    nullptr,  // FILE *fp

    // PWU_UNFDATA UnfData
    {   0,                // PWP_UINT32 status
        nullptr,          // FILE *fp
        0,                // sysFILEPOS fPos
        PWP_FALSE,        // PWP_BOOL hadError
        PWP_FALSE,        // PWP_BOOL inRec
        0,                // PWP_UINT32 recBytes
        0,                // PWP_UINT32 totRecBytes
        0,                // PWP_UINT32 recCnt
        PWP_ENDIAN_ERROR, // PWP_ENDIANNESS endianness
        0 },              // PWP_UINT32 fixedBytes

    0,  // PWGM_HGRIDMODEL model

    nullptr,  // const CAEP_WRITEINFO *pWriteInfo

    0,    // PWP_UINT32 progTotal
    0,    // PWP_UINT32 progComplete
    {0},  // clock_t clocks[CAEPU_CLKS_SIZE];
    0,    // PWP_BOOL opAborted

    // if you added any custom data in rtCaepInstanceData.h,
    // you need to initialize it here. The init below matches the 
    // example MY_CAEP_DATA struct given in rtCaepInstanceData.h
    
    {   0,
        0,
        0.0,
        "string" },
    */
},

#endif /* _RTCAEPINITITEMS_H_ */
