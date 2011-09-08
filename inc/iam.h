#ifndef	_IAM_H
#define	_IAM_H

#include <lvtypes.h>
#include <gscan.h>

#define MAX_R_SENSORS   7
#define MAX_PH_SENSORS  3
#define MAX_PH_SOLUTION 4

//#define TAG_IAM                0xEA00
#define TAG_IAM_RESEARCH       TAG_IAM + 1
#define TAG_IAM_DATA_CONFIG_R  TAG_IAM + 2
#define TAG_IAM_DATA_CONFIG_PH TAG_IAM + 3
#define TAG_IAM_CALIBR         TAG_IAM + 4
//#define TAG_IAM_DATA_7R        TAG_IAM + 5
#define TAG_IAM_DATA_3PH       TAG_IAM + 6
#define TAG_IAM_DATA_7R_3PH    TAG_IAM + 7
#define TAG_IAM_FINAL_RESEARCH TAG_IAM + 8
#define TAG_IAM_EVENT          TAG_IAM + 9
#define TAG_IAM_CALIBR_PH      TAG_IAM + 10
#define TAG_IAM_PROBE_PH       TAG_IAM + 11
#define TAG_IAM_RESEARCH_EX    TAG_IAM + 12

#define POWER_ALARM_CODE  0x0C80 // 1.95 Â

/*
typedef struct _gscan_config_tag {
  U16 Tag;
  U16 Bytes;
  char NameDevice[8];
  char SwVersion[6];
  U8 IoVersion;
  char SerNumDevice[8];
} TGscanConfig, *PGscanConfig;

typedef struct _calibr_rec_tag {
  U16 Tag;
  U16 Bytes;
  U16 R[7];
  U16 Code[7];
  U32 RDateTime;
  U16 pH[12];
  U32 pHDateTime;
} TCalibrRecord, *PCalibrRecord;

typedef struct _research_config_tag {
  U16 Tag;
  U16 Bytes;
  char MedicalCard[12];
  U32 StartTime;
  U16 MeasuringTag;
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
  U8  Reserve[48];
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
*/

typedef struct _iam_environment_tag {
  U8 TypeResearch;
  U8 HoursResearch;
  char MedicalCard[12];
  char NumberZond[8];
  U8 PositionPHProbe;
  PCalibrPHRecord pProbe;
} TIamEnvironment, *PIamEnvironment;

#define MAX_TYPE_RESEARCH  2
#define MAX_POSITION_PH    5

//#define TYPE_R_RESEARCH    0
#define TYPE_PH_RESEARCH   0
#define TYPE_RPH_RESEARCH  1
//#define MAX_PH_PROBE       4

#define MAX_RECORD_PH_PROBE 10

#define DEFAULT_POSITION_PH_PROBE   0
#define DEFAULT_HOURS_RESEARCH     24
#define DEFAULT_TYPE_RESEARCH      TYPE_RPH_RESEARCH
#define DEFAULT_NUMBER_SOLUTION     2

#endif
