/******************************************************************************
 * Copyright(C),2015-2015, Yaozhongkai
 * FileName:
 * Author:Yaozhongkai
 * Version:
 * Date:
 * Description:
 * Others:
 * Function List:
 * History:
 * ****************************************************************************/

#include <ui_mainwin.hxx>

namespace SupplyUi {

AboutDlg::AboutDlg(QWidget *parent)
:QDialog(parent)
{
	/* Top layout
	 * -----------------------
	 *      Html view
	 * -----------------------
	 *                 Button
	 * -----------------------*/
	QVBoxLayout *top_layout = new QVBoxLayout(this);
	setLayout(top_layout);

	QTextBrowser *about_show = new QTextBrowser(this);
	top_layout->addWidget(about_show);
	about_show->setReadOnly(true);

	QString about = "";
	about += "Author:Yao Zhongkai <br>";
	about += "Email:jlspyaozhongkai@126.com <br>";
	about += "Web:<a href=\"http://xxxxxx.com\"> http://xxxxxx.com </a> <br>";

	about_show->setHtml(about);
	about_show->setOpenExternalLinks(true);
	about_show->setFixedSize(300, 200);

	QHBoxLayout *btn_layout = new QHBoxLayout(this);
	top_layout->addLayout(btn_layout);

	QPushButton *ok_btn = new QPushButton("OK", this);
	btn_layout->addWidget(ok_btn, 0, Qt::AlignRight);

	return;
}

SupplyDlg::SupplyDlg(QWidget *parent)
:QDialog(parent)
{
	QMetaObject::Connection conn;
	UiMenuSetup *window_menu = g_mainwin->menuSetup(UiMenuSetup::SEQ_WIN, "&Window");
	UiMenuSetup *help_menu = g_mainwin->menuSetup(UiMenuSetup::SEQ_HELP, "&Help");

	window_menu->addAction(NULL);
	m_preferences = new QAction("&Preferences", this);
	m_preferences->setStatusTip("Preferences");
	window_menu->addAction(m_preferences);

	QAction *help_act_help = new QAction("&Help Contents", this);
	help_menu->addAction(help_act_help);
    conn = connect(help_act_help, SIGNAL(triggered()), this, SLOT(slotActionHelp()));
    UTER_ASSERT(conn != NULL);

    QAction *help_act_about = new QAction("&About Uter", this);
    help_menu->addAction(help_act_about);
    conn = connect(help_act_about, SIGNAL(triggered()), this, SLOT(slotActionAbout()));
    UTER_ASSERT(conn != NULL);

	return;
}

void SupplyDlg::slotActionHelp()
{
	QString path = QDir::cleanPath(QApplication::applicationDirPath() + "/../document/userguide.pdf");
	QFileInfo info(path);
	if (! info.exists()) {
		QMessageBox::critical(this, "Lost help document",
							  QString("Lost help document ") + path,
							  "OK");
		return;
	}

	QString url = QString("file:///") + path;
	bool bresult = QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
	if (! bresult) {
		QMessageBox::critical(this, "Open help document failed",
							  QString("Open help document ") + path + QString(" failed."),
							  "OK");
		return;
	}
	return;
}

void SupplyDlg::slotActionAbout()
{
	AboutDlg about(this);
	about.exec();
	return;
}

} /*namespace SupplyUi*/
