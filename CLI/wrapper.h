#ifndef WRAPPER_H
#define	WRAPPER_H

#include <iostream>
#include <string>

#include <string.h>

#include "parser.h"

using namespace std;

char* rmSuperscript(char* line);

void sendToParser(char* line);

#endif