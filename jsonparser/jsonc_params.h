#ifndef JSONC_PARAMS_H__
#define JSONC_PARAMS_H__

#include <stdio.h>
#include "jsonc.h"

struct{
	char sendType[10];
	char senderID[10];
	struct{
		unsigned char tempHumiSensor[10];
		unsigned char soilMoisSensor[10];
		unsigned char soilTempSensor[10];
		unsigned char illuminanceSensor[10];
	}orcaSetting;
	struct{
		char eep[10];
	}fourBSSetting;
	float temperature;
	float humidity;
	float soilMoisture;
	float soilTemperature;
	unsigned int illuminance;
	unsigned int sendPeriod;
}jsoncParams;

const JSONC_KeySet_t keyListOrcaSetting[5]={
	{"TempHumiSensor",(void*)jsoncParams.orcaSetting.tempHumiSensor},
	{"SoilMoisSensor",(void*)jsoncParams.orcaSetting.soilMoisSensor},
	{"SoilTempSensor",(void*)jsoncParams.orcaSetting.soilTempSensor},
	{"IlluminanceSensor",(void*)jsoncParams.orcaSetting.illuminanceSensor},
	{NULL,(void*)NULL}
};

const JSONC_KeySet_t keyLisFourBSSetting[2]={
	{"EEP",(void*)&jsoncParams.fourBSSetting.eep},
	{NULL,(void*)NULL}
};

const JSONC_KeySet_t keylist[15] = {
	{"Send Type",(void*)jsoncParams.sendType},
	{"Sender ID",(void*)jsoncParams.senderID},
	{"ORCA_Setting",(void*)&keyListOrcaSetting},
	{"4BS_Setting",(void*)&keyLisFourBSSetting},
	{"Temperature",(void*)&jsoncParams.temperature},
	{"Humidity",(void*)&jsoncParams.humidity},
	{"Soil Moisture",(void*)&jsoncParams.soilMoisture},
	{"Soil Temperature",(void*)&jsoncParams.soilTemperature},
	{"Illuminance",(void*)&jsoncParams.illuminance},
	{"Send Period",(void*)&jsoncParams.sendPeriod},
	{NULL,(void*)NULL}
};

#endif