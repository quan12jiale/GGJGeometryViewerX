/*!
 * @brief   主程序入口
 * @author  zhangyc-c
 * @date    2015.7.19
 * @remarks 
 * Copyright (c) 1998-2015 Glodon Corporation	
 */
#include <string>
#include <QFile>
#include <QRegularExpression>
#include <QApplication>
#include "GeometryViewer.h"

void addNode(CGeometryViewer& viewer)
{
	QFile file(QString::fromLocal8Bit("F:/GTJ/tools/GGJGeometryViewerX/剪力墙.txt"));
	bool res = file.open(QIODevice::ReadOnly);
	while (!file.atEnd())
	{
		QString line = file.readLine();
		// matches digits followed by colon space
		QRegularExpression re("\\d+: ");
		QRegularExpressionMatch match = re.match(line);
		if (match.hasMatch())
		{
			QString matched = match.captured(0);
			line = line.mid(matched.size());
			CGeometry* pGeo = CGeometry::LoadFromStr(const_cast<char*>(line.toStdString().c_str()));
			viewer.addGeometry(pGeo);
		}
	}
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	QString strExePath = QCoreApplication::applicationDirPath();
	SetCurrentDirectoryW(strExePath.toStdWString().c_str());
	CGeometryViewer viewer;

	addNode(viewer);

    viewer.show();
    return app.exec();
}
