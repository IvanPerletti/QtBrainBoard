#ifndef TCPCOMMAND_H
#define TCPCOMMAND_H

#define MAX_LEN_MESSAGE 64

class TcpProtocol
{
public:
    typedef enum {
        eCmdUnknown = 0,
        eCmdGet,
        eCmdSet,
        eCmdMax
    } ECommandType;

    typedef enum {
        eTargetNone = 0,
        eTargetDin,
        eTargetDout,
        eTargetAnalog,
        eTargetMax
    } ETargetType;

    typedef enum {
        eStateUnknown = 0,
        eStateOn,
        eStateOff,
        eStateMax
    } EStateType;

public:
    TcpProtocol(void);

    ECommandType getCommand(void) { return command; }
    ETargetType getTarget(void) { return target; }
    int getIdx(void) { return idx; }
    EStateType getState(void) { return state; }
    int getValue(void) { return value; }

protected:
    ECommandType command;
    ETargetType target;
    int idx;
    EStateType state;
    long value;
    char message[MAX_LEN_MESSAGE+1];

    const char *strCommand[eCmdMax];
    const char *strTarget[eTargetMax];
    const char *strState[eStateMax];
    const char *strSep;
    const char *strTerm;

    void fillVectors(void);
    int verifyToken(char *token, const char *tokens[], int ntokens);

};

class TcpProtocolMaster : public TcpProtocol
{
public:

    char *toCommand(ECommandType command, ETargetType target, int idx);
    char *toCommand(ECommandType command, ETargetType target, int idx, EStateType state);
    bool fromAnswer(char *message);
};

class TcpProtocolSlave : public TcpProtocol
{
public:

    bool fromCommand(char *message);
    char *toAnswer(ETargetType target, int idx, EStateType state);
    char *toAnswer(EStateType state);
    char *toAnswer(int val);
};

#endif // TCPCOMMAND_H
