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

#define OPTIONAL1 "french/kouza"
#define OPTIONAL2 "french/kouza2"
#define OPTIONAL3 "german/kouza"
#define OPTIONAL4 "german/kouza2"

QString ScrambleDialog::optional1;
QString ScrambleDialog::optional2;
QString ScrambleDialog::optional3;
QString ScrambleDialog::optional4;
QString ScrambleDialog::optional5;
QString ScrambleDialog::optional6;
QString ScrambleDialog::optional7;
QString ScrambleDialog::optional8;

QString ScrambleDialog::opt1[] = {
		"0953", //まいにちフランス語 入門編
		"4412", //まいにちフランス語 応用編
		"0943", //まいにちドイツ語 入門編／初級編
		"4410", //まいにちドイツ語 応用編
		"7155",	// Living in Japan
		"0701",	// やさしい日本語
		"7629",	// Learn Japanese from the News
		"7512"	// ニュースで学ぶ「現代英語」
};
QString ScrambleDialog::opt2[] = {
		"0953", //まいにちフランス語 入門編
		"4412", //まいにちフランス語 応用編
		"0943", //まいにちドイツ語 入門編／初級編
		"4410", //まいにちドイツ語 応用編
		"6806", //中学生の基礎英語 レベル1
		"6807", //中学生の基礎英語 レベル2
		"6808", //中高生の基礎英語 in English
		"3064"  //エンジョイ・シンプル・イングリッシュ
};
QString ScrambleDialog::opt3[] = {
		"0953", //まいにちフランス語 入門編
		"4412", //まいにちフランス語 応用編
		"0943", //まいにちドイツ語 入門編／初級編
		"4410", //まいにちドイツ語 応用編
		"0916", //ラジオ英会話
		"2331", //英会話タイムトライアル
		"6809", //ラジオビジネス英語
		"4121"  //ボキャブライダー
};
QString ScrambleDialog::opt4[] = {
		"0953", //まいにちフランス語 入門編
		"4412", //まいにちフランス語 応用編
		"0943", //まいにちドイツ語 入門編／初級編
		"4410", //まいにちドイツ語 応用編
		"0164", //青春アドベンチャー
		"0930", //新日曜名作座
		"8062", //朗読
		"0058"  //FMシアター
};
QString ScrambleDialog::opt5[] = {
		"0953", //まいにちフランス語 入門編
		"4412", //まいにちフランス語 応用編
		"0943", //まいにちドイツ語 入門編／初級編
		"4410", //まいにちドイツ語 応用編
		"1928", //カルチャーラジオ 芸術その魅力
		"1927", //カルチャーラジオ 歴史再発見
		"1890", //カルチャーラジオ NHKラジオアーカイブス
		"1940"  //カルチャーラジオ 日曜カルチャー
};

QString ScrambleDialog::opt6[] = {
		"0953", //まいにちフランス語 入門編
		"4412", //まいにちフランス語 応用編
		"0943", //まいにちドイツ語 入門編／初級編
		"4410", //まいにちドイツ語 応用編
		"6324", //高橋源一郎の飛ぶ教室
		"1929", //カルチャーラジオ 文学の世界
		"0442", //音の風景
		"1928"  //おしゃべりな古典教室
};

ScrambleDialog::ScrambleDialog( QString optional1, QString optional2, QString optional3, QString optional4, QString optional5, QString optional6, QString optional7, QString optional8, QWidget *parent )
//ScrambleDialog::ScrambleDialog( QString scramble, QWidget *parent )
		: QDialog(parent), ui(new Ui::ScrambleDialog) {
    ui->setupUi(this);
//	ui->scramble->setText( scramble );
	ui->optional1->setText( optional1 ),
	ui->optional2->setText( optional2 ),
	ui->optional3->setText( optional3 ),
	ui->optional4->setText( optional4 );
	ui->optional5->setText( optional5 ),
	ui->optional6->setText( optional6 ),
	ui->optional7->setText( optional7 ),
	ui->optional8->setText( optional8 );
	ui->radioButton_9->setChecked(true);
}

ScrambleDialog::~ScrambleDialog() {
    delete ui;
}

//QString ScrambleDialog::scramble() {
//	return ui->scramble->text();
//}

QString ScrambleDialog::scramble1() {
	if (ui->radioButton->isChecked())	optional1 = opt1[0];
	if (ui->radioButton_1->isChecked()) 	optional1 = opt2[0];
	if (ui->radioButton_2->isChecked()) 	optional1 = opt3[0];
	if (ui->radioButton_3->isChecked()) 	optional1 = opt4[0];
	if (ui->radioButton_4->isChecked()) 	optional1 = opt5[0];
	if (ui->radioButton_5->isChecked()) 	optional1 = opt6[0];
	if (!(ui->radioButton_9->isChecked())) 	ui->optional1->setText( optional1 );
	return ui->optional1->text();
}

QString ScrambleDialog::scramble2() {
	if (ui->radioButton->isChecked()) 	optional2 = opt1[1];
	if (ui->radioButton_1->isChecked()) 	optional2 = opt2[1];
	if (ui->radioButton_2->isChecked()) 	optional2 = opt3[1];
	if (ui->radioButton_3->isChecked())	optional2 = opt4[1];
	if (ui->radioButton_4->isChecked()) 	optional2 = opt5[1];
	if (ui->radioButton_5->isChecked()) 	optional2 = opt6[1];
	if (!(ui->radioButton_9->isChecked())) 	ui->optional2->setText( optional2 );
	return ui->optional2->text();
}

QString ScrambleDialog::scramble3() {
	if (ui->radioButton->isChecked()) 	optional3 = opt1[2];
	if (ui->radioButton_1->isChecked()) 	optional3 = opt2[2];
	if (ui->radioButton_2->isChecked()) 	optional3 = opt3[2];
	if (ui->radioButton_3->isChecked()) 	optional3 = opt4[2];
	if (ui->radioButton_4->isChecked()) 	optional3 = opt5[2];
	if (ui->radioButton_5->isChecked()) 	optional3 = opt6[2];
	if (!(ui->radioButton_9->isChecked())) 	ui->optional3->setText( optional3 );
	return ui->optional3->text();
}

QString ScrambleDialog::scramble4() {
	if (ui->radioButton->isChecked()) 	optional4 = opt1[3];
	if (ui->radioButton_1->isChecked()) 	optional4 = opt2[3];
	if (ui->radioButton_2->isChecked()) 	optional4 = opt3[3];
	if (ui->radioButton_3->isChecked()) 	optional4 = opt4[3];
	if (ui->radioButton_4->isChecked()) 	optional4 = opt5[3];
	if (ui->radioButton_5->isChecked()) 	optional4 = opt6[3];
	if (!(ui->radioButton_9->isChecked())) 	ui->optional4->setText( optional4 );
	return ui->optional4->text();
}

QString ScrambleDialog::scramble5() {
	if (ui->radioButton->isChecked()) 	optional5 = opt1[4];
	if (ui->radioButton_1->isChecked()) 	optional5 = opt2[4];
	if (ui->radioButton_2->isChecked()) 	optional5 = opt3[4];
	if (ui->radioButton_3->isChecked()) 	optional5 = opt4[4];
	if (ui->radioButton_4->isChecked()) 	optional5 = opt5[4];
	if (ui->radioButton_5->isChecked()) 	optional5 = opt6[4];
	if (!(ui->radioButton_9->isChecked())) 	ui->optional5->setText( optional5 );
	return ui->optional5->text();
}

QString ScrambleDialog::scramble6() {
	if (ui->radioButton->isChecked()) 	optional6 = opt1[5];
	if (ui->radioButton_1->isChecked())	optional6 = opt2[5];
	if (ui->radioButton_2->isChecked()) 	optional6 = opt3[5];
	if (ui->radioButton_3->isChecked()) 	optional6 = opt4[5];
	if (ui->radioButton_4->isChecked()) 	optional6 = opt5[5];
	if (ui->radioButton_5->isChecked()) 	optional6 = opt6[5];
	if (!(ui->radioButton_9->isChecked())) 	ui->optional6->setText( optional6 );
	return ui->optional6->text();
}

QString ScrambleDialog::scramble7() {
	if (ui->radioButton->isChecked()) 	optional7 = opt1[6];
	if (ui->radioButton_1->isChecked()) 	optional7 = opt2[6];
	if (ui->radioButton_2->isChecked()) 	optional7 = opt3[6];
	if (ui->radioButton_3->isChecked()) 	optional7 = opt4[6];
	if (ui->radioButton_4->isChecked()) 	optional7 = opt5[6];
	if (ui->radioButton_5->isChecked()) 	optional7 = opt6[6];
	if (!(ui->radioButton_9->isChecked())) 	ui->optional7->setText( optional7 );
	return ui->optional7->text();
}

QString ScrambleDialog::scramble8() {
	if (ui->radioButton->isChecked()) 	optional8 = opt1[7];
	if (ui->radioButton_1->isChecked()) 	optional8 = opt2[7];
	if (ui->radioButton_2->isChecked()) 	optional8 = opt3[7];
	if (ui->radioButton_3->isChecked()) 	optional8 = opt4[7];
	if (ui->radioButton_4->isChecked()) 	optional8 = opt5[7];
	if (ui->radioButton_5->isChecked()) 	optional8 = opt6[7];
	if (!(ui->radioButton_9->isChecked())) 	ui->optional8->setText( optional8 );
	return ui->optional8->text();
}
