#include "Cmd.h"

REGISTER_CMD(CMeasureCmd, "Measure")
CMeasureCmd::CMeasureCmd( CCmdCenter *pCmdCenter)
    : m_oPickVectorCmd(pCmdCenter, &m_oVector), CCmdBase(pCmdCenter)
{
}

EnCmdState CMeasureCmd::finish()
{
    if (m_nStep == 1)
    {
        m_nStep = 0;
        double dLength = m_oVector.Length();
        m_pCmdCenter->cmdLine()->cmdBox()->setText(QString::number(dLength, 'f'));
        return csFinish;
    }
    return csCancel;
}

EnCmdState CMeasureCmd::input( EnDataType nType, void *pData )
{
    EnCmdState nResult = csContinue;
    if (m_nStep == 0)
    {
        nResult = m_oPickVectorCmd.input(nType, pData);
        if (nResult == csFinish)
        {
            m_oPickVectorCmd.finish();
            ++m_nStep;
        }
    }
    return nResult;
}

REGISTER_CMD(CDrawLineCmd, "DrawLine")

CDrawLineCmd::CDrawLineCmd(CCmdCenter *pCmdCenter, CLine2d *pResult2d, CLine3d *pResult3d)
    : m_pResult2d(pResult2d), m_pResult3d(pResult3d), CPickVectorCmd(pCmdCenter)
{

}


EnCmdState CDrawLineCmd::finish()
{
    if (m_nStep == 2)
    {
        m_nStep = 0;
        m_pCmdCenter->geoViewer()->pushState(CGeometryViewer::egvsAutoCamera, false);
        CVector3d oCameraDir = m_pCmdCenter->geoViewer()->viewerWidget()->viewer()->ActiveViewport()->GetCamera()->Direction().Vec3d();
        if (ggp::Equals(abs(oCameraDir.Z), 1.0, g_DistEpsilon))
        {
            CLine2d *pLine2d = new CLine2d(m_oPts[0].Vec2(), m_oPts[1].Vec2());
            if (m_pResult2d)
            {
                m_pResult2d = pLine2d->Clone();
            }
            m_pCmdCenter->geoViewer()->addGeometry(pLine2d);
        }
        else
        {
            CLine3d *pLine3d = new CLine3d(m_oPts[0], m_oPts[1]);
            if (m_pResult3d)
            {
                m_pResult3d = pLine3d->Clone();
            }
            m_pCmdCenter->geoViewer()->addGeometry(pLine3d);
        }
        m_pCmdCenter->geoViewer()->popState(CGeometryViewer::egvsAutoCamera);
        return csFinish;
    }
    return csCancel;
}

EnCmdState CDrawLineCmd::input( EnDataType nType, void *pData )
{
    EnCmdState nResult = CPickVectorCmd::input(nType, pData);
    return nResult;
}