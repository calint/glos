#pragma once
#include<stdlib.h>
#include<execinfo.h>
void stacktrace_print(){
	void*array[10];
	int size=backtrace(array,10);
	char**strings=backtrace_symbols(array,size);
	fprintf(stderr,"stacktrace %d frames:\n",size);

	for(int i=0;i<size;i++)
		fprintf(stderr,"%s\n",strings[i]);

	free (strings);
}
