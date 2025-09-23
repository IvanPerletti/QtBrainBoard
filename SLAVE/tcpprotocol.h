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
    TcpProtocol(ECommandType command, ETargetType target);
    TcpProtocol(ETargetType target);
    TcpProtocol(void);

    char *toCommand(int idx);
    char *toCommand(int idx, EStateType state);
    bool fromCommand(char *message);
    char *toAnswer(ETargetType target, int idx, EStateType state);
    char *toAnswer(EStateType state);
    char *toAnswer(int val);
    bool fromAnswer(char *message);

    ECommandType getCommand(void) { return command; }
    ETargetType getTarget(void) { return target; }
    int getIdx(void) { return idx; }
    EStateType getState(void) { return state; }
    int getValue(void) { return value; }

private:
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

#endif // TCPCOMMAND_H
