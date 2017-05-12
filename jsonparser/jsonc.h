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

typedef struct{
	char *KeyName;
	void *value;
}JSONC_KeySet_t;

extern const char *keyTypeArray;

extern JSONC_ERROR_TYPE JSONC_parseStr(char *str);

#endif