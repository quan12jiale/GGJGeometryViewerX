/*!
* @brief   三维视窗控件，负责各种几何对象的显示
* @author  zhangyc-c
* @date    2015.7.19
* @remarks 
* Copyright (c) 1998-2015 Glodon Corporation	
*/
#include "viewerwidget.h"
#include "ViewCore/GSimpleVisualNodes.h "
#include "ViewCore/GVisualNodes.h "
#include "ViewCore/GWorldPos2DNodes.h"
#include "Algorithm/algmesh.h"
#include "Algorithm/algpolysplit.h"
#include <QDebug>

using namespace ggp;

/*!
* @author     zhangyc-c  2015.7.19
* @brief      构造函数
* @param[in]  QWidget * parent
* @return     
*/
CViewerWidget::CViewerWidget(QWidget *parent)
    : QWidget(parent), m_pFocusNode(NULL), m_bShowDir(true), m_bWireFrame(false), m_nMeshType(0), 
    m_dDistEps(100), m_dAngleEps(M_PI / 36), m_dLineWidth(2.0), m_nColor(CColor::White)
{
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setAutoFillBackground(true);    // for compatibility
    setFocusPolicy(Qt::WheelFocus);
    setMouseTracking(true);
    InitRenderEngine();
    m_pViewer = new CViewer((HWND)this->winId());
    m_pViewer->SetBackgroundColor(CColor(33, 40, 48));      //背景颜色和算量平台一致
    m_pViewer->Options()->SetMiddleButtonNavEnabled(true);
    m_pViewer->Options()->SetViewAxesEnabled(true);
    m_pViewer->CommandManager()->ExecuteCommand(L"navigate");

    //初始化捕捉点的亮显
    Vec3List vPts;  
    vPts.push_back(CVector3f(-4, -4, 0));
    vPts.push_back(CVector3f( 4, -4, 0));
    vPts.push_back(CVector3f( 4,  4, 0));
    vPts.push_back(CVector3f(-4,  4, 0));
    vPts.push_back(CVector3f(-4, -4, 0));
    ColorList colors(vPts.size(), CColor::Yellow);  
    m_pHitPtNode = new CVectorGraphics2D(vPts, colors, CVector3f::Zero);
    CPrimitiveRenderable *pRenderable = (CPrimitiveRenderable *)m_pHitPtNode->GetRenderable(0);
    pRenderable->SetPrimitiveType(PT_LINE_STRIP);
    pRenderable->GetOrCreateStateSet()->SetMaterial(CColor::Yellow);
    pRenderable->GetOrCreateStateSet()->SetLineWidth(2.0);
    m_pHitPtNode->GetOrCreateNodeStateSet()->SetLightingEnabled(false);
    m_pHitPtNode->SetVisible(false);
    m_pViewer->Scene()->GetRootNode()->AddChild(m_pHitPtNode);
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      析构函数
* @return     
*/
CViewerWidget::~CViewerWidget()
{
    delete m_pViewer;
    UnInitRenderEngine();
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      不使用Qt自带的绘制引擎
* @return     QPaintEngine *
*/
QPaintEngine * CViewerWidget::paintEngine() const
{
    return nullptr;
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      响应滚轮事件
* @param[in]  QWheelEvent * pEvent
* @return     void
*/
void CViewerWidget::wheelEvent( QWheelEvent *pEvent )
{
    if (m_pViewer == 0) return;
    bool bCtrlDown = (pEvent->modifiers() & Qt::ControlModifier) == Qt::ControlModifier;
    bool bShiftDown = (pEvent->modifiers() &  Qt::ShiftModifier) == Qt::ShiftModifier;
    bool bAltDown = (pEvent->modifiers() & Qt::AltModifier) == Qt::AltModifier;
    CMouseEventArgs mouseArgs(ET_MOUSEWHEEL, 0, pEvent->x(), pEvent->y(),
        (float)pEvent->delta() / (float)WHEEL_DELTA, bCtrlDown, bShiftDown, bAltDown);
    m_pViewer->ProcessEvent(&mouseArgs);
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      响应按键按下事件
* @param[in]  QKeyEvent * pEvent
* @return     void
*/
void CViewerWidget::keyPressEvent(QKeyEvent *pEvent)
{
    if (!m_pViewer || pEvent->key() == Qt::Key_Escape) return;
    CKeyEventArgs keyArgs(ET_KEYDOWN, pEvent->nativeVirtualKey(), 0);
    m_pViewer->ProcessEvent(&keyArgs);
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      响应按键释放事件
* @param[in]  QKeyEvent * pEvent
* @return     void
*/
void CViewerWidget::keyReleaseEvent(QKeyEvent *pEvent)
{
    if (m_pViewer == 0) return;
    CKeyEventArgs keyArgs(ET_KEYUP, pEvent->nativeVirtualKey(), 0);
    m_pViewer->ProcessEvent(&keyArgs);
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      高亮显示某个几何对象
* @param[in]  CGeometry * pGeo
* @return     void
*/
void CViewerWidget::highLight( EnItemType nType, void *pData, CColor oColor)
{
    if (m_pFocusNode)
    {
        if (m_pFocusNode->Tag().GetPointer())
        {
            CNodeExtend *pExtend = (CNodeExtend*)m_pFocusNode->Tag().GetPointer();
            delete pExtend;
        }
        m_pViewer->Scene()->GetRootNode()->RemoveChild(m_pFocusNode);
        m_pFocusNode = nullptr;
    }
    if (pData)
    {
        double dOldLineWidth = m_dLineWidth;
        CColor nOldColor = m_nColor;
        m_nColor = oColor;
        m_pFocusNode = showItem(nType, pData);
        m_nColor = nOldColor;
        m_dLineWidth = dOldLineWidth;
        setNodeColor(m_pFocusNode, CColor::White);
    }
    if (m_pFocusNode)
    {
        m_pFocusNode->SetPriority(-1);
    }
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      初始化场景
* @return     void
*/
void CViewerWidget::initScene()
{
    createAxis();
    m_pViewer->ActiveViewport()->GetCamera()->SetType(CT_ORTHO);
    //m_pViewer->ActiveViewport()->GetCamera()->SetNearFarDist(-1e8, 1e8);
    m_pViewer->SetViewPoint(VP_TOP);
    m_pViewer->SetDirty();
    m_pViewer->ZoomAll();
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      显示体及体上的三维边
* @param[in]  CBody * pBody
* @return     CGroupNode*
*/
CGroupNode* CViewerWidget::showBody( CBody *pBody )
{
    Vec3List oVertexList, oNormalList, oBorderList;
    vector<unsigned int> oVertexIndexes;
    CMeshParameter oMesh(m_dDistEps, m_dAngleEps);
    if (m_nMeshType == 1)
    {
        CMeshParameter::GetCommonPrecision(pBody, oMesh.m_tolerance, oMesh.m_angle);
    }
    else if(m_nMeshType == 2)
    {
        CMeshParameter::GetMinPrecision(pBody, oMesh.m_tolerance, oMesh.m_angle);
    }
    oMesh.GetMesh(pBody, oVertexList, oNormalList, oVertexIndexes);
    oMesh.GetLines(pBody, oBorderList);
    //三角面片渲染对象
    CPrimitiveRenderable *pRenderable0 = new CPrimitiveRenderable();
    pRenderable0->SetPrimitiveType(PT_TRIANGLES);
    pRenderable0->SetIndexArray(oVertexIndexes);
    pRenderable0->SetVertexArray(oVertexList);
    pRenderable0->SetNormalArray(oNormalList);
    CStateSet *pStateSet = pRenderable0->GetOrCreateStateSet();
    pStateSet->SetLightingEnabled(true);
    pStateSet->SetMaterial(m_nColor);
    pStateSet->SetFaceMode(FM_FRONT_AND_BACK);
    pStateSet->SetLightModelTwoSided(true);
    pStateSet->SetStencilTestEnabled(true);
    pStateSet->SetStencilFunction(FTF_ALWAYS, 0xFF, 0xFF);
    pStateSet->SetStencilOperation(STO_KEEP, STO_KEEP, STO_REPLACE);
    pStateSet->SetStencilWriteMask(0xFF);
    //pStateSet->SetPolygonMode(PM_LINE);
    //三角面片显示节点
    CVisualNode *pNode0 = new CVisualNode();
    pNode0->AddRenderable(pRenderable0);
    CNodeExtend *pExtend0 = (CNodeExtend*)pNode0->Tag().GetPointer();
    if (!pExtend0)
    {
        pExtend0 = new CNodeExtend();
    }
    pExtend0->m_bTriangle = true;
    pNode0->Tag().SetPointer(pExtend0);
    pNode0->SetVisible(!m_bWireFrame);

    //线条渲染对象
    CPrimitiveRenderable *pRenderable1 = new CPrimitiveRenderable();
    pRenderable1->SetPrimitiveType(PT_LINES);
    pRenderable1->SetVertexArray(oBorderList);
    //线条显示节点
    CVisualNode *pNode1 = new CVisualNode();
    pNode1->AddRenderable(pRenderable1);

    //记录几何体上可捕捉的点
    CNodeExtend *pExtend1 = (CNodeExtend*)pNode1->Tag().GetPointer();
    if (!pExtend1)
    {
        pExtend1 = new CNodeExtend();
    }
    CBrepBody *pTempBody = pBody->CreateBrepBody();
    if (pTempBody)
    {
        for (int i = 0; i < pTempBody->EdgeCount(); ++i)
        {
            CCurve3d *pCurve3d = pTempBody->GetEdge(i)->Curve();
            if (pCurve3d)
            {
                pExtend1->m_oVertexs.push_back(pCurve3d->StartPoint());
                pExtend1->m_oVertexs.push_back(pCurve3d->EndPoint());
            }
        }
        pTempBody->Free();
    }
    pNode1->Tag().SetPointer(pExtend1);

    CGroupNode *pGroupNode = new CGroupNode();
    pGroupNode->AddChild(pNode0);
    pGroupNode->AddChild(pNode1);
    m_pViewer->Scene()->GetRootNode()->AddChild(pGroupNode);
    return pGroupNode;
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      显示曲面（带拓扑信息）
* @param[in]  CFace * pFace
* @return     CGroupNode*
*/
CGroupNode* CViewerWidget::showFace( CFace *pFace )
{
    CSurface *pSurface = pFace->Surface();
    if (pSurface == NULL)
        return showPolygon(pFace);

    CGroupNode *pGroupNode = nullptr;
    if (pFace->Body())
    {
        int nFaceCount = pFace->Body()->FaceCount();
        if (nFaceCount == 1)
        {
            pGroupNode = showBody(pFace->Body());
        }
        else
        {
            int nFaceIndex = pFace->Index();
            CRefPtr<CBrepBody> pBody = pFace->Body()->Clone();
            for (int i = nFaceCount - 1; i >= 0; --i)
            {
                if (nFaceIndex != i)
                {
                    pBody->EraseFace(i);
                }
            }
            pGroupNode = showBody(pBody.get());
        }
    }
    else
    {
        //构造一个单面体进行显示
        CBrepBody *pBrepBody = new CBrepBody();
        CFace *pNewFace = new CFace(pSurface);
        pBrepBody->AddFace(pNewFace);
        for (int i = 0; i < pFace->LoopCount(); ++i)
        {
            CLoop *pLoop = pFace->GetLoop(i);
            CLoopEx *pLoopEx = new CLoopEx();
            pNewFace->AddLoop(pLoopEx);
            for (int j = 0; j < pLoop->CoedgeCount(); ++j)
            {
                CCurve2d *pCurve2d = pLoop->GetCurve(j);
                CCurve3d *pCurve3d = CImageCurve3d::CreateImageCurve(pCurve2d, pSurface);
                int nEdgeIndex = pBrepBody->AddEdge(pCurve3d, 0, i, j, -1, -1, -1);
                pLoopEx->AddCurve(pCurve2d->Clone(), nEdgeIndex, true);
            }
        }
        pBrepBody->SetIsShell(true);
        pGroupNode = showBody(pBrepBody);
        pBrepBody->Free();
    }
    //添加面的法向
    double dMidU = pSurface->GetRangeU().Middle(), dMidV = pSurface->GetRangeV().Middle();
    CVector3d ptPos = pSurface->GetPoint(dMidU, dMidV);
    CVector3d ptDir = pSurface->GetNormal(dMidU, dMidV);
    if (!pFace->SameDir())
    {
        ptDir.Negate();
    }
    addDirNode(pGroupNode, ptPos, ptDir, true);
    return pGroupNode;
}

/*!
 * @author           zhangyc-c
 * @brief            显示多面体的面
 * @param[in]        CPolyhedralFace * pPolyhedralFace
 * @return           CGroupNode*
 */
CGroupNode* CViewerWidget::showPolyhedralFace(CPolyhedralFace *pPolyhedralFace)
{
    CGroupNode *pGroupNode = nullptr;
    char *szBody = pPolyhedralFace->AsBodyString();
    CBodyPtr pBody = (CPolyhedronBody*)CGeometry::LoadFromStr(szBody);
    if (pBody)
    {
        pBody->SetIsShell(true);
        CRefPtr<CBrepBody> pBrepBody = pBody->CreateBrepBody();
        if (pBrepBody && pBrepBody->FaceCount() == 1)
        {
            pGroupNode = showFace(pBrepBody->GetFace(0));
        }
    }
    delete[] szBody;
    return pGroupNode;
}

CGroupNode* CViewerWidget::showPolyhedralLoop(CPolyhedralLoop *pPolyhedralLoop)
{
    CGroupNode *pGroupNode = new CGroupNode();
    m_pViewer->Scene()->GetRootNode()->AddChild(pGroupNode);
    for (int i = 0; i < pPolyhedralLoop->EdgeCount(); ++i)
    {
        showPolyhedralEdge(pPolyhedralLoop->GetEdge(i), true, pGroupNode);
    }
    return pGroupNode;
}

CGroupNode* CViewerWidget::showPolyhedralEdge(CPolyhedralEdge *pPolyhedralEdge, bool bNeedDir, CGroupNode *pGroupNode)
{
    CLine3d oLine(pPolyhedralEdge->StartPosition(), pPolyhedralEdge->EndPosition());
    return showCurve3d(&oLine, bNeedDir, pGroupNode);
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      显示曲面
* @param[in]  CSurface * pSurface
* @return     CGroupNode*
*/
CGroupNode* CViewerWidget::showSurface( CSurface *pSurface )
{
    CSurfacePtr pCloneSurface;
    CPlane *pPlane = dynamic_cast<CPlane*>(pSurface);
    if (pPlane && pPlane->GetRangeU().Length() > 1e8 && pPlane->GetRangeV().Length() > 1e8)
    {
        pCloneSurface = pSurface = pPlane = pPlane->Clone();
        pPlane->SetRangeU(CIntervald(ggp::Max(pPlane->StartU(), -1e4), ggp::Min(pPlane->EndU(), 1e5)));
        pPlane->SetRangeV(CIntervald(ggp::Max(pPlane->StartV(), -1e4), ggp::Min(pPlane->EndV(), 1e5)));
    }
    Vec3List vertexList, normalList;
    CMeshParameter oMesh(m_dDistEps, m_dAngleEps);
    if (m_nMeshType == 1)
    {
        CMeshParameter::GetCommonPrecision(pSurface, oMesh.m_tolerance, oMesh.m_angle);
    }
    else if(m_nMeshType == 2)
    {
        CMeshParameter::GetMinPrecision(pSurface, oMesh.m_tolerance, oMesh.m_angle);
    }
    oMesh.SurfaceToMesh(pSurface, vertexList, normalList);

    CPrimitiveRenderable *pRenderable = new CPrimitiveRenderable();
    pRenderable->SetPrimitiveType(PT_TRIANGLES);
    pRenderable->SetVertexArray(vertexList);
    pRenderable->SetNormalArray(normalList);
    CStateSet *pStateSet = pRenderable->GetOrCreateStateSet();
    pStateSet->SetMaterial(m_nColor);
    pStateSet->SetFaceMode(FM_FRONT_AND_BACK);
    pStateSet->SetLightModelTwoSided(true);
    CVisualNode *pNode = new CVisualNode();
    pNode->AddRenderable(pRenderable);

    //记录几何体上可捕捉的点
    CNodeExtend *pExtend = (CNodeExtend*)pNode->Tag().GetPointer();
    if (!pExtend)
    {
        pExtend = new CNodeExtend();
        pExtend->m_bTriangle = true;
    }
    pExtend->m_oVertexs.push_back(pSurface->GetPoint(pSurface->StartU(), pSurface->StartV()));
    pExtend->m_oVertexs.push_back(pSurface->GetPoint(pSurface->EndU(), pSurface->StartV()));
    pExtend->m_oVertexs.push_back(pSurface->GetPoint(pSurface->EndU(), pSurface->EndV()));
    pExtend->m_oVertexs.push_back(pSurface->GetPoint(pSurface->StartU(), pSurface->EndV()));
    pNode->Tag().SetPointer(pExtend);
    pNode->SetVisible(!m_bWireFrame);
    CGroupNode *pGroupNode = new CGroupNode();
    pGroupNode->AddChild(pNode);

    double dMidU = pSurface->GetRangeU().Middle(), dMidV = pSurface->GetRangeV().Middle();
    CVector3d ptPos = pSurface->GetPoint(dMidU, dMidV);
    CVector3d ptDir = pSurface->GetNormal(dMidU, dMidV);
    addDirNode(pGroupNode, ptPos, ptDir, true);
    m_pViewer->Scene()->GetRootNode()->AddChild(pGroupNode);
    /*if (pSurface->Type() == NurbsSurfaceType)
    {
    CColor vertexColor = color;
    vertexColor.R = vertexColor.R * 2 / 3;
    vertexColor.G = vertexColor.G * 2 / 3;
    vertexColor.B = vertexColor.B * 2 / 3;
    Vec3List oPts;
    CNurbsSurface *pNurbsSurface = static_cast<CNurbsSurface*>(pSurface);
    for (int i = 0; i < pNurbsSurface->CtrlPtCountU(); ++i)
    {
    for (int j = 0; j < pNurbsSurface->CtrlPtCountV(); ++j)
    {
    oPts.push_back(pNurbsSurface->CtrlPt(i, j).Point.Vec3f());
    }
    }
    showPoint3d(oPts, vertexColor, 2, false);
    }*/
    return pGroupNode;
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      显示多边形
* @param[in]  CPolygon * pPolygon
* @return     CGroupNode*
*/
CGroupNode* CViewerWidget::showPolygon( CPolygon *pPolygon )
{
    CGroupNode *pGroupNode = new CGroupNode();
    m_pViewer->Scene()->GetRootNode()->AddChild(pGroupNode);
    for (int i = 0; i < pPolygon->LoopCount(); ++i)
    {
        showCoedges(pPolygon->GetLoop(i), true, pGroupNode);
    }
    return pGroupNode;
}

/*!
 * @author           zhangyc-c
 * @brief            显示共边
 * @param[in]        CCoedgeList * pCoedges
 * @param[in]        bool bNeedDir
 * @param[in]        CGroupNode * pGroupNode
 * @return           CGroupNode*
 */
CGroupNode* CViewerWidget::showCoedges( CCoedgeList *pCoedges, bool bNeedDir, CGroupNode *pGroupNode)
{
    if (!pGroupNode)
    {
        pGroupNode = new CGroupNode();
        m_pViewer->Scene()->GetRootNode()->AddChild(pGroupNode);
    }
    for (int i = 0; i < pCoedges->CoedgeCount(); ++i)
    {
        pGroupNode = showCurve2d(pCoedges->GetCurve(i), bNeedDir, pGroupNode);
    }
    return pGroupNode;
}

/*!
 * @author           zhangyc-c
 * @brief            显示三维线
 * @param[in]        CCurve3d * pCurve
 * @param[in]        bool bNeedDir
 * @param[in]        CGroupNode * pGroupNode
 * @return           CGroupNode*
 */
CGroupNode* CViewerWidget::showCurve3d( CCurve3d *pCurve, bool bNeedDir, CGroupNode *pGroupNode )
{
    Vec3List vertexList;
    CMeshParameter oMesh(m_dDistEps, m_dAngleEps);
    if (m_nMeshType == 1)
    {
        CMeshParameter::GetCommonPrecision(pCurve, oMesh.m_tolerance, oMesh.m_angle);
    }
    else if (m_nMeshType == 2)
    {
        CMeshParameter::GetMinPrecision(pCurve, oMesh.m_tolerance, oMesh.m_angle);
    }
    oMesh.GetLines(pCurve, vertexList);

    if (pGroupNode == nullptr)
    {
        pGroupNode = new CGroupNode();
        m_pViewer->Scene()->GetRootNode()->AddChild(pGroupNode);
    }
    if (pGroupNode->NumChildren() < 1)
    {
        pGroupNode->AddChild(new CVisualNode());
    }
    CVisualNode *pNode = static_cast<CVisualNode*>(pGroupNode->GetChild(0));

    CPrimitiveRenderable *pRenderable = new CPrimitiveRenderable();
    pRenderable->SetPrimitiveType(PT_LINES);
    //pRenderable->SetIsEnableSingleColor(false);
    pRenderable->SetVertexArray(vertexList);
    CStateSet *pStateSet = pRenderable->GetOrCreateStateSet();
    pStateSet->SetMaterial(m_nColor);
    pStateSet->SetLineWidth(m_dLineWidth);
    pStateSet->SetLightingEnabled(false);
    pNode->AddRenderable(pRenderable);

    //记录几何体上可捕捉的点
    CNodeExtend *pExtend = (CNodeExtend*)pNode->Tag().GetPointer();
    if (!pExtend)
    {
        pExtend = new CNodeExtend();
    }
    pExtend->m_oVertexs.push_back(pCurve->StartPoint());
    pExtend->m_oVertexs.push_back(pCurve->EndPoint());
    pNode->Tag().SetPointer(pExtend);

    //创建“线条方向”显示节点
    if (bNeedDir)
    {
        double dMidT = (pCurve->StartT() + pCurve->EndT()) / 2.0;
        CVector3d ptMid = pCurve->GetPoint(dMidT);
        CVector3d ptDir = pCurve->GetFirstDer(dMidT);
        addDirNode(pGroupNode, ptMid, ptDir, false);
        //CVec3fList dirPts;
        //ColorList colorList;
        //dirPts.push_back(CVector3f(8, 0, 0));
        //dirPts.push_back(CVector3f(-8, 8, 0));
        //dirPts.push_back(CVector3f(-8, -8, 0));
        //CColor vertexColor = m_nColor;
        //vertexColor.R = vertexColor.R * 2 / 3;
        //vertexColor.G = vertexColor.G * 2 / 3;
        //vertexColor.B = vertexColor.B * 2 / 3;
        //colorList.push_back(vertexColor);
        //colorList.push_back(vertexColor);
        //colorList.push_back(vertexColor);
        //CVectorGraphics2D *pDirNode = new CVectorGraphics2D(dirPts, colorList, ptMid, ptDir);
        //pGroupNode->AddChild(pDirNode);
        //CNodeExtend *pDirExtend = (CNodeExtend*)pDirNode->Tag().GetPointer();
        //if (!pDirExtend)
        //{
        //	pDirExtend = new CNodeExtend();
        //	pDirNode->Tag().SetPointer(pDirExtend);
        //}
        //pDirExtend->m_bDirNode = true;
    }

    return pGroupNode;
}

/*!
 * @author           zhangyc-c
 * @brief            显示二维线
 * @param[in]        CCurve2d * pCurve
 * @param[in]        bool bNeedDir
 * @param[in]        CGroupNode * pGroupNode
 * @return           CGroupNode *
 */
CGroupNode * CViewerWidget::showCurve2d( CCurve2d *pCurve, bool bNeedDir, CGroupNode *pGroupNode)
{
    CCurve3dPtr pCurve3d = new CPlaneCurve3d(CVector3d::Zero, CVector3d::UnitX, CVector3d::UnitY, pCurve);
    return showCurve3d(pCurve3d.get(), bNeedDir, pGroupNode);
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      创建网格
* @return     void
*/
void CViewerWidget::createAxis()
{
    m_pAxisNode = new CGroupNode();
    m_pAxisNode->AddChild(new CVisualNode());
    m_pViewer->Scene()->GetRootNode()->AddChild(m_pAxisNode);
    const int c_AxisCount = 20;             //第一象限轴线个数
    const double c_AxisDist = 3000.0;       //轴线间距
    const double c_AxisMax = c_AxisCount * c_AxisDist;
    const double dZero = 0.0;
    m_nColor = CColor::DarkGray;
    m_dLineWidth = 1.0;
    for (int i = 1; i <= c_AxisCount; ++i)
    {
        CLine3d oLineX(CVector3d(0, i * c_AxisDist, dZero), CVector3d(c_AxisMax, i * c_AxisDist, dZero));
        CLine3d oLineY(CVector3d(i * c_AxisDist, 0, dZero), CVector3d(i * c_AxisDist, c_AxisMax, dZero));
        showCurve3d(&oLineX, false, m_pAxisNode);
        showCurve3d(&oLineY, false, m_pAxisNode);
    }
    CLine3d oLineX(CVector3d(dZero, dZero, dZero), CVector3d(c_AxisMax, dZero, dZero));
    CLine3d oLineY(CVector3d(dZero, dZero, dZero), CVector3d(dZero, c_AxisMax, dZero));
    CLine3d oLineZ(CVector3d(dZero, dZero, dZero), CVector3d(dZero, dZero, c_AxisMax));
    m_dLineWidth = 2.0;
    m_nColor = CColor::Red;		showCurve3d(&oLineX, false, m_pAxisNode);
    m_nColor = CColor::Green;	showCurve3d(&oLineY, false, m_pAxisNode);
    m_nColor = CColor::Blue;	showCurve3d(&oLineZ, false, m_pAxisNode);
    for (int i = m_pAxisNode->NumChildren() - 1; i > 0; --i)
    {
        m_pAxisNode->RemoveChild(m_pAxisNode->GetChild(i));
    }
    CNodeExtend *pExtend = (CNodeExtend*)m_pAxisNode->GetChild(0)->Tag().GetPointer();
    if (!pExtend)
    {
        pExtend = new CNodeExtend();
        m_pAxisNode->GetChild(0)->Tag().SetPointer(pExtend);
    }
    pExtend->m_oVertexs.clear();
    for (int i = 0; i <= c_AxisCount; ++i)
    {
        double dX = i * c_AxisDist;
        for (int j = 0; j <= c_AxisCount; ++j)
        {
            double dY = j * c_AxisDist;
            pExtend->m_oVertexs.push_back(CVector3d(dX, dY, 0));
        }
    }
    m_pAxisNode->SetPriority(1000000);   //优先级大一点，显示在下方
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      控制网格是否显示
* @param[in]  bool bVisible
* @return     void
*/
void CViewerWidget::showAxis( bool bVisible )
{
    m_pAxisNode->SetVisible(bVisible);
    m_pViewer->Scene()->SetDirty();
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      显示几何体，根据不同的几何类型分类处理
* @param[in]  CGeometry * pGeo
* @param[in]  CColor color
* @param[in]  double dWidth
* @return     CGroupNode*
*/
CGroupNode* CViewerWidget::showGeometry( CGeometry *pGeo )
{
    if (!pGeo)
        return NULL;
    CBody *pBody = dynamic_cast<CBody*>(pGeo);
    CEdge *pEdge = dynamic_cast<CEdge*>(pGeo);
    CFace *pFace = dynamic_cast<CFace*>(pGeo);
    CSurface *pSurface = dynamic_cast<CSurface*>(pGeo);
    CPolygon *pPolygon = dynamic_cast<CPolygon*>(pGeo);
    CCoedgeList *pCoedgeList = dynamic_cast<CCoedgeList*>(pGeo);
    CCoedge *pCoedge = dynamic_cast<CCoedge*>(pGeo);
    CCurve3d *pCurve3d = dynamic_cast<CCurve3d*>(pGeo);
    CCurve2d *pCurve2d = dynamic_cast<CCurve2d*>(pGeo);
    CPolyhedralFace *pPolyhedralFace = dynamic_cast<CPolyhedralFace*>(pGeo);
    if (CBody *pBody = dynamic_cast<CBody*>(pGeo))
    {
        return showBody(pBody);
    }
    else if (CFace *pFace = dynamic_cast<CFace*>(pGeo))
    {
        return showFace(pFace);
    }
    else if (CEdge *pEdge = dynamic_cast<CEdge*>(pGeo))
    {
        return showCurve3d(pEdge->Curve());
    }
    else if (CSurface *pSurface = dynamic_cast<CSurface*>(pGeo))
    {
        return showSurface(pSurface);
    }
    else if (CPolygon *pPolygon = dynamic_cast<CPolygon*>(pGeo))
    {
        return showPolygon(pPolygon);
    }
    else if (CCoedgeList *pCoedgeList = dynamic_cast<CCoedgeList*>(pGeo))
    {
        return showCoedges(pCoedgeList);
    }
    else if (CCurve3d *pCurve3d = dynamic_cast<CCurve3d*>(pGeo))
    {
        return showCurve3d(pCurve3d);
    }
    else if (CCurve2d *pCurve2d = dynamic_cast<CCurve2d*>(pGeo))
    {
        return showCurve2d(pCurve2d);
    }
    else if (CCoedge *pCoedge = dynamic_cast<CCoedge*>(pGeo))
    {
        return showCurve2d(pCoedge->Curve());
    }
    else if (CPolyhedralFace *pPolyhedralFace = dynamic_cast<CPolyhedralFace*>(pGeo))
    {
        return showPolyhedralFace(pPolyhedralFace);
    }
    else if (CPolyhedralLoop *pPolyhedralLoop = dynamic_cast<CPolyhedralLoop*>(pGeo))
    {
        return showPolyhedralLoop(pPolyhedralLoop);
    }
    else if (CPolyhedralEdge *pPolyhedralEdge = dynamic_cast<CPolyhedralEdge*>(pGeo))
    {
        return showPolyhedralEdge(pPolyhedralEdge);
    }
    else if (CPolyhedralVertex *pPolyhedralVertex = dynamic_cast<CPolyhedralVertex*>(pGeo))
    {
        return showItem(itVector3d, (void*)&pPolyhedralVertex->GetPosition());
    }
    return NULL;
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      控制线条方向是否显示
* @param[in]  bool bVisible
* @return     void
*/
void CViewerWidget::showDir( bool bVisible )
{
    if (m_bShowDir != bVisible)
    {
        m_bShowDir = bVisible;
        CGroupNode *pRootNode = m_pViewer->Scene()->GetRootNode();
        int nGroupCount = pRootNode->NumChildren();
        for (int i = 0; i < nGroupCount; ++i)
        {
            CGroupNode *pGroupNode = dynamic_cast<CGroupNode*>(pRootNode->GetChild(i));
            if (pGroupNode)
            {
                int nChildCount = pGroupNode->NumChildren();
                for (int j = 0; j < nChildCount; ++j)
                {
                    CSceneNode *pChildNode = pGroupNode->GetChild(j);
                    CNodeExtend *pNodeExtend = static_cast<CNodeExtend*>(pChildNode->Tag().GetPointer());
                    if (pNodeExtend && pNodeExtend->m_bDirNode)
                    {
                        pChildNode->SetVisible(bVisible);
                    }
                }
            }
        }
        m_pViewer->Scene()->SetDirty();
    }
}

/*!
* @author     zhangyc-c  2015.11.20
* @brief      亮显捕捉点
* @param[in]  const CVector3d * pt
* @return     void
*/
void CViewerWidget::showHitPoint( const CVector3d* pt )
{
    if(pt)
    {
        m_pHitPtNode->SetPosition(pt->Vec3f());
        m_pHitPtNode->SetVisible(true);
    }
    else
    {
        m_pHitPtNode->SetVisible(false);
    }
    m_pViewer->Scene()->SetDirty();
}

void CViewerWidget::setNodeColor( CSceneNode *pNode, CColor nColor )
{
    CGroupNode *pGroupNode = dynamic_cast<CGroupNode*>(pNode);
    if (pGroupNode)
    {
        for (int i = 0; i < pGroupNode->NumChildren(); ++i)
        {
            setNodeColor(pGroupNode->GetChild(i), nColor);
        }
        return;
    }
    CPointSet *pPointSet = dynamic_cast<CPointSet*>(pNode);
    if (pPointSet)
    {
        CPrimitiveRenderable* pRenderable = static_cast<CPrimitiveRenderable*>(pPointSet->GetOnlyRenderable());
        ColorList oColorList;
        pRenderable->GetColorArray(oColorList);
        for (size_t i = 0; i < oColorList.size(); ++i)
        {
            oColorList[i] = nColor;
        }
        pRenderable->SetColorArray(oColorList);
        return;
    }
    CVectorGraphics2D *pDirNode = dynamic_cast<CVectorGraphics2D*>(pNode);
    if (pDirNode)
    {
        ColorList oColorList;
        CColor vertexColor = nColor;
        vertexColor.R = vertexColor.R * 2 / 3;
        vertexColor.G = vertexColor.G * 2 / 3;
        vertexColor.B = vertexColor.B * 2 / 3;
        oColorList.push_back(vertexColor);
        oColorList.push_back(vertexColor);
        oColorList.push_back(vertexColor);
        pDirNode->SetColorList(oColorList);
        return;
    }
    CVisualNode *pVisualNode = dynamic_cast<CVisualNode*>(pNode);
    if (pVisualNode)
    {
        CColor vertexColor = nColor;
        CNodeExtend *pNodeExtend = (CNodeExtend*)pVisualNode->Tag().GetPointer();
        if (pNodeExtend && pNodeExtend->m_bDirNode)
        {
            vertexColor.R = ~vertexColor.R;
            vertexColor.G = ~vertexColor.G;
            vertexColor.B = ~vertexColor.B;
        }
        for (int i = 0; i < pVisualNode->GetRenderableCount(); ++i)
        {
            CRenderable *pRenderable = pVisualNode->GetRenderable(i);
            CColor nOldColor = pRenderable->GetOrCreateStateSet()->GetDiffuse();
            pRenderable->GetOrCreateStateSet()->SetMaterial(nColor);
            if (pRenderable->PrimitiveType() == PT_TRIANGLES && nColor == CColor::White)
            {
                nOldColor.R = nOldColor.R >> 2;
                nOldColor.G = nOldColor.G >> 2;
                nOldColor.B = nOldColor.B >> 2;
                pVisualNode->GetRenderable(i)->GetOrCreateStateSet()->SetEmissive(nOldColor);
            }
        }
        return;
    }
}

CGroupNode* CViewerWidget::showVector2d(const CVector2d *pVec2d, bool bNewNode )
{
    CVector3d oVec3d = pVec2d->Vec3();
    return showVector3d(&oVec3d, bNewNode);
}

CGroupNode* CViewerWidget::showVector3d(const CVector3d *pVec3d, bool bNewNode )
{
    CGroupNode *pGroupNode;
    if (bNewNode)
    {
        pGroupNode = new CGroupNode();
        pGroupNode->AddChild(new CVisualNode());
        m_pViewer->Scene()->GetRootNode()->AddChild(pGroupNode);
    }
    CGroupNode *pRootNode = m_pViewer->Scene()->GetRootNode();
    pGroupNode = dynamic_cast<CGroupNode*>(pRootNode->GetChild(pRootNode->NumChildren() - 1));
    CPointSet *pPointNode = new CPointSet(Vec3List(1, pVec3d->Vec3f()), m_nColor, (float)m_dLineWidth);
    CNodeExtend *pExend = (CNodeExtend*)pPointNode->Tag().GetPointer();
    if (!pExend)
    {
        pExend = new CNodeExtend();
        pPointNode->Tag().SetPointer(pExend);
    }
    pExend->m_oVertexs.push_back(*pVec3d);
    pGroupNode->AddChild(pPointNode);
    return pGroupNode;
}

CGroupNode* CViewerWidget::showBox2d(CBox2d *pBox2d)
{
    CGroupNode *pGroupNode = new CGroupNode();
    pGroupNode->AddChild(new CVisualNode());
    m_pViewer->Scene()->GetRootNode()->AddChild(pGroupNode);
    double m_bOldShowDir = m_bShowDir;
    m_bShowDir = false;
    CVector2d oPts[4];
    pBox2d->GetCorners(oPts, false);
    for (int i = 0; i < 4; ++i)
    {
        showCurve2d(&CLine2d(oPts[i], oPts[(i + 1) % 4]), false, pGroupNode);
    }
    m_bShowDir = m_bOldShowDir;
    return pGroupNode;
}

CGroupNode* CViewerWidget::showBox3d(CBox3d *pBox3d)
{
    CGroupNode *pGroupNode = new CGroupNode();
    pGroupNode->AddChild(new CVisualNode());
    m_pViewer->Scene()->GetRootNode()->AddChild(pGroupNode);
    double m_bOldShowDir = m_bShowDir;
    m_bShowDir = false;
    CVector3d oPts[8];
    pBox3d->GetCorners(oPts, false);
    for (int i = 0; i < 4; ++i)
    {
        showCurve3d(&CLine3d(oPts[i], oPts[(i + 1) % 4]), false, pGroupNode);
        showCurve3d(&CLine3d(oPts[i], oPts[i + 4]), false, pGroupNode);
        showCurve3d(&CLine3d(oPts[i + 4], oPts[(i + 1) % 4 + 4]), false, pGroupNode);
    }
    m_bShowDir = m_bOldShowDir;
    return pGroupNode;
}

CGroupNode* CViewerWidget::showCoord(CCoordinates3d *pCoord)
{
    CGroupNode *pGroupNode = new CGroupNode();
    pGroupNode->AddChild(new CVisualNode());
    m_pViewer->Scene()->GetRootNode()->AddChild(pGroupNode);
    double m_bOldShowDir = m_bShowDir;
    m_bShowDir = true;
    showCurve3d(&CLine3d(pCoord->Origin, pCoord->Origin + 12500 * pCoord->X), true, pGroupNode);
    showCurve3d(&CLine3d(pCoord->Origin, pCoord->Origin + 10000 * pCoord->Y), true, pGroupNode);
    showCurve3d(&CLine3d(pCoord->Origin, pCoord->Origin + 7500 * pCoord->Z), true, pGroupNode);
    m_bShowDir = m_bOldShowDir;
    return pGroupNode;
}

CGroupNode* CViewerWidget::showItem(EnItemType nType, void *ptr)
{
    switch (nType)
    {
    case itGeometry:
        m_dLineWidth = 2.0;
        return showGeometry((CGeometry*)ptr);
    case itVector2d:
        m_dLineWidth = 10.0;
        return showVector2d((CVector2d*)ptr);
    case itVector3d:
        m_dLineWidth = 10.0;
        return showVector3d((CVector3d*)ptr);
    case itBox2d:
        m_dLineWidth = 4.0;
        return showBox2d((CBox2d*)ptr);
    case itBox3d:
        m_dLineWidth = 4.0;
        return showBox3d((CBox3d*)ptr);
    case itCoord:
        m_dLineWidth = 4.0;
        return showCoord((CCoordinates3d*)ptr);
    }
    return nullptr;
}

/*!
* @author           zhangyc-c
* @brief            添加方向显示节点
* @param[in]        CGroupNode * pGroupNode
* @param[in]        const CVector3d & ptPos
* @param[in]        const CVector3d & ptDir
* @return           void
*/
void CViewerWidget::addDirNode(CGroupNode *pGroupNode, const CVector3d& ptPos, const CVector3d& ptDir, bool bLine)
{
    CColor vertexColor = m_nColor;
    vertexColor.R = vertexColor.R * 2 / 3;
    vertexColor.G = vertexColor.G * 2 / 3;
    vertexColor.B = vertexColor.B * 2 / 3;
    CVisualNode *pDirNode = nullptr;
    if (bLine)
    {
        vertexColor.R = ~vertexColor.R;
        vertexColor.G = ~vertexColor.G;
        vertexColor.B = ~vertexColor.B;
        CVector3d ptStart = ptPos - 200 * ptDir;
        CVector3d ptEnd = ptPos + 200 * ptDir;

        CPrimitiveRenderable *pRenderable0 = new CPrimitiveRenderable();
        pRenderable0->SetPrimitiveType(PT_LINES);
        CStateSet *pStateSet0 = pRenderable0->GetOrCreateStateSet();
        pStateSet0->SetMaterial(vertexColor);
        pStateSet0->SetLineWidth(8.0f);
        pStateSet0->SetLineStipple(1, 0x3333);
        CVec3fList oLinePts0;
        oLinePts0.push_back(ptStart.Vec3f());
        oLinePts0.push_back(ptPos.Vec3f());
        pRenderable0->SetVertexArray(oLinePts0);

        CPrimitiveRenderable *pRenderable1 = new CPrimitiveRenderable();
        pRenderable1->SetPrimitiveType(PT_LINES);
        CStateSet *pStateSet1 = pRenderable1->GetOrCreateStateSet();
        pStateSet1->SetMaterial(vertexColor);
        pStateSet1->SetLineWidth(8.0f);
        CVec3fList oLinePts1;
        oLinePts1.push_back(ptPos.Vec3f());
        oLinePts1.push_back(ptEnd.Vec3f());
        pRenderable1->SetVertexArray(oLinePts1);

        pDirNode = new CVisualNode();
        pDirNode->AddRenderable(pRenderable0);
        pDirNode->AddRenderable(pRenderable1);
    }
    else
    {
        CVec3fList dirPts;
        dirPts.push_back(CVector3f(8, 0, 0));
        dirPts.push_back(CVector3f(-8, 8, 0));
        dirPts.push_back(CVector3f(-8, -8, 0));
        ColorList colorList(dirPts.size(), vertexColor);
        pDirNode = new CVectorGraphics2D(dirPts, colorList, ptPos.Vec3f(), ptDir.Vec3f());
    }
    CNodeExtend *pDirExtend = (CNodeExtend*)pDirNode->Tag().GetPointer();
    if (!pDirExtend)
    {
        pDirExtend = new CNodeExtend();
        pDirNode->Tag().SetPointer(pDirExtend);
    }
    pDirExtend->m_bDirNode = true;
    pDirNode->SetVisible(m_bShowDir);
    pGroupNode->AddChild(pDirNode);
}

/*!
 * @author           zhangyc-c
 * @brief            显示线框
 * @param[in]        bool bVisible
 * @return           void
 */
void CViewerWidget::showWireFrame(bool bWireFrame)
{
    if (m_bWireFrame != bWireFrame)
    {
        m_bWireFrame = bWireFrame;
        CGroupNode *pRootNode = m_pViewer->Scene()->GetRootNode();
        int nGroupCount = pRootNode->NumChildren();
        for (int i = 0; i < nGroupCount; ++i)
        {
            CGroupNode *pGroupNode = dynamic_cast<CGroupNode*>(pRootNode->GetChild(i));
            if (pGroupNode)
            {
                int nChildCount = pGroupNode->NumChildren();
                for (int j = 0; j < nChildCount; ++j)
                {
                    CSceneNode *pChildNode = pGroupNode->GetChild(j);
                    CNodeExtend *pNodeExtend = static_cast<CNodeExtend*>(pChildNode->Tag().GetPointer());
                    if (pNodeExtend && pNodeExtend->m_bTriangle)
                    {
                        pChildNode->SetVisible(!bWireFrame);
                    }
                }
            }
        }
        m_pViewer->Scene()->SetDirty();
    }
}

void CViewerWidget::setNodeVisible(CSceneNode *pNode, bool bVisible)
{
    pNode->SetVisible(bVisible);
    if (bVisible && !m_bShowDir)
    {
        CGroupNode *pGroupNode = dynamic_cast<CGroupNode*>(pNode);
        if (pGroupNode)
        {
            int nChildCount = pGroupNode->NumChildren();
            for (int j = 0; j < nChildCount; ++j)
            {
                CSceneNode *pChildNode = pGroupNode->GetChild(j);
                CNodeExtend *pNodeExtend = static_cast<CNodeExtend*>(pChildNode->Tag().GetPointer());
                if (pNodeExtend && pNodeExtend->m_bDirNode)
                {
                    pChildNode->SetVisible(false);
                }
            }
        }
    }
}

