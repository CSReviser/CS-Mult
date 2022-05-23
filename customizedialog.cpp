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

#include "customizedialog.h"
#include "mainwindow.h"
#include "utility.h"

#include <QDir>
#include <QSettings>
#include <QDesktopWidget>

#define SETTING_GROUP "CustomizeDialog"
#define DefaultTitle "%k_%Y_%M_%D"
#define DefaultFileName "%k_%Y_%M_%D"

#ifdef QT4_QT5_WIN
#define STYLE_SHEET "stylesheet-win.qss"
#else
#ifdef QT4_QT5_MAC
#define STYLE_SHEET "stylesheet-mac.qss"
#else
#define STYLE_SHEET "stylesheet-ubu.qss"
#endif
#endif

typedef struct LineEdit {
	QLineEdit* lineEdit;
	QString titleKey;
	QString fileNameKey;
} LineEdit;

QStringList CustomizeDialog::courses = QStringList()
		<< QString::fromUtf8( "まいにちフランス語【入門編】" ) << QString::fromUtf8( "まいにちフランス語【応用編】" )
		<< QString::fromUtf8( "まいにちドイツ語【入門編】" ) << QString::fromUtf8( "まいにちドイツ語【応用編】" ) 
		<< QString::fromUtf8( "まいにちスペイン語【入門編】" ) << QString::fromUtf8( "まいにちスペイン語【応用編】" )
		<< QString::fromUtf8( "まいにちイタリア語【入門編】" ) << QString::fromUtf8( "まいにちイタリア語【応用編】" ) 
		<< QString::fromUtf8( "まいにちロシア語【入門編】" ) << QString::fromUtf8( "まいにちロシア語【応用編】" )
		<< QString::fromUtf8( "まいにち中国語" ) << QString::fromUtf8( "ステップアップ中国語" ) 
		<< QString::fromUtf8( "まいにちハングル講座" ) << QString::fromUtf8( "ステップアップハングル講座" )
		<< QString::fromUtf8( "小学生の基礎英語" ) << QString::fromUtf8( "中学生の基礎英語【レベル1】" ) 
		<< QString::fromUtf8( "中学生の基礎英語【レベル2】" ) << QString::fromUtf8( "中高生の基礎英語_in_English" )
		<< QString::fromUtf8( "ラジオ英会話" ) << QString::fromUtf8( "英会話タイムトライアル" ) 
		<< QString::fromUtf8( "ラジオビジネス英語" ) << QString::fromUtf8( "エンジョイ・シンプル・イングリッシュ" );
QStringList CustomizeDialog::titleKeys = QStringList()
		<< "french_title" << "french_title2"
		<< "german_title" << "german_title2"
		<< "spanish_title" << "spanish_title2"
		<< "italian_title" << "italian_title2"
		<< "russian_title" << "russian_title2"
		<< "chinese_title" << "stepup-chinese_title" 
		<< "hangeul_title" << "stepup-hangeul_title"
		<< "basic0_title" << "basic1_title" << "basic2_title" << "basic3_title"
		<< "kaiwa_title" << "timetrial_title"
		<< "business1_title" << "enjoy_title";
QStringList CustomizeDialog::fileNameKeys = QStringList()
		<< "french_file_name" << "french_file_name2"
		<< "german_file_name" << "german_file_name2"
		<< "spanish_file_name" << "spanish_file_name2"
		<< "italian_file_name" << "italian_file_name2"
		<< "russian_file_name" << "russian_file_name2"
		<< "chinese_file_name" << "stepup-chinese_file_name" 
		<< "hangeul_file_name" << "stepup-hangeul_file_name"
		<< "basic0_file_name" << "basic1_file_name" << "basic2_file_name" << "basic3_file_name"
		<< "kaiwa_file_name" << "timetrial_file_name"
		<< "business1_file_name" << "enjoy_file_name";

void CustomizeDialog::formats( QString course, QString& titleFormat, QString& fileNameFormat ) {
        course.replace( QString::fromUtf8( "【初級編】" ), QString::fromUtf8( "【入門編】" ) );
        course.replace( QString::fromUtf8( "【中級編】" ), QString::fromUtf8( "【応用編】" ) );
	int index = courses.indexOf( course );
	if ( index >= 0 ) {
		QString path = Utility::applicationBundlePath();
		QSettings settings( path + INI_FILE, QSettings::IniFormat );
		settings.beginGroup( SETTING_GROUP );
		titleFormat = settings.value( titleKeys[index], DefaultTitle ).toString();
		fileNameFormat = settings.value( fileNameKeys[index], DefaultFileName ).toString();
		settings.endGroup();
	} else {
		titleFormat = DefaultTitle;
		fileNameFormat = DefaultFileName;
	}
}

CustomizeDialog::CustomizeDialog( Ui::DialogMode mode, QWidget *parent ) :
	QDialog(parent), mode(mode)
{
		ui.setupUi(this);

		this->setWindowTitle( mode == Ui::TitleMode ? QString::fromUtf8( "タイトルタグ設定" ) : QString::fromUtf8( "ファイル名設定" ) );
		settings( false );
		connect( this, SIGNAL( accepted() ), this, SLOT( accepted() ) );
}

void CustomizeDialog::settings( bool write ) {
	QLineEdit* lineEdits[] = {
		ui.lineEdit_0, ui.lineEdit_0_1, ui.lineEdit_1, ui.lineEdit_1_1,
		ui.lineEdit_2, ui.lineEdit_2_1, ui.lineEdit_3, ui.lineEdit_3_1,
		ui.lineEdit_4, ui.lineEdit_4_1,
		ui.lineEdit_5, ui.lineEdit_6,
		ui.lineEdit_7, ui.lineEdit_8, ui.lineEdit_9, ui.lineEdit_10,
		ui.lineEdit_11, ui.lineEdit_12, ui.lineEdit_13, ui.lineEdit_14,
		ui.lineEdit_15, ui.lineEdit_16,
		NULL
	};

	QString path = Utility::applicationBundlePath();
	QSettings settings( path + INI_FILE, QSettings::IniFormat );
	settings.beginGroup( SETTING_GROUP );

	adjustSize();                             //高DPIディスプレイ対応
	setFixedSize(size());
        int dpiX = qApp->desktop()->logicalDpiX();
	QFont f;
	int defaultFontSize = f.pointSize() * ( 96.0 / dpiX );
	f.setPointSize( defaultFontSize );
	qApp->setFont(f);

	if ( !write ) {
		for ( int i = 0; lineEdits[i] != NULL; i++ ) {
			QString format = mode == Ui::TitleMode ?
							 settings.value( titleKeys[i], DefaultTitle ).toString() :
							 settings.value( fileNameKeys[i], DefaultFileName ).toString();
			lineEdits[i]->setText( format );
		}
	} else {
		for ( int i = 0; lineEdits[i] != NULL; i++ ) {
			QString text = lineEdits[i]->text();
			if ( mode == Ui::TitleMode )
				settings.setValue( titleKeys[i], text.length() == 0 ? DefaultTitle : text );
			else
				settings.setValue( fileNameKeys[i], text.length() == 0 ? DefaultFileName : text );
		}
	}
	settings.endGroup();
}

void CustomizeDialog::accepted() {
	settings( true );
}
