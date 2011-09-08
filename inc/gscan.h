#ifndef	_GSCAN_CONFIG_H
#define	_GSCAN_CONFIG_H

#include <lvtypes.h>

#define TAG_GSCAN_CONFIG       0xE000
#define TAG_IAM                0xEA00

typedef struct _gscan_config_tag {
  U16 Tag;
  U16 Bytes;
  char NameDevice[8];
  char FwVersion[6];
  U8 IoVersion;
  char SerNumDevice[8];
  U8 reserv1;
} TGscanConfig, *PGscanConfig;

typedef struct _calibr_r_rec_tag {
  U16 Tag;
  U16 Bytes;
  U16 R[7];
  U16 Code[7];
  U32 RDateTime;
  U16 reserv[14];
} TCalibrRRecord, *PCalibrRRecord;

typedef struct _research_config_tag {
  U16 Tag;
  U16 Bytes;
  char MedicalCard[12];
  U32 StartTime;
  U16 MeasuringTag;
  U8  PositionPHProbe;
  U8  Reserve;
} TResearchConfig, *PResearchConfig;

typedef struct _research_data_config_tag {
  U16 Tag;
  U16 Bytes;
  U16 K;
  U8  Bits;
  U8  Polarity;
  U8  Endian;
  U8  Offset;
  U16 BaseVoltage;
  U16 SamplePerSec;
  U8  Reserve[46];
} TResearchDataConfig, *PResearchDataConfig;

typedef struct _iam_research_data_config_tag {
  TResearchDataConfig R;
  TResearchDataConfig pH;
} TIamResearchDataConfig, *PIamResearchDataConfig;

typedef struct _iam_final_research_config_tag {
  U16 Tag;
  U16 Bytes;
  U32 FinishTime;
  U8  Flags;
  U8  Reserve[7];
} TIamFinalResearchRecord, *PIamFinalResearchRecord;

typedef struct _calibr_ph_probe_tag {
  U16 pH;
  U16 Code[3];
} TpHProbeCalibr, *PpHProbeCalibr;

typedef struct _calibr_ph_probe_record_tag {
//  U16 Tag;
//  U16 Bytes;
  unsigned char No[8];
  U8 MaxAnodpH;
  U8 Type;
  TpHProbeCalibr pHProbe[4];
  U32 DateTime;
} TCalibrPHRecord, *PCalibrPHRecord;


#endif
