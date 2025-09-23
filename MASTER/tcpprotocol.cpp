#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tcpprotocol.h"

TcpProtocol::TcpProtocol(ECommandType command, ETargetType target) :
    command(command),
    target(target)
{
    fillVectors();
}

TcpProtocol::TcpProtocol(ETargetType target) :
    target(target)
{
    fillVectors();
}

TcpProtocol::TcpProtocol(void)
{
    fillVectors();
}

void TcpProtocol::fillVectors(void)
{
    strCommand[0] = "";
    strCommand[1] = "GET";
    strCommand[2] = "SET";

    strTarget[0] = "";
    strTarget[1] = "DIN";
    strTarget[2] = "DOUT";
    strTarget[3] = "ANALOG";

    strState[0] = "";
    strState[1] = "ON";
    strState[2] = "OFF";

    strSep = " ";
    strTerm = "\n";
}

char *TcpProtocol::toCommand(int idx)
{
    char strIdx[10];

    itoa(idx, strIdx, 10);

    strcpy(message, strCommand[command]);
    strcat(message, strSep);
    strcat(message, strTarget[target]);
    strcat(message, strSep);
    strcat(message, strIdx);
    strcat(message, strTerm);

    return message;
}

char *TcpProtocol::toCommand(int idx, EStateType state)
{
    char strIdx[10];

    itoa(idx, strIdx, 10);

    strcpy(message, strCommand[command]);
    strcat(message, strSep);
    strcat(message, strTarget[target]);
    strcat(message, strSep);
    strcat(message, strIdx);
    strcat(message, strSep);
    strcat(message, strState[state]);
    strcat(message, strTerm);

    return message;
}

char *TcpProtocol::toAnswer(ETargetType target, int idx, EStateType state)
{
    char strIdx[10];

    itoa(idx, strIdx, 10);
    strcpy(message, strTarget[target]);
    strcat(message, strSep);
    strcat(message, strIdx);
    strcat(message, strSep);
    strcat(message, strState[state]);
    strcat(message, strTerm);
    return message;
}

char *TcpProtocol::toAnswer(EStateType state)
{
    char strIdx[10];

    itoa(idx, strIdx, 10);
    strcpy(message, strTarget[target]);
    strcat(message, strSep);
    strcat(message, strIdx);
    strcat(message, strSep);
    strcat(message, strState[state]);
    strcat(message, strTerm);
    return message;
}

int TcpProtocol::verifyToken(char *token, const char *tokens[], int ntokens)
{
    int ind;

    for (ind=0; ind<ntokens; ind++)
    {
        if (strcmp(tokens[ind], token) == 0)
            return ind;
    }
    return 0;
}

bool TcpProtocol::fromCommand(char *message)
{
    bool ret = false;
    char *token;

    if ((token = strtok(message, (const char *)" ")) != NULL)
    {
        if ((command = (ECommandType)verifyToken(token, strCommand, (int)eCmdMax)) > 0)
        {
            if ((token = strtok(NULL, (const char *)" ")) != NULL)
            {
                if ((target = (ETargetType)verifyToken(token, strTarget, (int)eTargetMax)) > 0)
                {
                    if ((token = strtok(NULL, (const char *)" ")) != NULL)
                    {
                        if ((idx = atoi(token)) > 0)
                        {
                            if (command == eCmdSet)
                            {
                                if ((token = strtok(NULL, (const char *)" ")) != NULL)
                            {
                                if (target == eTargetDin || target == eTargetDout)
                                {
                                    if ((state = (EStateType)verifyToken(token, strState, (int)eStateMax)) > 0)
                                        ret = true;
                                }
                                else if (target == eTargetAnalog)
                                {
                                    value = strtol(token, NULL, 10);
                                    ret = true;
                                }
                            }
                        }
                            else
                                ret = true;
                        }
                    }
                }
            }
        }
    }

    return ret;
}

bool TcpProtocol::fromAnswer(char *message)
{
    bool ret = false;
    char *token;

    if ((token = strtok(message, (const char *)" ")) != NULL)
    {
        if ((target = (ETargetType)verifyToken(token, strTarget, (int)eTargetMax)) > 0)
        {
            if ((token = strtok(NULL, (const char *)" ")) != NULL)
            {
                if ((idx = atoi(token)) > 0)
                {
                    if ((token = strtok(NULL, (const char *)"\n")) != NULL)
                    {
                        if (target == eTargetDin || target == eTargetDout)
                        {
                            if ((state = (EStateType)verifyToken(token, strState, (int)eStateMax)) > 0)
                                ret = true;
                        }
                        else if (target == eTargetAnalog)
                        {
                            value = strtol(token, NULL, 10);
                            ret = true;
                        }
                    }
                }
            }
        }
    }

    return ret;
}
