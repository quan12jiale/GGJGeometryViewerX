#pragma once
#include <QWidget>

#include "GMath/GVec2.h"
#include "GMath/GVec3.h"
#include "Geometry/Polygon.h"

class QLabel;
class QString;
class QSlider;
class QDial;
class QPlainTextEdit;

class CDoubleInput : public QWidget
{
    Q_OBJECT
public:
    CDoubleInput(const QString& strName, double dValue, double dMin, double dMax, QWidget *parent = nullptr);
    double value();
signals:
    void valueChanged();
protected slots:
    void onValueChanged(int nValue);
private:
    QSlider *m_pSlider;
    QLabel *m_pLabel;
    double m_dMin, m_dMax;
};

class CVector2dInput : public QWidget
{
    Q_OBJECT
public:
    CVector2dInput(const QString& strName, const ggp::CVector2d &oValue, QWidget *parent = nullptr);
    ggp::CVector2d value();
signals:
    void valueChanged();
protected slots:
    void onValueChanged(int nValue);
private:
    QDial *m_pDial;
    QLabel *m_pLabel;
};

class CVector3dInput : public QWidget
{
    Q_OBJECT
public:
    CVector3dInput(const QString& strName, const ggp::CVector3d &oValue, QWidget *parent = nullptr);
    ggp::CVector3d value();
signals:
    void valueChanged();
protected slots:
    void onValueChanged(int nValue);
private:
    QDial *m_pDial;
    QSlider *m_pSlider;
    QLabel *m_pLabel;
};

class CPoint3dInput : public QWidget
{
    Q_OBJECT
public:
    CPoint3dInput(const QString& strName, const ggp::CVector3d &oValue, QWidget *parent = nullptr);
    ggp::CVector3d value();
signals:
    void valueChanged();
protected slots:
    void onValueChanged();
private:
    QSlider *m_pSliderX, *m_pSliderY, *m_pSliderZ;
    QLabel *m_pLabel;
};

class CPolygonInput : public QWidget
{
    Q_OBJECT
public:
    CPolygonInput(const QString& strName, const ggp::CPolygonPtr oValue, QWidget *parent = nullptr);
    ggp::CPolygonPtr value();
signals:
    void valueChanged();
protected slots:
    void onValueChanged();
private:
    QPlainTextEdit *m_pText;
    QLabel *m_pLabel;
    ggp::CPolygonPtr m_pPoly;
};