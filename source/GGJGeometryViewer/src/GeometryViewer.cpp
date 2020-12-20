/*!
* @brief   几何查看器主界面
* @author  zhangyc-c
* @date    2015.2.13
* @remarks 
* Copyright (c) 1998-2015 Glodon Corporation
*/

#include <typeinfo.h>
#include <ostream>
#include <stdlib.h>
#include <time.h>

#include <QListView>
#include <QListWidget>
#include <QListWidgetItem>

#include "Algorithm/algbodyextrema.h"
#include "Algorithm/algbool.h"
#include "Algorithm/algcurvepolybool.h"
#include "Algorithm/algdistance.h"
#include "Algorithm/algpolygon.h"
#include "Algorithm/algpositionJudge.h"
#include "Algorithm/algprojection.h"
#include "Algorithm/HLR/HLRAPI.h"
#include "Common/ErrorHandle.h"
#include "Cmd.h"

#include "GeometryInput.h"
#include "GeometryViewer.h"
#include "Example.h"

using namespace std;

const QString c_ToolBarBgColor[] = {"rgba(153, 204, 255, 255)","rgba(204, 153, 255, 255)", "rgba(153, 255, 204, 255)","rgba(204, 255, 153, 255)", "rgba(255, 153, 204, 255)","rgba(255, 204, 153, 255)",};
const QString c_IsValid = "IsValid";
const QString c_IsStrictlyValid = "IsStrictlyValid";
const QString c_IsClose = "IsClose";
const QString c_IsShell = "IsShell";
const QString c_DistEpsilon = "DistEpsilon";
const QString c_SurfaceArea = "SurfaceArea";
const QString c_Volume = "Volume";
const QString c_Length = "Length";
const QString c_SameDir = "SameDir";
const QString c_ClockSign = "ClockSign";
const QString c_HeightCoef = "HeightCoef";
const QString c_RadiusCoef = "RadiusCoef";
const QString c_Origin = "Origin";
const QString c_A = "A";
const QString c_B = "B";
const QString c_C = "C";
const QString c_D = "D";
const QString c_X = "X";
const QString c_Y = "Y";
const QString c_Z = "Z";
const QString c_MinPt = "MinPt";
const QString c_MaxPt = "MaxPt";
const QString c_MinX = "MinPt().X";
const QString c_MinY = "MinPt().Y";
const QString c_MinZ = "MinPt().Z";
const QString c_MaxX = "MaxPt().X";
const QString c_MaxY = "MaxPt().Y";
const QString c_MaxZ = "MaxPt().Z";
const QString c_Pos = "Pos";
const QString c_Dir = "Dir";
const QString c_DirU = "DirU";
const QString c_DirV = "DirV";
const QString c_DirX = "DirX";
const QString c_DirY = "DirY";
const QString c_DirZ = "DirZ";
const QString c_Range = "Range";
const QString c_RangeU = "RangeU";
const QString c_RangeV = "RangeV";
const QString c_CenterPt = "CenterPt";
const QString c_Radius = "Radius";
const QString c_StartPt = "StartPt";
const QString c_EndPt = "EndPt";
const QString c_IsLine = "IsLine";
const QString c_Profile = "Profile";
const QString c_Area = "Area";
const QString c_ReferenceVector = "ReferenceVector";
const QString c_OffsetDistance = "OffsetDistance";
const QString c_VertexCount = "VertexCount";
const QString c_Tips = QStringLiteral("提示");
const QString c_ReturnNull = QStringLiteral("返回空值！");
const QString c_ParamError = QStringLiteral("参数有误！");
const QString c_ExtrudeMergeFacesMethod = QStringLiteral("调用pExtrudeBody->Section()->MergeCoedges()也许能达到效果！");
//const QString c_DailyTips[] = 
//{
//    QStringLiteral("右键点击删除按钮可以清空几何列表。"),
//};
const int c_nCameraAnimateSteps = 15;
const int c_nCameraAnimateTime = 20;
const double c_dYawAngle[] = {M_PI_2, -M_PI_2, 0, M_PI, M_PI_2, -M_PI_2};    //对应EnViewPoint，前后左右上下
const double c_dPitchAngle[] = {0, 0, 0, 0, -M_PI_2, M_PI_2};  //对应EnViewPoint，前后左右上下
const QColor c_nInfoColor[] = {QColor(250, 200, 150), QColor(250, 150, 200), QColor(200, 250, 150), QColor(200, 150, 250), QColor(150, 250, 200), QColor(150, 200, 250)};

CToolButton::CToolButton(QWidget * parent /*= 0*/)
    : m_bLeftPress(false), m_bRightPress(false), QToolButton(parent)
{

}

void CToolButton::mousePressEvent(QMouseEvent *pEvent)
{
    if (pEvent->button() == Qt::LeftButton)
    {
        m_bLeftPress = true;
        setDown(true);
    }
    else if (pEvent->button() == Qt::RightButton)
    {
        m_bRightPress = true;
        setDown(true);
    }
    QToolButton::mousePressEvent(pEvent);
}

void CToolButton::mouseReleaseEvent(QMouseEvent *pEvent)
{
    if (m_bLeftPress && pEvent->button() == Qt::LeftButton)
    {
        emit leftClicked();
    }
    else if (m_bRightPress && pEvent->button() == Qt::RightButton)
    {
        emit rightClicked();
    }
    setDown(false);
    m_bLeftPress = m_bRightPress = false;
    QToolButton::mouseReleaseEvent(pEvent);
}

void CToolButton::leaveEvent(QEvent *pEvent)
{
    m_bLeftPress = m_bRightPress = false;
    QToolButton::leaveEvent(pEvent);
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      构造函数，界面初始化
* @param[in]  QWidget * parent
* @return     
*/
CGeometryViewer::CGeometryViewer(QWidget *parent)
    : QMainWindow(parent), m_nColorIndex(0), m_pCurItem(nullptr), m_bMouseDown(false), m_bAutoCamera(true), m_bAnimateView(true), 
    m_dBoolEps(0.1), m_pGGDBHelper(nullptr), m_pDiscoveryWidget(nullptr), m_pOptionWidget(nullptr),m_isGGJ(false)
{
    srand((unsigned)time(nullptr));
    m_pCmdCenter = new CCmdCenter(this);

    m_pViewerWidget = new CViewerWidget(this);
    setWindowTitle(QStringLiteral("几何查看器"));
    setWindowIcon(QIcon("image/Logo.ico"));
    setContextMenuPolicy(Qt::CustomContextMenu);
    resize(1008, 730);
    initMenu();
    initToolBar();
    initStatusBar();
    m_trGeometry = new QTreeWidget(this);
    m_trGeometry->headerItem()->setText(0, QStringLiteral("几何列表"));
    m_trGeometry->setAnimated(true);
    m_trGeometry->setContextMenuPolicy(Qt::CustomContextMenu);
    m_trGeometry->setDragEnabled(true);
    m_trGeometry->setDragDropMode(QAbstractItemView::InternalMove);
    qApp->installEventFilter(this);
    connect(m_trGeometry, &QTreeWidget::itemChanged, this, &CGeometryViewer::onItemChanged);
    connect(m_trGeometry, &QTreeWidget::itemClicked, this, &CGeometryViewer::onItemClicked);
    connect(m_trGeometry, &QTreeWidget::itemSelectionChanged, this, &CGeometryViewer::onSelectionChanged);
    connect(m_trGeometry, &QWidget::customContextMenuRequested, this, &CGeometryViewer::onMenuRequested);
    m_pViewerWidget->initScene();

    m_tbInfo = new QTableWidget(this);
    m_tbInfo->setColumnCount(2);
    m_tbInfo->setHorizontalHeaderLabels(QStringList() << QStringLiteral("属性") << QStringLiteral("值"));
    m_tbInfo->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_tbInfo->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_tbInfo->horizontalHeader()->setStretchLastSection(true);
    m_tbInfo->horizontalHeader()->setFixedHeight(20);
    m_tbInfo->verticalHeader()->setDefaultSectionSize(20);
    //m_tbInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tbInfo->setTextElideMode(Qt::ElideNone);

    m_splLeft = new QSplitter(Qt::Vertical, this);
    m_splLeft->addWidget(m_trGeometry);
    m_splLeft->addWidget(m_tbInfo);
    m_splLeft->setStretchFactor(0, 5);
    m_splLeft->setStretchFactor(1, 2);

    m_pLeftWidget = new QWidget(this);
    QVBoxLayout *pLeftLayout = new QVBoxLayout(this);
    pLeftLayout->setMargin(0);
    pLeftLayout->setSpacing(0);
    pLeftLayout->addWidget(m_splLeft, 1);
    m_pLeftWidget->setLayout(pLeftLayout);

    m_pRightWidget = new QWidget(this);
    QVBoxLayout *pRightLayout = new QVBoxLayout(this);
    pRightLayout->setMargin(0);
    pRightLayout->setSpacing(0);
    pRightLayout->addWidget(m_pViewerWidget, 2);
    m_pRightWidget->setLayout(pRightLayout);

    m_splMain = new QSplitter(Qt::Horizontal, this);
    m_splMain->addWidget(m_pLeftWidget);
    m_splMain->addWidget(m_pRightWidget);
    m_splMain->setStretchFactor(0, 1);
    m_splMain->setStretchFactor(1, 10);
    m_splMain->setSizes(QList<int>() << 200 << 800);
    setCentralWidget(m_splMain);

    setMouseTracking(true);
    m_splMain->setMouseTracking(true);
    m_pRightWidget->setMouseTracking(true);
    m_pViewerWidget->setMouseTracking(true);

    m_pErrorDlg = new QErrorMessage(this);
    m_pErrorDlg->setWindowTitle(QStringLiteral("提示"));

    connect(m_chkGrid, SIGNAL(clicked(bool)), m_pViewerWidget, SLOT(showAxis(bool)));
    connect(m_chkDir, SIGNAL(clicked(bool)), m_pViewerWidget, SLOT(showDir(bool)));
    connect(m_chkWireFrame, SIGNAL(clicked(bool)), m_pViewerWidget, SLOT(showWireFrame(bool)));
    connect(m_chkListen, &QCheckBox::clicked, this, &CGeometryViewer::onListenClicked);
    connect(m_trGeometry->header(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(deleteAll()));
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      析构函数
* @return     
*/
CGeometryViewer::~CGeometryViewer()
{
    delete m_pViewerWidget;
    delete m_pCmdCenter;
}

bool CGeometryViewer::hitPoint( int nX, int nY, CVector3d& ptHit, int nPickSize )
{
    bool bHit = false;
    CBox2i oBox(CVector2i(nX, nY), nPickSize, nPickSize); 
    vector<CSceneNode*> oNodes;
    m_pViewerWidget->viewer()->PickObject(oBox.MaxPt().X, oBox.MaxPt().Y, oBox.MinPt().X, oBox.MinPt().Y, oNodes);
    vector<CVector3d> oPts;
    for (unsigned i = 0; i < oNodes.size(); ++i)
    {
        CVisualNode *pNode = dynamic_cast<CVisualNode*>(oNodes[i]);
        if (pNode && pNode->Tag().GetPointer())
        {
            CNodeExtend *pExtend = (CNodeExtend*)pNode->Tag().GetPointer();
            oPts.insert(oPts.end(), pExtend->m_oVertexs.begin(), pExtend->m_oVertexs.end());
        }
    }
    CVector3f ptStartF, ptEndF, ptStartF1, ptEndF1, ptDirF;
    CViewer *pViewer = m_pViewerWidget->viewer();
    ptHit = pViewer->ScreenToWorld(nX, nY, 0.0f).Vec3d();
    pViewer->ScreenToRay(nX, nY, ptStartF, ptEndF);
    pViewer->ScreenToRay(nX + 1, nY, ptStartF1, ptEndF1);
    ptDirF = ptEndF - ptStartF;
    ptDirF.Normalize();
    double dSqrDist = nPickSize * ((ptStartF1 - ptStartF).Dot(ptDirF) * ptDirF + ptStartF).DistanceTo(ptStartF1);
    CVector3d ptStart = ptStartF.Vec3d(), ptEnd = ptEndF.Vec3d(), ptDir = ptDirF.Vec3d();
    for (unsigned i = 0; i < oPts.size(); ++i)
    {
        CVector3d ptProject = (oPts[i] - ptStart).Dot(ptDir) * ptDir + ptStart;
        double dCurSqrDist = ptProject.DistanceTo(oPts[i]);
        if (dCurSqrDist < dSqrDist)
        {
            bHit = true;
            ptHit = oPts[i];
        }
    }
    return bHit;
}

/*!
* @author     zhangyc-c  2016.1.5
* @brief      鼠标双击事件
* @param[in]  QMouseEvent * pEvent
* @return     void
*/
void CGeometryViewer::mouseDoubleClickEvent( QMouseEvent *pEvent )
{
    Qt::MouseButton nButton = pEvent->button();
    bool bViewerUnderMouse = m_pViewerWidget->underMouse();
    if ((nButton & Qt::LeftButton) && bViewerUnderMouse)
    {
        CVector3d ptHit;
        QPoint oLocalPt = m_pViewerWidget->mapFromGlobal(pEvent->globalPos());
        int nX = oLocalPt.x(), nY = oLocalPt.y();
        bool bHit = hitPoint(nX, nY, ptHit, 8);
        m_bAutoCamera = false;
        if (bHit)
        {
            CVector3d oCameraDir = m_pViewerWidget->viewer()->ActiveViewport()->GetCamera()->Direction().Vec3d();
            if (ggp::Equals(abs(oCameraDir.Z), 1.0, g_DistEpsilon))
            {
                addTopItem(itVector2d, new CVector2d(ptHit.Vec2()));
            }
            else
            {
                addTopItem(itVector3d, new CVector3d(ptHit));
            }
        }
        else
        {
            CVector3d oCameraDir = m_pViewerWidget->viewer()->ActiveViewport()->GetCamera()->Direction().Vec3d();
            if (ggp::Equals(abs(oCameraDir.Z), 1.0, g_DistEpsilon))
            {
                addTopItem(itVector2d, new CVector2d(ptHit.Vec2()));
            }
        }
        m_bAutoCamera = true;
    }
    if ((nButton & Qt::MiddleButton) && bViewerUnderMouse)
    {
        if (m_pCurItem)
        {
            EnItemType nType = (EnItemType)m_pCurItem->data(0, rtType).toInt();
            void *ptr = (void*)m_pCurItem->data(0, rtData).toInt();
            CBox3d oBox = getItemBox(nType, ptr);
            if (oBox.NotEmpty())
            {
                oBox.Expand(1, 1, 1, false);
                oBox.Expand(0.2, 0.2, 0.2, true);
                m_pViewerWidget->viewer()->ZoomToBox(oBox.Box3f());
            }
        }
        else
        {
            m_pViewerWidget->viewer()->ZoomAll();
        }
    }
}

/*!
* @author     zhangyc-c  2015.11.12
* @brief      处理鼠标移动
* @param[in]  QMouseEvent * pEvent
* @return     void
*/
void CGeometryViewer::mouseMoveEvent( QMouseEvent *pEvent )
{
    bool bHit = false;
    CVector3d ptHit;
    if (!m_bMouseDown && m_pViewerWidget->underMouse())
    {
        QPoint oLocalPt = m_pViewerWidget->mapFromGlobal(pEvent->globalPos());
        int nX = oLocalPt.x(), nY = oLocalPt.y();
        bHit = hitPoint(nX, nY, ptHit, 8);
    }
    m_pCmdCenter->input(dtHitPoint, &ptHit);
    if (bHit)
    {
        m_pViewerWidget->showHitPoint(&ptHit);
        QString sPtInfo = QString::number(ptHit.X, 'f') + ",  " + QString::number(ptHit.Y, 'f') + ",  " + QString::number(ptHit.Z, 'f');
        m_pStatusBar->showMessage(sPtInfo);
    }
    else
    {
        m_pStatusBar->showMessage("");
        m_pViewerWidget->showHitPoint(nullptr);
    }
    return QMainWindow::mouseMoveEvent(pEvent);
}

/*!
* @author     zhangyc-c  2015.12.5
* @brief      处理鼠标按下事件
* @param[in]  QMouseEvent * pEvent
* @return     void
*/
void CGeometryViewer::mousePressEvent( QMouseEvent *pEvent )
{
    m_bMouseDown = true;
    Qt::MouseButton nButton = pEvent->button();
    if (nButton & Qt::LeftButton)
    {
        if (m_pViewerWidget->underMouse())
        {
            bool bHit = false;
            CVector3d ptHit;
            QPoint oLocalPt = m_pViewerWidget->mapFromGlobal(pEvent->globalPos());
            int nX = oLocalPt.x(), nY = oLocalPt.y();
            bHit = hitPoint(nX, nY, ptHit, 8);
            if (bHit)
            {
                QString sPtInfo = QString::number(ptHit.X, 'f') + ",  " + QString::number(ptHit.Y, 'f') + ",  " + QString::number(ptHit.Z, 'f');
                m_pCmdCenter->cmdLine()->cmdBox()->setText(sPtInfo);
                m_pCmdCenter->input(dtVector3d, &ptHit);
            }
        }
    }
    if ((nButton & Qt::RightButton) && m_pViewerWidget->underMouse())
    {
        m_pCmdCenter->finishCmd();
    }
}

/*!
* @author     zhangyc-c  2016.1.4
* @brief      处理鼠标弹起事件
* @param[in]  QMouseEvent * pEvent
* @return     void
*/
void CGeometryViewer::mouseReleaseEvent( QMouseEvent *pEvent )
{
    m_bMouseDown = false;
}

#define REGISTER_EXAMPLE(strName, strHint, classWidget) \
{\
    QListWidgetItem *pItem = new QListWidgetItem(strName);\
    pItem->setToolTip(strHint);\
    m_pExampleList->addItem(pItem);\
    m_oExamples[pItem] = [&]() -> QWidget*\
{\
    return new classWidget(m_pViewerWidget, m_pDiscoveryWidget);\
};\
}\

void CGeometryViewer::showDiscovery()
{
    if (m_pDiscoveryWidget)
    {
        m_pExampleList->setCurrentItem(nullptr);
        return;
    }
    if (m_pViewerWidget->viewer()->GetCamera()->Direction().IsParallel(CVector3f::UnitZ, g_MinDistEpsilon))
    {
        m_pViewerWidget->viewer()->SetViewPoint(VP_FRONT_TOP_RIGHT);
    }
    m_pDiscoveryWidget = new QWidget(this);

    m_pExampleList = new QListWidget(m_pDiscoveryWidget);
    m_pExampleList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pExampleList->setFixedWidth(133);
    connect(m_pExampleList, &QListWidget::currentItemChanged, [&](QListWidgetItem *pCurr, QListWidgetItem *pPrev)
    {
        QHBoxLayout *pLayout = static_cast<QHBoxLayout*>(m_pDiscoveryWidget->layout());
        if (pPrev)
        {
            QWidget *pWidget = (QWidget*)pPrev->data(Qt::UserRole).toInt();
            delete pWidget;
            pPrev->setData(Qt::UserRole, 0);
        }
        if (pCurr)
        {
            QWidget *pWidget = m_oExamples[pCurr]();
            pCurr->setData(Qt::UserRole, (int)pWidget);
            pLayout->insertWidget(1, pWidget, 1);
        }
    });
    QHBoxLayout *pMainLayout = new QHBoxLayout(m_pDiscoveryWidget);
    pMainLayout->setMargin(0);
    pMainLayout->addWidget(m_pExampleList);
    REGISTER_EXAMPLE("CCuboidBody", "Geometry/CuboidBody.h", CCuboidBodyExample);
    REGISTER_EXAMPLE("CExtrudedBody", "Geometry/ExtrudedBody.h", CExtrudedBodyExample);
    REGISTER_EXAMPLE("CPolyhedronBody", "Geometry/PolyhedronBody.h", CPolyhedronBodyExample);
    pMainLayout->addStretch(0);
    m_pDiscoveryWidget->setLayout(pMainLayout);

    QVBoxLayout *pRightLayout = (QVBoxLayout*)m_pRightWidget->layout();
    pRightLayout->insertWidget(0, m_pDiscoveryWidget);
    connect(m_acDiscovery, &QAction::triggered, m_pDiscoveryWidget, &QWidget::setVisible);
}

/*!
* @author     zhangyc-c  2015.12.31
* @brief      初始化选项
* @return     void
*/
void CGeometryViewer::showOption()
{
    if (m_pOptionWidget)
    {
        return;
    }
    m_pOptionWidget = new QWidget(this);
    QVBoxLayout *pMainLayout = new QVBoxLayout(m_pOptionWidget);

    QHBoxLayout *pBoolEpsLayout = new QHBoxLayout(m_pOptionWidget);
    pBoolEpsLayout->setMargin(0);
    pBoolEpsLayout->setSpacing(0);
    pBoolEpsLayout->addWidget(new QLabel(QStringLiteral("布尔运算精度：")));
    m_edtBoolEps = new QLineEdit(QString::number(m_dBoolEps), m_pOptionWidget);
    pBoolEpsLayout->addWidget(m_edtBoolEps);
    pBoolEpsLayout->addStretch(1);
    pMainLayout->addLayout(pBoolEpsLayout);
    connect(m_edtBoolEps, &QLineEdit::editingFinished, [&]()
    {
        bool bOK = false;
        double dDistEps = m_edtBoolEps->text().toDouble(&bOK);
        if (bOK)
        {
            m_dBoolEps = dDistEps;
        }
    });

    QHBoxLayout *pMeshLayout = new QHBoxLayout(m_pOptionWidget);
    pMeshLayout->setMargin(0);
    pMeshLayout->setSpacing(0);
    pMeshLayout->addWidget(new QLabel(QStringLiteral("离散参数：")));
    QComboBox *cbMeshType = new QComboBox(m_pOptionWidget);
    cbMeshType->addItem(QStringLiteral("固定值"));
    cbMeshType->addItem(QStringLiteral("通用值"));
    cbMeshType->addItem(QStringLiteral("最小值"));
    pMeshLayout->addWidget(cbMeshType);
    pMeshLayout->addStretch(1);
    pMainLayout->addLayout(pMeshLayout);
    connect(cbMeshType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int nIndex)
    {
        m_pViewerWidget->m_nMeshType = nIndex;
        m_pEpsWidget->setVisible(!(bool)nIndex);
    });

    m_pEpsWidget = new QWidget(m_pOptionWidget);
    QLabel *lblDistEps = new QLabel(QStringLiteral("距离精度："));
    QLabel *lblAngleEps = new QLabel(QStringLiteral("角度精度："));

    m_edtDistEps = new QLineEdit(m_pOptionWidget);
    m_edtDistEps->setText(QString::number(m_pViewerWidget->m_dDistEps));
    QDoubleValidator *pValidator1 = new QDoubleValidator(m_edtDistEps);
    pValidator1->setRange(0, 1000, 3);
    m_edtDistEps->setValidator(pValidator1);
    connect(m_edtDistEps, &QLineEdit::editingFinished, [&]()
    {
        bool bOK = false;
        double dDistEps = m_edtDistEps->text().toDouble(&bOK);
        if (bOK)
        {
            m_pViewerWidget->m_dDistEps = dDistEps;
        }
    });

    m_edtAngleEps = new QLineEdit(m_pOptionWidget);
    m_edtAngleEps->setText(QString::number(m_pViewerWidget->m_dAngleEps));
    QDoubleValidator *pValidator2 = new QDoubleValidator(m_edtAngleEps);
    pValidator2->setRange(0, 1, 20);
    m_edtAngleEps->setValidator(pValidator2);
    connect(m_edtAngleEps, &QLineEdit::editingFinished, [&]()
    {
        bool bOK = false;
        double dAngleEps = m_edtAngleEps->text().toDouble(&bOK);
        if (bOK)
        {
            m_pViewerWidget->m_dAngleEps = dAngleEps;
        }
    });
    QHBoxLayout *pEpsLayout1 = new QHBoxLayout();
    QHBoxLayout *pEpsLayout2 = new QHBoxLayout();
    pEpsLayout1->setMargin(0);
    pEpsLayout1->setSpacing(0);
    pEpsLayout1->addWidget(lblDistEps);
    pEpsLayout1->addWidget(m_edtDistEps);
    pEpsLayout2->setMargin(0);
    pEpsLayout2->setSpacing(0);
    pEpsLayout2->addWidget(lblAngleEps);
    pEpsLayout2->addWidget(m_edtAngleEps);
    QVBoxLayout *pEpsLayout = new QVBoxLayout(m_pEpsWidget);
    pEpsLayout->setMargin(0);
    pEpsLayout->setSpacing(0);
    pEpsLayout->addLayout(pEpsLayout1);
    pEpsLayout->addLayout(pEpsLayout2);
    m_pEpsWidget->setLayout(pEpsLayout);

    pMainLayout->addWidget(m_pEpsWidget);
    m_pOptionWidget->setLayout(pMainLayout);
    QVBoxLayout *pLeftLayout = (QVBoxLayout*)m_pLeftWidget->layout();
    pLeftLayout->insertWidget(0, m_pOptionWidget);
    connect(m_acOption, &QAction::triggered, m_pOptionWidget, &QWidget::setVisible);
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      初始化状态栏
* @return     void
*/
void CGeometryViewer::initStatusBar()
{
    m_pStatusBar = statusBar();
    m_pStatusBar->addPermanentWidget(m_pCmdCenter->cmdLine());
    m_pStatusBar->showMessage(QStringLiteral("作者：张永楚"));
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      初始化工具栏
* @return     void
*/
void CGeometryViewer::initToolBar()
{
    m_pToolBar = addToolBar("");
    m_pToolBar->setFloatable(false);
    m_pToolBar->setMovable(false);
    m_pToolBar->toggleViewAction()->setEnabled(false);
    QString strStyleSheet("QToolBar{background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255, 255, 255, 255), stop:1 %1);}");
    strStyleSheet = strStyleSheet.arg(c_ToolBarBgColor[rand() % 6]);
    m_pToolBar->setStyleSheet(strStyleSheet);

    m_btnViewXY = new CToolButton(m_pToolBar);
    m_btnViewXY->setIcon(QIcon("image/ViewXY.png"));
    connect(m_btnViewXY, &CToolButton::leftClicked, this, &CGeometryViewer::viewXY);
    connect(m_btnViewXY, &CToolButton::rightClicked, this, &CGeometryViewer::view_XY);
    m_btnViewXY->setToolTip(QStringLiteral("◐俯视\r\n◑仰视"));
    m_pToolBar->addWidget(m_btnViewXY);

    m_btnViewXZ = new CToolButton(m_pToolBar);
    m_btnViewXZ->setIcon(QIcon("image/ViewXZ.png"));
    connect(m_btnViewXZ, &CToolButton::leftClicked, this, &CGeometryViewer::viewXZ);
    connect(m_btnViewXZ, &CToolButton::rightClicked, this, &CGeometryViewer::view_XZ);
    m_btnViewXZ->setToolTip(QStringLiteral("◐前视\r\n◑后视"));
    m_pToolBar->addWidget(m_btnViewXZ);

    m_btnViewYZ = new CToolButton(m_pToolBar);
    m_btnViewYZ->setIcon(QIcon("image/ViewYZ.png"));
    connect(m_btnViewYZ, &CToolButton::leftClicked, this, &CGeometryViewer::viewYZ);
    connect(m_btnViewYZ, &CToolButton::rightClicked, this, &CGeometryViewer::view_YZ);
    m_btnViewYZ->setToolTip(QStringLiteral("◐左视\r\n◑右视"));
    m_pToolBar->addWidget(m_btnViewYZ);

    m_pToolBar->addSeparator();

    m_btnPaste = new CToolButton(m_pToolBar);
    m_btnPaste->setIcon(QIcon("image/Paste.png"));
    m_btnPaste->setToolTip(QStringLiteral("左击：粘贴\r\n右击：粘贴GGDB"));
    m_btnPaste->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
	/*
	void QAbstractButton::clicked(bool checked = false)
	This signal is emitted when the button is activated
	(i.e., pressed down then released while the mouse cursor is inside the button), 
	when the shortcut key is typed, or when click() or animateClick() is called. 
	当按钮被激活、按下快捷键或调用click()或animateClick()时，会发出clicked信号。
	因此，当我们按下快捷键CTRL + Key_V时，会触发paste槽函数。
	*/
    connect(m_btnPaste, &QToolButton::clicked, this, &CGeometryViewer::paste);
    connect(m_btnPaste, &CToolButton::leftClicked, this, &CGeometryViewer::paste);
    connect(m_btnPaste, &CToolButton::rightClicked, this, &CGeometryViewer::pasteFromGGDB);
    m_pToolBar->addWidget(m_btnPaste);

    m_btnViewString = new CToolButton(m_pToolBar);
    m_btnViewString->setIcon(QIcon("image/View.png"));
    m_btnViewString->setToolTip(QStringLiteral("查看"));
    connect(m_btnViewString, &CToolButton::leftClicked, this, &CGeometryViewer::viewItemString);
    m_pToolBar->addWidget(m_btnViewString);

    m_btnDelete = new CToolButton(m_pToolBar);
    m_btnDelete->setIcon(QIcon("image/Delete.png"));
    m_btnDelete->setToolTip(QStringLiteral("左击：删除\r\n右击：全部删除"));
    m_btnDelete->setShortcut(QKeySequence(Qt::Key_Delete));
    connect(m_btnDelete, &QToolButton::clicked, this, &CGeometryViewer::deleteItem);
    connect(m_btnDelete, &CToolButton::leftClicked, this, &CGeometryViewer::deleteItem);
    connect(m_btnDelete, &CToolButton::rightClicked, this, &CGeometryViewer::deleteAll);
    m_pToolBar->addWidget(m_btnDelete);

	m_btnAddJson = new CToolButton(m_pToolBar);
	m_btnAddJson->setIcon(QIcon("image/Json.jpg"));
	m_btnAddJson->setToolTip(QStringLiteral("导入json"));
	connect(m_btnAddJson, &CToolButton::leftClicked, this, &CGeometryViewer::addJson);
	m_pToolBar->addWidget(m_btnAddJson);

    m_btnDrawLine = new CToolButton(m_pToolBar);
    m_btnDrawLine->setIcon(QIcon("image/DrawLine.png"));
    m_btnDrawLine->setToolTip(QStringLiteral("左击：测距\r\n右击：画线"));
    connect(m_btnDrawLine, &CToolButton::leftClicked, this, &CGeometryViewer::measure);
    connect(m_btnDrawLine, &CToolButton::rightClicked, this, &CGeometryViewer::drawLine);
    m_pToolBar->addWidget(m_btnDrawLine);

    m_acOption = new QAction(this);
    m_acOption->setIcon(QIcon("image/Option.png"));
    m_acOption->setToolTip(QStringLiteral("选项"));
    m_acOption->setCheckable(true);
    m_pToolBar->addAction(m_acOption);
    connect(m_acOption, &QAction::triggered, this, &CGeometryViewer::showOption);
    connect(m_acOption, &QAction::triggered, this, &CGeometryViewer::option);

    //m_acDiscovery = new QAction(this);
    //m_acDiscovery->setIcon(QIcon("image/Discovery.png"));
    //m_acDiscovery->setToolTip(QStringLiteral("探索"));
    //m_acDiscovery->setCheckable(true);
    //m_pToolBar->addAction(m_acDiscovery);
    //connect(m_acDiscovery, &QAction::triggered, this, &CGeometryViewer::showDiscovery);

    m_pToolBar->addSeparator();
    const QString c_Style = "QToolButton{background-color:rgb(%1,%2,%3);}";
    QWidget *pColorWidget = new QWidget(this);
    QHBoxLayout *pColorLayout = new QHBoxLayout(pColorWidget);
    for (int i = 0; i < c_ColorCount; ++i)
    {
        m_btnColor[i] = new QToolButton(this);
        m_btnColor[i]->setCheckable(true);
        QString sCurStyle = c_Style.arg(QString::number(c_Color[i].R), QString::number(c_Color[i].G), QString::number(c_Color[i].B));
        m_btnColor[i]->setStyleSheet(sCurStyle);
        pColorLayout->addWidget(m_btnColor[i]);
        connect(m_btnColor[i], SIGNAL(clicked()), this, SLOT(updateColor()));
    }
    pColorLayout->setSpacing(1);
    pColorLayout->setMargin(1);
    pColorWidget->setLayout(pColorLayout);
    m_pToolBar->addWidget(pColorWidget);
    m_btnColor[m_nColorIndex]->setChecked(true);
    m_pToolBar->addSeparator();
    pColorWidget->setCursor(QCursor(Qt::PointingHandCursor));

    m_chkGrid = new QCheckBox(QStringLiteral("网格"), this);
    m_chkGrid->setChecked(true);
    m_pToolBar->addWidget(m_chkGrid);

    m_chkDir = new QCheckBox(QStringLiteral("方向"), this);
    m_chkDir->setChecked(true);
    m_pToolBar->addWidget(m_chkDir);

    m_chkWireFrame = new QCheckBox(QStringLiteral("线框"), this);
    m_pToolBar->addWidget(m_chkWireFrame);

    m_chkListen = new QCheckBox(QStringLiteral("监听"), this);
    m_pToolBar->addWidget(m_chkListen);
}

/*!
* @author     zhangyc-c  2015.7.30
* @brief      扩展
* @return     void
*/
void CGeometryViewer::option()
{
    QPushButton *pButton = new QPushButton(this);
    connect(m_btnPaste,SIGNAL(clicked(bool)), pButton, SLOT(showMenu()));
    delete pButton;
    QTreeWidgetItem *pItem = getSeletedItem();
    if (pItem)
    {
        CGeometry *pGeo = (CGeometry*)pItem->data(0, rtData).toInt(); //当前选中的图元
        int nIndex = m_trGeometry->indexOfTopLevelItem(pItem);
        CGeometry *pPrevGeo = nIndex ? (CGeometry*)m_trGeometry->topLevelItem(nIndex - 1)->data(0, rtData).toInt() : NULL;
        CGeometry *pGeo1 = nIndex + 1 < m_trGeometry->topLevelItemCount() ?
            (CGeometry*)m_trGeometry->topLevelItem(nIndex + 1)->data(0, rtData).toInt() : NULL;
        CGeometry *pGeo2 = nIndex + 2 < m_trGeometry->topLevelItemCount() ?
            (CGeometry*)m_trGeometry->topLevelItem(nIndex + 2)->data(0, rtData).toInt() : NULL;
        CGeometry *pGeo3 = nIndex + 3 < m_trGeometry->topLevelItemCount() ?
            (CGeometry*)m_trGeometry->topLevelItem(nIndex + 3)->data(0, rtData).toInt() : NULL;
        CBody *pBody = dynamic_cast<CBody*>(pGeo);
        CBody *pBody1 = dynamic_cast<CBody*>(pGeo1);
        CBody *pBody2 = dynamic_cast<CBody*>(pGeo2);
        CBody *pBody3 = dynamic_cast<CBody*>(pGeo3);
        CBrepBody *pBrepBody = dynamic_cast<CBrepBody*>(pGeo);
        CBrepBody *pBrepBody1 = dynamic_cast<CBrepBody*>(pGeo1);
        CBrepBody *pBrepBody2 = dynamic_cast<CBrepBody*>(pGeo2);
        CBrepBody *pBrepBody3 = dynamic_cast<CBrepBody*>(pGeo3);
        CSurface *pSurface = dynamic_cast<CSurface*>(pGeo);
        CCurve3d *pCurve3d = dynamic_cast<CCurve3d*>(pGeo);
        CCurve3d *pCurve3d1 = dynamic_cast<CCurve3d*>(pGeo1);
        CCurve3d *pCurve3d2 = dynamic_cast<CCurve3d*>(pGeo2);
        CCurve3d *pCurve3d3 = dynamic_cast<CCurve3d*>(pGeo3);
        CCurve2d *pCurve2d = dynamic_cast<CCurve2d*>(pGeo);
        CCurve2d *pCurve2d1 = dynamic_cast<CCurve2d*>(pGeo1);
        CCoedgeList *pCoedges = dynamic_cast<CCoedgeList*>(pGeo);
        CPolygon *pPolygon = dynamic_cast<CPolygon*>(pGeo);
        CPolygon *pPolygon1 = dynamic_cast<CPolygon*>(pGeo1);
        CLoop *pLoop = dynamic_cast<CLoop*>(pGeo);
        CNurbsCurve2d *pNurbsCurve2d = dynamic_cast<CNurbsCurve2d*>(pGeo);
    }
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      响应对象的数据变化事件，控制几何对象显隐
* @param[in]  QTreeWidgetItem * pItem
* @param[in]  int nCol
* @return     void
*/
void CGeometryViewer::onItemChanged(QTreeWidgetItem *pItem, int nCol)
{
    CGroupNode *pGroupNode = (CGroupNode *)pItem->data(0, rtNode).toInt();
    pItem->setSelected(false);
	bool bVisible = pItem->checkState(0) == Qt::Checked;
    pGroupNode->SetVisible(bVisible);
    m_pViewerWidget->viewer()->Scene()->SetDirty();
}

/*!
* @author     zhangyc-c  2015.12.30
* @brief      点击条目的时候
* @param[in]  QTreeWidgetItem * pItem
* @param[in]  int nCol
* @return     void
*/
void CGeometryViewer::onItemClicked( QTreeWidgetItem *pItem, int nCol )
{
    while(pItem->parent())
    {
        pItem = pItem->parent();
    }
	QString text = QString("$") + QString::number(m_trGeometry->indexOfTopLevelItem(pItem));
    m_pCmdCenter->cmdLine()->cmdBox()->setText(text);
    m_pCmdCenter->input(dtPointer, (void*)pItem->data(0, rtData).toUInt());
}

/*!
* @author           zhangyc-c
* @brief            是否监听剪贴板
* @param[in]        bool bListen
* @return           void
*/
void CGeometryViewer::onListenClicked(bool bListen)
{
    if (bListen)
    {
        connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &CGeometryViewer::paste);
    }
    else
    {
        disconnect(QApplication::clipboard(), &QClipboard::dataChanged, this, &CGeometryViewer::paste);
    }
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      选中对象发生变化时触发，亮显
* @return     void
*/
void CGeometryViewer::onSelectionChanged()
{
    QList<QTreeWidgetItem*> oItems = m_trGeometry->selectedItems();
    //先还原上次选中的根节点的颜色
    if (m_pCurItem && !m_pCurItem->parent())
    {
        CSceneNode *pSceneNode = (CSceneNode*)m_pCurItem->data(0, rtNode).toInt();
        int nColorIndex = m_pCurItem->data(0, rtColor).toInt();
        m_pViewerWidget->setNodeColor(pSceneNode, c_Color[nColorIndex]);
        m_pViewerWidget->setNodeVisible(pSceneNode, m_pCurItem->checkState(0) == Qt::Checked);
    }
    m_pViewerWidget->highLight(itGeometry, nullptr);
    if (!oItems.empty())
    {
        m_pCurItem = oItems[0];
        EnItemType nType = (EnItemType)m_pCurItem->data(0, rtType).toInt();
        void *ptr = (void*)m_pCurItem->data(0, rtData).toInt();
        if (m_tbInfo->height())
        {
            int nColorCount = sizeof(c_nInfoColor) / sizeof(QColor);
            int nColorIndex0 = rand() % nColorCount;
            int nColorIndex1 = (nColorIndex0 + rand() % (nColorCount - 1) + 1) % nColorCount;
            QColor nColor0 = c_nInfoColor[nColorIndex0];
            QColor nColor1 = c_nInfoColor[nColorIndex1];
            m_tbInfo->setRowCount(0);
            QStringList slInfo = getItemProps(nType, ptr);
            m_tbInfo->setRowCount(slInfo.count());
            for (int i = 0; i < slInfo.count(); ++i)
            {
                QStringList slCurInfo = slInfo[i].split(" = ");
                QTableWidgetItem *pItem0 = new QTableWidgetItem(slCurInfo[0]);
                QTableWidgetItem *pItem1 = new QTableWidgetItem(slCurInfo[1]);
                m_tbInfo->setItem(i, 0, pItem0);
                m_tbInfo->setItem(i, 1, pItem1);
                bool bValid = true;
                if (slCurInfo[0].contains("Valid") && slCurInfo[1] == "false" ||
                    slCurInfo[0].contains("DistEpsilon") && slCurInfo[1].toDouble() > g_DistEpsilon)
                {
                    bValid = false;
                }
                pItem0->setBackgroundColor(nColor0);
                pItem1->setBackgroundColor(bValid ? nColor1 : QColor(255, 0, 0));
            }
            m_tbInfo->resizeColumnsToContents();
            m_tbInfo->horizontalHeader()->setStretchLastSection(true);
            m_tbInfo->setVisible(true);
        }
        if (m_pCurItem && m_pCurItem->parent())
        {
            int nColorIndex = m_pCurItem->data(0, rtColor).toInt();
            m_pViewerWidget->highLight(nType, ptr, c_Color[nColorIndex]);
        }
        else
        {
            CSceneNode *pSceneNode = (CSceneNode*)m_pCurItem->data(0, rtNode).toInt();
            m_pViewerWidget->setNodeColor(pSceneNode, CColor::White);
            m_pViewerWidget->setNodeVisible(pSceneNode, true);
        }
    }
    else
    {
        m_pCurItem = nullptr;
        m_pStatusBar->showMessage("");
        m_tbInfo->setRowCount(0);
        m_pViewerWidget->highLight(itGeometry, nullptr);
    }
    m_pViewerWidget->viewer()->Scene()->SetDirty();
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      从剪贴板粘贴几何对象，支持多行多个几何对象一起粘贴
* @return     void
*/
void CGeometryViewer::paste()
{
    QString sGeometry = QApplication::clipboard()->text();
    bool bSuccess = true;
    if (!sGeometry.isEmpty())
    {
        try
        {
            QStringList slGeo = sGeometry.split('\n', QString::SkipEmptyParts);
            for (int i = 0; i < slGeo.count(); ++i)
            {
                slGeo[i].trimmed();
                CGeometry *pGeo = CGeometry::LoadFromStr(slGeo[i].toLatin1().data());
                if (pGeo)
                {
                    addGeometry(pGeo);
                }
                else
                {
                    pGeo = CPolyhedronBody::LoadFromDelphiString(slGeo[i].toLatin1().data());
                    if (pGeo)
                    {
                        CPolyhedronBody *pBody = static_cast<CPolyhedronBody*>(pGeo);
                        if (pBody->VertexCount())
                        {
                            addGeometry(pGeo);
                        }
                        else
                        {
                            pBody->Free();
                            pGeo = nullptr;
                        }
                    }
                    if (!pGeo)
                    {
                        CVector2d oVec2d;
                        CVector3d oVec3d;
                        CBox2d oBox2d;
                        CBox3d oBox3d;
                        CCoordinates3d oCoord;
                        std::string strInput = slGeo[i].toStdString();
                        const char *szInput = strInput.c_str();
                        if (12 == sscanf(szInput, "Origin{X=%lf Y=%lf Z=%lf}, X{X=%lf Y=%lf Z=%lf},Y{X=%lf Y=%lf Z=%lf},Z{X=%lf Y=%lf Z=%lf}",
                            &oCoord.Origin.X, &oCoord.Origin.Y, &oCoord.Origin.Z, &oCoord.X.X, &oCoord.X.Y, &oCoord.X.Z, &oCoord.Y.X, &oCoord.Y.Y, &oCoord.Y.Z, &oCoord.Z.X, &oCoord.Z.Y, &oCoord.Z.Z))
                        {
                            addTopItem(itCoord, new CCoordinates3d(oCoord));
                        }
                        else if (6 == sscanf(szInput, "Box3d=[(%lf %lf %lf),(%lf %lf %lf)]",
                            &oBox3d.MinPt().X, &oBox3d.MinPt().Y, &oBox3d.MinPt().Z, &oBox3d.MaxPt().X, &oBox3d.MaxPt().Y, &oBox3d.MaxPt().Z))
                        {
                            addTopItem(itBox3d, new CBox3d(oBox3d));
                        }
                        else if (4 == sscanf(szInput, "Box2d=[(%lf %lf),(%lf %lf)]",
                            &oBox2d.MinPt().X, &oBox2d.MinPt().Y, &oBox2d.MaxPt().X, &oBox2d.MaxPt().Y))
                        {
                            addTopItem(itBox2d, new CBox2d(oBox2d));
                        }
                        else if (3 == sscanf(szInput, "{X=%lf Y=%lf Z=%lf}",
                            &oVec3d.X, &oVec3d.Y, &oVec3d.Z))
                        {
                            addTopItem(itVector3d, new CVector3d(oVec3d));
                        }
                        else if (3 == sscanf(szInput, "%lf%*[, ]%lf%*[, ]%lf",
                            &oVec3d.X, &oVec3d.Y, &oVec3d.Z))
                        {
                            addTopItem(itVector3d, new CVector3d(oVec3d));
                        }
                        else if (2 == sscanf(szInput, "{X=%lf Y=%lf}", 
                            &oVec2d.X, &oVec2d.Y))
                        {
                            addTopItem(itVector2d, new CVector2d(oVec2d));
                        }
                        else if (2 == sscanf(szInput, "%lf%*[, ]%lf%", 
                            &oVec2d.X, &oVec2d.Y))
                        {
                            addTopItem(itVector2d, new CVector2d(oVec2d));
                        }
                    }
                }
            }
        }
        catch(...)
        {
            bSuccess = false;
        }
    }
    else
    {
        m_pStatusBar->showMessage(QStringLiteral("剪贴板没有几何字符串！"));
    }
    if (!bSuccess)
        m_pStatusBar->showMessage(QStringLiteral("导入几何数据失败！"));
}

/*!
* @author           zhangyc-c
* @brief            从GGDB的剪贴板粘贴几何数据，使用懒加载的方式优化工具的启动效率
* @return           void
*/
void CGeometryViewer::pasteFromGGDB()
{
    if (!m_pGGDBHelper)
    {
        QString strDLL;
#ifdef _DEBUG
        strDLL = "GGDBHelperd.dll";
#else
        strDLL = "GGDBHelper.dll";
#endif
        m_pGGDBHelper = new QLibrary(strDLL, this);
        if (!m_pGGDBHelper->load())
        {
            delete m_pGGDBHelper;
            m_pGGDBHelper = nullptr;
            m_pStatusBar->showMessage(QStringLiteral("GGDBHelper.dll加载失败！"));
            return;
        }
    }
    typedef void (*LoadFunction)(vector<pair<int, void*> >&) ;
    auto loadFromClipDB = (void (*)(vector<pair<int, void*> >&))m_pGGDBHelper->resolve("loadFromClipDB");
    if (loadFromClipDB)
    {
        vector<pair<int, void*> > oItems;
        loadFromClipDB(oItems);
        for (auto it = oItems.begin(); it != oItems.end(); ++it)
        {
            addTopItem((EnItemType)it->first, it->second);
        }
    }
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      切换调色板颜色到下一个
* @return     void
*/
void CGeometryViewer::updateColor()
{
    for (int i = 0; i < c_ColorCount; ++i)
    {
        if (m_btnColor[i]->underMouse())
        {
            m_btnColor[m_nColorIndex]->setChecked(false);
            m_nColorIndex = i;
            m_btnColor[i]->setChecked(true);
            break;
        }
    }
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      查看选中对象的几何字符串
* @return     void
*/
void CGeometryViewer::viewItemString()
{
    QList<QTreeWidgetItem*> oItems = m_trGeometry->selectedItems();
    if (oItems.empty())
        return;

    QTreeWidgetItem *pItem = oItems[0];
    QString strItemString = getItemString(pItem);
    QDialog *pTextDlg = new QDialog(this);
    pTextDlg->setWindowTitle(QStringLiteral("几何字符串"));
    pTextDlg->setWindowFlags(( pTextDlg->windowFlags() | Qt::WindowMinMaxButtonsHint) & ~Qt::WindowContextHelpButtonHint );
    QPlainTextEdit *pTextEdit = new QPlainTextEdit(strItemString);
    QFont font = pTextDlg->font();
    font.setPixelSize(16);
    pTextEdit->setFont(font);
    pTextEdit->setStyleSheet("selection-color: rgb(255, 255, 255);selection-background-color: rgb(51, 153, 255);");
    QVBoxLayout *pLayout = new QVBoxLayout(pTextDlg);
    pLayout->addWidget(pTextEdit);
    pLayout->setMargin(0);
    pTextDlg->setLayout(pLayout);
    pTextDlg->resize(800, 600);
    //pTextDlg->showMaximized();
    pTextDlg->exec();
    delete pTextDlg;
}

/*!
* @author    duzk  2020.5.8
* @brief      读取json文件，批量显示钢筋对象
* @return     void
*/
void CGeometryViewer::addJson()
{

	QStringList sFilesName = QFileDialog::getOpenFileNames(this, QStringLiteral("打开JSON文件"), ".", "Json File(*.json)");
	if ( !sFilesName.isEmpty())
	{
		//读取json文件，获取里面的数据，显示所有的钢筋数据
		int fileCount = sFilesName.count();
		for (int index = 0; index < fileCount; ++index)
		{
			QString FileName = sFilesName.at(index);
			QFile jsonFile(FileName);
			if (!jsonFile.open(QIODevice::ReadOnly))//防护：文件是否正确打开
			{
				QMessageBox::information(this, "Waring", QStringLiteral("打开文件错误！"));
				return;
			}
			QByteArray jsonData = jsonFile.readAll();
			jsonFile.close();
			QJsonParseError errorInfo;
			QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonData, &errorInfo));
			if (errorInfo.error != QJsonParseError::NoError)//防护：文件格式或内容
			{
				QMessageBox::information(this, "Waring", QStringLiteral("文件为空 或 文件内容格式有误！"));
				return;	
			}

			//正式处理数据
			QString sGeometry;//钢筋数据字符串，每条钢筋一行，多条钢筋时多行布局
			QJsonObject qObject = jsonDoc.object();//总的钢筋数据对象
			if (!qObject.isEmpty())
			{
				QString sEdoID = QString("%1").arg(qObject["edo_id"].toInt());
				m_sName = sEdoID;//显示用

				//遍历所有的键值对
				QStringList sKeys = qObject.keys();
				for (int i = 0; i < sKeys.size(); ++i)
				{
					//钢筋数据
					if (sKeys.at(i).contains("bars"))
					{
						QString sBar = sKeys.at(i);
						QJsonObject qObject_bar = qObject[sBar].toObject();
						QStringList sKeys_bar = qObject_bar.keys();
						for (int j = 0; j < sKeys_bar.size(); ++j)
						{
							//基本钢筋线
							if (sKeys_bar.at(j).compare("bars", Qt::CaseSensitive) == 0)
							{
								QString sBar_1 = sKeys_bar.at(j);
								QJsonArray arrayBars = qObject_bar[sBar_1].toArray();//存放在bars的Array
								for (int k = 0; k < arrayBars.size(); ++k)
								{
									if (arrayBars.at(k).isObject())
									{
										QJsonObject objectLines = arrayBars.at(k).toObject();
										QJsonArray arrayLines = objectLines["lines"].toArray();
										//遍历钢筋线对象
										for (int m = 0; m < arrayLines.size(); ++m)
										{
											QJsonObject objectLine = arrayLines.at(m).toObject();
											//一根锚固钢筋线可能包含多条线（净长线，起点线，终点线）
											QStringList sLines = objectLine.keys();
											for (int index = 0; index < sLines.size(); ++index)
											{
												if (sLines.at(index).contains("_line"))
												{
													QString _line = sLines.at(index);
													QString sLine = objectLine[_line].toString();
													sGeometry += (sLine + "\n");
												}
											}
											
										}
									}
								}
							}
							//基础线数据
							if (sKeys_bar.at(j).contains("base_line"))
							{
								QString sBase_Line = qObject_bar["base_line"].toString();
								sGeometry += (sBase_Line + "\n");
							}
						}
					}

					//主、客体的poly数据
					if (sKeys.at(i).contains("_poly"))
					{
						QString sPoly = qObject[sKeys.at(i)].toString();
						sGeometry += (sPoly + "\n");
					}
					// 图元体数据
					if (sKeys.at(i).contains("_body"))
					{
						QJsonObject qObject_Body = qObject[sKeys.at(i)].toObject();
						QStringList sBodyKeys = qObject_Body.keys();

						for (int j = 0; j < sBodyKeys.size(); ++j)
						{
							QString sBodyKey = sBodyKeys.at(j);
							if (sBodyKey.contains("_body"))
							{
								QString sBody = qObject_Body[sBodyKey].toString();
								sGeometry += (sBody + "\n");
							}
						}
					}
				}
			}

			//对每个json文件的几何字符串显示
			bool bSuccess = true;
			if (!sGeometry.isEmpty())
			{
				try
				{
					QStringList slGeo = sGeometry.split('\n', QString::SkipEmptyParts);
					for (int i = 0; i < slGeo.count(); ++i)
					{
						slGeo[i].trimmed();
						CGeometry *pGeo = CGeometry::LoadFromStr(slGeo[i].toLatin1().data());
						m_isGGJ = true;
						if (pGeo)
						{
							addGeometry(pGeo);
						}
						else
						{
							pGeo = CPolyhedronBody::LoadFromDelphiString(slGeo[i].toLatin1().data());
							if (pGeo)
							{
								CPolyhedronBody *pBody = static_cast<CPolyhedronBody*>(pGeo);
								if (pBody->VertexCount())
								{
									addGeometry(pGeo);
								}
								else
								{
									pBody->Free();
									pGeo = nullptr;
								}
							}
							if (!pGeo)
							{
								CVector2d oVec2d;
								CVector3d oVec3d;
								CBox2d oBox2d;
								CBox3d oBox3d;
								CCoordinates3d oCoord;
								std::string strInput = slGeo[i].toStdString();
								const char *szInput = strInput.c_str();
								if (12 == sscanf(szInput, "Origin{X=%lf Y=%lf Z=%lf}, X{X=%lf Y=%lf Z=%lf},Y{X=%lf Y=%lf Z=%lf},Z{X=%lf Y=%lf Z=%lf}",
									&oCoord.Origin.X, &oCoord.Origin.Y, &oCoord.Origin.Z, &oCoord.X.X, &oCoord.X.Y, &oCoord.X.Z, &oCoord.Y.X, &oCoord.Y.Y, &oCoord.Y.Z, &oCoord.Z.X, &oCoord.Z.Y, &oCoord.Z.Z))
								{
									addTopItem(itCoord, new CCoordinates3d(oCoord));
								}
								else if (6 == sscanf(szInput, "Box3d=[(%lf %lf %lf),(%lf %lf %lf)]",
									&oBox3d.MinPt().X, &oBox3d.MinPt().Y, &oBox3d.MinPt().Z, &oBox3d.MaxPt().X, &oBox3d.MaxPt().Y, &oBox3d.MaxPt().Z))
								{
									addTopItem(itBox3d, new CBox3d(oBox3d));
								}
								else if (4 == sscanf(szInput, "Box2d=[(%lf %lf),(%lf %lf)]",
									&oBox2d.MinPt().X, &oBox2d.MinPt().Y, &oBox2d.MaxPt().X, &oBox2d.MaxPt().Y))
								{
									addTopItem(itBox2d, new CBox2d(oBox2d));
								}
								else if (3 == sscanf(szInput, "{X=%lf Y=%lf Z=%lf}",
									&oVec3d.X, &oVec3d.Y, &oVec3d.Z))
								{
									addTopItem(itVector3d, new CVector3d(oVec3d));
								}
								else if (3 == sscanf(szInput, "%lf%*[, ]%lf%*[, ]%lf",
									&oVec3d.X, &oVec3d.Y, &oVec3d.Z))
								{
									addTopItem(itVector3d, new CVector3d(oVec3d));
								}
								else if (2 == sscanf(szInput, "{X=%lf Y=%lf}",
									&oVec2d.X, &oVec2d.Y))
								{
									addTopItem(itVector2d, new CVector2d(oVec2d));
								}
								else if (2 == sscanf(szInput, "%lf%*[, ]%lf%",
									&oVec2d.X, &oVec2d.Y))
								{
									addTopItem(itVector2d, new CVector2d(oVec2d));
								}
							}
						}
					}
				}
				catch (...)
				{
					bSuccess = false;
				}
			}
			else
			{
				m_pStatusBar->showMessage(QStringLiteral("json文件里没有几何字符串！"));
			}
			if (!bSuccess)
				m_pStatusBar->showMessage(QStringLiteral("导入几何数据失败！"));
		}

	}
	else
	{
		QMessageBox::information(this, "Waring", QStringLiteral("打开文件失败！"));
		return;
	}
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      删除选中的几何对象（连根拔起），删除的总是最顶层节点
* @return     void
*/
void CGeometryViewer::deleteItem()
{
    m_pCurItem = nullptr;
    m_pCmdCenter->cancelCmd();
    QTreeWidgetItem *pItem = getSeletedItem();
    if (pItem)
    {
        EnItemType nType = (EnItemType)pItem->data(0, rtType).toInt();
        CVisualNode *pNode = (CVisualNode*)pItem->data(0, rtNode).toInt();
        if (pNode->Tag().GetPointer())
        {
            CNodeExtend *pExtend = (CNodeExtend*)pNode->Tag().GetPointer();
            delete pExtend;
        }
        m_pViewerWidget->viewer()->Scene()->GetRootNode()->RemoveChild(pNode);
        int nIndex = m_trGeometry->indexOfTopLevelItem(pItem);
        delete m_trGeometry->takeTopLevelItem(nIndex);
        m_pViewerWidget->viewer()->Scene()->SetDirty();
    }
}

void CGeometryViewer::drawLine()
{
    m_pCmdCenter->onExecuteCmd("DrawLine");
}

void CGeometryViewer::measure()
{
    m_pCmdCenter->onExecuteCmd("Measure");
}

/*!
* @author     zhangyc-c  2015.9.22
* @brief      删除所有
* @return     void
*/
void CGeometryViewer::deleteAll()
{
    m_pCurItem = nullptr;
    m_pCmdCenter->cancelCmd();
    for (int i = m_trGeometry->topLevelItemCount() - 1; i >= 0; --i)
    {
        QTreeWidgetItem *pItem = m_trGeometry->takeTopLevelItem(i);
        CVisualNode *pNode = (CVisualNode*)pItem->data(0, rtNode).toInt();
        if (pNode->Tag().GetPointer())
        {
            CNodeExtend *pExtend = (CNodeExtend*)pNode->Tag().GetPointer();
            delete pExtend;
        }
        m_pViewerWidget->viewer()->Scene()->GetRootNode()->RemoveChild(pNode);
    }
    m_pViewerWidget->viewer()->Scene()->SetDirty();
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      往几何列表中添加一个几何对象
* @param[in]  void * ptr
* @param[in]  EnItemType nType
* @return     bool
*/
bool CGeometryViewer::addTopItem( EnItemType nType, void *ptr )
{
    if (!ptr)
    {
        return false;
    }
    m_pViewerWidget->m_nColor = c_Color[m_nColorIndex];

    CGroupNode *pGroupNode = m_pViewerWidget->showItem(nType, ptr);
    if (!pGroupNode)
        return false;
    disconnect(m_trGeometry, &QTreeWidget::itemChanged, this, &CGeometryViewer::onItemChanged);
    QTreeWidgetItem *pItem = new QTreeWidgetItem();
	if (m_isGGJ)
	{
		QString sName = getItemName(nType, ptr);
		sName += (" | "+m_sName);
		pItem->setText(0, sName);
	}
	else
		pItem->setText(0, getItemName(nType, ptr));
    pItem->setData(0, rtType, nType);
    pItem->setData(0, rtData, (int)ptr);
    pItem->setData(0, rtNode, (int)pGroupNode);
    pItem->setData(0, rtColor, (int)m_nColorIndex);
    pItem->setData(0, rtNew, true);
    pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsEditable);
    pItem->setCheckState(0, Qt::Checked);
    m_trGeometry->addTopLevelItem(pItem);
    addChildItem(pItem);                            //递归构建

    m_btnColor[m_nColorIndex]->setChecked(false);
    m_nColorIndex += 2;
    if (m_nColorIndex >= c_ColorCount)
    {
        m_nColorIndex = (m_nColorIndex + 1) % c_ColorCount;
    }
    m_btnColor[m_nColorIndex]->setChecked(true);
    if (m_bAutoCamera)
    {
        CBox3d oBox = getItemBox(nType, ptr);
        if (oBox.NotEmpty())
        {
            CVector3d oSize = oBox.GetSize();
            if (abs(oSize.Z) < g_MaxDistEpsilon)
            {
                bool bOldAnimateView = m_bAnimateView;
                m_bAnimateView = false;
                viewXY();
                m_bAnimateView = bOldAnimateView;
            }
            oBox.Expand(1, 1, 1, false);
            if (oSize.Length() > 1e9)
            {
                m_pErrorDlg->showMessage(QStringLiteral("包围盒存在异常！（对角线长度大于1e9）"));
            }
            else
            {
                oBox.Expand(0.2, 0.2, 0.2, true);
                m_pViewerWidget->viewer()->ZoomToBox(oBox.Box3f());    //这个方法有BUG
            }
        }
        else
        {
            m_pViewerWidget->viewer()->ZoomAll();
        }
    }
    m_pViewerWidget->viewer()->Scene()->SetDirty();
    connect(m_trGeometry, &QTreeWidget::itemChanged, this, &CGeometryViewer::onItemChanged);
    return true;
}

/*!
* @author           zhangyc-c
* @brief            视角切换动画
* @return           void
*/
void CGeometryViewer::viewAnimation()
{
    if (m_nAnimateStep < c_nCameraAnimateSteps)
    {
        CCamera *pCamera = m_pViewerWidget->viewer()->GetCamera();
        double dYawAngle = pCamera->GetYawAngle();
        double dPitchAngle = pCamera->GetPitchAngle();
        int nDelta = c_nCameraAnimateSteps - m_nAnimateStep;
        double dDeltaYaw = (c_dYawAngle[m_nAnimateDest] - dYawAngle) / nDelta;
        double dDeltaPitch = (c_dPitchAngle[m_nAnimateDest] - dPitchAngle) / nDelta;
        if (!ggp::isZero(dDeltaYaw, g_AngleEpsilon)  || !ggp::isZero(dDeltaPitch, g_AngleEpsilon))
        {
            CBox3f oBox = m_pViewerWidget->viewer()->Scene()->GetRootNode()->GetBoundingBox();
            CVector3f oCenterPt = m_pViewerWidget->viewer()->ScreenToWorld(m_pViewerWidget->width() >> 1, m_pViewerWidget->height() >> 1);
            pCamera->PivotAroundPoint(dDeltaYaw, dDeltaPitch, oBox.CenterPt());
        }
        ++m_nAnimateStep;
    }
}

/*!
* @author           zhangyc-c
* @brief            以动画的方式切换视角
* @param[in]        EnViewPoint nViewPoint
* @return           void
*/
void CGeometryViewer::setViewPoint(EnViewPoint nViewPoint)
{
    if (m_bAnimateView)
    {
        CCamera *pCamera = m_pViewerWidget->viewer()->GetCamera();
        double dYawAngle = pCamera->GetYawAngle();
        double dPitchAngle = pCamera->GetPitchAngle();
        if (ggp::Equals(dYawAngle, c_dYawAngle[nViewPoint], g_AngleEpsilon) && 
            ggp::Equals(dPitchAngle, c_dPitchAngle[nViewPoint], g_AngleEpsilon))
        {
            m_pViewerWidget->viewer()->SetViewPoint(nViewPoint);
        }
        else
        {
            m_nAnimateDest = nViewPoint;
            m_nAnimateStep = 0;
            for (int i = 0; i < c_nCameraAnimateSteps; ++i)
            {
                QTimer::singleShot(i * c_nCameraAnimateTime, this, SLOT(viewAnimation()));
            }
        }
    }
    else
    {
        m_pViewerWidget->viewer()->SetViewPoint(nViewPoint);
    }
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      以XOY方式查看，俯视图
* @return     void
*/
void CGeometryViewer::viewXY()
{
    setViewPoint(VP_TOP);
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      以XOZ方式查看
* @return     void
*/
void CGeometryViewer::viewXZ()
{
    setViewPoint(VP_FRONT);
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      以YOZ方式查看
* @return     void
*/
void CGeometryViewer::viewYZ()
{
    setViewPoint(VP_LEFT);
}

void CGeometryViewer::view_XY()
{
    setViewPoint(VP_BOTTOM);
}

void CGeometryViewer::view_XZ()
{
    setViewPoint(VP_BACK);
}

void CGeometryViewer::view_YZ()
{
    setViewPoint(VP_RIGHT);
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      递归添加几何对象，以树状形式在几何列表中呈现，分类讨论
* @param[in]  QTreeWidgetItem * pParentItem
* @return     void
*/
void CGeometryViewer::addChildItem( QTreeWidgetItem *pParentItem )
{
    EnItemType nType = (EnItemType)pParentItem->data(0, rtType).toInt();
    void *pData = (void*)pParentItem->data(0, rtData).toInt();
    auto addChildData = [&](EnItemType nType, const void *pChildData, bool bNew, int nIndex) -> QTreeWidgetItem*
    {
        QTreeWidgetItem *pItem = new QTreeWidgetItem(pParentItem);
        QString sText = getItemName(nType, pChildData);
        if (nType == itGeometry && pData == pChildData)
        {
            sText = "CPolygon";
        }
        if (nIndex >= 0)
            sText += QString::number(nIndex);
        pItem->setText(0, sText);
        pItem->setData(0, rtType, (int)nType);
        pItem->setData(0, rtData, (int)pChildData);
        pItem->setData(0, rtNew, (bool)bNew);
        pItem->setFlags((pItem->flags() | Qt::ItemIsEditable) & ~Qt::ItemIsDragEnabled & ~Qt::ItemIsDropEnabled);
        addChildItem(pItem);
        return pItem;
    };
    if (nType == itVector2d || nType == itVector3d)
    {
        return;
    }
    if (nType == itBox2d)
    {
        CBox2d *pBox2d = (CBox2d*)pData;
        addChildData(itVector2d, &pBox2d->MinPt(), false, 0)->setText(0, c_MinPt);
        addChildData(itVector2d, &pBox2d->MaxPt(), false, 1)->setText(0, c_MaxPt);
        return;
    }
    if (nType == itBox3d)
    {
        CBox3d *pBox3d = (CBox3d*)pData;
        addChildData(itVector3d, &pBox3d->MinPt(), false, 0)->setText(0, c_MinPt);
        addChildData(itVector3d, &pBox3d->MaxPt(), false, 1)->setText(0, c_MaxPt);
        return;
    }
    if (nType == itCoord)
    {
        CCoordinates3d *pCoord = (CCoordinates3d*)pData;
        addChildData(itVector3d, &pCoord->Origin, false, -1);
        return;
    }
    if (nType != itGeometry)
    {
        return;
    }
    CGeometry *pGeo = (CGeometry*)pData;
    if (CBody *pBody = dynamic_cast<CBody*>(pGeo))
    {
        addChildData(itBox3d, new CBox3d(pBody->Box()), true, -1);
        CBrepBody *pBrepBody = dynamic_cast<CBrepBody*>(pGeo);
        if (pBrepBody)
        {
            for (int i = 0; i < pBrepBody->FaceCount(); ++i)
            {
                addChildData(itGeometry, pBrepBody->GetFace(i), false, i);
            }
            for (int i = 0; i < pBrepBody->EdgeCount(); ++i)
            {
                addChildData(itGeometry, pBrepBody->GetEdge(i), false, i);
            }
            return;
        }
        CPolyhedronBody *pPolyhedronBody = dynamic_cast<CPolyhedronBody*>(pGeo);
        if (pPolyhedronBody)
        {
            for (int i = 0; i < pPolyhedronBody->FaceCount(); ++i)
            {
                addChildData(itGeometry, pPolyhedronBody->GetFace(i), false, i);
            }
        }
        CExtrudedBody *pExtrudeBody = dynamic_cast<CExtrudedBody*>(pGeo);
        if (pExtrudeBody)
        {
            addChildData(itCoord, &pExtrudeBody->GetCoordinate(), false, -1);
            addChildData(itGeometry, pExtrudeBody->Section(), false, -1);
            return;
        }
        CCuboidBody *pCuboidBody = dynamic_cast<CCuboidBody*>(pGeo);
        if (pCuboidBody)
        {
            addChildData(itCoord, &pCuboidBody->GetCoordinate(), true, -1);
            return;
        }
        return;
    }
    CFace *pFace = dynamic_cast<CFace*>(pGeo);
    if (pFace)
    {
        addChildData(itGeometry, pFace->Surface(), false, -1);
        for (int i = 0; i < pFace->LoopCount(); ++i)
        {
            addChildData(itGeometry, pFace->GetLoop(i), false, i);
        }
        return;
    }
    CEdge *pEdge = dynamic_cast<CEdge*>(pGeo);
    if (pEdge)
    {
        addChildData(itGeometry, pEdge->Curve(), false, -1);
        return;
    }
    CPolyhedralFace *pPolyhedralFace = dynamic_cast<CPolyhedralFace*>(pGeo);
    if (pPolyhedralFace)
    {
        for (int i = 0; i < pPolyhedralFace->LoopCount(); ++i)
        {
            addChildData(itGeometry, pPolyhedralFace->GetLoop(i), false, i);
        }
        return;
    }
    CPolyhedralLoop *pPolyhedralLoop = dynamic_cast<CPolyhedralLoop*>(pGeo);
    if (pPolyhedralLoop)
    {
        for (int i = 0; i < pPolyhedralLoop->EdgeCount(); ++i)
        {
            addChildData(itGeometry, pPolyhedralLoop->GetEdge(i), false, i);
        }
    }
    CPolyhedralEdge *pPolyhedralEdge = dynamic_cast<CPolyhedralEdge*>(pGeo);
    if (pPolyhedralEdge)
    {
        addChildData(itGeometry, pPolyhedralEdge->StartVertex(), false, 0);
        addChildData(itGeometry, pPolyhedralEdge->EndVertex(), false, 1);
    }
    CPolygon *pPolygon = dynamic_cast<CPolygon*>(pGeo);
    if (pPolygon)
    {
        for (int i = 0; i < pPolygon->LoopCount(); ++i)
        {
            addChildData(itGeometry, pPolygon->GetLoop(i), false, i);
        }
        return;
    }
    CCoedgeList *pCoedges = dynamic_cast<CCoedgeList*>(pGeo);
    if (pCoedges)
    {
        for (int i = 0; i < pCoedges->CoedgeCount(); ++i)
        {
            addChildData(itGeometry, pCoedges->GetCoedge(i), false, i);
        }
        return;
    }
    CCoedge *pCoedge = dynamic_cast<CCoedge*>(pGeo);
    if (pCoedge)
    {
        addChildData(itGeometry, pCoedge->Curve(), false, -1);
        return;
    }
    if (dynamic_cast<CSurface*>(pGeo))
    {
        CCylinder *pCylinder = dynamic_cast<CCylinder*>(pGeo);
        if (pCylinder)
        {
            CCoordinates3d *pCoord = new CCoordinates3d(pCylinder->Pos(), pCylinder->DirX(), pCylinder->DirY(), pCylinder->DirZ());
            addChildData(itCoord, pCoord, true, -1);
            addChildData(itGeometry, pCylinder->Curve2d(), false, -1);
            return;
        }
        CBevel *pBevel = dynamic_cast<CBevel*>(pGeo);
        if (pBevel)
        {
            addChildData(itCoord, new CCoordinates3d(pBevel->Coord()), true, -1);
            addChildData(itGeometry, pBevel->Curve2d(), false, -1);
            return;
        }
        CHelicoid *pHelicoid = dynamic_cast<CHelicoid*>(pGeo);
        if (pHelicoid)
        {
            addChildData(itCoord, new CCoordinates3d(pHelicoid->Coord()), true, -1);
            addChildData(itGeometry, pHelicoid->Curve2d(), false, -1);
            return;
        }
        CRuledSurface *pRuledSurface = dynamic_cast<CRuledSurface*>(pGeo);
        if (pRuledSurface)
        {
            addChildData(itGeometry, pRuledSurface->Directrix1(), false, 1);
            addChildData(itGeometry, pRuledSurface->Directrix2(), false, 2);
            return;
        }
        CSweep *pSweep = dynamic_cast<CSweep*>(pGeo);
        if (pSweep)
        {
            addChildData(itGeometry, pSweep->Profile(), false, -1);
            addChildData(itGeometry, pSweep->Spine(), false, -1);
            return;
        }
        CTorus *pTorus = dynamic_cast<CTorus*>(pGeo);
        if (pTorus)
        {
            addChildData(itCoord, new CCoordinates3d(pTorus->Coord()), false, -1);
            addChildData(itGeometry, pTorus->Curve2d(), false, -1);
            return;
        }
        COffsetSurface *pOffsetSurface = dynamic_cast<COffsetSurface*>(pGeo);
        if (pOffsetSurface)
        {
            addChildData(itGeometry, pOffsetSurface->InitialSurface(), false, -1);
            return;
        }
        return;
    }
    if (dynamic_cast<CCurve3d*>(pGeo))
    {
        CPlaneCurve3d *pPlaneCurve = dynamic_cast<CPlaneCurve3d*>(pGeo);
        if (pPlaneCurve)
        {
            addChildData(itCoord, new CCoordinates3d(pPlaneCurve->GetCoordinate()), true, -1);
            addChildData(itGeometry, pPlaneCurve->Curve2d(), false, -1);
            return;
        }
        CSpiralLine *pSpiralLine = dynamic_cast<CSpiralLine*>(pGeo);
        if (pSpiralLine)
        {
            CCoordinates3d *pCoord = new CCoordinates3d(pSpiralLine->Coord());
            addChildData(itCoord, pCoord, true, -1);
        }
        CSweepCurve3d *pSweepCurve = dynamic_cast<CSweepCurve3d*>(pGeo);
        if (pSweepCurve)
        {
            addChildData(itGeometry, pSweepCurve->Spine(), false, -1);
            CVector2d *pVec2d = new CVector2d(pSweepCurve->Profile());
            addChildData(itVector2d, pVec2d, true, -1);
            return;
        }
        CImageCurve3d *pImageCurve = dynamic_cast<CImageCurve3d*>(pGeo);
        if (pImageCurve)
        {
            addChildData(itGeometry, pImageCurve->Curve(), false, -1);
            addChildData(itGeometry, pImageCurve->Surface(), false, -1);
            return;
        }
        CIntersectionCurve3d *pIntersectionCurve = dynamic_cast<CIntersectionCurve3d*>(pGeo);
        if (pIntersectionCurve)
        {
            CSurfacePtr pSurfaces[2];
            pIntersectionCurve->GetBaseSurfaces(pSurfaces);
            addChildData(itGeometry, pSurfaces[0].get(), false, 0);
            addChildData(itGeometry, pSurfaces[1].get(), false, 1);
            return;
        }
        return;
    }
    if (dynamic_cast<CCurve2d*>(pGeo))
    {
        CPreimageCurve2d *pPreimageCurve = dynamic_cast<CPreimageCurve2d*>(pGeo);
        if (pPreimageCurve)
        {
            addChildData(itGeometry, pPreimageCurve->GetBaseCurve().get(), false, -1);
            addChildData(itGeometry, pPreimageCurve->GetBaseSurface().get(), false, -1);
            return;
        }
        COffsetCurve2d *pOffsetCurve = dynamic_cast<COffsetCurve2d*>(pGeo);
        if (pOffsetCurve)
        {
            addChildData(itGeometry, pOffsetCurve->InitialCurve(), false, -1);
            return;
        }
        return;
    }
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      获取几何对象的几何信息，分类讨论
* @param[in]  CGeometry * pGeo
* @return     QT_NAMESPACE::QStringList    字符串列表形式返回，每个字符串包含一个“=”，左侧是属性名，右侧是属性值
*/
QStringList CGeometryViewer::getGeoInfo( CGeometry *pGeo )
{
    QStringList slInfo;
    //往结果集中添加一条记录
    auto pushInfo = [&](QString sName, QVariant val)
    {
        slInfo.push_back(sName + " = " + val.toString());
    };
    //解析顺逆时针信息
    auto getClockSignStr = [](int nClockSign) -> QString
    {
        return nClockSign == csClockwise ? "csClockwise" : (nClockSign == csAnticlockwise ? "csAnticlockwise" : "csUnknown");
    };
    //解析二维点信息
    auto getVector2dStr = [](const CVector2d& vec) -> QString
    {
        return QString("(%1, %2)").arg(QString::number(vec.X), QString::number(vec.Y));
    };
    //解析三维点信息
    auto getVector3dStr = [](const CVector3d& vec) -> QString
    {
        return QString("(%1, %2, %3)").arg(QString::number(vec.X), QString::number(vec.Y), QString::number(vec.Z));
    };
    //解析参数域信息
    auto getRangeStr = [](const CIntervald& range) -> QString
    {
        return QString("[%1, %2]").arg(QString::number(range.Min), QString::number(range.Max));
    };
    //往结果集中增加坐标系信息
    auto pushCoord3d = [&](const CCoordinates3d& coord)
    {
        pushInfo(c_Pos, getVector3dStr(coord.Origin));
        pushInfo(c_DirX, getVector3dStr(coord.X));
        pushInfo(c_DirY, getVector3dStr(coord.Y));
        pushInfo(c_DirZ, getVector3dStr(coord.Z));
    };
    //往结果集中增加包围盒信息
    auto pushBox3d = [&](const CBox3d& box)
    {
        pushInfo("Box", getVector3dStr(box.MinPt()) + "-" + getVector3dStr(box.MaxPt()));
    };
    //下面就是分类处理了，多而杂，不得已啊
    CBox3d box;
    pGeo->BoundingBox(box, false);
    pushBox3d(box);
    CTopology *pTopology = dynamic_cast<CTopology*>(pGeo);
    if (pTopology)
    {
        pTopology->ResetAllStates();
        CPolygon *pPolygon = dynamic_cast<CPolygon*>(pTopology);
        if (pPolygon)
        {
            pPolygon->CalLoopDistEpsilon();
        }
        ostringstream oStrStream;
        bool bValid = pTopology->IsValid(ValidType(0x001f), &oStrStream);
        bool bStrictlyValid = bValid && pTopology->IsStrictlyValid(&oStrStream);
        QString sValidInfo = QString(bStrictlyValid ? "true" : "false");
        if (!bStrictlyValid)
        {
            sValidInfo += "(" + QString(oStrStream.str().c_str()).trimmed() + ")";
        }
        pushInfo(bValid ? c_IsStrictlyValid : c_IsValid, sValidInfo);
    }
    CBody *pBody = dynamic_cast<CBody*>(pGeo);
    if (pBody)
    {
        pushInfo(c_IsClose, pBody->IsClosed());
        pushInfo(c_SurfaceArea, pBody->SurfaceArea());
        pushInfo(c_Volume, pBody->Volume());
        CBrepBody *pBrepBody = dynamic_cast<CBrepBody*>(pGeo);
        if (pBrepBody)
        {
            pushInfo(c_IsShell, pBrepBody->IsShell());
            pushInfo(c_DistEpsilon, pBrepBody->DistEpsilon());
            return slInfo;
        }
        CPolyhedronBody *pPolyhedronBody = dynamic_cast<CPolyhedronBody*>(pGeo);
        if (pPolyhedronBody)
        {
            pushInfo(c_VertexCount, pPolyhedronBody->VertexCount());
            pushInfo(c_DistEpsilon, pPolyhedronBody->DistEpsilon());
        }
        CExtrudedBody *pExtrudeBody = dynamic_cast<CExtrudedBody*>(pGeo);
        if (pExtrudeBody)
        {
            pushInfo(c_Length, pExtrudeBody->GetLength());
            pushInfo(c_DistEpsilon, pExtrudeBody->Section() ? pExtrudeBody->Section()->DistEpsilon() : 0.0);
            return slInfo;
        }
        CCuboidBody *pCuboidBody = dynamic_cast<CCuboidBody*>(pGeo);
        if (pCuboidBody)
        {
            CVector3d dimension = pCuboidBody->GetDimension();
            pushInfo(QStringLiteral("长"), dimension.X);
            pushInfo(QStringLiteral("宽"), dimension.Y);
            pushInfo(QStringLiteral("高"), dimension.Z);
            return slInfo;
        }
        return slInfo;
    }
    CFace *pFace = dynamic_cast<CFace*>(pGeo);
    if (pFace)
    {
        pushInfo(c_SameDir, pFace->SameDir());
        pushInfo(c_SurfaceArea, pFace->SurfaceArea());
        pushInfo(c_DistEpsilon, pFace->DistEpsilon());
        return slInfo;
    }
    CEdge *pEdge = dynamic_cast<CEdge*>(pGeo);
    if (pEdge)
    {
        pushInfo("FaceIndex", QString::number(pEdge->FaceIndex(0)) + ", " + QString::number(pEdge->FaceIndex(1)));
        pushInfo("LoopIndex", QString::number(pEdge->LoopIndex(0)) + ", " + QString::number(pEdge->LoopIndex(1)));
        pushInfo("CoedgeIndex", QString::number(pEdge->CoedgeIndex(0)) + ", " + QString::number(pEdge->CoedgeIndex(1)));
        return slInfo;
    }
    CPolyhedralFace *pPolyhedralFace = dynamic_cast<CPolyhedralFace*>(pGeo);
    if (pPolyhedralFace)
    {
        CPlaneCoef & oPlaneCoef = pPolyhedralFace->GetPlaneCoef();
        pushInfo(c_A, oPlaneCoef.A);
        pushInfo(c_B, oPlaneCoef.B);
        pushInfo(c_C, oPlaneCoef.C);
        pushInfo(c_D, oPlaneCoef.D);
        pushInfo(c_Area, pPolyhedralFace->Area());
        pushInfo(c_DistEpsilon, pPolyhedralFace->DistEpsilon());
    }
    CPolyhedralLoop *pPolyhedralLoop = dynamic_cast<CPolyhedralLoop*>(pGeo);
    if (pPolyhedralLoop)
    {
        pushInfo(c_ClockSign, getClockSignStr(pPolyhedralLoop->ClockSign()));
        pushInfo(c_Area, pPolyhedralLoop->Area());
    }
    CPolyhedralEdge *pPolyhedralEdge = dynamic_cast<CPolyhedralEdge*>(pGeo);
    if (pPolyhedralEdge)
    {
        CPolyhedralEdge *pTwinEdge = pPolyhedralEdge->TwinEdge();
        int nTwinFaceIndex = -1, nTwinLoopIndex = -1, nTwinEdgeIndex = -1;
        if (pTwinEdge)
        {
            nTwinFaceIndex = pTwinEdge->Face()->GetIndex();
            nTwinLoopIndex = pTwinEdge->Loop()->GetIndex();
            nTwinEdgeIndex = pTwinEdge->GetIndex();
        }
        pushInfo("TwinFace", nTwinFaceIndex);
        pushInfo("TwinLoop", nTwinLoopIndex);
        pushInfo("TwinEdge", nTwinEdgeIndex);
    }
    CPolyhedralVertex *pPolyhedralVertex = dynamic_cast<CPolyhedralVertex*>(pGeo);
    if (pPolyhedralVertex)
    {
        pushInfo("Positon", getVector3dStr(pPolyhedralVertex->GetPosition()));
    }
    CPolygon *pPolygon = dynamic_cast<CPolygon*>(pGeo);
    if (pPolygon)
    {
        pushInfo("CoedgeCount", pPolygon->CoedgeCount());
        pushInfo("IsConnectedPolygon", pPolygon->IsConnectedPolygon());
        pushInfo("IsLoopDirCorrect", pPolygon->IsLoopDirCorrect());
        pushInfo(c_Area, pPolygon->Area());
        pushInfo(c_DistEpsilon, pPolygon->DistEpsilon());
        return slInfo;
    }
    CCoedgeList *pCoedges = dynamic_cast<CCoedgeList*>(pGeo);
    if (pCoedges)
    {
        CLoop *pLoop = dynamic_cast<CLoop*>(pGeo);
        if (pLoop)
        {
            pushInfo(c_IsClose, pLoop->IsClosed());
            pushInfo(c_ClockSign, getClockSignStr(pLoop->ClockSign()));
            pushInfo("Area", pLoop->Area());
        }
        pushInfo("IsConnected", pCoedges->IsConnected());
        pushInfo("Perimeter", pCoedges->Perimeter());
        pushInfo(c_DistEpsilon, pCoedges->DistEpsilon());
        return slInfo;
    }
    CCoedge *pCoedge = dynamic_cast<CCoedge*>(pGeo);
    if (pCoedge)
    {
        CCoedgeEx *pCoedgeEx = dynamic_cast<CCoedgeEx*>(pGeo);
        if (pCoedgeEx)
        {
            pushInfo("EdgeIndex", pCoedgeEx->EdgeIndex());
            pushInfo("EdgeSameDir", pCoedgeEx->EdgeSameDir());
        }
        return slInfo;
    }
    CSurface *pSurface = dynamic_cast<CSurface*>(pGeo);
    if (pSurface)
    {
        pushInfo("IsPlane", pSurface->IsPlane());
        pushInfo(c_RangeU, getRangeStr(pSurface->GetRangeU()));
        pushInfo(c_RangeV, getRangeStr(pSurface->GetRangeV()));
        CPlane *pPlane = dynamic_cast<CPlane*>(pGeo);
        if (pPlane)
        {
            pushInfo(c_Pos, getVector3dStr(pPlane->Pos()));
            pushInfo(c_DirU, getVector3dStr(pPlane->DirU()));
            pushInfo(c_DirV, getVector3dStr(pPlane->DirV()));
            pushInfo(c_DirZ, getVector3dStr(pPlane->GetNormal()));
            return slInfo;
        }
        CCylinder *pCylinder = dynamic_cast<CCylinder*>(pGeo);
        if (pCylinder)
        {
            pushInfo(c_Pos, getVector3dStr(pCylinder->Pos()));
            pushInfo(c_DirX, getVector3dStr(pCylinder->DirX()));
            pushInfo(c_DirY, getVector3dStr(pCylinder->DirY()));
            pushInfo(c_DirZ, getVector3dStr(pCylinder->DirZ()));
            return slInfo;
        }
        CBevel *pBevel = dynamic_cast<CBevel*>(pGeo);
        if (pBevel)
        {
            pushCoord3d(pBevel->Coord());
            pushInfo("Height", pBevel->Height());
            return slInfo;
        }
        CHelicoid *pHelicoid = dynamic_cast<CHelicoid*>(pGeo);
        if (pHelicoid)
        {
            pushCoord3d(pHelicoid->Coord());
            pushInfo(c_Radius, pHelicoid->Radius());
            pushInfo(c_ClockSign, getClockSignStr(pHelicoid->ClockSign()));
            pushInfo(c_HeightCoef, pHelicoid->HeightCoef());
            pushInfo(c_RadiusCoef, pHelicoid->RadiusCoef());
            return slInfo;
        }
        CNurbsSurface *pNurbsSurface = dynamic_cast<CNurbsSurface*>(pGeo);
        if (pNurbsSurface)
        {
            pushInfo("DegreeU", pNurbsSurface->DegreeU());
            pushInfo("DegreeV", pNurbsSurface->DegreeV());
            pushInfo("CtrlPtCountU", pNurbsSurface->CtrlPtCountU());
            pushInfo("CtrlPtCountV", pNurbsSurface->CtrlPtCountV());
            return slInfo;
        }
        CSphere *pSphere = dynamic_cast<CSphere*>(pGeo);
        if (pSphere)
        {
            pushCoord3d(pSphere->Coord());
            pushInfo(c_Radius, pSphere->Radius());
            return slInfo;
        }
        CSweep *pSweep = dynamic_cast<CSweep*>(pGeo);
        if (pSweep)
        {
            pushInfo(c_ReferenceVector, getVector3dStr(pSweep->GetReferenceVector()));
            return slInfo;
        }
        CTorus *pTorus = dynamic_cast<CTorus*>(pGeo);
        if (pTorus)
        {
            pushCoord3d(pTorus->Coord());
            pushInfo(c_Radius, pTorus->Radius());
            pushInfo(c_ClockSign, getClockSignStr(pTorus->ClockSign()));
            return slInfo;
        }
        COffsetSurface *pOffsetSurface = dynamic_cast<COffsetSurface*>(pGeo);
        if (pOffsetSurface)
        {
            pushInfo(c_OffsetDistance, pOffsetSurface->OffsetDistance());
            return slInfo;
        }
        CRuledSurface *pRuledSurface = dynamic_cast<CRuledSurface*>(pGeo);
        if (pRuledSurface)
        {

        }
        return slInfo;
    }
    CCurve3d *pCurve3d = dynamic_cast<CCurve3d*>(pGeo);
    if (pCurve3d)
    {
        pushInfo(c_Range, getRangeStr(pCurve3d->GetRange()));
        pushInfo(c_Length, pCurve3d->Length());
        pushInfo(c_StartPt, getVector3dStr(pCurve3d->StartPoint()));
        pushInfo(c_EndPt, getVector3dStr(pCurve3d->EndPoint()));
        pushInfo(c_IsLine, pCurve3d->IsLine());
        CLine3d *pLine = dynamic_cast<CLine3d*>(pGeo);
        if (pLine)
        {
            pushInfo(c_Dir, getVector3dStr(pLine->Dir()));
            return slInfo;
        }
        CPlaneCurve3d *pPlaneCurve = dynamic_cast<CPlaneCurve3d*>(pGeo);
        if (pPlaneCurve)
        {
            pushCoord3d(pPlaneCurve->GetCoordinate());
            return slInfo;
        }
        CSweepCurve3d *pSweepCurve = dynamic_cast<CSweepCurve3d*>(pGeo);
        if (pSweepCurve)
        {
            pushInfo(c_Profile, getVector2dStr(pSweepCurve->Profile()));
            pushInfo(c_ReferenceVector, getVector3dStr(pSweepCurve->GetReferenceVector()));
            return slInfo;
        }
        CSpiralLine *pSpiralLine = dynamic_cast<CSpiralLine*>(pGeo);
        if (pSpiralLine)
        {
            pushCoord3d(pSpiralLine->Coord());
            pushInfo(c_Radius, pSpiralLine->Radius());
            pushInfo(c_ClockSign, getClockSignStr(pSpiralLine->ClockSign()));
            pushInfo(c_HeightCoef, pSpiralLine->HeightCoef());
            pushInfo(c_RadiusCoef, pSpiralLine->RadiusCoef());
            return slInfo;
        }
        CNurbsCurve3d *pNurbsCurve = dynamic_cast<CNurbsCurve3d*>(pGeo);
        if (pNurbsCurve)
        {
            pushInfo("Degree", pNurbsCurve->Degree());
            pushInfo("CtrlPtCount", pNurbsCurve->CtrlPtCount());
            return slInfo;
        }
        return slInfo;
    }
    CCurve2d *pCurve2d = dynamic_cast<CCurve2d*>(pGeo);
    if (pCurve2d)
    {
        pushInfo(c_Range, getRangeStr(pCurve2d->GetRange()));
        pushInfo(c_Length, pCurve2d->Length());
        pushInfo(c_StartPt, getVector2dStr(pCurve2d->StartPoint()));
        pushInfo(c_EndPt, getVector2dStr(pCurve2d->EndPoint()));
        CLine2d *pLine = dynamic_cast<CLine2d*>(pGeo);
        if (pLine)
        {
            pushInfo(c_Dir, getVector2dStr(pLine->Dir()));
            return slInfo;
        }
        CArc2d *pArc = dynamic_cast<CArc2d*>(pGeo);
        if (pArc)
        {
            pushInfo(c_CenterPt, getVector2dStr(pArc->CenterPt()));
            pushInfo(c_Radius, pArc->Radius());
            pushInfo(c_ClockSign, getClockSignStr(pArc->ClockSign()));
            return slInfo;
        }
        CEllipse2d *pEllipse = dynamic_cast<CEllipse2d*>(pGeo);
        if (pEllipse)
        {
            pushInfo(c_CenterPt, getVector2dStr(pEllipse->CenterPt()));
            pushInfo("MajorAxis", getVector2dStr(pEllipse->MajorAxis()));
            pushInfo("MajorRadius", pEllipse->MajorRadius());
            pushInfo("MinorRadisu", pEllipse->MinorRadius());
            pushInfo(c_ClockSign, getClockSignStr(pEllipse->ClockSign()));
            return slInfo;
        }
        CSinCurve2d *pSinCurve = dynamic_cast<CSinCurve2d*>(pGeo);
        if (pSinCurve)
        {
            pushInfo(c_Pos, getVector2dStr(pSinCurve->Pos()));
            pushInfo(c_DirX, getVector2dStr(pSinCurve->DirX()));
            pushInfo("A", pSinCurve->A());
            pushInfo("B", pSinCurve->B());
            pushInfo(c_ClockSign, getClockSignStr(pSinCurve->ClockSign()));
            return slInfo;
        }
        CSpiralLine2d *pSpiralLine = dynamic_cast<CSpiralLine2d*>(pGeo);
        if (pSpiralLine)
        {
            CCoordinates2d coord = pSpiralLine->Coord();
            pushInfo(c_Pos, getVector2dStr(coord.Origin));
            pushInfo(c_DirX, getVector2dStr(coord.X));
            pushInfo(c_DirY, getVector2dStr(coord.Y));
            pushInfo(c_Radius, pSpiralLine->Radius());
            pushInfo(c_ClockSign, getClockSignStr(pSpiralLine->ClockSign()));
            pushInfo(c_RadiusCoef, pSpiralLine->RadiusCoef());
            return slInfo;
        }
        return slInfo;
    }
    return slInfo;
}

/*!
* @author     zhangyc-c  2015.7.19
* @brief      当鼠标在几何列表空白区域点击时，取消选中
* @param[in]  QObject * pObj
* @param[in]  QEvent * pEvent
* @return     bool
*/
bool CGeometryViewer::eventFilter( QObject *pObj, QEvent *pEvent )
{
    if (pEvent->type() == QEvent::MouseButtonPress)
    {
        if (m_trGeometry->underMouse())
        {
            QPoint oPos = m_trGeometry->viewport()->mapFromGlobal(QCursor::pos());
            if (!m_trGeometry->itemAt(oPos))
            {
                m_trGeometry->setCurrentItem(NULL);
            }
        }
    }
    return QMainWindow::eventFilter(pObj, pEvent);
}

/*!
* @author           zhangyc-c
* @brief            获取下一个item
* @return           QTreeWidgetItem *
*/
QTreeWidgetItem * CGeometryViewer::getNextItem(QTreeWidgetItem *pItem)
{
    if (!pItem)
    {
        return nullptr;
    }
    QTreeWidgetItem *pResult = nullptr;
    QTreeWidgetItem *pParentItem = pItem->parent();
    if (pParentItem)
    {
        int nIndex = pParentItem->indexOfChild(pItem);
        if (nIndex + 1< pParentItem->childCount())
        {
            pResult = pParentItem->child(nIndex + 1);
        }
    }
    else
    {
        int nIndex = m_trGeometry->indexOfTopLevelItem(pItem);
        if (nIndex + 1< m_trGeometry->topLevelItemCount())
        {
            pResult = m_trGeometry->topLevelItem(nIndex + 1);
        }
    }
    return pResult;
}

/*!
* @author     zhangyc-c  2015.12.30
* @brief      返回选中的项（第一层节点）
* @return     QTreeWidgetItem *
*/
QTreeWidgetItem * CGeometryViewer::getSeletedItem()
{
    QList<QTreeWidgetItem*> oItems = m_trGeometry->selectedItems();
    if (oItems.empty())
    {
        return nullptr;
    }
    else
    {
        QTreeWidgetItem *pItem = oItems.front();
        while (pItem->parent())
        {
            pItem = pItem->parent();
        }
        return pItem;
    }
}

CBox3d CGeometryViewer::getItemBox(EnItemType nType, void *ptr)
{
    const double c_dExtraExpand = 3000;
    CBox3d oResult;
    switch(nType)
    {
    case itGeometry:
        {
            CGeometry *pGeo = (CGeometry*)(ptr);
            if (!pGeo->BoundingBox(oResult, false))
            {
                oResult.MakeEmpty();
            }
            break;
        }
    case itVector2d:
        {
            CVector2d *pVec2d = (CVector2d*)ptr;
            oResult = CBox2d(*pVec2d, c_dExtraExpand, c_dExtraExpand).Box3();
            break;
        }
    case itVector3d:
        {
            CVector3d *pVec3d = (CVector3d*)ptr;
            oResult = CBox3d(*pVec3d, *pVec3d);
            oResult.Expand(c_dExtraExpand, c_dExtraExpand, c_dExtraExpand, false);
            break;
        }
    case itBox2d:
        {
            CBox2d *pBox2d = (CBox2d*)ptr;
            oResult = pBox2d->Box3();
            oResult.Expand(c_dExtraExpand, c_dExtraExpand, 0, false);
            break;
        }
    case itBox3d:
        {
            CBox3d *pBox3d = (CBox3d*)ptr;
            oResult = *pBox3d;
            oResult.Expand(c_dExtraExpand, c_dExtraExpand, c_dExtraExpand, false);
            break;
        }
    case  itCoord:
        {
            CCoordinates3d *pCoord = (CCoordinates3d*)(ptr);
            CVector3d oMaxPt = pCoord->Origin + c_dExtraExpand * (pCoord->X + pCoord->Y + pCoord->Z);
            oResult = CBox3d(pCoord->Origin, oMaxPt);
            oResult.Expand(c_dExtraExpand, c_dExtraExpand, c_dExtraExpand, false);
            break;
        }
        break;
    }
    return oResult;
}

QString CGeometryViewer::getItemName(EnItemType nType, const void *ptr)
{
    switch(nType)
    {
    case itGeometry:
        return QString(typeid(*(CGeometry*)ptr).name()).replace("class ggp::", "");
    case itVector2d:
        return "CVector2d";
    case itVector3d:
        return "CVector3d";
    case itBox2d:
        return "CBox2d";
    case itBox3d:
        return "CBox3d";
    case  itCoord:
        return "CCoordinates3d";
    }
    return QString();
}

QStringList CGeometryViewer::getItemProps( EnItemType nType, const void *ptr )
{
    QStringList slResult;
    auto pushInfo = [&](const QString& sName, const QVariant& oValue)
    {
        slResult.push_back(sName + " = " + oValue.toString());
    };
    auto getVector3dStr = [](const CVector3d& vec) -> QString
    {
        return QString("(%1, %2, %3)").arg(QString::number(vec.X), QString::number(vec.Y), QString::number(vec.Z));
    };
    switch (nType)
    {
    case itGeometry:
        slResult = getGeoInfo((CGeometry*)ptr);
        break;
    case itVector2d:
        {
            CVector2d *pVec2d = (CVector2d*)ptr;
            pushInfo(c_X, pVec2d->X);
            pushInfo(c_Y, pVec2d->Y);
            break;
        }
    case itVector3d:
        {
            CVector3d *pVec3d = (CVector3d*)ptr;
            pushInfo(c_X, pVec3d->X);
            pushInfo(c_Y, pVec3d->Y);
            pushInfo(c_Z, pVec3d->Z);
            break;
        }
    case itBox2d:
        {
            CBox2d *pBox2d = (CBox2d*)ptr;
            pushInfo(c_MinX, pBox2d->MinPt().X);
            pushInfo(c_MinY, pBox2d->MinPt().Y);
            pushInfo(c_MaxX, pBox2d->MaxPt().X);
            pushInfo(c_MaxY, pBox2d->MaxPt().Y);
            break;
        }
    case itBox3d:
        {
            CBox3d *pBox3d = (CBox3d*)ptr;
            pushInfo(c_MinX, pBox3d->MinPt().X);
            pushInfo(c_MinY, pBox3d->MinPt().Y);
            pushInfo(c_MinZ, pBox3d->MinPt().Z);
            pushInfo(c_MaxX, pBox3d->MaxPt().X);
            pushInfo(c_MaxY, pBox3d->MaxPt().Y);
            pushInfo(c_MaxZ, pBox3d->MaxPt().Z);
            break;
        }
    case  itCoord:
        {
            CCoordinates3d *pCoord = (CCoordinates3d*)ptr;
            pushInfo(c_Origin, getVector3dStr(pCoord->Origin));
            pushInfo(c_X, getVector3dStr(pCoord->X));
            pushInfo(c_Y, getVector3dStr(pCoord->Y));
            pushInfo(c_Z, getVector3dStr(pCoord->Z));
            break;
        }
    }
    return slResult;
}

QString CGeometryViewer::getItemString(QTreeWidgetItem *pItem)
{
    EnItemType nType = (EnItemType)pItem->data(0, rtType).toInt();
    void *ptr = (void*)pItem->data(0, rtData).toInt();
    char *szResult = nullptr;
    switch(nType)
    {
    case itGeometry:
        szResult = ((CGeometry*)ptr)->AsString();
        break;
    case itVector2d:
        szResult = ((CVector2d*)ptr)->AsString();
        break;
    case itVector3d:
        szResult = ((CVector3d*)ptr)->AsString();
        break;
    case itBox2d:
        szResult = ((CBox2d*)ptr)->AsString();
        break;
    case itBox3d:
        szResult = ((CBox3d*)ptr)->AsString();
        break;
    case  itCoord:
        szResult = ((CCoordinates3d*)ptr)->AsString();
        break;
    }
    QString strResult;
    if (szResult)
    {
        strResult = szResult;
        delete[] szResult;
    }
    return strResult;
}

/*!
* @author           zhangyc-c
* @brief            初始化右键菜单
* @return           void
*/
void CGeometryViewer::initMenu()
{
    CMenuAction oMenuAction;
    oMenuAction.key = "";
    oMenuAction.text = "AsString";
    oMenuAction.function = [&](QTreeWidgetItem *pItem)
    {
        QString strResult = getItemString(pItem);
        QApplication::clipboard()->setText(strResult);
    };
    m_oMenus.push_back(oMenuAction);

    oMenuAction.key = "Body";
    oMenuAction.text = "BodyExtremalPoints";
    oMenuAction.function = [&](QTreeWidgetItem *pItem)
    {
        EnItemType nType = (EnItemType)m_pCurItem->data(0, rtType).toInt();
        if (nType == itGeometry)
        {
            CGeometry *pGeo = (CGeometry*)(m_pCurItem->data(0, rtData).toInt());
            CBody *pBody = dynamic_cast<CBody*>(pGeo);
            if (pBody)
            {
                ExtremalPoint oMinX, oMinY, oMinZ, oMaxX, oMaxY, oMaxZ;
                BodyExtremalPoints(pBody, CVector3d::UnitX, oMinX, oMaxX);
                BodyExtremalPoints(pBody, CVector3d::UnitY, oMinY, oMaxY);
                BodyExtremalPoints(pBody, CVector3d::UnitZ, oMinZ, oMaxZ);
                addTopItem(itBox3d, new CBox3d(oMinX.Point.X, oMinY.Point.Y, oMinZ.Point.Z, oMaxX.Point.X, oMaxY.Point.Y, oMaxZ.Point.Z));
            }
        }
    };
    m_oMenus.push_back(oMenuAction);

    oMenuAction.key = "Body";
    oMenuAction.text = "CreateBrepBody";
    oMenuAction.function = [&](QTreeWidgetItem *pItem)
    {
        EnItemType nType = (EnItemType)m_pCurItem->data(0, rtType).toInt();
        if (nType == itGeometry)
        {
            CGeometry *pGeo = (CGeometry*)(m_pCurItem->data(0, rtData).toInt());
            CBody *pBody = dynamic_cast<CBody*>(pGeo);
            if (pBody)
            {
                CBody *pNewBody = pBody->CreateBrepBody();
                addGeometry(pNewBody);
            }
        }
    };
    m_oMenus.push_back(oMenuAction);


    oMenuAction.key = "Body";
    oMenuAction.text = "DecomposedIntoConnectedBodys";
    oMenuAction.function = [&](QTreeWidgetItem *pItem)
    {
        EnItemType nType = (EnItemType)m_pCurItem->data(0, rtType).toInt();
        if (nType == itGeometry)
        {
            CGeometry *pGeo = (CGeometry*)(m_pCurItem->data(0, rtData).toInt());
            CBody *pBody = dynamic_cast<CBody*>(pGeo);
            if (pBody)
            {
                vector<CBody*> oBodys;
                {
                    QP_FUN("DecomposedIntoConnectedBodys");
                    GBodyBuilder().DecomposedIntoConnectedBodys(pBody, oBodys);
                }
                if (oBodys.size())
                {
                    pItem->setCheckState(0, Qt::Unchecked);
                    for (size_t i = 0; i < oBodys.size(); ++i)
                    {
                        addGeometry(oBodys[i]);
                    }
                }
                else
                {
                    QMessageBox::information(this, c_Tips, c_ReturnNull);
                }
            }
        }
    };
    m_oMenus.push_back(oMenuAction);

    oMenuAction.key = "Polygon";
    oMenuAction.text = "PolygonExtremalPoints";
    oMenuAction.function = [&](QTreeWidgetItem *pItem)
    {
        EnItemType nType = (EnItemType)m_pCurItem->data(0, rtType).toInt();
        if (nType == itGeometry)
        {
            CGeometry *pGeo = (CGeometry*)(m_pCurItem->data(0, rtData).toInt());
            CPolygon *pPoly = dynamic_cast<CPolygon*>(pGeo);
            if (pPoly)
            {
                ExtremalPoint2d oMinX, oMinY, oMaxX, oMaxY;
                PolygonExtremalPoints(pPoly, CVector2d::UnitX, oMinX, oMaxX);
                PolygonExtremalPoints(pPoly, CVector2d::UnitY, oMinY, oMaxY);
                addTopItem(itBox2d, new CBox2d(oMinX.Point.X, oMinY.Point.Y, oMaxX.Point.X, oMaxY.Point.Y));
            }
        }
    };
    m_oMenus.push_back(oMenuAction);

    oMenuAction.key = "Polygon";
    oMenuAction.text = "CreateRegularPolygon";
    oMenuAction.function = [&](QTreeWidgetItem *pItem)
    {
        EnItemType nType = (EnItemType)m_pCurItem->data(0, rtType).toInt();
        if (nType == itGeometry)
        {
            CGeometry *pGeo = (CGeometry*)(m_pCurItem->data(0, rtData).toInt());
            CPolygon *pPoly = dynamic_cast<CPolygon*>(pGeo);
            if (pPoly)
            {
                vector<CPolygon*> oPolys;
                {
                    QP_FUN("CreateRegularPolygons");
                    pPoly->CreateRegularPolygons(oPolys);
                }
                for (size_t i = 0; i < oPolys.size(); ++i)
                {
                    addGeometry(oPolys[i]);
                }
            }
        }
    };
    m_oMenus.push_back(oMenuAction);

    oMenuAction.key = "Body";
    oMenuAction.text = "Intersect Next";
    oMenuAction.function = [&](QTreeWidgetItem *pItem)
    {
        CGeometry *pResult = nullptr;
        QTreeWidgetItem *pNextItem = getNextItem(pItem);
        bool bCall = false;
        if (pItem && pNextItem)
        {
            EnItemType nType1 = (EnItemType)pItem->data(0, rtType).toInt();
            EnItemType nType2 = (EnItemType)pNextItem->data(0, rtType).toInt();
            if (nType1 == itGeometry && nType2 == itGeometry)
            {
                CGeometry *pGeo1 = (CGeometry*)(pItem->data(0, rtData).toInt());
                CGeometry *pGeo2 = (CGeometry*)(pNextItem->data(0, rtData).toInt());
                CBody *pBody1 = dynamic_cast<CBody*>(pGeo1);
                CBody *pBody2 = dynamic_cast<CBody*>(pGeo2);
                CPolygon *pPoly1 = dynamic_cast<CPolygon*>(pGeo1);
                CPolygon *pPoly2 = dynamic_cast<CPolygon*>(pGeo2);
                if (pBody1 && pBody2)
                {
                    CBooleanOperate oBoolOpr;
                    //oBoolOpr.SetDistEpsilon(m_dBoolEps);
                    bool bShell1 = pBody1->IsShell();
                    bool bShell2 = pBody2->IsShell();
                    if (bShell1 && bShell2)
                    {
                        QP_FUN("ShellIntersectShell");
                        pResult = oBoolOpr.ShellIntersectShell(pBody1, pBody2);
                        bCall = true;
                    }
                    else if (bShell1 && !bShell2)
                    {
                        QP_FUN("ShellIntersectBody");
                        pResult = oBoolOpr.ShellIntersectBody(pBody1, pBody2);
                        bCall = true;
                    }
                    else if (!bShell1 && !bShell2)
                    {
                        QP_FUN("BodyIntersectBody");
                        pResult = oBoolOpr.Intersect(pBody1, pBody2);
                        bCall = true;
                    }
                }
                else if (pPoly1 && pPoly2)
                {
                    QP_FUN("PolyIntersectPoly");
                    CBooleanOperate2D oBoolOpr2d;
                    oBoolOpr2d.SetDistEpsilon(m_dBoolEps);
                    pResult = oBoolOpr2d.Intersect(pPoly1, pPoly2);
                    bCall = true;
                }
            }
        }
        if (bCall)
        {
            if (pResult)
            {
                pItem->setCheckState(0, Qt::Unchecked);
                pNextItem->setCheckState(0, Qt::Unchecked);
                addGeometry(pResult);
            }
            else
            {
                QMessageBox::information(this, c_Tips, c_ReturnNull);
            }
        }
        else
        {
            QMessageBox::warning(this, c_Tips, c_ParamError);
        }
    };
    m_oMenus.push_back(oMenuAction);
    oMenuAction.key = "polygon";
    m_oMenus.push_back(oMenuAction);

    oMenuAction.key = "Body";
    oMenuAction.text = "Subtract Next";
    oMenuAction.function = [&](QTreeWidgetItem *pItem)
    {
        CGeometry *pResult = nullptr;
        QTreeWidgetItem *pNextItem = getNextItem(pItem);
        bool bCall = false;
        if (pItem && pNextItem)
        {
            EnItemType nType1 = (EnItemType)pItem->data(0, rtType).toInt();
            EnItemType nType2 = (EnItemType)pNextItem->data(0, rtType).toInt();
            if (nType1 == itGeometry && nType2 == itGeometry)
            {
                CGeometry *pGeo1 = (CGeometry*)(pItem->data(0, rtData).toInt());
                CGeometry *pGeo2 = (CGeometry*)(pNextItem->data(0, rtData).toInt());
                CBody *pBody1 = dynamic_cast<CBody*>(pGeo1);
                CBody *pBody2 = dynamic_cast<CBody*>(pGeo2);
                CPolygon *pPoly1 = dynamic_cast<CPolygon*>(pGeo1);
                CPolygon *pPoly2 = dynamic_cast<CPolygon*>(pGeo2);
                if (pBody1 && pBody2)
                {
                    CBooleanOperate oBoolOpr;
                    oBoolOpr.SetDistEpsilon(m_dBoolEps);
                    bool bShell1 = pBody1->IsShell();
                    bool bShell2 = pBody2->IsShell();
                    if (bShell1 && bShell2)
                    {
                        QP_FUN("ShellSubstractShell");
                        pResult = oBoolOpr.ShellSubstractShell(pBody1, pBody2);
                        bCall = true;
                    }
                    else if (bShell1 && !bShell2)
                    {
                        QP_FUN("ShellSubstractBody");
                        pResult = oBoolOpr.ShellSubstractBody(pBody1, pBody2);
                        bCall = true;
                    }
                    else if (!bShell1 && !bShell2)
                    {
                        QP_FUN("BodySubtractBody");
                        pResult = oBoolOpr.Subtract(pBody1, pBody2);
                        bCall = true;
                    }
                }
                else if (pPoly1 && pPoly2)
                {
                    QP_FUN("PolySubtractPoly");
                    CBooleanOperate2D oBoolOpr2d;
                    oBoolOpr2d.SetDistEpsilon(m_dBoolEps);
                    pResult = oBoolOpr2d.Subtract(pPoly1, pPoly2);
                    bCall = true;
                }
            }
        }
        if (bCall)
        {
            if (pResult)
            {
                pItem->setCheckState(0, Qt::Unchecked);
                pNextItem->setCheckState(0, Qt::Unchecked);
                addGeometry(pResult);
            }
            else
            {
                QMessageBox::information(this, c_Tips, c_ReturnNull);
            }
        }
        else
        {
            QMessageBox::warning(this, c_Tips, c_ParamError);
        }
    };
    m_oMenus.push_back(oMenuAction);
    oMenuAction.key = "Polygon";
    m_oMenus.push_back(oMenuAction);

    oMenuAction.key = "Body";
    oMenuAction.text = "Unite Next";
    oMenuAction.function = [&](QTreeWidgetItem *pItem)
    {
        CGeometry *pResult = nullptr;
        QTreeWidgetItem *pNextItem = getNextItem(pItem);
        bool bCall = false;
        if (pItem && pNextItem)
        {
            EnItemType nType1 = (EnItemType)pItem->data(0, rtType).toInt();
            EnItemType nType2 = (EnItemType)pNextItem->data(0, rtType).toInt();
            if (nType1 == itGeometry && nType2 == itGeometry)
            {
                CGeometry *pGeo1 = (CGeometry*)(pItem->data(0, rtData).toInt());
                CGeometry *pGeo2 = (CGeometry*)(pNextItem->data(0, rtData).toInt());
                CBody *pBody1 = dynamic_cast<CBody*>(pGeo1);
                CBody *pBody2 = dynamic_cast<CBody*>(pGeo2);
                CPolygon *pPoly1 = dynamic_cast<CPolygon*>(pGeo1);
                CPolygon *pPoly2 = dynamic_cast<CPolygon*>(pGeo2);
                if (pBody1 && pBody2)
                {
                    CBooleanOperate oBoolOpr;
                    oBoolOpr.SetDistEpsilon(m_dBoolEps);
                    bool bShell1 = pBody1->IsShell();
                    bool bShell2 = pBody2->IsShell();
                    if (bShell1 && bShell2)
                    {
                        QP_FUN("ShellUniteShell");
                        pResult = oBoolOpr.ShellUniteShell(pBody1, pBody2);
                        bCall = true;
                    }
                    else if (!bShell1 && !bShell2)
                    {
                        QP_FUN("BodyUniteBody");
                        pResult = oBoolOpr.Unite(pBody1, pBody2);
                        bCall = true;
                    }
                }
                else if (pPoly1 && pPoly2)
                {
                    QP_FUN("PolyUnitePoly");
                    CBooleanOperate2D oBoolOpr2d;
                    oBoolOpr2d.SetDistEpsilon(m_dBoolEps);
                    pResult = oBoolOpr2d.Unite(pPoly1, pPoly2);
                    bCall = true;
                }
            }
        }
        if (bCall)
        {
            if (pResult)
            {
                pItem->setCheckState(0, Qt::Unchecked);
                pNextItem->setCheckState(0, Qt::Unchecked);
                addGeometry(pResult);
            }
            else
            {
                QMessageBox::information(this, c_Tips, c_ReturnNull);
            }
        }
        else
        {
            QMessageBox::warning(this, c_Tips, c_ParamError);
        }
    };
    m_oMenus.push_back(oMenuAction);
    oMenuAction.key = "Polygon";
    m_oMenus.push_back(oMenuAction);

    oMenuAction.key = "Face";
    oMenuAction.text = "CreateFacePolygon";
    oMenuAction.function = [&](QTreeWidgetItem *pItem)
    {
        EnItemType nType = (EnItemType)m_pCurItem->data(0, rtType).toInt();
        if (nType == itGeometry)
        {
            CGeometry *pGeo = (CGeometry*)(m_pCurItem->data(0, rtData).toInt());
            CFace *pFace = dynamic_cast<CFace*>(pGeo);
            if (pFace)
            {
                CPolygon *pResult = pFace->CreateFacePolygon();
                addGeometry(pResult);
            }
        }
    };
    m_oMenus.push_back(oMenuAction);

    oMenuAction.key = "Face";
    oMenuAction.text = "EraseFace";
    oMenuAction.function = [&](QTreeWidgetItem *pItem)
    {
        EnItemType nType = (EnItemType)m_pCurItem->data(0, rtType).toInt();
        if (nType == itGeometry)
        {
            CGeometry *pGeo = (CGeometry*)(m_pCurItem->data(0, rtData).toInt());
            CFace *pFace = dynamic_cast<CFace*>(pGeo);
            if (pFace)
            {
                CBrepBody *pBrepBody = dynamic_cast<CBrepBody*>(pFace->Parent());
                if (pBrepBody)
                {
                    pBrepBody->EraseFace(pFace->Index());
                    refreshTopItem(pItem->parent());
                }
                else
                {
                    deleteItem();
                }
            }
        }
    };
    m_oMenus.push_back(oMenuAction);

    oMenuAction.key = "Loop";
    oMenuAction.text = "DeleteLoop";
    oMenuAction.function = [&](QTreeWidgetItem *pItem)
    {
        EnItemType nType = (EnItemType)m_pCurItem->data(0, rtType).toInt();
        if (nType == itGeometry)
        {
            CGeometry *pGeo = (CGeometry*)(m_pCurItem->data(0, rtData).toInt());
            if (pGeo && pGeo->Type() == LoopType)
            {
                CLoop *pLoop = dynamic_cast<CLoop*>(pGeo);
                CPolygon *pPolygon = dynamic_cast<CPolygon*>(pLoop->Parent());
                if (pPolygon)
                {
                    pPolygon->DeleteLoop(pLoop->Index());
                    refreshTopItem(pItem->parent());
                }
                else
                {
                    deleteItem();
                }
            }
        }
    };
    m_oMenus.push_back(oMenuAction);

    oMenuAction.key = "Body";
    oMenuAction.text = "MergeFaces";
    oMenuAction.function = [&](QTreeWidgetItem *pItem)
    {
        EnItemType nType = (EnItemType)m_pCurItem->data(0, rtType).toInt();
        if (nType == itGeometry)
        {
            CGeometry *pGeo = (CGeometry*)(m_pCurItem->data(0, rtData).toInt());
            CBody *pBody = dynamic_cast<CBody*>(pGeo);
            if (pBody)
            {
                {
                    QP_FUN("MergeFaces");
                    pBody->MergeFaces();
                }
                CExtrudedBody *pExtrudeBody = dynamic_cast<CExtrudedBody*>(pGeo);
                if (pExtrudeBody)
                {
                    //pExtrudeBody->Section()->MergeCoedges();
                    QMessageBox::information(this, c_Tips, c_ExtrudeMergeFacesMethod);
                }
                refreshTopItem(pItem);
            }
        }
    };
    m_oMenus.push_back(oMenuAction);

    oMenuAction.key = "Polygon";
    oMenuAction.text = "MergeCoedges";
    oMenuAction.function = [&](QTreeWidgetItem *pItem)
    {
        EnItemType nType = (EnItemType)m_pCurItem->data(0, rtType).toInt();
        if (nType == itGeometry)
        {
            CGeometry *pGeo = (CGeometry*)(m_pCurItem->data(0, rtData).toInt());
            CPolygon *pPoly = dynamic_cast<CPolygon*>(pGeo);
            CCoedgeList *pCoedges = dynamic_cast<CCoedgeList*>(pGeo);
            if (pPoly)
            {
                {
                    QP_FUN("MergeCoedges");
                    pPoly->MergeCoedges();
                }
                refreshTopItem(pItem);
            }
            if (pCoedges)
            {
                {
                    QP_FUN("MergeCoedges");
                    pCoedges->MergeCoedges();
                }
                refreshTopItem(pItem);
            }
        }
    };
    m_oMenus.push_back(oMenuAction);
    oMenuAction.key = "Loop";
    m_oMenus.push_back(oMenuAction);
    oMenuAction.key = "CoedgeList";
    m_oMenus.push_back(oMenuAction);

    oMenuAction.key = "Polygon";
    oMenuAction.text = "OrientProjectPolygon Next";
    oMenuAction.function = [&](QTreeWidgetItem *pItem)
    {
        EnItemType nType = (EnItemType)m_pCurItem->data(0, rtType).toInt();
        if (nType == itGeometry)
        {
            QTreeWidgetItem *pNextItem = getNextItem(pItem);
            if (!pNextItem || pNextItem->data(0, rtType).toInt() != itCoord)
            {
                return;
            }
            CGeometry *pGeo = (CGeometry*)(m_pCurItem->data(0, rtData).toInt());
            CCoordinates3d *pCoord = (CCoordinates3d*)(pNextItem->data(0, rtData).toInt());
            CPolygon *pPoly = dynamic_cast<CPolygon*>(pGeo);
            if (pPoly)
            {
                QP_FUN("OrientProjectPolygon");
                CPolygon *pNewPoly = ggp::OrientProjectPolygon(*pCoord, pPoly, CCoordinates3d::GlobalCCoord3D, CVector3d::UnitZ);
                if (pNewPoly)
                {
                    addGeometry(pNewPoly);
                }
                else
                {
                    QMessageBox::information(this, c_Tips, c_ReturnNull);
                }
            }
        }
    };
    m_oMenus.push_back(oMenuAction);

    oMenuAction.key = "Polygon";
    oMenuAction.text = "PolygonSmooth";
    oMenuAction.function = [&](QTreeWidgetItem *pItem)
    {
        EnItemType nType = (EnItemType)m_pCurItem->data(0, rtType).toInt();
        if (nType == itGeometry)
        {
            CGeometry *pGeo = (CGeometry*)(m_pCurItem->data(0, rtData).toInt());
            CPolygon *pPoly = dynamic_cast<CPolygon*>(pGeo);
            CCoedgeList *pCoedges = dynamic_cast<CCoedgeList*>(pGeo);
            if (pPoly)
            {
                {
                    QP_FUN("PolygonSmooth");
                    CPolygon *pNewPoly = PolygonSmooth(pPoly, m_dBoolEps);
                    if (pNewPoly)
                    {
                        addGeometry(pNewPoly);
                    }
                    else
                    {
                        QMessageBox::information(this, c_Tips, c_ReturnNull);
                    }
                }
                refreshTopItem(pItem);
            }
        }
    };
    m_oMenus.push_back(oMenuAction);

    oMenuAction.key = "Body";
    oMenuAction.text = "hpartCreateContourLine";
    oMenuAction.function = [&](QTreeWidgetItem *pItem)
    {
        EnItemType nType = (EnItemType)m_pCurItem->data(0, rtType).toInt();
        if (nType == itGeometry)
        {
            CGeometry *pGeo = (CGeometry*)(m_pCurItem->data(0, rtData).toInt());
            CBody *pBody = dynamic_cast<CBody*>(pGeo);
            CFace *pFace = dynamic_cast<CFace*>(pGeo);
            if (pBody || pFace)
            {
                double dDistEps = m_pViewerWidget->m_dDistEps;
                double dAngleEps = m_pViewerWidget->m_dAngleEps;
                if (m_pViewerWidget->m_nMeshType == 1)
                {
                    CMeshParameter::GetCommonPrecision(pGeo, dDistEps, dAngleEps);
                }
                else if(m_pViewerWidget->m_nMeshType == 2)
                {
                    CMeshParameter::GetMinPrecision(pGeo, dDistEps, dAngleEps);
                }
                CPolygon *pPolygon = nullptr;
                if (pBody)
                {
                    CBodyPtr pTempBody;
                    CBrepBody *pBrepBody = dynamic_cast<CBrepBody*>(pBody);
                    if (!pBrepBody)
                    {
                        pTempBody = pBrepBody = pBody->CreateBrepBody();
                    }
                    QP_FUN("hpartCreateContourLine");
                    pPolygon = hpartCreateContourLine(CCoordinates3d::GlobalCCoord3D, pBrepBody, dDistEps, dAngleEps);
                }
                if (pFace)
                {
                    QP_FUN("hpartCreateContourLine");
                    pPolygon = hpartCreateContourLine(CCoordinates3d::GlobalCCoord3D, pFace, dDistEps, dAngleEps);
                }
                addGeometry(pPolygon);
            }
        }
    };
    m_oMenus.push_back(oMenuAction);
    oMenuAction.key = "Face";
    m_oMenus.push_back(oMenuAction);
}

/*!
* @author           zhangyc-c
* @brief            响应几何列表的右键点击
* @param[in]        const QPoint & oPos
* @return           void
*/
void CGeometryViewer::onMenuRequested(const QPoint &oPos)
{
    QTreeWidgetItem *pItem = m_trGeometry->itemAt(oPos);
    if (pItem)
    {
        QString strText = pItem->text(0);
        QMenu *pMenu = new QMenu(this);
        for (size_t i = 0; i < m_oMenus.size(); ++i)
        {
            CMenuAction &oMenu = m_oMenus[i];
            if (oMenu.key.isEmpty() || strText.contains(oMenu.key, Qt::CaseInsensitive))
            {
                QAction *pAction = new QAction(oMenu.text, pMenu);
                connect(pAction, &QAction::triggered, [&](bool bChecked)
                {
                    oMenu.function(pItem);
                });
                pMenu->addAction(pAction);
            }
        }
        if (!pMenu->actions().isEmpty())
        {
            pMenu->exec(QCursor::pos());
        }
        delete pMenu;
    }
}

/*!
* @author           zhangyc-c
* @brief            刷新根节点
* @param[in]        QTreeWidgetItem * pTopItem
* @return           void
*/
void CGeometryViewer::refreshTopItem(QTreeWidgetItem *pTopItem)
{
    QList<QTreeWidgetItem*> oItems = pTopItem->takeChildren();
    qDeleteAll(oItems);
    QObject::disconnect(m_trGeometry, &QTreeWidget::itemChanged, this, &CGeometryViewer::onItemChanged);
    CGroupNode *pGroupNode = (CGroupNode*)pTopItem->data(0, rtNode).toInt();
    if (pGroupNode)
    {
        pGroupNode->GetParent()->RemoveChild(pGroupNode);
    }
    EnItemType nType = (EnItemType)pTopItem->data(0, rtType).toInt();
    void *pData = (void*)pTopItem->data(0, rtData).toInt();
    pGroupNode = m_pViewerWidget->showItem(nType, pData);
    pTopItem->setData(0, rtNode, (int)pGroupNode);
    addChildItem(pTopItem);
    if (pTopItem->isSelected())
    {
        pTopItem->setSelected(false);
        pTopItem->setSelected(true);
    }
    QObject::connect(m_trGeometry, &QTreeWidget::itemChanged, this, &CGeometryViewer::onItemChanged);
    m_pViewerWidget->viewer()->Scene()->SetDirty();
}

/*!
* @author           zhangyc-c
* @brief            状态压栈
* @param[in]        EnGeometryViewerState nState
* @param[in]        bool bNewValue
* @return           void
*/
void CGeometryViewer::pushState(EnGeometryViewerState nState, bool bNewValue)
{
    switch(nState)
    {
    case egvsAutoCamera: m_bStateBackup = m_bAutoCamera; m_bAutoCamera = bNewValue; break;
    }
}

/*!
* @author           zhangyc-c
* @brief            状态出栈
* @param[in]        EnGeometryViewerState nState
* @return           void
*/
void CGeometryViewer::popState(EnGeometryViewerState nState)
{
    switch(nState)
    {
    case egvsAutoCamera: m_bAutoCamera = m_bStateBackup; break;
    }
}

void CGeometryViewer::closeEvent(QCloseEvent *pEvent)
{
    if (m_pDiscoveryWidget)
    {
        delete m_pDiscoveryWidget;
    }
    deleteAll();
}
