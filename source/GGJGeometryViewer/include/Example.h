#pragma once
#include <QWidget>
#include <QLabel>
#include "Common\Refptr.h"
#include "ViewCore/GSceneNode.h"

class CPolygonInput;
class CPoint3dInput;
class CDoubleInput;
class CVector2dInput;
class CVector3dInput;

class QPushButton;
class QTextEdit;
class QString;
class QVBoxLayout;

class CViewerWidget;

class CExampleWidget : public QWidget
{
protected:
    CExampleWidget(CViewerWidget *pViewerWidget, QWidget *parent = nullptr);
    virtual ~CExampleWidget();
    virtual QString asString();
    virtual void updateDisplay();
    virtual void updateValidInfo();
protected:
    QVBoxLayout *m_pMainLayout;
    QTextEdit *m_pCode;
    QPushButton *m_pAsString;
    QLabel *m_pValidInfo, *m_pTimeInfo;
    CViewerWidget *m_pViewerWidget;
    ggp::CRefPtr<CGroupNode> m_pNode;
    ggp::CRefPtr<CGeometry> m_pGeo;
    clock_t m_nTime;
};

class CCuboidBodyExample : public CExampleWidget
{
public:
    CCuboidBodyExample(CViewerWidget *pViewerWidget, QWidget *parent = nullptr);
protected slots:
    void onValueChanged();
private:
    CVector3dInput *m_pVectorX, *m_pVectorY, *m_pVectorZ;
    CDoubleInput *m_pLengthX, *m_pLengthY, *m_pLengthZ;
};

class CExtrudedBodyExample : public CExampleWidget
{
public:
    CExtrudedBodyExample(CViewerWidget *pViewerWidget, QWidget *parent = nullptr);
protected slots:
    void onValueChanged();
private:
    CPolygonInput *m_pPoly;
    CVector3dInput *m_pVector;
    CDoubleInput *m_pLength;
};

class CPolyhedronBodyExample : public CExampleWidget
{
public:
    CPolyhedronBodyExample(CViewerWidget *pViewerWidget, QWidget *parent = nullptr);
protected slots:
    void onValueChanged();
private:
    CPoint3dInput *m_pTopPt0, *m_pTopPt1, *m_pTopPt2;
    CPoint3dInput *m_pBtmPt0, *m_pBtmPt1, *m_pBtmPt2;
};