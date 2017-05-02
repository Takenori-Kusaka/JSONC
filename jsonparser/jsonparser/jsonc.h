#ifndef __JSONC__H__
#define __JSONC__H__

typedef enum{
	JSONC_SUCCESS,
	JSONC_ILLEGAL_STR,
	JSONC_FAIL_ALLOCATE,
	JSONC_MISSING_END_POINT,
	JSONC_OVER_MAX_INIT,
	JSONC_ALL_ERROR
}JSONC_ERROR_TYPE;

typedef enum{
	MAIN_KEY,
	SUB_KEY,
	SSUB_KEY,
	KEY_LEVEL_ALL
}KEY_LEVEL;

typedef struct{
	char *keyName[KEY_LEVEL_ALL];
	void *value;
}JsonInit_Arg_t;

extern JSONC_ERROR_TYPE JSONC_Init(JsonInit_Arg_t *arg);
extern JSONC_ERROR_TYPE JSONC_parseStr( char *str );

#endif