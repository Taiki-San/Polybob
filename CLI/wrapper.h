#ifndef WRAPPER_H
#define	WRAPPER_H

#ifdef __cplusplus

#include <iostream>
#include <string>
#include <string.h>
#include "../parser.h"

using namespace std;

#endif

char* rmSuperscript(char* line);
char * getSuggestion(const char * begining);

#endif