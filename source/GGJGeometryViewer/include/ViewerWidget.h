#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include "ViewManager/GViewer.h"
#include "ViewCore/GViewport.h"
#include "ViewCore/GScene.h"
#include "ViewCore/GWorldPos2DNodes.h"
using namespace ggp;

#include <QWidget>
#include <QWheelEvent>
#include "Algorithm/alggeobuilder.h"
#include "Algorithm/alggeohelper.h"
#include "Algorithm/algint.h"
#include "Algorithm/algmesh.h"
#include "Algorithm/algoffset.h"
#include "Algorithm/algpolygon.h"
#include "Algorithm/algpositionJudge.h"
#include "Geometry/Polygon.h"
#include "Geometry/BrepBody.h"
#include "Geometry/Face.h"
#include "Geometry/Edge.h"
#include "Geometry/Curve2d.h"
#include "Geometry/Curve3d.h"
#include "Geometry/LoopEx.h"
#include "GMath/GVec2.h"
#include "GMath/GVec3.h"
#include "GMath/GBox2.h"
#include "GMath/GBox3.h"
#include "GMath/GCoordinates3.h"

enum EnItemType
{
	itGeometry = 1,
	itVector2d = 2,
	itVector3d = 3,
	itBox2d = 4,
	itBox3d = 5,
	itCoord = 6,
};

struct CNodeExtend
{
    CNodeExtend(): m_bDirNode(false), m_bTriangle(false){}
public:
    vector<CVector3d> m_oVertexs;
	bool m_bDirNode, m_bTriangle;
};

class CViewerWidget : public QWidget
{
    Q_OBJECT
public:
    int m_nMeshType;
    double m_dDistEps, m_dAngleEps, m_dLineWidth;
	CColor m_nColor;
    CViewerWidget(QWidget *parent = 0);
    ~CViewerWidget();
    void initScene();
    void highLight(EnItemType nType, void *ptr, CColor oColor = CColor::White);
    void createAxis();
    CViewer *viewer(){return m_pViewer;}
    void setNodeColor(CSceneNode *pNode, CColor nColor);
    void setNodeVisible(CSceneNode *pNode, bool bVisible);
    CGroupNode* showBody(CBody *pBody);
    CGroupNode* showFace(CFace *pFace);
    CGroupNode* showPolyhedralFace(CPolyhedralFace *pPolyhedralFace);
    CGroupNode* showPolyhedralLoop(CPolyhedralLoop *pPolyhedralLoop);
    CGroupNode* showPolyhedralEdge(CPolyhedralEdge *pPolyhedralEdge, bool bNeedDir = true, CGroupNode *pGroupNode = nullptr);
    CGroupNode* showSurface(CSurface *pSurface);
    CGroupNode* showPolygon(CPolygon *pPolygon);
    CGroupNode* showCoedges(CCoedgeList *pCoedges, bool bNeedDir = true, CGroupNode *pGroupNode = nullptr);
    CGroupNode* showCurve3d(CCurve3d *pCurve, bool bNeedDir = true, CGroupNode *pGroupNode = nullptr);
    CGroupNode* showCurve2d(CCurve2d *pCurve, bool bNeedDir = true, CGroupNode *pGroupNode = nullptr);
	CGroupNode* showGeometry(CGeometry *pGeo);
	CGroupNode* showVector2d(const CVector2d *pVec2d, bool bNewNode = true);
	CGroupNode* showVector3d(const CVector3d *pVec3d, bool bNewNode = true);
	CGroupNode* showBox2d(CBox2d *pBox2d);
	CGroupNode* showBox3d(CBox3d *pBox3d);
	CGroupNode* showCoord(CCoordinates3d *pCoord);
	CGroupNode* showItem(EnItemType nType, void *ptr);
    virtual QPaintEngine *paintEngine() const;
    virtual void wheelEvent(QWheelEvent *pEvent);
    void keyPressEvent(QKeyEvent *pEvent);
    void keyReleaseEvent(QKeyEvent *pEvent);
public Q_SLOTS:
    void showAxis(bool bVisible);
    void showDir(bool bVisible);
    void showHitPoint(const CVector3d* pt);
    void showWireFrame(bool bWireFrame);
private:
    void addDirNode(CGroupNode *pGroupNode, const CVector3d& ptPos, const CVector3d& ptDir, bool bLine);
    CViewer *m_pViewer;
    CGroupNode *m_pFocusNode, *m_pAxisNode;
    CVectorGraphics2D *m_pHitPtNode;
    bool m_bShowDir, m_bWireFrame;
};

#endif // VIEWERWIDGET_H
