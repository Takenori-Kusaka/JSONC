#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsonc.h"

#define CHECK_ALLOCATE(p,tmp,rtn) { \
	if( tmp == NULL ){ \
		free(p); \
		return rtn; \
	} \
	else{ \
		p = tmp; \
	} \
}

typedef enum{
	KEY_TYPE_TYPE,
	KEY_TYPE_SIGNAL,
	KEY_TYPE_VALUE,
	KEY_TYPE_RESOLUTION,
	KEY_TYPE_MINIMUM,
	KEY_TYPE_MAXIMUM,
	ALL_KEY_TYPE
}KEY_TYPE_e;

static char *keylist[ALL_KEY_TYPE]={
	"Type",
	"Signal",
	"Value",
	"Resolution",
	"Minimum",
	"Maximum"
};
typedef enum{
	KEY_TYPE_TYPE_TEMPERATURE,
	KEY_TYPE_TYPE_HUMIDITY,
	KEY_TYPE_TYPE_VALUE,
	KEY_TYPE_TYPE_RESOLUTION,
	KEY_TYPE_TYPE_MINIMUM,
	KEY_TYPE_TYPE_MAXIMUM,
	ALL_KEY_TYPE_TYPE
}KEY_TYPE_TYPE_e;

static char *keylist_type[ALL_KEY_TYPE_TYPE]={
	"Temperature",
	"Humidity",
	"Value",
	"Resolution",
	"Minimum",
	"Maximum"
};
typedef enum{
	KEY_TYPE_TYPE_TEMPERATURE_VALUE,
	KEY_TYPE_TYPE_TEMPERATURE_MAXIMUM,
	KEY_TYPE_TYPE_TEMPERATURE_MINIMUM,
	ALL_KEY_TYPE_TYPE_TEMPERATURE
}KEY_TYPE_TYPE_TEMPERATURE_e;

static char *keylist_type_temperature[ALL_KEY_TYPE_TYPE_TEMPERATURE]={
	"Value",
	"Maximum",
	"Minimum"
};

typedef struct{
	JsonInit_Arg_t type;
	JsonInit_Arg_t signal;
	JsonInit_Arg_t value;
	JsonInit_Arg_t resolution;
	JsonInit_Arg_t minimum;
	JsonInit_Arg_t maximum;
}SampleKeyList_t;

SampleKeyList_t keyList={0};

typedef struct{
	float value;
	float maximum;
	float minimum;
}TemperatureData_t;

TemperatureData_t temperature = {0};

void main_init( void )
{
	keyList.type.keyName[MAIN_KEY] = keylist[KEY_TYPE_TYPE];
	keyList.type.keyName[SUB_KEY] = keylist_type[KEY_TYPE_TYPE_TEMPERATURE];
	keyList.type.keyName[SSUB_KEY] = keylist_type_temperature[KEY_TYPE_TYPE_TEMPERATURE_VALUE];
	keyList.type.value = (void*)&temperature.value;
	JSONC_Init( &keyList.type );
}

int main() {
	int i;
	FILE *fp = NULL;
	char *str = (char*)malloc(1);
	char *temp = NULL;

	main_init();

    fopen_s(&fp,"GenericPrifileSetting.json","r");
	if(fp==NULL){
        //失敗と表示し終了
        printf("ファイルオープン失敗\n");
        return -1;
    }
	for( i = 0 ; (str[i] = fgetc(fp)) != EOF ; i++ ){
		temp = realloc(str,i+2);
		CHECK_ALLOCATE(str,temp,0);
	}
	str[i] = 0;
	printf("%s\n",str);

	fclose(fp);
	JSONC_parseStr( str );
	free(str);

	printf("温度値は%fです\r\n",temperature.value);

	return 0;
}
