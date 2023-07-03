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

#include "scrambledialog.h"
#include "ui_scrambledialog.h"
#include "mainwindow.h"

QString ScrambleDialog::optional1;
QString ScrambleDialog::optional2;
QString ScrambleDialog::optional3;
QString ScrambleDialog::optional4;
QString ScrambleDialog::optional5;
QString ScrambleDialog::optional6;
QString ScrambleDialog::optional7;
QString ScrambleDialog::optional8;

QString ScrambleDialog::opt1[] = {
		"6805", //小学生の基礎英語
		"6806", //中学生の基礎英語 レベル1
		"6807", //中学生の基礎英語 レベル2
		"6808", //中高生の基礎英語 in English
		"0916", // ラジオ英会話
		"6809", // ラジオビジネス英語
		"3064", // エンジョイ・シンプル・イングリッシュ
		"2331"	// 英会話タイムトライアル
};
QString ScrambleDialog::opt2[] = {
		"6806", //中学生の基礎英語 レベル1
		"6807", //中学生の基礎英語 レベル2
		"6808", //中高生の基礎英語 in English
		"0916", // ラジオ英会話
		"6809", // ラジオビジネス英語
		"3064", // エンジョイ・シンプル・イングリッシュ
		"4121", //ボキャブライダー
		"7512"  //ニュースで学ぶ「現代英語」
};
QString ScrambleDialog::opt3[] = {
		"7155", //Living in Japan
		"0701", //やさしい日本語
		"7629", //Learn Japanese from the News
		"7512", //ニュースで学ぶ「現代英語」
		"0164", //青春アドベンチャー
		"0930", //新日曜名作座
		"8062", //朗読
		"0058"  //FMシアター

};
QString ScrambleDialog::opt4[] = {
		"0953", //まいにちフランス語 入門編
		"0943", //まいにちドイツ語 入門編／初級編
		"0946", //まいにちイタリア語 入門編
		"0948", //まいにちスペイン語 入門編／初級編
		"0956", //まいにちロシア語 入門編
		"1893", //ポルトガル語講座 入門
		"0915", //まいにち中国語
		"0951"  //まいにちハングル講座
};
QString ScrambleDialog::opt5[] = {
		"4412", //まいにちフランス語 応用編
		"4410", //まいにちドイツ語 応用編
		"4411", //まいにちイタリア語 応用編
		"4413", //まいにちスペイン語 中級編／応用編
		"4414", //まいにちロシア語 応用編
		"2769", //ポルトガル語ステップアップ
		"6581", //ステップアップ中国語
		"6810"  //ステップアップ ハングル講座
};

QString ScrambleDialog::opt6[] = {
		"1928", //カルチャーラジオ 芸術その魅力
		"1927", //カルチャーラジオ 歴史再発見
		"1890", //カルチャーラジオ NHKラジオアーカイブス
		"1940", //カルチャーラジオ 日曜カルチャー
		"6324", //高橋源一郎の飛ぶ教室
		"1929", //カルチャーラジオ 文学の世界
		"0442", //音の風景
		"1928"  //おしゃべりな古典教室
};

ScrambleDialog::ScrambleDialog( QString optional1, QString optional2, QString optional3, QString optional4, QString optional5, QString optional6, QString optional7, QString optional8, QWidget *parent )
//ScrambleDialog::ScrambleDialog( QString scramble, QWidget *parent )
		: QDialog(parent), ui(new Ui::ScrambleDialog) {
    ui->setupUi(this);
	QString optional[] = { optional1, optional2, optional3, optional4, optional5, optional6, optional7, optional8 };
	QLineEdit*  Button2[] = { ui->optional1, ui->optional2, ui->optional3, ui->optional4, ui->optional5, ui->optional6, ui->optional7, ui->optional8, NULL };
	for ( int i = 0 ; i < 8 ; i++ ) Button2[i]->setText( optional[i] );
	ui->radioButton_9->setChecked(true);
}

ScrambleDialog::~ScrambleDialog() {
//    delete ui;
}

QString ScrambleDialog::scramble_set( QString opt, int i ) {
	QString opt_set[] = { opt1[i], opt2[i], opt3[i], opt4[i], opt5[i], opt6[i] };
	QAbstractButton*  Button[] = { ui->radioButton, ui->radioButton_1, ui->radioButton_2, ui->radioButton_3, ui->radioButton_4, ui->radioButton_5, NULL };
	QLineEdit*  Button2[] = { ui->optional1, ui->optional2, ui->optional3, ui->optional4, ui->optional5, ui->optional6, ui->optional7, ui->optional8, NULL };
	for ( int i = 0 ; Button[i] != NULL ; i++ ) 
		if (Button[i]->isChecked())	opt = opt_set[i];
	if (!(ui->radioButton_9->isChecked())) 	Button2[i]->setText( opt );
	return opt;
}
QString ScrambleDialog::scramble1() {
	optional1 = scramble_set( optional1, 0);
	return ui->optional1->text();
}
QString ScrambleDialog::scramble2() {
	optional2 = scramble_set( optional2, 1 );
	return ui->optional2->text();
}
QString ScrambleDialog::scramble3() {
	optional3 = scramble_set( optional3, 2 );
	return ui->optional3->text();
}
QString ScrambleDialog::scramble4() {
	optional4 = scramble_set( optional4, 3 );
	return ui->optional4->text();
}
QString ScrambleDialog::scramble5() {
	optional5 = scramble_set( optional5, 4 );
	return ui->optional5->text();
}
QString ScrambleDialog::scramble6() {
	optional6 = scramble_set( optional6, 5 );
	return ui->optional6->text();
}
QString ScrambleDialog::scramble7() {
	optional7 = scramble_set( optional7, 6 );
	return ui->optional7->text();
}
QString ScrambleDialog::scramble8() {
	optional8 = scramble_set( optional8, 7 );
	return ui->optional8->text();
}

