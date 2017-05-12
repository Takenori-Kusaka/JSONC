#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsonc_params.h"

#define CHECK_ALLOCATE(p,tmp,rtn) { \
	if(tmp == NULL){ \
		free(p); \
		return rtn; \
	} \
	else{ \
		p = tmp; \
	} \
}

int main() {
	int i;
	FILE *fp = NULL;
	char *str = (char*)malloc(1);
	char *temp = NULL;

    fopen_s(&fp,"Example.json","r");
	if(fp==NULL){
        printf("missing file\r\n");
        return -1;
    }
	for(i = 0 ; (str[i] = fgetc(fp)) != EOF ; i++){
		temp = realloc(str,i+2);
		CHECK_ALLOCATE(str,temp,0);
	}
	str[i] = 0;
	printf("%s\n",str);

	fclose(fp);
	JSONC_parseStr(str);
	free(str);

	printf("success parse");

	return 0;
}