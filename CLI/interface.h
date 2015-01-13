#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "linenoise.h"

void completion(const char *buf, linenoiseCompletions *lc);
void parseCommand(const char *command);
void printLogo();

#endif