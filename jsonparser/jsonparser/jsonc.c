#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsonc.h"

#define MAX_CMD 100

#define CHECK_ALLOCATE(p,tmp,rtn) { \
	if( tmp == NULL ){ \
		free(p); \
		return rtn; \
	} \
	else{ \
		p = tmp; \
	} \
}

typedef struct Unit{
	char *key;
	struct Unit *value;
}Unit;

typedef struct{
	Unit *unit;
	char *str;
	size_t max;
}Extraction_Arg_t;

static unsigned char maxKey = 0;
static JsonInit_Arg_t *allkey[MAX_CMD] = {0};

typedef struct{
	Unit *unit;
	unsigned char listNum;
	KEY_LEVEL level;
}CmpKeyList_Arg_t;

typedef enum{
	VALUE_TYPE_INTEGER,
	VALUE_TYPE_FLOAT,
	VALUE_TYPE_STRING,
	ALL_VALUE_TYPE
}VALUE_TYPE;

#define MAX_ARRAY 10

static char *arrayIndex[MAX_ARRAY] = {
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9"
};

static void setValueType( void *pDestination, char *value )
{
	unsigned int i;
	unsigned int length = strlen(value);
	float ftemp;
	int s32temp;
	VALUE_TYPE type = VALUE_TYPE_INTEGER;

	for( i = 0 ; i < length ; i++ )
	{
		if( value[i] < '.' ){
			type = VALUE_TYPE_STRING;
			break;
		}
		else if( value[i] >= ':' ){
			type = VALUE_TYPE_STRING;
			break;
		}
		else if( value[i] == '/' ){
			type = VALUE_TYPE_STRING;
			break;
		}
		else if( value[i] == '.' ){
			type = VALUE_TYPE_FLOAT;
		}
		else{
		}
	}

	switch( type )
	{
	case VALUE_TYPE_INTEGER:
		s32temp = atoi( value );
		memcpy( pDestination, &s32temp, sizeof(int) );
		break;
	case VALUE_TYPE_FLOAT:
		ftemp = (float)atof( value );
		memcpy( pDestination, &ftemp, sizeof(float) );
		break;
	case VALUE_TYPE_STRING:
		memcpy( pDestination, value, length+1 );
		break;
	default:
		break;
	}
}

static void cmpKeyList( CmpKeyList_Arg_t *arg )
{
	CmpKeyList_Arg_t next;

	next.listNum = arg->listNum;
	if( arg->unit->key == NULL )return;
	else if( strcmp(arg->unit->key,allkey[arg->listNum]->keyName[arg->level]) == 0 ){
		if( arg->unit->value->value == NULL ){
			setValueType( allkey[arg->listNum]->value, arg->unit->value->key );
		}
		else{
			next.level = arg->level + 1;
			next.unit = arg->unit->value;
			cmpKeyList( &next );
		}
	}
	else{
	}
}

static void processAllCbFunc(Unit *unit)
{
	unsigned char i,j;
	CmpKeyList_Arg_t arg;

	for( j = 0 ; unit[j].key != NULL ; j++ )
	{
		for( i = 0 ; i < maxKey ; i++ )
		{
			arg.unit = &unit[j];
			arg.listNum = i;
			arg.level = MAIN_KEY;
			cmpKeyList(&arg);
		}
	}
}

static int extraction(Extraction_Arg_t *arg)
{
	unsigned int j;
	unsigned int start = 0;
	unsigned int index = 0;
	unsigned int array = 0;
	Unit *temp = NULL;
	Extraction_Arg_t *next = (Unit*)malloc(sizeof(Unit));
	next[0].max = arg->max;

	/* Allocate the first "Key" and "Value" */
	arg->unit = (Unit*)malloc(sizeof(Unit));
	
	/* Roop maximum number of characters */
	for( j = 0; j < arg->max ; j++){
		/* Top detection */
		if((start==0)&&(arg->str[j]=='{')){
			start = j+1;
		}

		/* "Key" detection */
		else if((start!=0)&&(arg->str[j]==':')){

			/* Allocate the "Key" save destination */
			arg->unit[index].key = (char*)calloc(j-start+1,sizeof(char));
			memcpy(arg->unit[index].key,&arg->str[start],j-start);
			start = j+1;
		}

		/* Detect next JSON starting point */
		else if((start!=0)&&(arg->str[j]=='{')){

			/* Pass the next JSON by self-reference */
			next[0].str = &arg->str[j];
			j += extraction(&next[0]);
			arg->unit[index].value = next[0].unit;
			
			/* Allocate next save destination */
			temp = (Unit*)realloc(arg->unit,sizeof(Unit)*(index+2));
			CHECK_ALLOCATE(arg->unit,temp,-1);

			/* Termination determination processing */
			if(arg->str[j+1] == ','){
				j++;
				start = j + 1;
				index++;
			}
			else{
				arg->unit[index+1].key = NULL;
				arg->unit[index+1].value = NULL;
				return j;
			}
		}

		/* Detect array */
		else if((start!=0)&&(arg->str[j]=='[')){
			/* Allocate the first array */
			array = 0;
			arg->unit[index].value = (Unit*)malloc(sizeof(Unit));

			/* Self-reference to the end of the array */
			do{
				next[array].str = &arg->str[++j];
				j += extraction(&next[array])+1;
				start = j + 1;
				arg->unit[index].value[array].key = arrayIndex[array];
				arg->unit[index].value[array].value = next[array].unit;
				array++;
				temp = realloc(arg->unit[index].value,sizeof(Unit)*(array+1));
				CHECK_ALLOCATE(arg->unit[index].value,temp,-1);
				temp = realloc(next,sizeof(Extraction_Arg_t)*(array+1));
				CHECK_ALLOCATE(next,temp,-1);
			}while(arg->str[j] == ',');
			
			/* Termination determination processing */
			if( arg->str[j] == ']' ){
				j++;
				temp = (Unit*)realloc(arg->unit,sizeof(Unit)*(index+2));
				CHECK_ALLOCATE(arg->unit,temp,-1);
				if(arg->str[j] == '}'){
					arg->unit[index+1].key = NULL;
					arg->unit[index+1].value = NULL;
					return j;
				}
				else if(arg->str[j] == ','){
					index++;
					start = j + 1;
				}
				else{
					return 0;
				}
			}
			else{
				return 0;
			}
		}

		/* Detect Value */
		else if((start!=0)&&(arg->str[j]==',')){
			/* Save the previous value as "Value" */
			arg->unit[index].value = (Unit*)malloc(sizeof(Unit));
			arg->unit[index].value->key = (char*)calloc(j-start+1,sizeof(char));
			arg->unit[index].value->value = NULL;
			memcpy(arg->unit[index].value->key, &arg->str[start],j-start);

			/* Extend the next save destination */
			start = j+1;
			temp = (Unit*)realloc(arg->unit,sizeof(Unit)*(index+2));
			CHECK_ALLOCATE(arg->unit,temp,-1);
			index++;
		}

		/* Detect ending point */
		else if((start!=0)&&(arg->str[j]=='}')){
			
			/* Save the previous value as "Value" */
			arg->unit[index].value = (Unit*)malloc(sizeof(Unit));
			arg->unit[index].value->key = (char*)calloc(j-start+1,sizeof(char));
			arg->unit[index].value->value = NULL;
			memcpy(arg->unit[index].value->key, &arg->str[start],j-start);

			/* Terminus */
			temp = (Unit*)realloc(arg->unit,sizeof(Unit)*(index+2));
			CHECK_ALLOCATE(arg->unit,temp,-1);
			arg->unit[index+1].key = NULL;
			arg->unit[index+1].value = NULL;
			return j;
		}
		else{
		}
	}
	return -2;
}

JSONC_ERROR_TYPE JSONC_Init(JsonInit_Arg_t *arg)
{
	if( maxKey == MAX_CMD )return JSONC_OVER_MAX_INIT;
	allkey[maxKey] = arg;
	maxKey++;
	return JSONC_SUCCESS;
}

JSONC_ERROR_TYPE JSONC_parseStr( char *str )
{
	unsigned int success;
	unsigned int j,k;
	unsigned int length = strlen(str);
	char *minJson = (char*)malloc(1);
	char *temp = NULL;
	Extraction_Arg_t extractionArg;

	if( minJson == NULL ){
		free(minJson);
		return JSONC_FAIL_ALLOCATE;
	}
	else{
	}

	/* Remove unnecessary characters */
	k=0;
	for(j = 0 ; j <= length ; j++){
		if( ( str[j] <= ' ' ) || ( str[j] >= 0x7f ) || ( str[j] == '"' ) ){
		}
		else{
			minJson[k++]=str[j];
			temp = realloc(minJson,k+1);
			CHECK_ALLOCATE(minJson,temp,JSONC_FAIL_ALLOCATE);
		}
	}
	minJson[k] = 0;
	printf("%s\n",minJson);

	/* Extract "Key" and "Value" */
	extractionArg.str = minJson;
	extractionArg.max = strlen(minJson);
	success = extraction( &extractionArg.unit );
	free(minJson);
	if( success == 0 ){
		free(extractionArg.unit);
		return JSONC_ILLEGAL_STR;
	}
	else if( success == -1 ){
		free(extractionArg.unit);
		return JSONC_FAIL_ALLOCATE;
	}
	else if( success == -2 ){
		free(extractionArg.unit);
		return JSONC_MISSING_END_POINT;
	}
	else{
	}

	/* Execute the set "CBFunc" */
	processAllCbFunc( extractionArg.unit );	
	free(extractionArg.unit);

	return JSONC_SUCCESS;
}