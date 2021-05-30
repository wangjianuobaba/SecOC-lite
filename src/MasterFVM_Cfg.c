#include "MasterFVM_Cfg.h"


uint8 resetData[3*NUM_RESET];


ConfirmECU_Type confirmECU[]={  //可配置的
    {
        .state=0,
        .ackv = 0x2be
    },
    {
        .state=0,
        .ackv = 0x2bf
    }
};

ResetCnt_Type resetCnt[]={
    {
        .resetdata = resetData,
	    .ResetCntLength = 11,
        .resetcanid = 0xffff,
        .resettag = 0,
	    .resetSynTime = 1000
    },
    {
        .resetdata = resetData,
	    .ResetCntLength = 11,
        .resetcanid = 0xffff,
        .resettag = 0,
	    .resetSynTime = 2500
    }
};


