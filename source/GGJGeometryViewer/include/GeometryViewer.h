#ifndef GEOMETRYVIEWER_H
#define GEOMETRYVIEWER_H
#include <QtCore/QtCore>
#include <QApplication>
#include <QtGui>
#include <QMainWindow>
#include <QDialog>
#include <QClipboard>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolBar>
#include <QStatusBar>
#include <QToolButton>
#include <QRadioButton>
#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>
#include <QAction>
#include <QDebug>
#include <QPlainTextEdit>
#include <QTreeWidget>
#include <QSplitter>
#include <QComboBox>
#include <QErrorMessage>
#include <QInputDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMenu>
#include <QHeaderView>
#include <QtAlgorithms>
#include <QFileDialog>
#include <functional>
#include "ViewManager/GViewer.h"
#include "ViewerWidget.h"
#include "CmdCenter.h"

using namespace ggp;

class CCmdCenter;
class QListWidget;
class QListWidgetItem;

static const int c_ColorCount = 12;
static const CColor c_Color[c_ColorCount] = 
{
    CColor(255, 0, 0), CColor(255, 127, 0), CColor(255, 255, 0), CColor(127, 255, 0),
    CColor(0, 255, 0), CColor(0, 255, 127), CColor(0, 255, 255), CColor(0, 127, 255),
    CColor(0, 0, 255), CColor(127, 0, 255), CColor(255, 0, 255), CColor(255, 0, 127),
};

enum EnRoleType
{
	rtType = Qt::UserRole,
	rtData,
	rtNode,
	rtColor,
    rtNew,
};

typedef std::function<void (QTreeWidgetItem *pItem)> CMenuFunction;

struct CMenuAction
{
    QString key, text;
    CMenuFunction function; 
};

class CToolButton : public QToolButton
{
    Q_OBJECT
public:
    CToolButton(QWidget * parent = 0);
signals:
    void leftClicked();
    void rightClicked();
protected:
    virtual void mousePressEvent(QMouseEvent *pEvent) override;
    virtual void mouseReleaseEvent(QMouseEvent *pEvent) override;
    virtual void leaveEvent(QEvent *pEvent) override;
private:
    bool m_bLeftPress;
    bool m_bRightPress;
};

class CGeometryViewer : public QMainWindow
{
    Q_OBJECT

private:
    QVBoxLayout *m_pMainLayout;
    QToolBar *m_pToolBar;
    QStatusBar *m_pStatusBar;
    CToolButton *m_btnViewXY, *m_btnViewXZ, *m_btnViewYZ;
    CToolButton *m_btnPaste, *m_btnViewString, *m_btnDrawLine, *m_btnDelete;

	CToolButton *m_btnAddJson;//添加解析json按钮；

    QAction *m_acOption, *m_acDiscovery;
    QAction *m_acShowAxis, *m_acShowDir;
    QToolButton *m_btnColor[c_ColorCount];
    CViewerWidget *m_pViewerWidget;
    QCheckBox *m_chkDir, *m_chkGrid, *m_chkWireFrame, *m_chkListen;
    QTreeWidget *m_trGeometry;
    QTableWidget *m_tbInfo;
    QWidget *m_pOptionWidget, *m_pDiscoveryWidget, *m_pLeftWidget, *m_pRightWidget, *m_pEpsWidget;
    QLineEdit *m_edtBoolEps, *m_edtDistEps, *m_edtAngleEps;
    QSplitter *m_splMain, *m_splLeft;
    QListWidget *m_pExampleList;
    QErrorMessage *m_pErrorDlg;
protected:
    void closeEvent(QCloseEvent *pEvent);
    void mouseDoubleClickEvent(QMouseEvent *pEvent);
    void mouseMoveEvent(QMouseEvent *pEvent);
    void mousePressEvent(QMouseEvent *pEvent);
    void mouseReleaseEvent(QMouseEvent *pEvent);
public:
    QTreeWidgetItem *getSeletedItem();
    QTreeWidgetItem *getNextItem(QTreeWidgetItem *pItem);
    void refreshTopItem(QTreeWidgetItem *pTopItem);
    bool hitPoint(int nX, int nY, CVector3d& ptHit, int nPickSize);
    inline QToolBar *toolbar(){return m_pToolBar;};
public:
    CGeometryViewer(QWidget *parent = 0);
    ~CGeometryViewer();
    bool addGeometry(CGeometry *pGeo){return addTopItem(itGeometry, pGeo);}
    bool addTopItem(EnItemType nType, void *ptr);
    CBox3d getItemBox(EnItemType nType, void *ptr);
    QString getItemName(EnItemType nType, const void *ptr);
    QStringList getItemProps(EnItemType nType, const void *ptr);
    QString getItemString(QTreeWidgetItem *pItem);
    QStringList getGeoInfo(CGeometry *pGeo);
    bool eventFilter(QObject *obj, QEvent *event) override;

enum EnGeometryViewerState
{
    egvsAutoCamera          = 0,
};
    void pushState(EnGeometryViewerState nState, bool bNewValue);
    void popState(EnGeometryViewerState nState);
    inline CViewerWidget *viewerWidget(){return m_pViewerWidget;}

public Q_SLOTS:
    void option();
    void deleteAll();
    void deleteItem();
	void addJson();
    void measure();
    void drawLine();
    void onItemChanged(QTreeWidgetItem *pItem, int nCol);
    void onItemClicked(QTreeWidgetItem *pItem, int nCol);
    void onListenClicked(bool bListen);
    void onMenuRequested(const QPoint &oPos);
    void onSelectionChanged();
    void paste();
    void pasteFromGGDB();
    void showDiscovery();
    void showOption();
    void updateColor();
    void viewAnimation();
    void viewItemString();
    void setViewPoint(EnViewPoint nViewPoint);
    void viewXY();
    void viewXZ();
    void viewYZ();
    void view_XY();
    void view_XZ();
    void view_YZ();

private:
    void addChildItem(QTreeWidgetItem *pParentItem);
    void initMenu();
    void initStatusBar();
    void initToolBar();
    int m_nColorIndex;
    vector<CMenuAction> m_oMenus;
    QTreeWidgetItem *m_pCurItem;
    vector<CGroupNode*> m_oNodes;
    CCmdCenter *m_pCmdCenter;
    bool m_bMouseDown, m_bAutoCamera, m_bStateBackup, m_bAnimateView;
    double m_dBoolEps;
    CVector3d m_oHitPt;
    EnViewPoint m_nAnimateDest;
    int m_nAnimateStep;
    QLibrary *m_pGGDBHelper;
    map<QListWidgetItem*, std::function<QWidget *()> > m_oExamples;
	bool m_isGGJ;//是否通过json获取几何字符串的标志
	QString m_sName;//左边树的钢筋对象显示名称
};

#endif // TEST_H
