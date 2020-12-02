
#include <QApplication>
#include <QClipboard>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include "GMath/GVec2.h"
#include "GMath/GVec3.h"
#include "Geometry/CuboidBody.h"
#include "Geometry/ExtrudedBody.h"

#include "GeometryInput.h"
#include "ViewerWidget.h"
#include "Example.h"

using namespace ggp;

QString getValidInfo(CTopology *pTopology, bool & bStrictlyValid)
{
    if (!pTopology)
    {
        return QString();
    }
    pTopology->ResetAllStates();
    ostringstream oStrStream;
    bool bValid = pTopology->IsValid(ValidType(0x001f), &oStrStream);
    bStrictlyValid = bValid && pTopology->IsStrictlyValid(&oStrStream);
    QString sValidInfo = QString(bStrictlyValid ? "IsStrictlyValid = true" : (bValid ? "IsStrictlyValid = false" : "IsValid = false"));
    if (!bStrictlyValid)
    {
        sValidInfo += "(" + QString(oStrStream.str().c_str()).trimmed() + ")";
    }
    return sValidInfo;
}

CExampleWidget::CExampleWidget(CViewerWidget *pViewerWidget, QWidget *parent /*= nullptr*/)
    : m_pViewerWidget(pViewerWidget), m_pNode(nullptr), m_nTime(0), QWidget(parent)
{
    m_pCode = new QTextEdit(this);
    QPalette oPalette = m_pCode->palette();
    oPalette.setBrush(QPalette::Base, QColor(192, 255, 192));
    m_pCode->setPalette(oPalette);

    m_pAsString = new QPushButton("AsString", this);
    connect(m_pAsString, &QPushButton::clicked, [&]()
    {
        QString strAsString = asString();
        if (!strAsString.isEmpty())
        {
            QApplication::clipboard()->setText(strAsString);
        }
    });

    QWidget *pStatusWidget = new QWidget(this);
    m_pValidInfo = new QLabel(this);
    m_pTimeInfo = new QLabel(this);
    m_pTimeInfo->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QHBoxLayout *pStatusLayout = new QHBoxLayout(this);
    pStatusLayout->setMargin(0);
    pStatusLayout->addWidget(m_pAsString);
    pStatusLayout->addWidget(m_pValidInfo, 1);
    pStatusLayout->addWidget(m_pTimeInfo);
    pStatusWidget->setLayout(pStatusLayout);

    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->setMargin(0);
    m_pMainLayout->setSpacing(0);
    m_pMainLayout->addWidget(m_pCode);
    m_pMainLayout->addWidget(pStatusWidget);
    setLayout(m_pMainLayout);
}

CExampleWidget::~CExampleWidget()
{
    if (m_pNode)
    {
        m_pNode->GetParent()->RemoveChild(m_pNode.get());
    }
}

QString CExampleWidget::asString()
{
    QString strResult;
    if (m_pGeo)
    {
        char *szBody = m_pGeo->AsString();
        strResult = szBody;
        delete[] szBody;
    }
    return strResult;
}

void CExampleWidget::updateValidInfo()
{
    bool bValid = false;
    QString sValidInfo;
    if (m_pGeo)
    {
        CBody *pBody = dynamic_cast<CBody*>(m_pGeo.get());
        CPolygon *pPoly = dynamic_cast<CPolygon*>(m_pGeo.get());
        CCoedgeList *pCoedges = dynamic_cast<CCoedgeList*>(m_pGeo.get());
        if (pBody && pBody->IsEmpty() || pPoly && pPoly->IsEmpty() || pCoedges && pCoedges->CoedgeCount() == 0)
        {
            sValidInfo = "result = empty";
        }
        else if (CTopology *pTopology = dynamic_cast<CTopology*>(m_pGeo.get()))
        {
            sValidInfo = getValidInfo(pTopology, bValid);
        }
        if (bValid && pBody)
        {
            if (CPolyhedronBody *pPolyhedronBody = dynamic_cast<CPolyhedronBody*>(pBody))
            {
                pPolyhedronBody->ResteAllStateOfFaces();
                pPolyhedronBody->CalFacesPlaneCoef();
                if (pPolyhedronBody->DistEpsilon() > g_MinDistEpsilon)
                {
                    bValid = false;
                    sValidInfo += " but DistEpsilon = " + QString::number(pPolyhedronBody->DistEpsilon());
                }
            }
            if (CBrepBody *pBrepBody = dynamic_cast<CBrepBody*>(pBody))
            {
                pBrepBody->CalDisEpsilon();
                if (pBrepBody->DistEpsilon() > g_MinDistEpsilon)
                {
                    bValid = false;
                    sValidInfo += " but DistEpsilon = " + QString::number(pBrepBody->DistEpsilon());
                }
            }
        }
    }
    else
    {
        sValidInfo = "result = nullptr";
    }
    m_pValidInfo->setText(sValidInfo);
    m_pValidInfo->setStyleSheet(bValid ? "color: rgb(0, 0, 255);" : "color: rgb(255, 0, 0);");
    m_pTimeInfo->setText(QString::number(m_nTime) + "ms ");
}

void CExampleWidget::updateDisplay()
{
    if (m_pNode)
    {
        m_pNode->GetParent()->RemoveChild(m_pNode.get());
    }
    m_pNode = m_pViewerWidget->showGeometry(m_pGeo.get());
    m_pViewerWidget->viewer()->Scene()->SetDirty();
}

CCuboidBodyExample::CCuboidBodyExample(CViewerWidget *pViewerWidget, QWidget *parent)
    : CExampleWidget(pViewerWidget, parent)
{
    m_pVectorX = new CVector3dInput("oX", CVector3d::UnitX, this);
    m_pVectorY = new CVector3dInput("oY", CVector3d::UnitY, this);
    m_pVectorZ = new CVector3dInput("oZ", CVector3d::UnitZ, this);
    m_pLengthX = new CDoubleInput("dX", 5000, 0, 30000, this);
    m_pLengthY = new CDoubleInput("dY", 4000, 0, 30000, this);
    m_pLengthZ = new CDoubleInput("dZ", 3000, 0, 30000, this);
    QHBoxLayout *pParamLayout = new QHBoxLayout(this);
    pParamLayout->setMargin(0);
    pParamLayout->setSpacing(0);
    pParamLayout->addWidget(m_pVectorX);
    pParamLayout->addWidget(m_pVectorY);
    pParamLayout->addWidget(m_pVectorZ);
    pParamLayout->addWidget(m_pLengthX);
    pParamLayout->addWidget(m_pLengthY);
    pParamLayout->addWidget(m_pLengthZ);
    QWidget *pParamWidget = new QWidget(this);
    pParamWidget->setLayout(pParamLayout);

    m_pCode->setPlainText("\
    CCoordinates3d oCoord(CVector3d::Zero, oX, oY, oZ);\r\n\
    CVector3d oDimension(dX, dY, dZ);\r\n\
    CBody *pBody = new CCuboidBody(oCoord, oDimension);");
    m_pCode->setFixedHeight(54);

    m_pMainLayout->insertWidget(0, pParamWidget);
    connect(m_pVectorX, &CVector3dInput::valueChanged, this, &CCuboidBodyExample::onValueChanged);
    connect(m_pVectorY, &CVector3dInput::valueChanged, this, &CCuboidBodyExample::onValueChanged);
    connect(m_pVectorZ, &CVector3dInput::valueChanged, this, &CCuboidBodyExample::onValueChanged);
    connect(m_pLengthX, &CDoubleInput::valueChanged, this, &CCuboidBodyExample::onValueChanged);
    connect(m_pLengthY, &CDoubleInput::valueChanged, this, &CCuboidBodyExample::onValueChanged);
    connect(m_pLengthZ, &CDoubleInput::valueChanged, this, &CCuboidBodyExample::onValueChanged);
    onValueChanged();
}

void CCuboidBodyExample::onValueChanged()
{
    CCoordinates3d oCoord(CVector3d::Zero, m_pVectorX->value(), m_pVectorY->value(), m_pVectorZ->value());
    CVector3d oDimension(m_pLengthX->value(), m_pLengthY->value(), m_pLengthZ->value());
    clock_t nStartTime = clock();
    m_pGeo = new CCuboidBody(oCoord, oDimension);
    m_nTime = clock() - nStartTime;
    updateDisplay();
    updateValidInfo();
}

CExtrudedBodyExample::CExtrudedBodyExample(CViewerWidget *pViewerWidget, QWidget *parent /*= nullptr*/)
    : CExampleWidget(pViewerWidget, parent)
{
    CPolygonPtr pPoly = new CPolygon();
    pPoly->AddLoop();
    pPoly->AddCurve(new CLine2d(CVector2d::Zero, CVector2d(6000, 0)));
    pPoly->AddCurve(new CArc2d(CVector2d(3000, 0), 3000, 0.0, M_PI));
    m_pPoly = new CPolygonInput("pPoly", pPoly, this);
    m_pVector = new CVector3dInput("oZ", CVector3d::UnitZ, this);
    m_pLength = new CDoubleInput("dLength", 6666, -5000, 30000, this);

    QHBoxLayout *pParamLayout = new QHBoxLayout(this);
    pParamLayout->setMargin(0);
    pParamLayout->setSpacing(0);
    pParamLayout->addWidget(m_pPoly);
    pParamLayout->addWidget(m_pVector);
    pParamLayout->addWidget(m_pLength);
    QWidget *pParamWidget = new QWidget(this);
    pParamWidget->setLayout(pParamLayout);

    m_pCode->setPlainText("\
    CCoordinates3d oCoord(CVector3d::Zero, CVector3d::UnitX, CVector3dUnitY, oZ);\r\n\
    CBody *pBody = new CExtrudedBody(oCoord, dLength, pPoly);");
    m_pCode->setFixedHeight(40);

    m_pMainLayout->insertWidget(0, pParamWidget);
    connect(m_pPoly, &CPolygonInput::valueChanged, this, &CExtrudedBodyExample::onValueChanged);
    connect(m_pVector, &CVector3dInput::valueChanged, this, &CExtrudedBodyExample::onValueChanged);
    connect(m_pLength, &CDoubleInput::valueChanged, this, &CExtrudedBodyExample::onValueChanged);
    onValueChanged();
}

void CExtrudedBodyExample::onValueChanged()
{
    CCoordinates3d oCoord(CVector3d::Zero, CVector3d::UnitX, CVector3d::UnitY, m_pVector->value());
    clock_t nStartTime = clock();
    m_pGeo = new CExtrudedBody(oCoord, m_pLength->value(), m_pPoly->value().get());
    m_nTime = clock() - nStartTime;
    updateDisplay();
    updateValidInfo();
}

CPolyhedronBodyExample::CPolyhedronBodyExample(CViewerWidget *pViewerWidget, QWidget *parent /*= nullptr*/)
    : CExampleWidget(pViewerWidget, parent)
{
    m_pTopPt0 = new CPoint3dInput("oTopPt[0]", CVector3d(0, 0, 7000), this);
    m_pTopPt1 = new CPoint3dInput("oTopPt[1]", CVector3d(6000, 0, 10000), this);
    m_pTopPt2 = new CPoint3dInput("oTopPt[2]", CVector3d(0, 6000, 10000), this);
    m_pBtmPt0 = new CPoint3dInput("oBtmPt[0]", CVector3d(0, 0, 0), this);
    m_pBtmPt1 = new CPoint3dInput("oBtmPt[1]", CVector3d(10000, 0, 0), this);
    m_pBtmPt2 = new CPoint3dInput("oBtmPt[2]", CVector3d(0, 10000, 0), this);

    QHBoxLayout *pParamLayout = new QHBoxLayout(this);
    pParamLayout->setMargin(0);
    pParamLayout->setSpacing(0);
    pParamLayout->addWidget(m_pTopPt0);
    pParamLayout->addWidget(m_pTopPt1);
    pParamLayout->addWidget(m_pTopPt2);
    pParamLayout->addWidget(m_pBtmPt0);
    pParamLayout->addWidget(m_pBtmPt1);
    pParamLayout->addWidget(m_pBtmPt2);
    QWidget *pParamWidget = new QWidget(this);
    pParamWidget->setLayout(pParamLayout);

    m_pCode->setPlainText("\
    CVector2d oTopPts[3], oBtmPts[3];\r\n\
    CBody *pBody = new CPolyhedronBody(3, oTopPts, oBtmPts);");
    m_pCode->setFixedHeight(40);

    m_pMainLayout->insertWidget(0, pParamWidget);
    connect(m_pTopPt0, &CPoint3dInput::valueChanged, this, &CPolyhedronBodyExample::onValueChanged);
    connect(m_pTopPt1, &CPoint3dInput::valueChanged, this, &CPolyhedronBodyExample::onValueChanged);
    connect(m_pTopPt2, &CPoint3dInput::valueChanged, this, &CPolyhedronBodyExample::onValueChanged);
    connect(m_pBtmPt0, &CPoint3dInput::valueChanged, this, &CPolyhedronBodyExample::onValueChanged);
    connect(m_pBtmPt1, &CPoint3dInput::valueChanged, this, &CPolyhedronBodyExample::onValueChanged);
    connect(m_pBtmPt2, &CPoint3dInput::valueChanged, this, &CPolyhedronBodyExample::onValueChanged);
    onValueChanged();
}

void CPolyhedronBodyExample::onValueChanged()
{
    CVector3d oTopPts[3] = {m_pTopPt0->value(), m_pTopPt1->value(), m_pTopPt2->value()};
    CVector3d oBtmPts[3] = {m_pBtmPt0->value(), m_pBtmPt1->value(), m_pBtmPt2->value()};
    clock_t nStartTime = clock();
    m_pGeo = new CPolyhedronBody(3, oTopPts, oBtmPts);
    m_nTime = clock() - nStartTime;
    updateDisplay();
    updateValidInfo();
}
