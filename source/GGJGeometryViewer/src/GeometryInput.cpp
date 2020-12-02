#include <QDial>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QSlider>
#include <QVBoxLayout>
#include "GeometryInput.h"

using namespace ggp;
const int c_nFixWidth = 120;
const int c_nFixHeight = 120;
CDoubleInput::CDoubleInput(const QString& strName, double dValue, double dMin, double dMax, QWidget *parent)
    : QWidget(parent), m_dMin(dMin), m_dMax(dMax)
{
    //setFixedSize(c_nFixWidth, c_nFixHeight);
    m_pSlider = new QSlider(Qt::Horizontal, this);
    m_pSlider->setRange(0, 100);
    m_pLabel = new QLabel(this);
    int nValue = (dValue - dMin) * 100.0 / (dMax - dMin);
    m_pSlider->setValue(nValue);
    onValueChanged(nValue);

    QVBoxLayout *pLayout = new QVBoxLayout(this);
    pLayout->addWidget(new QLabel(strName, this));
    pLayout->addStretch(1);
    pLayout->addWidget(m_pSlider);
    pLayout->addStretch(1);
    pLayout->addWidget(m_pLabel);
    this->setLayout(pLayout);

    connect(m_pSlider, &QSlider::valueChanged, this, &CDoubleInput::valueChanged);
    connect(m_pSlider, &QSlider::valueChanged, this, &CDoubleInput::onValueChanged);
}

double CDoubleInput::value()
{
    int nValue = m_pSlider->value();
    return (m_dMax - m_dMin) * nValue / 100 + m_dMin;
}

void CDoubleInput::onValueChanged(int nValue)
{
    QString strText = QString::number(value());
    m_pLabel->setText(strText);
}

CVector2dInput::CVector2dInput(const QString& strName, const CVector2d &oValue, QWidget *parent)
    : QWidget(parent)
{
    //setFixedSize(c_nFixWidth, c_nFixHeight);
    m_pDial = new QDial(parent);
    m_pDial->setRange(0, 360);
    m_pDial->setSingleStep(30);
    m_pDial->setWrapping(true);
    m_pLabel = new QLabel(this);
    int nValue = oValue.AngleFromXAxis() * M_180_PI;
    nValue = (630 - nValue) % 360;
    m_pDial->setValue(nValue);
    onValueChanged(nValue);

    QVBoxLayout *pLayout = new QVBoxLayout(this);
    pLayout->addWidget(new QLabel(strName, this));
    pLayout->addWidget(m_pDial);
    pLayout->addWidget(m_pLabel);
    this->setLayout(pLayout);

    connect(m_pDial, &QSlider::valueChanged, this, &CVector2dInput::valueChanged);
    connect(m_pDial, &QSlider::valueChanged, this, &CVector2dInput::onValueChanged);
}

ggp::CVector2d CVector2dInput::value()
{
    int nValue = m_pDial->value();
    nValue = (630 - nValue) % 360;
    CVector2d oResult;
    switch (nValue)
    {
    case 0: oResult = CVector2d::UnitX; break;
    case 90: oResult = CVector2d::UnitY; break;
    case 180: oResult = -CVector2d::UnitX; break;
    case 270: oResult = -CVector2d::UnitY; break;
    default: oResult = CVector2d(cos(nValue * M_PI_180), sin(nValue * M_PI_180)); break;
    }
    return oResult;
}

void CVector2dInput::onValueChanged(int nValue)
{
    CVector2d oValue = value();
    QString strTextX = "X=" + QString::number(oValue.X);
    QString strTextY = "Y=" + QString::number(oValue.Y);
    m_pLabel->setText(strTextX + "\r\n" + strTextY);
}

CVector3dInput::CVector3dInput(const QString& strName, const ggp::CVector3d &oValue, QWidget *parent)
    : QWidget(parent)
{
    //setFixedSize(c_nFixWidth, c_nFixHeight);
    m_pDial = new QDial(parent);
    m_pDial->setRange(0, 360);
    m_pDial->setSingleStep(30);
    m_pDial->setWrapping(true);
    m_pLabel = new QLabel(this);
    int nValue = oValue.Vec2().AngleFromXAxis() * M_180_PI;
    nValue = (630 - nValue) % 360;
    m_pDial->setValue(nValue);
    m_pSlider = new QSlider(Qt::Vertical, this);
    m_pSlider->setRange(-100, 100);
    m_pSlider->setValue(oValue.Z * 100);
    onValueChanged(0);

    QWidget *pToolWidget = new QWidget(this);
    QHBoxLayout *pToolLayout = new QHBoxLayout(this);
    pToolLayout->setMargin(0);
    pToolLayout->addWidget(m_pDial);
    pToolLayout->addWidget(m_pSlider);
    pToolWidget->setLayout(pToolLayout);

    QVBoxLayout *pLayout = new QVBoxLayout(this);
    pLayout->addWidget(new QLabel(strName, this));
    pLayout->addWidget(pToolWidget);
    pLayout->addWidget(m_pLabel);
    this->setLayout(pLayout);

    connect(m_pDial, &QDial::valueChanged, this, &CVector3dInput::valueChanged);
    connect(m_pDial, &QDial::valueChanged, this, &CVector3dInput::onValueChanged);
    connect(m_pSlider, &QSlider::valueChanged, this, &CVector3dInput::valueChanged);
    connect(m_pSlider, &QSlider::valueChanged, this, &CVector3dInput::onValueChanged);
}

ggp::CVector3d CVector3dInput::value()
{
    int nZValue = m_pSlider->value();
    if (nZValue == 100)
    {
        return CVector3d::UnitZ;
    }
    else if (nZValue == -100)
    {
        return CVector3d::NegaUnitZ;
    }
    int nXYValue = m_pDial->value();
    nXYValue = (630 - nXYValue) % 360;
    CVector2d oResult;
    switch (nXYValue)
    {
    case 0: oResult = CVector2d::UnitX; break;
    case 90: oResult = CVector2d::UnitY; break;
    case 180: oResult = -CVector2d::UnitX; break;
    case 270: oResult = -CVector2d::UnitY; break;
    default: oResult = CVector2d(cos(nXYValue * M_PI_180), sin(nXYValue * M_PI_180)); break;
    }
    return oResult.Vec3(nZValue / 100.0).Unit();
}

void CVector3dInput::onValueChanged(int nValue)
{
    CVector3d oValue = value();
    QString strTextX = "X=" + QString::number(oValue.X);
    QString strTextY = "Y=" + QString::number(oValue.Y);
    QString strTextZ = "Z=" + QString::number(oValue.Z);
    m_pLabel->setText(strTextX + "\r\n" + strTextY + "\r\n" + strTextZ);
}

CPoint3dInput::CPoint3dInput(const QString& strName, const ggp::CVector3d &oValue, QWidget *parent /*= nullptr*/)
{
    m_pSliderX = new QSlider(Qt::Horizontal, this);
    m_pSliderX->setRange(-20000, 20000);
    m_pSliderX->setValue(oValue.X);
    m_pSliderY = new QSlider(Qt::Horizontal, this);
    m_pSliderY->setRange(-20000, 20000);
    m_pSliderY->setValue(oValue.Y);
    m_pSliderZ = new QSlider(Qt::Horizontal, this);
    m_pSliderZ->setRange(-20000, 20000);
    m_pSliderZ->setValue(oValue.Z);
    m_pLabel = new QLabel(this);
    onValueChanged();

    QVBoxLayout *pLayout = new QVBoxLayout(this);
    pLayout->addWidget(new QLabel(strName, this));
    pLayout->addWidget(m_pSliderX);
    pLayout->addWidget(m_pSliderY);
    pLayout->addWidget(m_pSliderZ);
    pLayout->addWidget(m_pLabel);
    this->setLayout(pLayout);

    connect(m_pSliderX, &QSlider::valueChanged, this, &CPoint3dInput::valueChanged);
    connect(m_pSliderX, &QSlider::valueChanged, this, &CPoint3dInput::onValueChanged);
    connect(m_pSliderY, &QSlider::valueChanged, this, &CPoint3dInput::valueChanged);
    connect(m_pSliderY, &QSlider::valueChanged, this, &CPoint3dInput::onValueChanged);
    connect(m_pSliderZ, &QSlider::valueChanged, this, &CPoint3dInput::valueChanged);
    connect(m_pSliderZ, &QSlider::valueChanged, this, &CPoint3dInput::onValueChanged);
}

ggp::CVector3d CPoint3dInput::value()
{
    return CVector3d(m_pSliderX->value(), m_pSliderY->value(), m_pSliderZ->value());
}

void CPoint3dInput::onValueChanged()
{
    CVector3d oValue = value();
    QString strTextX = "X=" + QString::number(oValue.X);
    QString strTextY = "Y=" + QString::number(oValue.Y);
    QString strTextZ = "Z=" + QString::number(oValue.Z);
    m_pLabel->setText(strTextX + "\r\n" + strTextY + "\r\n" + strTextZ);
}

CPolygonInput::CPolygonInput(const QString& strName, const ggp::CPolygonPtr oValue, QWidget *parent)
    : m_pPoly(oValue)
{
    m_pText = new QPlainTextEdit(this);
    m_pText->setMaximumHeight(c_nFixHeight);
    if (oValue)
    {
        char *szPoly = oValue->AsString();
        m_pText->setPlainText(szPoly);
        delete[] szPoly;
    }
    connect(m_pText, &QPlainTextEdit::textChanged, this, &CPolygonInput::onValueChanged);
    m_pLabel = new QLabel(this);
    QVBoxLayout *pLayout = new QVBoxLayout(this);
    pLayout->addWidget(new QLabel(strName, this));
    pLayout->addWidget(m_pText);
    pLayout->addWidget(m_pLabel);
    this->setLayout(pLayout);
}

void CPolygonInput::onValueChanged()
{
    CGeometry *pGeo = nullptr;
    try
    {
        pGeo = CGeometry::LoadFromStr((char*)m_pText->toPlainText().toStdString().c_str());
    }
    catch (...)
    {
    }
    QString strValidInfo;
    if (pGeo)
    {
        if (pGeo->Type() == PolygonType)
        {
            CPolygon *pPoly = (CPolygon*)pGeo;
            if (pPoly->IsStrictlyValid())
            {
                m_pPoly = pPoly;
                emit valueChanged();
            }
            else
            {
                pGeo->Free();
                strValidInfo = "Invalid Polygon";
            }
        }
        else
        {
            pGeo->Free();
            strValidInfo = "Invalid Type";
        }
    }
    else
    {
        strValidInfo = "Invalid String";
    }
    m_pLabel->setText(strValidInfo);
}

ggp::CPolygonPtr CPolygonInput::value()
{
    return m_pPoly;
}
