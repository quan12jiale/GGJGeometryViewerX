#ifndef CMD_H
#define CMD_H
#include "CmdCenter.h"

class CMeasureCmd: public CCmdBase
{
public:
    CMeasureCmd(CCmdCenter *pCmdCenter);
    virtual EnCmdState finish();
    virtual EnCmdState input(EnDataType nType, void *pData);
private:
    double *m_pResult;
    CVector3d m_oVector;
    CPickVectorCmd m_oPickVectorCmd;
};

class CDrawLineCmd: public CPickVectorCmd
{
public:
    CDrawLineCmd(CCmdCenter *pCmdCenter, CLine2d *pResult2d = nullptr, CLine3d *pResult3d = nullptr);
    virtual EnCmdState finish();
    virtual EnCmdState input(EnDataType nType, void *pData);
private:
    CLine2d *m_pResult2d;
    CLine3d *m_pResult3d;
};

#endif