/*!
 * @brief   主程序入口
 * @author  zhangyc-c
 * @date    2015.7.19
 * @remarks 
 * Copyright (c) 1998-2015 Glodon Corporation	
 */
#include <string>
#include <QApplication>
#include "GeometryViewer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	QString strExePath = QCoreApplication::applicationDirPath();
	SetCurrentDirectoryW(strExePath.toStdWString().c_str());
    CGeometryViewer viewer;
    /*
    CBodyPtr ipRetBody = {TYPE=1403,origin=(2218.04539949999980308348 26095.94284334999974817038 0.00000000000000000000),coordX=(1.00000000000000000000 0.00000000000000000000 0.00000000000000000000),coordY=(-0.00000000000000000000 0.00000000000000000000 1.00000000000000000000),coordZ=(0.00000000000000000000 -1.00000000000000000000 0.00000000000000000000),length=225.00000000000000000000,IsShell=0,}{TYPE=1309,LoopCount=1,DistEpsilon=0.00010000000000000000,BoxValid=1,Box2d=[(0.00000000000000000000 0.00000000000000000000),(2136.02682955999898695154 4430.78201342964985087747)],Loop0={TYPE=1304,DistEpsilon=0.00010000000000000000,CoedgeCount=4,BoxValid=1,Box2d=[(0.00000000000000000000 0.00000000000000000000),(2136.02682955999898695154 4430.78201342964985087747)]Coedge0={TYPE=1301,CurveData={TYPE=1,Pos=(0.00000000000000000000 0.00000000000000000000),Dir=(1.00000000000000000000 0.00000000000000000000),Range=[0.00000000000000000000 2136.02682955999898695154]}}Coedge1={TYPE=1301,CurveData={TYPE=1,Pos=(2136.02682955999898695154 0.00000000000000000000),Dir=(0.00000000000000000000 1.00000000000000000000),Range=[0.00000000000000000000 4430.78201342964985087747]}}Coedge2={TYPE=1301,CurveData={TYPE=1,Pos=(2136.02682955999898695154 4430.78201342964985087747),Dir=(-0.83083399724741402448 -0.55652032219666858204),Range=[0.00000000000000000000 2570.94297613812204872374]}}Coedge3={TYPE=1301,CurveData={TYPE=1,Pos=(0.00000000000000000000 3000.00000000000000000000),Dir=(0.00000000000000000000 -1.00000000000000000000),Range=[0.00000000000000000000 3000.00000000000000000000]}}}}
    CPolygonPtr pWorldPoly = {TYPE=1309,LoopCount=1,DistEpsilon=0.00010000000000000000,BoxValid=1,Box2d=[(-12912.00729252999917662237 10536.60316910000074130949),(-12807.30880242000057478435 10636.60316910000074130949)],Loop0={TYPE=1304,DistEpsilon=0.00010000000000000000,CoedgeCount=4,BoxValid=1,Box2d=[(-12912.00729252999917662237 10536.60316910000074130949),(-12807.30880242000057478435 10636.60316910000074130949)]Coedge0={TYPE=1301,CurveData={TYPE=1,Pos=(-12807.30880242000057478435 10536.60316910000074130949),Dir=(0.00000000000000000000 1.00000000000000000000),Range=[0.00000000000000000000 100.00000000000000000000]}}Coedge1={TYPE=1301,CurveData={TYPE=1,Pos=(-12807.30880242000057478435 10636.60316910000074130949),Dir=(-1.00000000000000000000 0.00000000000000000000),Range=[0.00000000000000000000 104.69849010999858762716]}}Coedge2={TYPE=1301,CurveData={TYPE=1,Pos=(-12912.00729252999917662237 10636.60316910000074130949),Dir=(0.00000000000000000000 -1.00000000000000000000),Range=[0.00000000000000000000 100.00000000000000000000]}}Coedge3={TYPE=1301,CurveData={TYPE=1,Pos=(-12807.30880242000057478435 10536.60316910000074130949),Dir=(1.00000000000000000000 -0.00000000000000000000),Range=[-104.69849010999858762716 -0.00000000000000000000]}}}}

    */

    /*
    //double dRadius = 1000, dHeight = 2500;
    //CBrepBody *pBrepBody = new CBrepBody();
    //// 创建曲面
    //CArc2d *pCircle = new CArc2d(CVector2d::Zero, dRadius, 0.0, M_2PI);
    //CBevel *pBevel = new CBevel(CCoordinates3d::GlobalCCoord3D, dHeight, CIntervald(0, dHeight), pCircle);
    //CPlane *pPlane = new CPlane(CVector3d::Zero, CVector3d::UnitX, CVector3d::UnitY, CIntervald(-dRadius, dRadius), CIntervald(-dRadius, dRadius));
    //// 创建三维边
    //CArc3d *pCircle3d = new CArc3d(CVector3d::Zero, CVector3d::UnitX, CVector3d::UnitY, pCircle);
    //CLine3d *pGeneratrix = new CLine3d(CVector3d(dRadius, 0, 0), CVector3d(0, 0, dHeight));
    //CLine3d *pVertex = new CLine3d(CVector3d(0, 0, dHeight), CVector3d::UnitX, CIntervald(0, 0));
    //// 添加曲面
    //pBrepBody->AddFace(pBevel, true);
    //pBrepBody->AddFace(pPlane, false);
    //// 添加锥面的环
    //{
    //    CVector2d oPts[5] = 
    //    {
    //        CVector2d(pBevel->StartU(), pBevel->StartV()),
    //        CVector2d(pBevel->EndU(), pBevel->StartV()),
    //        CVector2d(pBevel->EndU(), pBevel->EndV()),
    //        CVector2d((pBevel->StartU() + pBevel->EndU()) * 0.5, pBevel->EndV()),
    //        CVector2d(pBevel->StartU(), pBevel->EndV()),
    //    };
    //    CLoopEx *pLoopEx = new CLoopEx();
    //    CCoedgeEx *pCoedgeEx0 = new CCoedgeEx(new CLine2d(oPts[0], oPts[1]));
    //    CCoedgeEx *pCoedgeEx1 = new CCoedgeEx(new CLine2d(oPts[1], oPts[2]));
    //    CCoedgeEx *pCoedgeEx2 = new CCoedgeEx(new CLine2d(oPts[2], oPts[3]));
    //    CCoedgeEx *pCoedgeEx3 = new CCoedgeEx(new CLine2d(oPts[3], oPts[4]));
    //    CCoedgeEx *pCoedgeEx4 = new CCoedgeEx(new CLine2d(oPts[4], oPts[0]));
    //    pCoedgeEx0->SetEdgeIndex(0);
    //    pCoedgeEx1->SetEdgeIndex(1);
    //    pCoedgeEx2->SetEdgeIndex(2);
    //    pCoedgeEx3->SetEdgeIndex(2);
    //    pCoedgeEx4->SetEdgeIndex(1);
    //    pCoedgeEx0->SetEdgeSameDir(true);
    //    pCoedgeEx1->SetEdgeSameDir(true);
    //    pCoedgeEx2->SetEdgeSameDir(true);
    //    pCoedgeEx3->SetEdgeSameDir(false);
    //    pCoedgeEx4->SetEdgeSameDir(false);
    //    pLoopEx->AddCoedge(pCoedgeEx0);
    //    pLoopEx->AddCoedge(pCoedgeEx1);
    //    pLoopEx->AddCoedge(pCoedgeEx2);
    //    pLoopEx->AddCoedge(pCoedgeEx3);
    //    pLoopEx->AddCoedge(pCoedgeEx4);
    //    pBrepBody->GetFace(0)->AddLoop(pLoopEx);
    //}

    //// 添加平面的环
    //{
    //    CLoopEx *pLoopEx = new CLoopEx();
    //    CCoedgeEx *pCoedgeEx = new CCoedgeEx(pCircle);
    //    pCoedgeEx->SetEdgeIndex(0);
    //    pCoedgeEx->SetEdgeSameDir(true);
    //    pLoopEx->AddCoedge(pCoedgeEx);
    //    pBrepBody->GetFace(1)->AddLoop(pLoopEx);
    //}

    //// 添加三维边
    //pBrepBody->AddEdge(pCircle3d, 0, 0, 0, 1, 0, 0);
    //pBrepBody->AddEdge(pGeneratrix, 0, 0, 1, 0, 0, 4);
    //pBrepBody->AddEdge(pVertex, 0, 0, 2, 0, 0, 3);

    //// 设置成体或壳
    //pBrepBody->SetIsShell(false);

    //viewer.addGeometry(pBrepBody);
    ////


    //CPolyhedronBody *pBody = new CPolyhedronBody();
    //
    //CPolyhedralFace *pFaceCBA = new CPolyhedralFace(pBody);
    //CPolyhedralFace *pFaceBCD = new CPolyhedralFace(pBody);
    //CPolyhedralFace *pFaceDCA = new CPolyhedralFace(pBody);
    //CPolyhedralFace *pFaceABD = new CPolyhedralFace(pBody);

    //CPolyhedralVertex *pVertexA = pBody->AddVertex(CVector3d::Zero);
    //CPolyhedralVertex *pVertexB = pBody->AddVertex(CVector3d(3000, 0, 0));
    //CPolyhedralVertex *pVertexC = pBody->AddVertex(CVector3d(0, 3000, 0));
    //CPolyhedralVertex *pVertexD = pBody->AddVertex(CVector3d(0, 0, 3000));
    //
    //CPolyhedralEdge *pEdgeAB = new CPolyhedralEdge(pVertexA, pVertexB);
    //CPolyhedralEdge *pEdgeAC = new CPolyhedralEdge(pVertexA, pVertexC);
    //CPolyhedralEdge *pEdgeAD = new CPolyhedralEdge(pVertexA, pVertexD);
    //CPolyhedralEdge *pEdgeBA = new CPolyhedralEdge(pVertexB, pVertexA);
    //CPolyhedralEdge *pEdgeBC = new CPolyhedralEdge(pVertexB, pVertexC);
    //CPolyhedralEdge *pEdgeBD = new CPolyhedralEdge(pVertexB, pVertexD);
    //CPolyhedralEdge *pEdgeCA = new CPolyhedralEdge(pVertexC, pVertexA);
    //CPolyhedralEdge *pEdgeCB = new CPolyhedralEdge(pVertexC, pVertexB);
    //CPolyhedralEdge *pEdgeCD = new CPolyhedralEdge(pVertexC, pVertexD);
    //CPolyhedralEdge *pEdgeDA = new CPolyhedralEdge(pVertexD, pVertexA);
    //CPolyhedralEdge *pEdgeDB = new CPolyhedralEdge(pVertexD, pVertexB);
    //CPolyhedralEdge *pEdgeDC = new CPolyhedralEdge(pVertexD, pVertexC);

    //CPolyhedralLoop *pLoopCBA = new CPolyhedralLoop(pFaceCBA);
    //CPolyhedralLoop *pLoopBCD = new CPolyhedralLoop(pFaceBCD);
    //CPolyhedralLoop *pLoopDCA = new CPolyhedralLoop(pFaceDCA);
    //CPolyhedralLoop *pLoopABD = new CPolyhedralLoop(pFaceABD);

    //pFaceCBA->AddLoop(pLoopCBA);
    //pFaceBCD->AddLoop(pLoopBCD);
    //pFaceDCA->AddLoop(pLoopDCA);
    //pFaceABD->AddLoop(pLoopABD);

    //pLoopABD->AddEdge(pEdgeAB);
    //pLoopABD->AddEdge(pEdgeBD);
    //pLoopABD->AddEdge(pEdgeDA);
    //pLoopBCD->AddEdge(pEdgeBC);
    //pLoopBCD->AddEdge(pEdgeCD);
    //pLoopBCD->AddEdge(pEdgeDB);
    //pLoopCBA->AddEdge(pEdgeCB);
    //pLoopCBA->AddEdge(pEdgeBA);
    //pLoopCBA->AddEdge(pEdgeAC);
    //pLoopDCA->AddEdge(pEdgeDC);
    //pLoopDCA->AddEdge(pEdgeCA);
    //pLoopDCA->AddEdge(pEdgeAD);

    //pBody->AddFace(pFaceABD);
    //pBody->AddFace(pFaceBCD);
    //pBody->AddFace(pFaceCBA);
    //pBody->AddFace(pFaceDCA);

    //pBody->SetTwinEdges();
    //pBody->CalFacesPlaneCoef();

    ////这里可以添加构造几何数据的代码，构造完后调用下面的方法进行显示
    //viewer.addGeometry(pBody);
    */

    viewer.show();
    return app.exec();
}
