/*
	Copyright (C) 2009-2013 jakago

	This file is part of CaptureStream, the flv downloader for NHK radio
	language courses.

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "downloadthread.h"
#include "customizedialog.h"
#include "scrambledialog.h"
#include "utility.h"
#include "qt4qt5.h"

#include <QMessageBox>
#include <QByteArray>
#include <QXmlQuery>
#include <QStringList>
#include <QProcess>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QThread>
#include <QSettings>
#include <QDesktopWidget>
#include <QInputDialog>
#include <QFileDialog>
#include <QTextStream>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QtNetwork>
#include <QTemporaryFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#define SETTING_GROUP "MainWindow"
#define SETTING_GEOMETRY "geometry"
#define SETTING_WINDOWSTATE "windowState"
#define SETTING_MAINWINDOW_POSITION "Mainwindow_Position"
#define SETTING_SAVE_FOLDER "save_folder"
#define SETTING_SCRAMBLE "scramble"
#define SETTING_SCRAMBLE_URL1 "scramble_url1"
#define SETTING_SCRAMBLE_URL2 "scramble_url2"
#define SCRAMBLE_URL1 "http://www47.atwiki.jp/jakago/pub/scramble.xml"
#define SCRAMBLE_URL2 "http://cdn47.atwikiimg.com/jakago/pub/scramble.xml"
#define X11_WINDOW_VERTICAL_INCREMENT 5

#define OPTIONAL1 "6805_01"	// 小学生の基礎英語
#define OPTIONAL2 "6806_01"	// 中学生の基礎英語 レベル1
#define OPTIONAL3 "6807_01"	// 中学生の基礎英語 レベル2
#define OPTIONAL4 "6808_01"	// 中高生の基礎英語 in English
#define OPTIONAL5 "0916_01"	// ラジオ英会話
#define OPTIONAL6 "6809_01"	// ラジオビジネス英語
#define OPTIONAL7 "3064_01"	// エンジョイ・シンプル・イングリッシュ
#define OPTIONAL8 "2331_01"	// 英会話タイムトライアル
#define Program_TITLE1 "小学生の基礎英語"
#define Program_TITLE2 "中学生の基礎英語 レベル１"
#define Program_TITLE3 "中学生の基礎英語 レベル２"
#define Program_TITLE4 "中高生の基礎英語 in English"
#define Program_TITLE5 "ラジオ英会話"
#define Program_TITLE6 "ラジオビジネス英語"
#define Program_TITLE7 "エンジョイ・シンプル・イングリッシュ"
#define Program_TITLE8 "英会話タイムトライアル"

#ifdef QT4_QT5_WIN
#define STYLE_SHEET "stylesheet-win.qss"
#else
#ifdef QT4_QT5_MAC
#define STYLE_SHEET "stylesheet-mac.qss"
#else
#define STYLE_SHEET "stylesheet-ubu.qss"
#endif
#endif

namespace {
	bool outputDirSpecified = false;
	QString version() {
		QString result;
		// 日本語ロケールではQDate::fromStringで曜日なしは動作しないのでQRegExpを使う
		// __DATE__の形式： "Jul  8 2011"
		static QRegExp regexp( "([a-zA-Z]{3})\\s+(\\d{1,2})\\s+(\\d{4})" );
		static QStringList months = QStringList()
				<< "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun"
				<< "Jul" << "Aug" << "Sep" << "Oct" << "Nov" << "Dec";
		if ( regexp.indexIn( __DATE__ ) != -1 ) {
//			int month = months.indexOf( regexp.cap( 1 ) ) + 1;
//			int day = regexp.cap( 2 ).toInt();
//			result = QString( " (%1/%2/%3)" ).arg( regexp.cap( 3 ) )
//					.arg( month, 2, 10, QLatin1Char( '0' ) ).arg( day, 2, 10, QLatin1Char( '0' ) );
			result = QString( " (2023/07/05)" ); 
		}
		return result;
	}
}

QString MainWindow::outputDir;
QString MainWindow::ini_file_path;
QString MainWindow::scramble;
QString MainWindow::scrambleUrl1;
QString MainWindow::scrambleUrl2;
QString MainWindow::optional[] = {"0953", "0943", "0946", "0948", "0953", "0943", "0946", "0948"};
QString MainWindow::optional1;
QString MainWindow::optional2;
QString MainWindow::optional3;
QString MainWindow::optional4;
QString MainWindow::optional5;
QString MainWindow::optional6;
QString MainWindow::optional7;
QString MainWindow::optional8;
QString MainWindow::program_title1;
QString MainWindow::program_title2;
QString MainWindow::program_title3;
QString MainWindow::program_title4;
QString MainWindow::program_title5;
QString MainWindow::program_title6;
QString MainWindow::program_title7;
QString MainWindow::program_title8;
QString MainWindow::prefix = "http://www.nhk.or.jp/gogaku/st/xml/";
QString MainWindow::suffix = "listdataflv.xml";
QString MainWindow::json_prefix = "https://www.nhk.or.jp/radioondemand/json/";
QString MainWindow::no_write_ini;

MainWindow::MainWindow( QWidget *parent )
		: QMainWindow( parent ), ui( new Ui::MainWindowClass ), downloadThread( NULL ) {
#ifdef QT4_QT5_MAC
	ini_file_path = Utility::ConfigLocationPath();
#endif
#if !defined( QT4_QT5_MAC )
	ini_file_path = Utility::applicationBundlePath();
#endif	
	ui->setupUi( this );
	settings( ReadMode );
	this->setWindowTitle( this->windowTitle() + version() );
	no_write_ini = "yes";
	
#ifdef QT4_QT5_MAC		// Macのウィンドウにはメニューが出ないので縦方向に縮める
//	setMaximumHeight( maximumHeight() - menuBar()->height() );
//	setMinimumHeight( maximumHeight() - menuBar()->height() );
	setMaximumHeight( maximumHeight() );		// ダウンロードボタンが表示されない問題対策　2022/04/16
	setMinimumHeight( maximumHeight() );		// ダウンロードボタンが表示されない問題対策　2022/04/16
//	QRect rect = geometry();
//	rect.setHeight( rect.height() - menuBar()->height() );
//	rect.moveTop( rect.top() + menuBar()->height() );	// 4.6.3だとこれがないとウィンドウタイトルがメニューバーに隠れる
//	setGeometry( rect );
#endif
#ifdef Q_OS_LINUX		// Linuxでは高さが足りなくなるので縦方向に伸ばしておく
	menuBar()->setNativeMenuBar(false);	// メニューバーが表示されなくなったに対応
	setMaximumHeight( maximumHeight() + X11_WINDOW_VERTICAL_INCREMENT );
	setMinimumHeight( maximumHeight() + X11_WINDOW_VERTICAL_INCREMENT );
	QRect rect = geometry();
	rect.setHeight( rect.height() + X11_WINDOW_VERTICAL_INCREMENT );
	setGeometry( rect );
#endif

#if !defined( QT4_QT5_MAC ) && !defined( QT4_QT5_WIN )
	QPoint bottomLeft = geometry().bottomLeft();
	bottomLeft += QPoint( 0, menuBar()->height() + statusBar()->height() + 3 );
	messagewindow.move( bottomLeft );
#endif

	// 「カスタマイズ」メニューの構築
	customizeMenu = menuBar()->addMenu( QString::fromUtf8( "カスタマイズ" ) );

	QAction* action = new QAction( QString::fromUtf8( "保存フォルダ..." ), this );
	connect( action, SIGNAL( triggered() ), this, SLOT( customizeSaveFolder() ) );
	customizeMenu->addAction( action );
	customizeMenu->addSeparator();
	action = new QAction( QString::fromUtf8( "ファイル名設定..." ), this );
	connect( action, SIGNAL( triggered() ), this, SLOT( customizeFileName() ) );
	customizeMenu->addAction( action );

	action = new QAction( QString::fromUtf8( "タイトルタグ設定..." ), this );
	connect( action, SIGNAL( triggered() ), this, SLOT( customizeTitle() ) );
	customizeMenu->addAction( action );
	customizeMenu->addSeparator();
	action = new QAction( QString::fromUtf8( "任意番組設定..." ), this );
	connect( action, SIGNAL( triggered() ), this, SLOT( customizeScramble() ) );
	customizeMenu->addAction( action );

	customizeMenu->addSeparator();
	action = new QAction( QString::fromUtf8( "設定削除（終了）..." ), this );
	connect( action, SIGNAL( triggered() ), this, SLOT( closeEvent2() ) );
	customizeMenu->addAction( action );
	
	//action = new QAction( QString::fromUtf8( "スクランブル文字列..." ), this );
	//connect( action, SIGNAL( triggered() ), this, SLOT( customizeScramble() ) );
	//customizeMenu->addAction( action );

	QString styleSheet;
	QFile real( Utility::applicationBundlePath() + STYLE_SHEET );
	if ( real.exists() ) {
		real.open( QFile::ReadOnly );
		styleSheet = QLatin1String( real.readAll() );
	} else {
		QFile res( QString( ":/" ) + STYLE_SHEET );
		res.open( QFile::ReadOnly );
		styleSheet = QLatin1String( res.readAll() );
	}
	qApp->setStyleSheet( styleSheet );

//	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // DPI support
//	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps); //HiDPI pixmaps
//	adjustSize();                             //高DPIディスプレイ対応
//	setFixedSize(size());
//      int dpiX = qApp->desktop()->logicalDpiX();
//	QFont f;
//	int defaultFontSize = f.pointSize() * ( 96.0 / dpiX );
//	f.setPointSize( defaultFontSize );
//	qApp->setFont(f);
}

MainWindow::~MainWindow() {
	if ( downloadThread ) {
		downloadThread->terminate();
		delete downloadThread;
	}
	if ( !Utility::nogui() && no_write_ini == "yes" )
		settings( WriteMode );
	delete ui;
}

void MainWindow::closeEvent( QCloseEvent *event ) {
	Q_UNUSED( event )
	if ( downloadThread ) {
		messagewindow.appendParagraph( QString::fromUtf8( "レコーディングをキャンセル中..." ) );
		download();
	}
	messagewindow.close();
	QCoreApplication::exit();
}

void MainWindow::settings( enum ReadWriteMode mode ) {
	typedef struct CheckBox {
		QAbstractButton* checkBox;
		QString key;
		QVariant defaultValue;
		QString titleKey;
		QVariant titleFormat;
		QString fileNameKey;
		QVariant fileNameFormat;
	} CheckBox;
#define DefaultTitle "%k_%Y_%M_%D"
#define DefaultFileName "%k_%Y_%M_%D.m4a"
	CheckBox checkBoxes[] = {
		{ ui->toolButton_chinese, "chinese", false, "chinese_title", DefaultTitle, "chinese_file_name", DefaultFileName },
		{ ui->toolButton_french, "french", false, "french_title", DefaultTitle, "french_file_name", DefaultFileName },
		{ ui->toolButton_french2, "french2", false, "french_title2", DefaultTitle, "french_file_name2", DefaultFileName },
		{ ui->toolButton_italian, "italian", false, "italian_title", DefaultTitle, "italian_file_name", DefaultFileName },
		{ ui->toolButton_italian2, "italian2", false, "italian_title2", DefaultTitle, "italian_file_name2", DefaultFileName },
		{ ui->toolButton_hangeul, "hangeul", false, "hangeul_title", DefaultTitle, "hangeul_file_name", DefaultFileName },
		{ ui->toolButton_german, "german", false, "german_title", DefaultTitle, "german_file_name", DefaultFileName },
		{ ui->toolButton_german2, "german2", false, "german_title2", DefaultTitle, "german_file_name2", DefaultFileName },
		{ ui->toolButton_spanish, "spanish", false, "spanish_title", DefaultTitle, "spanish_file_name", DefaultFileName },
		{ ui->toolButton_spanish2, "spanish2", false, "spanish_title2", DefaultTitle, "spanish_file_name2", DefaultFileName },
		{ ui->toolButton_russian, "russian", false, "russian_title", DefaultTitle, "russian_file_name", DefaultFileName },
		{ ui->toolButton_russian2, "russian2", false, "russian_title2", DefaultTitle, "russian_file_name2", DefaultFileName },
		{ ui->toolButton_stepup_chinese, "stepup-chinese", false, "stepup-chinese_title", DefaultTitle, "stepup-chinese_file_name", DefaultFileName },
		{ ui->toolButton_stepup_hangeul, "stepup-hangeul", false, "stepup-hangeul_title", DefaultTitle, "stepup-hangeul_file_name", DefaultFileName },
		{ ui->toolButton_arabic, "arabic", false, "arabic_title", DefaultTitle, "arabic_file_name", DefaultFileName },
		{ ui->toolButton_polish, "polish", false, "polish_title", DefaultTitle, "spolish_file_name", DefaultFileName },
//		{ ui->toolButton_basic0, "basic0", false, "basic0_title", DefaultTitle, "basic0_file_name", DefaultFileName },
//		{ ui->toolButton_basic1, "basic1", false, "basic1_title", DefaultTitle, "basic1_file_name", DefaultFileName },
//		{ ui->toolButton_basic2, "basic2", false, "basic2_title", DefaultTitle, "basic2_file_name", DefaultFileName },
//		{ ui->toolButton_basic3, "basic3", false, "basic3_title", DefaultTitle, "basic3_file_name", DefaultFileName },
//		{ ui->toolButton_timetrial, "timetrial", false, "timetrial_title", DefaultTitle, "timetrial_file_name", DefaultFileName },
//		{ ui->toolButton_kaiwa, "kaiwa", false, "kaiwa_title", DefaultTitle, "kaiwa_file_name", DefaultFileName },
//		{ ui->toolButton_business1, "business1", false, "business1_title", DefaultTitle, "business1_file_name", DefaultFileName },
//		{ ui->toolButton_business2, "business2", false, "business2_title", DefaultTitle, "business2_file_name", DefaultFileName },
//		{ ui->toolButton_gakusyu, "gakusyu", false, "gakusyu_title", DefaultTitle, "gakusyu_file_name", DefaultFileName },
//		{ ui->toolButton_gendai, "gendai", false, "gendai_title", DefaultTitle, "gendai_file_name", DefaultFileName },
//		{ ui->toolButton_enjoy, "enjoy", false, "enjoy_title", DefaultTitle, "enjoy_file_name", DefaultFileName },
//		{ ui->toolButton_vrradio, "vrradio", false, "vrradio_title", DefaultTitle, "vrradio_file_name", DefaultFileName },
		{ ui->toolButton_optional1, "optional_1", false, "optional1_title", DefaultTitle, "optional1_file_name", DefaultFileName },
		{ ui->toolButton_optional2, "optional_2", false, "optional2_title", DefaultTitle, "optional2_file_name", DefaultFileName },
		{ ui->toolButton_optional3, "optional_3", false, "optional3_title", DefaultTitle, "optional3_file_name", DefaultFileName },
		{ ui->toolButton_optional4, "optional_4", false, "optional4_title", DefaultTitle, "optional4_file_name", DefaultFileName },
		{ ui->toolButton_optional5, "optional_5", false, "optional5_title", DefaultTitle, "optional5_file_name", DefaultFileName },
		{ ui->toolButton_optional6, "optional_6", false, "optional6_title", DefaultTitle, "optional6_file_name", DefaultFileName },
		{ ui->toolButton_optional7, "optional_7", false, "optional7_title", DefaultTitle, "optional7_file_name", DefaultFileName },
		{ ui->toolButton_optional8, "optional_8", false, "optional8_title", DefaultTitle, "optional8_file_name", DefaultFileName },
		{ ui->checkBox_13, "charo", false, "charo_title", DefaultTitle, "charo_file_name", DefaultFileName },
		{ ui->checkBox_14, "e-news", false, "e-news_title", DefaultTitle, "e-news_file_name", DefaultFileName },
		{ ui->checkBox_shower, "shower", false, "shower_title", DefaultTitle, "shower_file_name", DefaultFileName },
		{ ui->checkBox_15, "e-news-reread", false, "e-news-reread_title", DefaultTitle, "e-news-reread_file_name", DefaultFileName },
		{ ui->toolButton_skip, "skip", true, "", "", "", "" },
		{ ui->checkBox_keep_on_error, "keep_on_error", false, "", "", "", "" },
		{ ui->checkBox_this_week, "this_week", true, "", "", "", "" },
		{ ui->checkBox_next_week, "next_week", false, "", "", "", "" },
		{ ui->checkBox_next_week2, "next_week", false, "", "", "", "" },
		{ ui->checkBox_past_week, "past_week", false, "", "", "", "" },
		{ ui->toolButton_detailed_message, "detailed_message", false, "", "", "", "" },
		{ NULL, NULL, false, "", "", "", "" }
	};
	typedef struct ComboBox {
		QComboBox* comboBox;
		QString key;
		QVariant defaultValue;
	} ComboBox;
	ComboBox comboBoxes[] = {
		{ ui->comboBox_enews, "e-news-index", ENewsSaveBoth },
		{ ui->comboBox_shower, "shower_index", ENewsSaveBoth },
		{ NULL, NULL, false }
	};
	ComboBox textComboBoxes[] = {
		{ ui->comboBox_extension, "audio_extension", "m4a" },	// 拡張子のデフォルトを「mp3」から「m4a」に変更。
		{ NULL, NULL, false }
	};
	
	typedef struct CheckBox2 {
		QAbstractButton* checkBox;
		QString titleKey;
		QVariant defaultValue;
		QString idKey;
		QVariant defaul;
		QString id;
	} CheckBox2;
	CheckBox2 checkBoxes2[] = {
		{ ui->toolButton_optional1, "opt_title1", Program_TITLE1, "optional1", OPTIONAL1, optional1 },
		{ ui->toolButton_optional2, "opt_title2", Program_TITLE2, "optional2", OPTIONAL2, optional2 },
		{ ui->toolButton_optional3, "opt_title3", Program_TITLE3, "optional3", OPTIONAL3, optional3 },
		{ ui->toolButton_optional4, "opt_title4", Program_TITLE4, "optional4", OPTIONAL4, optional4 },
		{ ui->toolButton_optional5, "opt_title5", Program_TITLE5, "optional5", OPTIONAL5, optional5 },
		{ ui->toolButton_optional6, "opt_title6", Program_TITLE6, "optional6", OPTIONAL6, optional6 },
		{ ui->toolButton_optional7, "opt_title7", Program_TITLE7, "optional7", OPTIONAL7, optional7 },
		{ ui->toolButton_optional8, "opt_title8", Program_TITLE8, "optional8", OPTIONAL8, optional8 },
		{ ui->toolButton_french, "french_buttom_title", "まいにちフランス語【月/火/水】", "", "", "" },
		{ ui->toolButton_french2, "french2_buttom_title", "まいにちフランス語【木/金】", "", "", "" },
		{ ui->toolButton_italian, "italian_buttom_title", "まいにちイタリア語【月/火/水】", "", "", "" },
		{ ui->toolButton_italian2, "italian2_buttom_title", "まいにちイタリア語【木/金】", "", "", "" },
		{ ui->toolButton_german, "german_buttom_title", "まいにちドイツ語【月/火/水】", "", "", "" },
		{ ui->toolButton_german2, "german2_buttom_title", "まいにちドイツ語【木/金】", "", "", "" },
		{ ui->toolButton_spanish, "spanish_buttom_title", "まいにちスペイン語【月/火/水】", "", "", "" },
		{ ui->toolButton_spanish2, "spanish2_buttom_title", "まいにちスペイン語【木/金】", "", "", "" },
		{ ui->toolButton_russian, "russian_buttom_title", "まいにちロシア語【月/火/水】", "", "", "" },
		{ ui->toolButton_russian2, "russian2_buttom_title", "まいにちロシア語【木/金】", "", "", "" },
		{ NULL, NULL, "", NULL, "", "" }
	};

#if !defined( QT4_QT5_MAC )
	QSettings settings( Utility::applicationBundlePath() + INI_FILE, QSettings::IniFormat );
#endif
#ifdef QT4_QT5_MAC
	QSettings settings( Utility::ConfigLocationPath() + INI_FILE, QSettings::IniFormat );
#endif

	settings.beginGroup( SETTING_GROUP );

	if ( mode == ReadMode ) {	// 設定読み込み
		QVariant saved;
#if !defined( QT4_QT5_MAC )
//#if defined( QT4_QT5_MAC ) || defined( QT4_QT5_WIN )	// X11では正しく憶えられないので位置をリストアしない(2022/11/01:Linux向けに変更）
		saved = settings.value( SETTING_GEOMETRY );
		if ( saved.type() == QVariant::Invalid )
			move( 70, 22 );
		else {
			// ウィンドウサイズはバージョン毎に変わる可能性があるのでウィンドウ位置だけリストアする
			QSize windowSize = size();
			restoreGeometry( saved.toByteArray() );
			resize( windowSize );
		}
//#endif                                              　//(2022/11/01:Linux向けに変更） 
#endif
#ifdef QT4_QT5_MAC
		saved = settings.value( SETTING_MAINWINDOW_POSITION );
		if ( saved.type() == QVariant::Invalid ) {
			move( 70, 22 );
			QRect rect = geometry();
			rect.setHeight( rect.height() );
			rect.moveTop( rect.top() );
			setGeometry( rect );
		} else {
			QSize windowSize = size();
			move( saved.toPoint() );
			resize( windowSize );
		}
		saved = settings.value( SETTING_WINDOWSTATE );
		if ( !(saved.type() == QVariant::Invalid) )
			restoreState( saved.toByteArray() );
#endif

		saved = settings.value( SETTING_SAVE_FOLDER );
#if !defined( QT4_QT5_MAC )
		outputDir = saved.type() == QVariant::Invalid ? Utility::applicationBundlePath() : saved.toString();
#endif
#ifdef QT4_QT5_MAC
		if ( saved.type() == QVariant::Invalid ) {
			outputDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
			MainWindow::customizeSaveFolder();
		} else
			outputDir = saved.toString();
#endif
		saved = settings.value( SETTING_SCRAMBLE );
		scramble = saved.type() == QVariant::Invalid ? "" : saved.toString();

		saved = settings.value( SETTING_SCRAMBLE_URL1 );
		scrambleUrl1 = saved.type() == QVariant::Invalid ? SCRAMBLE_URL1 : saved.toString();
		saved = settings.value( SETTING_SCRAMBLE_URL2 );
		scrambleUrl2 = saved.type() == QVariant::Invalid ? SCRAMBLE_URL2 : saved.toString();
		
		for ( int i = 0; checkBoxes[i].checkBox != NULL; i++ ) {
			checkBoxes[i].checkBox->setChecked( settings.value( checkBoxes[i].key, checkBoxes[i].defaultValue ).toBool() );
		}
		for ( int i = 0; comboBoxes[i].comboBox != NULL; i++ )
			comboBoxes[i].comboBox->setCurrentIndex( settings.value( comboBoxes[i].key, comboBoxes[i].defaultValue ).toInt() );
		for ( int i = 0; textComboBoxes[i].comboBox != NULL; i++ ) {
			QString extension = settings.value( textComboBoxes[i].key, textComboBoxes[i].defaultValue ).toString();
			textComboBoxes[i].comboBox->setCurrentIndex( textComboBoxes[i].comboBox->findText( extension ) );
		}
		for ( int i = 0; checkBoxes2[i].checkBox != NULL; i++ ) {
			checkBoxes2[i].checkBox->setText( settings.value( checkBoxes2[i].titleKey, checkBoxes2[i].defaultValue ).toString() );
			if ( checkBoxes2[i].idKey == NULL ) continue;
			optional[i] = settings.value( checkBoxes2[i].idKey, checkBoxes2[i].defaul ).toString();
			switch ( i ) {
				case 0: optional1 = settings.value( checkBoxes2[i].idKey, checkBoxes2[i].defaul ).toString(); break;
				case 1: optional2 = settings.value( checkBoxes2[i].idKey, checkBoxes2[i].defaul ).toString(); break;
				case 2: optional3 = settings.value( checkBoxes2[i].idKey, checkBoxes2[i].defaul ).toString(); break;
				case 3: optional4 = settings.value( checkBoxes2[i].idKey, checkBoxes2[i].defaul ).toString(); break;
				case 4: optional5 = settings.value( checkBoxes2[i].idKey, checkBoxes2[i].defaul ).toString(); break;
				case 5: optional6 = settings.value( checkBoxes2[i].idKey, checkBoxes2[i].defaul ).toString(); break;
				case 6: optional7 = settings.value( checkBoxes2[i].idKey, checkBoxes2[i].defaul ).toString(); break;
				case 7: optional8 = settings.value( checkBoxes2[i].idKey, checkBoxes2[i].defaul ).toString(); break;
				default: break;
			}
		}
	} else {	// 設定書き出し
#if !defined( QT4_QT5_MAC )
		settings.setValue( SETTING_GEOMETRY, saveGeometry() );
#endif
#ifdef QT4_QT5_MAC
		settings.setValue( SETTING_WINDOWSTATE, saveState());
		settings.setValue( SETTING_MAINWINDOW_POSITION, pos() );
#endif
		if ( outputDirSpecified )
			settings.setValue( SETTING_SAVE_FOLDER, outputDir );
		settings.setValue( SETTING_SCRAMBLE, scramble );
		settings.setValue( SETTING_SCRAMBLE_URL1, scrambleUrl1 );
		settings.setValue( SETTING_SCRAMBLE_URL2, scrambleUrl2 );
		
		for ( int i = 0; checkBoxes[i].checkBox != NULL; i++ ) {
			settings.setValue( checkBoxes[i].key, checkBoxes[i].checkBox->isChecked() );
		}
		for ( int i = 0; comboBoxes[i].comboBox != NULL; i++ )
			settings.setValue( comboBoxes[i].key, comboBoxes[i].comboBox->currentIndex() );
		for ( int i = 0; textComboBoxes[i].comboBox != NULL; i++ )
			settings.setValue( textComboBoxes[i].key, textComboBoxes[i].comboBox->currentText() );
		for ( int i = 0; checkBoxes2[i].checkBox != NULL; i++ ) {
			settings.setValue( checkBoxes2[i].titleKey, checkBoxes2[i].checkBox->text() );
			if ( checkBoxes2[i].idKey == NULL ) continue;
			settings.setValue( checkBoxes2[i].idKey, checkBoxes2[i].id );
		}
	}

	settings.endGroup();
}

void MainWindow::customizeTitle() {
	CustomizeDialog dialog( Ui::TitleMode );
	dialog.exec();
}

void MainWindow::customizeFileName() {
	CustomizeDialog dialog( Ui::FileNameMode );
	dialog.exec();
}

void MainWindow::customizeSaveFolder() {
	QString dir = QFileDialog::getExistingDirectory( 0, QString::fromUtf8( "書き込み可能な保存フォルダを指定してください" ),
									   outputDir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
	if ( dir.length() ) {
		outputDir = dir + QDir::separator();
		outputDirSpecified = true;
	}
}

void MainWindow::customizeScramble() {
//	ScrambleDialog dialog( scramble );
//	dialog.exec();
//	scramble = dialog.scramble();

	QString optional_temp[] = { optional1, optional2, optional3, optional4, optional5, optional6, optional7, optional8, NULL };
	ScrambleDialog dialog( optional1, optional2, optional3, optional4, optional5, optional6, optional7, optional8 );
    if (dialog.exec() ) {
    	QRegExp r1( "[0-9]{4}" );
	for ( int i = 0; optional_temp[i] != NULL; i++ ) 
	    	if ( r1.exactMatch(optional_temp[i]) ) optional_temp[i] += "_01";

	QString scramble_temp[] = { dialog.scramble1(), dialog.scramble2(), dialog.scramble3(), dialog.scramble4(), dialog.scramble5(), dialog.scramble6(), dialog.scramble7(), dialog.scramble8(), NULL };	
	QString title[8];
	for ( int i = 0; scramble_temp[i] != NULL; i++ ) {
		optional[i] = scramble_temp[i];
		if ( r1.exactMatch(optional[i]) ) optional[i] += "_01" ;
		title[i] = getJsonData( optional[i] );
	}
	optional1 = optional[0]; optional2 = optional[1]; optional3 = optional[2]; optional4 = optional[3];
	optional5 = optional[4]; optional6 = optional[5]; optional7 = optional[6]; optional8 = optional[7];
	program_title1 = title[0];  program_title2 = title[1]; program_title3 = title[2]; program_title4 = title[3];
	program_title5 = title[4];  program_title6 = title[5]; program_title7 = title[6]; program_title8 = title[7];

	QString program_title[] = { program_title1, program_title2, program_title3, program_title4, program_title5, program_title6, program_title7, program_title8, NULL };
	QAbstractButton* checkboxx[] = { ui->toolButton_optional1, ui->toolButton_optional2,
					 ui->toolButton_optional3, ui->toolButton_optional4,
					 ui->toolButton_optional5, ui->toolButton_optional6,
					 ui->toolButton_optional7, ui->toolButton_optional8,
					 NULL
		 	};
	for ( int i = 0; program_title[i] != NULL; i++ ) {
		if ( optional[i] != optional_temp[i] ) {
				checkboxx[i]->setChecked(false);
				checkboxx[i]->setText( QString( program_title[i] ) );
		}
	}
	optional1 = optional[0]; optional2 = optional[1]; optional3 = optional[2]; optional4 = optional[3];
	optional5 = optional[4]; optional6 = optional[5]; optional7 = optional[6]; optional8 = optional[7];
 	ScrambleDialog dialog( optional1, optional2, optional3, optional4, optional5, optional6, optional7, optional8 );
   	setButtomTitle();
    }
}

void MainWindow::download() {	//「レコーディング」または「キャンセル」ボタンが押されると呼び出される
	if ( !downloadThread ) {	//レコーディング実行
		if ( messagewindow.text().length() > 0 )
			messagewindow.appendParagraph( "\n----------------------------------------" );
		ui->downloadButton->setEnabled( false );
		downloadThread = new DownloadThread( ui );
		connect( downloadThread, SIGNAL( finished() ), this, SLOT( finished() ) );
		connect( downloadThread, SIGNAL( critical( QString ) ), &messagewindow, SLOT( appendParagraph( QString ) ), Qt::BlockingQueuedConnection );
		connect( downloadThread, SIGNAL( information( QString ) ), &messagewindow, SLOT( appendParagraph( QString ) ), Qt::BlockingQueuedConnection );
		connect( downloadThread, SIGNAL( current( QString ) ), &messagewindow, SLOT( appendParagraph( QString ) ) );
		connect( downloadThread, SIGNAL( messageWithoutBreak( QString ) ), &messagewindow, SLOT( append( QString ) ) );
		downloadThread->start();
		ui->downloadButton->setText( QString::fromUtf8( "キャンセル" ) );
		ui->downloadButton->setEnabled( true );
	} else {	//キャンセル
		downloadThread->disconnect();	//wait中にSIGNALが発生するとデッドロックするためすべてdisconnect
		finished();
	}
}

QString MainWindow::getJsonData( QString url ) {
	QString attribute;
	attribute.clear() ;
    	QEventLoop eventLoop;
	QNetworkAccessManager mgr;
 	QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
 	QRegExp r1( "[0-9]{4}" );
    	if ( r1.exactMatch( url ) ) url += "_01";
	const QString jsonUrl = json_prefix + url.left(4) + "/bangumi_" + url + ".json";
	QUrl url_json( jsonUrl );
	QNetworkRequest req;
	req.setUrl(url_json);
	QNetworkReply *reply = mgr.get(req);
	eventLoop.exec(); 
	
	if (reply->error() == QNetworkReply::NoError) {
		QString strReply = (QString)reply->readAll();
		QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
		QJsonObject jsonObject = jsonResponse.object();
		QJsonObject jsonObj = jsonResponse.object();
    
		QJsonArray jsonArray = jsonObject[ "main" ].toArray();
		QJsonObject objx2 = jsonObject[ "main" ].toObject();
		attribute = objx2[ "program_name" ].toString().replace( "　", " " );
		if ( !(objx2[ "corner_name" ].toString().isNull()) ) attribute = objx2[ "corner_name" ].toString().replace( "　", " " );
		    for (ushort i = 0xFF1A; i < 0xFF5F; ++i) {
		        attribute = attribute.replace(QChar(i), QChar(i - 0xFEE0));
		    }
		    for (ushort i = 0xFF10; i < 0xFF1A; ++i) {
		        attribute = attribute.replace( QChar(i - 0xFEE0), QChar(i) );
		    }
		attribute = attribute.left( 20 );
	}
	return attribute;
}

void MainWindow::setButtomTitle() {

	QString json_paths2[] = { 
		"0953_01", "4412_01",
		"0943_01", "4410_01",
		"0948_01", "4413_01",
		"0946_01", "4411_01", 
		"0956_01", "4414_01", 
		NULL
	};

	QAbstractButton* checkbox[] = {
		ui->toolButton_french, ui->toolButton_french2, 
		ui->toolButton_german, ui->toolButton_german2,
		ui->toolButton_spanish, ui->toolButton_spanish2,
		ui->toolButton_italian, ui->toolButton_italian2,
		ui->toolButton_russian, ui->toolButton_russian2,
		NULL
	};
	
	bool chk_status;
	for ( int i = 0; json_paths2[i] != NULL; i++ ) {
	if ( checkbox[i]->text().remove( "✓ " ) == getJsonData( json_paths2[i] )) continue;
	if ( checkbox[i]->isChecked()) chk_status = true; else chk_status = false;
	checkbox[i]->setChecked( false );
	checkbox[i]->setText( QString( getJsonData( json_paths2[i] ) ) );
	if ( chk_status ) checkbox[i]->setChecked( chk_status );
	}

	return;
}

void MainWindow::toggled( bool checked ) {
	QObject* sender = this->sender();
	if ( sender ) {
		QToolButton* button = (QToolButton*)sender;
		QString text = button->text();
		if ( checked )
			text.insert( 0, QString::fromUtf8( "✓ " ) );
		else
			text.remove( 0, 2 );
		button->setText( text );
	}
}

void MainWindow::finished() {
	if ( downloadThread ) {
		ui->downloadButton->setEnabled( false );
		if ( downloadThread->isRunning() ) {	//キャンセルでMainWindow::downloadから呼ばれた場合
			downloadThread->cancel();
			downloadThread->wait();
			messagewindow.appendParagraph( QString::fromUtf8( "レコーディングをキャンセルしました。" ) );
		}
		delete downloadThread;
		downloadThread = NULL;
		ui->downloadButton->setText( QString::fromUtf8( "レコーディング" ) );
		ui->downloadButton->setEnabled( true );
	}
	//ui->label->setText( "" );
	if ( Utility::nogui() )
		QCoreApplication::exit();
}

void MainWindow::closeEvent2( ) {
	int res = QMessageBox::question(this, tr("設定削除"), tr("削除しますか？"));
	if (res == QMessageBox::Yes) {
	no_write_ini = "no";
	
	QFile::remove( ini_file_path + INI_FILE );
	
	if ( downloadThread ) {
		messagewindow.appendParagraph( QString::fromUtf8( "レコーディングをキャンセル中..." ) );
		download();
	}
	messagewindow.close();
	QCoreApplication::exit();
	}
}

