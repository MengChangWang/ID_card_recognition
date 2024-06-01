#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <QString>
#include <QFile>
#include <unordered_map>
#include <vector>
#include <QMessageBox>
#include "ID_rule.h"

using namespace cv;
using namespace std;

struct Card
{
	Mat mat;
	Rect rect;
};


class image_processing
{
private:
	vector<Card> templates_cards;
	vector<Card> ID_cards;
	vector<Card> ID_number;
	int result[18];
	ID_rule* id_rule;

public:
	image_processing();
	~image_processing();
	bool pretreatment_image(QString& filename);
	Mat get_image(String& fileName);
	String QStirng_to_String(QString& path);
	Mat enhance_image(Mat& img);
	//void draw_contours(Mat& img, vector<vector<Point>>& contours);
	bool digital_template_preprocessing(QString& template_path);
	bool ID_card_preprocessing(Mat& image);
	bool recognise_ID_number();
	char* get_ID_number(int* raw_ID_number);
	QString char_to_QString();
	QString get_provience();
	QString get_birthday();
	QString get_sex();

};

