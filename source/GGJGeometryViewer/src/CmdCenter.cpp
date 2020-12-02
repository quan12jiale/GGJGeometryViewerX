#include "CmdCenter.h"

CCmdLine::CCmdLine(QWidget *parent): QWidget(parent)
{
    m_pHintWidget = new QWidget(this);
    m_pHintLayout = new QHBoxLayout(m_pHintWidget);
    m_pHintLayout->setMargin(0);
    m_pHintLayout->setSpacing(2);
    m_pHintWidget->setLayout(m_pHintLayout);
    
    m_pCmdBox = new QLineEdit(this);

    m_pMainLayout = new QHBoxLayout(this);
    m_pMainLayout->setMargin(0);
    m_pMainLayout->setSpacing(0);
    m_pMainLayout->addWidget(m_pHintWidget);
    m_pMainLayout->addWidget(m_pCmdBox, 1);
    setLayout(m_pMainLayout);
}

QLineEdit * CCmdLine::cmdBox()
{
    return m_pCmdBox;
}

void CCmdLine::addHint( QWidget *pWidget )
{
    m_pHintLayout->addWidget(pWidget);
}

void CCmdLine::reset()
{
    for (int i = m_pHintLayout->count() - 1; i >= 0; --i)
    {
        QLayoutItem *pItem = m_pHintLayout->itemAt(i);
        m_pHintLayout->removeItem(pItem);
        delete pItem;
    }
    m_pCmdBox->setText("");
}

CCmdCenter::CCmdCenter( CGeometryViewer *pGeoViewer )
    : m_pGeoViewer(pGeoViewer), m_pCurCmd(nullptr)
{
    m_pCmdLine = new CCmdLine();
}

CCmdCenter::~CCmdCenter()
{
    cancelCmd();
    delete m_pCmdLine;
}

EnCmdState CCmdCenter::input( EnDataType nType, void *pData )
{
    EnCmdState nState = csContinue;
    if (m_pCurCmd)
    {
        nState = m_pCurCmd->input(nType, pData);
        if (nState == csCancel)
        {
            nState = cancelCmd();
        }
        else if (nState == csFinish)
        {
            nState = finishCmd();
        }
    }
    return nState;
}

CCmdLine * CCmdCenter::cmdLine()
{
    return m_pCmdLine;
}

void CCmdCenter::onExecuteCmd( const QString& strCmd )
{
    cancelCmd();
    m_sLastCmd = strCmd;
    m_pGeoViewer->setFocus(Qt::MouseFocusReason);
    CCmdCreator pCmdCreator = _allCmds()[strCmd];
    if (pCmdCreator)
    {
        m_pCurCmd = pCmdCreator(this);
    }
}

EnCmdState CCmdCenter::cancelCmd()
{
    EnCmdState nState = csContinue;
    if (m_pCurCmd)
    {
        nState = m_pCurCmd->cancel();
        delete m_pCurCmd;
        m_pCurCmd = nullptr;
    }
    return nState;
}

EnCmdState CCmdCenter::finishCmd()
{
    EnCmdState nState = csContinue;
    if (m_pCurCmd)
    {
        nState = m_pCurCmd->finish();
        delete m_pCurCmd;
        m_pCurCmd = nullptr;
        return nState;
    }
    else
    {
        onExecuteCmd(m_sLastCmd);
    }
    return nState;
}

CGeometryViewer * CCmdCenter::geoViewer()
{
    return m_pGeoViewer;
}


CCmdBase::CCmdBase(CCmdCenter *pCmdCenter)
    :m_nStep(0), m_pCmdCenter(pCmdCenter)
{

}

EnCmdState CCmdBase::cancel()
{
    m_nStep = 0;
    return csCancel;
}

EnCmdState CCmdBase::finish()
{
    m_nStep = 0;
    return csFinish;
}

EnCmdState CCmdBase::input( EnDataType nType, void *pData )
{
    return csContinue;
}

CCmdBase::~CCmdBase()
{

}

CPickPointCmd::CPickPointCmd( CCmdCenter *pCmdCenter, CVector3d *pResult /*= nullptr*/ )
    : m_pResult(pResult), CCmdBase(pCmdCenter)
{
    m_pCmdCenter->cmdLine()->cmdBox()->setText("");
    m_pLabel = new QLabel(QStringLiteral("指定一个点"));
    m_pCmdCenter->cmdLine()->addHint(m_pLabel);
}

CPickPointCmd::~CPickPointCmd()
{
    delete m_pLabel;
}

EnCmdState CPickPointCmd::input( EnDataType nType, void *pData )
{
    if (nType == dtVector3d)
    {
        ++m_nStep;
        m_oPoint = *static_cast<CVector3d*>(pData);
        m_pCmdCenter->cmdLine()->cmdBox()->setText("");
        return csFinish;
    }
    return csContinue;
}

EnCmdState CPickPointCmd::finish()
{
    if (m_nStep == 1)
    {
        m_nStep = 0;
        if (m_pResult)
        {
            *m_pResult = m_oPoint;
        }
        return csFinish;
    }
    return csCancel;
}

void CPickPointCmd::setLabel( const QString& strHint )
{
    m_pLabel->setText(strHint);
}

CPickVectorCmd::CPickVectorCmd( CCmdCenter *pCmdCenter, CVector3d *pResult /*= nullptr*/ )
    : m_pResult(pResult), m_oPickPointCmd(pCmdCenter, &m_oPoint), CCmdBase(pCmdCenter)
{
    m_oPickPointCmd.setLabel(QStringLiteral("指定第一个点："));
    m_pNode = new CVisualNode();
    m_pNode->SetTempNodeType(CVisualNode::TNT_ALWAYS);
    CPrimitiveRenderable *pRenderable = new CPrimitiveRenderable();
    pRenderable->SetPrimitiveType(PT_LINES);
    pRenderable->GetOrCreateStateSet()->SetMaterial(CColor::White);
    m_pNode->AddRenderable(pRenderable);
    m_pCmdCenter->geoViewer()->viewerWidget()->viewer()->Scene()->GetRootNode()->AddChild(m_pNode);
}

CPickVectorCmd::~CPickVectorCmd()
{
    m_pNode->GetParent()->RemoveChild(m_pNode);
}

EnCmdState CPickVectorCmd::finish()
{
    if (m_nStep == 2)
    {
        m_nStep = 0;
        if (m_pResult)
        {
            *m_pResult = m_oPts[1] - m_oPts[0];
        }
        return csFinish;
    }
    return csCancel;
}

EnCmdState CPickVectorCmd::input( EnDataType nType, void *pData )
{
    EnCmdState nResult = csContinue;
    if (m_nStep < 2)
    {
        if (m_nStep == 1 && nType == dtHitPoint)
        {
            CVector3d *pHitPt = (CVector3d*)pData;
            CVec3fList oPts;
            oPts.push_back(m_oPts[0].Vec3f());
            oPts.push_back(pHitPt->Vec3f());
            CPrimitiveRenderable *pRenderable = dynamic_cast<CPrimitiveRenderable*>(m_pNode->GetRenderable(0));
            pRenderable->SetVertexArray(oPts);
            m_pCmdCenter->geoViewer()->viewerWidget()->viewer()->Scene()->SetTempDirty(true);
        }

        nResult = m_oPickPointCmd.input(nType, pData);
        if (nResult == csFinish)
        {
            m_oPickPointCmd.finish();
            m_oPts[m_nStep] = m_oPoint;
            ++m_nStep;
            if (m_nStep == 1)
            {
                m_oPickPointCmd.setLabel(QStringLiteral("指定第二个点："));
                nResult = csContinue;
            }
        }
    }
    return nResult;
}

int _registerCmd( const QString& strCmd, CCmdCreator oCreator )
{
    _allCmds()[strCmd] = oCreator;
    return _allCmds().size();
}

std::map<QString, CCmdCreator>& _allCmds()
{
    static std::map<QString, CCmdCreator> s_oCmds;
    return s_oCmds;
}

CPickGeometryCmd::CPickGeometryCmd( CCmdCenter *pCmdCenter, CGeometry **pResult /*= nullptr*/ )
    : m_pGeoResult(pResult), m_pItemResult(nullptr), CCmdBase(pCmdCenter)
{
    m_pLabel = new QLabel(QStringLiteral("指定一个对象："));
}

CPickGeometryCmd::CPickGeometryCmd( CCmdCenter *pCmdCenter, QTreeWidgetItem **pResult /*= nullptr*/ )
    : m_pGeoResult(nullptr), m_pItemResult(pResult), CCmdBase(pCmdCenter)
{
    m_pLabel = new QLabel(QStringLiteral("指定一个对象："));
    m_pCmdCenter->cmdLine()->addHint(m_pLabel);
}

EnCmdState CPickGeometryCmd::finish()
{
    if (m_nStep == 1)
    {
        m_nStep = 0;
        if (m_pGeoResult)
        {
            *m_pGeoResult = m_pGeo;
        }
        if (m_pItemResult)
        {
            *m_pItemResult = m_pItem;
        }
        return csFinish;
    }
    return csCancel;
}

EnCmdState CPickGeometryCmd::input( EnDataType nType, void *pData )
{
    if (nType == dtPointer)
    {
        ++m_nStep;
        m_pGeo = static_cast<CGeometry*>(pData);
        m_pItem = m_pCmdCenter->geoViewer()->getSeletedItem();
        m_pCmdCenter->cmdLine()->cmdBox()->setText("");
        return csFinish;
    }
    return csContinue;
}

CPickGeometryCmd::~CPickGeometryCmd()
{
    delete m_pLabel;
}

void CPickGeometryCmd::setLabel( const QString& strHint )
{
    m_pLabel->setText(strHint);
}

