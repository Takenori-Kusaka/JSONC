#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsonc.h"

#define CHECK_ALLOCATE(p,tmp,rtn) { \
	if(tmp == NULL){ \
		free(p); \
		return rtn; \
	} \
	else{ \
		p = tmp; \
	} \
}

typedef enum{
	VALUE_TYPE_INTEGER,
	VALUE_TYPE_FLOAT,
	VALUE_TYPE_STRING,
	ALL_VALUE_TYPE
}VALUE_TYPE;

typedef struct Unit{
	char *key;
	struct Unit *value;
}Unit;

typedef struct{
	Unit *unit;
	char *str;
	size_t max;
}Extraction_Arg_t;

extern const JSONC_KeySet_t keylist[];
const char *keyTypeArray = "KeyTypeArray";

static struct{
	float ftemp;
	int s32temp;
	void *temp;
}privateValue;

static void setValueType(void *pDestination, char *value);
static void cmpKeyList(Unit *unit, JSONC_KeySet_t *keySet);
static int arrayExtraction(Extraction_Arg_t *arg);
static int extraction(Extraction_Arg_t *arg);

static void setValueType(void *pDestination, char *value)
{
	unsigned char i;
	unsigned int length = strlen(value);
	VALUE_TYPE type = VALUE_TYPE_STRING;

	if(value[0] != '"'){
		type = VALUE_TYPE_INTEGER;
		for(i = 0 ; i < length ; i++)
		{
			if(value[i] == '.'){
				type = VALUE_TYPE_FLOAT;
				break;
			}
			else{
			}
		}
	}
	else{
	}

	switch(type)
	{
	case VALUE_TYPE_INTEGER:
		privateValue.s32temp = atoi(value);
		memcpy(pDestination, &privateValue.s32temp, sizeof(int));
		break;
	case VALUE_TYPE_FLOAT:
		privateValue.ftemp = (float)atof(value);
		memcpy(pDestination, &privateValue.ftemp, sizeof(float));
		break;
	case VALUE_TYPE_STRING:
		memcpy(pDestination, &value[1], length - 2);
		value[length - 1] = 0;
		break;
	default:
		break;
	}
}

static void cmpKeyList(Unit *unit, JSONC_KeySet_t *keySet)
{
	unsigned char i;

	for(; keySet->value != NULL ; keySet++){
		if(strcmp(unit->key,keySet->KeyName) == 0){
			if(unit->value->value == NULL){
				setValueType(keySet->value, unit->value->key);
			}
			else{
				for(i = 0 ; unit->value[i].key != NULL ; i++){
					cmpKeyList(&unit->value[i], (JSONC_KeySet_t*)keySet->value);
				}
			}
		}
		else{
		}
	}
}

static int arrayExtraction(Extraction_Arg_t *arg)
{
	unsigned char i;
	unsigned char index = 0;
	unsigned int j;
	unsigned int start = 0;
	Extraction_Arg_t *next = (Extraction_Arg_t*)malloc(sizeof(Extraction_Arg_t));
	
	/* Top detection */
	if((arg->str[0] == '[')){
		start = 1;
	}
	else{
		return 0;
	}

	next->max = arg->max;

	/* Allocate the first "Key" and "Value" */
	arg->unit = (Unit*)malloc(sizeof(Unit));
	
	/* Roop maximum number of characters */
	for(j = 1; j < arg->max ; j++){
		
		/* Pass the next JSON by self-reference */
		if((arg->str[j] == '{') || (arg->str[j] == '[')){

			next->str = &arg->str[j];
			if(arg->str[j] == '{'){
				j += extraction(next);
			}
			else{
				j += arrayExtraction(next);
			}
			arg->unit[index].value = next->unit;
			
			/* Allocate next save destination */
			privateValue.temp = (Unit*)realloc(arg->unit, sizeof(Unit) * (index + 2));
			CHECK_ALLOCATE(arg->unit, privateValue.temp, -1);

			/* Termination determination processing */
			if(arg->str[j + 1] == ','){
				j++;
				start = j + 1;
				index++;
			}
			else{
				for(i = 0 ; i <= index ; i++)arg->unit[i].key = (char*)keyTypeArray;
				arg->unit[index + 1].key = NULL;
				arg->unit[index + 1].value = NULL;
				return j+1;
			}
		}

		/* Detect Value */
		else if((arg->str[j] == ',')||(arg->str[j] == ']')){
			/* Save the previous value as "Value" */
			arg->unit[index].value = (Unit*)malloc(sizeof(Unit));
			arg->unit[index].value->key = (char*)calloc(j - start + 1,sizeof(char));
			arg->unit[index].value->value = NULL;
			memcpy(arg->unit[index].value->key, &arg->str[start],j - start);
			
			privateValue.temp = (Unit*)realloc(arg->unit, sizeof(Unit) * (index + 2));
			CHECK_ALLOCATE(arg->unit, privateValue.temp, -1);
			if(arg->str[j] == ','){
				/* Extend the next save destination */
				start = j + 1;
				index++;
			}
			else{
				for(i = 0 ; i <= index ; i++)arg->unit[i].key = (char*)keyTypeArray;
				arg->unit[index + 1].key = NULL;
				arg->unit[index + 1].value = NULL;
				return j;
			}
		}

		else{
		}
	}
	return -2;
}

static int extraction(Extraction_Arg_t *arg)
{
	unsigned int j;
	unsigned int start;
	unsigned int index = 0;
	Extraction_Arg_t *next = (Extraction_Arg_t*)malloc(sizeof(Extraction_Arg_t));
	
	/* Top detection */
	if((arg->str[0] == '{')){
		start = 1;
	}
	else{
		return 0;
	}

	next->max = arg->max;

	/* Allocate the first "Key" and "Value" */
	arg->unit = (Unit*)malloc(sizeof(Unit));
	
	/* Roop maximum number of characters */
	for(j = 1; j < arg->max ; j++){

		/* "Key" detection */
		if(arg->str[j] == ':'){

			/* Allocate the "Key" save destination */
			arg->unit[index].key = (char*)calloc(j - start - 1, sizeof(char));
			memcpy(arg->unit[index].key, &arg->str[start + 1], j - start - 2);
			start = j + 1;
		}
		
		/* Pass the next JSON by self-reference */
		if((arg->str[j] == '{') || (arg->str[j] == '[')){
			next->str = &arg->str[j];
			if(arg->str[j] == '{'){
				j += extraction(next);
			}
			else{
				j += arrayExtraction(next);
			}
			arg->unit[index].value = next->unit;
			
			/* Allocate next save destination */
			privateValue.temp = (Unit*)realloc(arg->unit, sizeof(Unit) * (index + 2));
			CHECK_ALLOCATE(arg->unit, privateValue.temp, -1);

			/* Termination determination processing */
			if(arg->str[j + 1] == ','){
				j++;
				start = j + 1;
				index++;
			}
			else{
				arg->unit[index + 1].key = NULL;
				arg->unit[index + 1].value = NULL;
				return j+1;
			}
		}
		else if((arg->str[j] == ',')||(arg->str[j] == '}')){
			/* Save the previous value as "Value" */
			arg->unit[index].value = (Unit*)malloc(sizeof(Unit));
			arg->unit[index].value->key = (char*)calloc(j-start+1,sizeof(char));
			arg->unit[index].value->value = NULL;
			memcpy(arg->unit[index].value->key, &arg->str[start],j-start);
			
			privateValue.temp = (Unit*)realloc(arg->unit,sizeof(Unit) * (index + 2));
			CHECK_ALLOCATE(arg->unit, privateValue.temp, -1);
			if(arg->str[j] == ','){
				/* Extend the next save destination */
				start = j+1;
				index++;
			}
			else{
				arg->unit[index + 1].key = NULL;
				arg->unit[index + 1].value = NULL;
				return j;
			}
		}

		else{
		}
	}
	return -2;
}

JSONC_ERROR_TYPE JSONC_parseStr(char *str)
{
	unsigned char strFlag = 0;
	unsigned int success;
	unsigned int j,k;
	char *minJson = (char*)malloc(1);
	Extraction_Arg_t extractionArg;

	if(minJson == NULL){
		free(minJson);
		return JSONC_FAIL_ALLOCATE;
	}
	else{
	}

	/* Remove unnecessary characters */
	k = 0;
	for(j = 0 ; str[j] != 0 ; j++){
		if(strFlag){
			if(str[j] == '"'){
				strFlag = 0;
			}
			else{
			}
			minJson[k++] = str[j];
			privateValue.temp = realloc(minJson,k+1);
			CHECK_ALLOCATE(minJson,privateValue.temp,JSONC_FAIL_ALLOCATE);
		}
		else{
			if((str[j] <= ' ') || (str[j] >= 0x7f)){
			}
			else{
				if(str[j] == '"'){
					strFlag = 1;
				}
				else{
				}
				minJson[k++] = str[j];
				privateValue.temp = realloc(minJson,k+1);
				CHECK_ALLOCATE(minJson, privateValue.temp, JSONC_FAIL_ALLOCATE);
			}
		}
	}
	minJson[k] = 0;
	printf("%s\n",minJson);

	/* Extract "Key" and "Value" */
	extractionArg.str = minJson;
	extractionArg.max = strlen(minJson);
	success = extraction(&extractionArg);
	free(minJson);
	if(success == 0){
		free(extractionArg.unit);
		return JSONC_ILLEGAL_STR;
	}
	else if(success == -1){
		free(extractionArg.unit);
		return JSONC_FAIL_ALLOCATE;
	}
	else if(success == -2){
		free(extractionArg.unit);
		return JSONC_MISSING_END_POINT;
	}
	else{
	}

	/* Execute the set "CBFunc" */
	for(j = 0 ; extractionArg.unit[j].key != NULL ; j++){
		cmpKeyList(&extractionArg.unit[j],(JSONC_KeySet_t*)keylist);
	}
	free(extractionArg.unit);

	return JSONC_SUCCESS;
}