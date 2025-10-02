#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tcpprotocol.h"

static const char *strNull = "";

static const char *strCommandGet = "GET";
static const char *strCommandSet = "SET";

static const char *strTargetDin = "DIN";
static const char *strTargetDout = "DOUT";
static const char *strTargetAnalog = "ANALOG";

static const char *strStateOn = "ON";
static const char *strStateOff = "OFF";

TcpProtocol::TcpProtocol(void)
{
    fillVectors();
}

void TcpProtocol::fillVectors(void)
{
    strCommand[0] = strNull;
    strCommand[1] = strCommandGet;
    strCommand[2] = strCommandSet;

    strTarget[0] = strNull;
    strTarget[1] = strTargetDin;
    strTarget[2] = strTargetDout;
    strTarget[3] = strTargetAnalog;

    strState[0] = strNull;
    strState[1] = strStateOn;
    strState[2] = strStateOff;

    strSep = " ";
    strTerm = "\n";
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

/* -----------------------------------------------------------------------------------*/
// TcpProtocolMaster
/* -----------------------------------------------------------------------------------*/

char *TcpProtocolMaster::toCommand(ECommandType command, ETargetType target, int idx)
{
    char strIdx[10];

    snprintf(strIdx, sizeof(strIdx), "%d", idx);

    strcpy(message, strCommand[command]);
    strcat(message, strSep);
    strcat(message, strTarget[target]);
    strcat(message, strSep);
    strcat(message, strIdx);
    strcat(message, strTerm);

    return message;
}

char *TcpProtocolMaster::toCommand(ECommandType command, ETargetType target, int idx, EStateType state)
{
    char strIdx[10];

    snprintf(strIdx, sizeof(strIdx), "%d", idx);

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

bool TcpProtocolMaster::fromAnswer(char *message)
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

/* -----------------------------------------------------------------------------------*/
// TcpProtocolSlave
/* -----------------------------------------------------------------------------------*/

bool TcpProtocolSlave::fromCommand(char *message)
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
                    if ((token = strtok(NULL, command == eCmdSet ? (const char *)" " : (const char *)"\n")) != NULL)
                    {
                        if ((idx = atoi(token)) > 0)
                        {
                            if (command == eCmdSet)
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

char *TcpProtocolSlave::toAnswer(ETargetType target, int idx, EStateType state)
{
    char strIdx[10];

    snprintf(strIdx, sizeof(strIdx), "%d", idx);
    strcpy(message, strTarget[target]);
    strcat(message, strSep);
    strcat(message, strIdx);
    strcat(message, strSep);
    strcat(message, strState[state]);
    strcat(message, strTerm);
    return message;
}

char *TcpProtocolSlave::toAnswer(EStateType state)
{
    char strIdx[10];

    snprintf(strIdx, sizeof(strIdx), "%d", idx);
    strcpy(message, strTarget[target]);
    strcat(message, strSep);
    strcat(message, strIdx);
    strcat(message, strSep);
    strcat(message, strState[state]);
    strcat(message, strTerm);
    return message;
}

char *TcpProtocolSlave::toAnswer(int val)
{
    char strIdx[10];
    char strVal[20];

    snprintf(strIdx, sizeof(strIdx), "%d", idx);
    snprintf(strVal, sizeof(strVal), "%d", val);
    strcpy(message, strTarget[target]);
    strcat(message, strSep);
    strcat(message, strIdx);
    strcat(message, strSep);
    strcat(message, strVal);
    strcat(message, strTerm);
    return message;
}
