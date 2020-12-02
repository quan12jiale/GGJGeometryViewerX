#ifndef CMDCENTER_H
#define CMDCENTER_H
#include "GeometryViewer.h"
using namespace std;
using namespace ggp;

class CGeometryViewer;
class CCmdLine;
class CCmdCenter;

enum EnCmdState
{
    csCancel        = 0,
    csContinue      = 1,
    csFinish        = 2,
    csError         = 3,
};

enum EnDataType
{
    dtUnknown       = 0,
    dtCommand       = 1,
    dtInteger       = 2,
    dtDouble        = 3,
    dtPointer       = 4,
    dtCString       = 5,
    dtQString       = 6,
    dtVector3d      = 7,
    dtHitPoint      = 8,    //鼠标移动过程中的点
};

//命令基类
class CCmdBase
{
public:
    CCmdBase(CCmdCenter *pCmdCenter);
    virtual ~CCmdBase();
    virtual EnCmdState cancel();
    virtual EnCmdState finish();
    virtual EnCmdState input(EnDataType nType, void *pData);
    int step(){return m_nStep;}
protected:
    CCmdCenter *m_pCmdCenter;
    int m_nStep;
    friend class CCmdCenter;
};

//选点命令
class CPickPointCmd: public CCmdBase
{
public:
    CPickPointCmd(CCmdCenter *pCmdCenter, CVector3d *pResult = nullptr);
    ~CPickPointCmd();
    virtual EnCmdState finish();
    virtual EnCmdState input(EnDataType nType, void *pData);
    void setLabel(const QString& strHint );
protected:
    QLabel *m_pLabel;
    CVector3d *m_pResult, m_oPoint;
};

//选向量命令
class CPickVectorCmd: public CCmdBase
{
public:
    CPickVectorCmd(CCmdCenter *pCmdCenter, CVector3d *pResult = nullptr);
    virtual ~CPickVectorCmd();
    virtual EnCmdState finish();
    virtual EnCmdState input(EnDataType nType, void *pData);
protected:
    CVector3d *m_pResult, m_oPoint, m_oPts[2];
    CPickPointCmd m_oPickPointCmd;
    CVisualNode *m_pNode;
};

//选几何对象命令
class CPickGeometryCmd: public CCmdBase
{
public:
    CPickGeometryCmd(CCmdCenter *pCmdCenter, CGeometry **pResult = nullptr);
    CPickGeometryCmd(CCmdCenter *pCmdCenter, QTreeWidgetItem **pResult = nullptr);
    ~CPickGeometryCmd();
    virtual EnCmdState finish();
    virtual EnCmdState input(EnDataType nType, void *pData);
    void setLabel(const QString& strHint );
protected:
    CGeometry **m_pGeoResult, *m_pGeo;
    QTreeWidgetItem **m_pItemResult, *m_pItem;
    QLabel *m_pLabel;
};

//选BrepBody命令
class CPickBrepBodyCmd: public CCmdBase
{

};

//命令行
class CCmdLine: public QWidget
{
public:
    CCmdLine(QWidget *parent = nullptr);
    void addHint(QWidget *pWidget);
    QLineEdit *cmdBox();
    void reset();
private:
    QLineEdit *m_pCmdBox;
    QWidget *m_pHintWidget;
    QHBoxLayout *m_pMainLayout, *m_pHintLayout;
};

typedef CCmdBase* (*CCmdCreator)(CCmdCenter*);

class CCmdCenter: public QObject
{
    Q_OBJECT
public:
    CCmdCenter(CGeometryViewer *pGeoViewer);
    ~CCmdCenter();
    EnCmdState input(EnDataType nType, void *pData);
    CCmdLine *cmdLine();
    CGeometryViewer *geoViewer();
    EnCmdState cancelCmd();
    EnCmdState finishCmd();
public slots:
    void onExecuteCmd( const QString& strCmd );
private:
    CCmdLine *m_pCmdLine;
    CCmdBase *m_pCurCmd;
    QString m_sLastCmd;
    CGeometryViewer *m_pGeoViewer;
};
std::map<QString, CCmdCreator>& _allCmds();
int _registerCmd(const QString& strCmd, CCmdCreator oCreator);

//最好在源文件而不是头文件中使用
#define REGISTER_CMD(cmd, name) \
CCmdBase *_new##cmd(CCmdCenter *pCmdCenter)\
{\
    return new cmd(pCmdCenter);\
}\
int _unuse##cmd = _registerCmd(name, _new##cmd);\

#endif