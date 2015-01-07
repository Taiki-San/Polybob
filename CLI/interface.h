#ifndef INTERFACE_H
#define INTERFACE_H


void completion(const char *buf, linenoiseCompletions *lc);
void parseCommand(const char *command);
void printLogo();

#endif