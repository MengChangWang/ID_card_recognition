#include "image_processing.h"


image_processing::image_processing()
{
	id_rule = new ID_rule();
}

image_processing::~image_processing()
{
	delete id_rule;
}

//身份证图像预处理
bool image_processing::pretreatment_image(QString& fileName)
{
	String imgPath = QStirng_to_String(fileName);

	Mat image = get_image(imgPath);


	//将图片转换为灰度图
	Mat gray_image;
	cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);
	//imshow("gray_image", gray_image);

	//进行双边滤波
	Mat edge_image;
	bilateralFilter(gray_image, edge_image, 9, 100, 100);

	Mat enhanced_image = enhance_image(edge_image);

	// 二值化
	Mat binary_image;
	threshold(enhanced_image, binary_image, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);

	//imshow("binary_image", binary_image);

	// 闭运算
	Mat closing_image;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(40, 18));
	morphologyEx(binary_image, closing_image, MORPH_CLOSE, kernel);

	//imshow("closing_image", closing_image);

	// 轮廓检测
	vector<vector<cv::Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(closing_image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	std::cout << "Number of contours detected: " << contours.size() << std::endl;
	
	Mat tmp_image = image.clone();
	ID_cards.clear();

	for (size_t i = 0; i < contours.size(); i++) {
		Rect boundingRect = cv::boundingRect(contours[i]);
		//rectangle(tmp_image, boundingRect, Scalar(0, 255, 0), 2);
		double ratio = (double)boundingRect.width / (double)boundingRect.height;
		/*cout << i << "area:" << boundingRect.area() << "   ";
		cout << "ratio:" << ratio << endl;*/

		/*Mat roi = image(boundingRect);
		ID_cards.push_back({ roi,boundingRect });*/
		Mat roi = image(boundingRect);
		if (ratio >= 14 && ratio <= 18)
		{
			ID_cards.push_back({ roi,boundingRect });
		}

	}
	/*for (int i = 0; i < ID_cards.size(); i++)
	{
		imshow("ID_cards" + to_string(i), ID_cards[i].mat);
	}*/

	if (ID_cards.size() == 0)
	{
		cout << "未检测到身份证" << endl;
		QMessageBox::warning(NULL, "警告", "未检测到身份证");
		return false;
	}
	if (ID_card_preprocessing(ID_cards[0].mat))
		cout << "身份证号拆分成功" << endl;
	else
	{
		cout << "身份证号拆分失败" << endl;
		QMessageBox::warning(NULL, "警告", "身份证号拆分失败");
	}


	/*Mat temp = image.clone();
	Rect boundConts = cv::boundingRect(contours[2]);
	cout << boundConts.width << " " << boundConts.height <<"area:"<<boundConts.area() << endl;
	imshow("boundConts", temp(boundConts));*/
	//drawContours(tmp_image, contours, -1, cv::Scalar(0, 255, 0), 4, cv::LINE_8, hierarchy);

	//namedWindow("ID card", 1);
	//imshow("ID card", tmp_image);

}

Mat image_processing::get_image(String& fileName)
{
	Mat image;
	image = imread(fileName, IMREAD_COLOR);

	if (image.empty())
	{
		cout << "Could not open or find the image" << endl;
		return Mat();
	}
	else
	{
		cout << "Image loaded successfully" << endl;
		return image;
	}

}

// QString转cv::String
cv::String image_processing::QStirng_to_String(QString& path)
{
	QString tmpPath = path.toLocal8Bit().data();
	cv::String imgPath = tmpPath.toStdString();
	return imgPath;
}

// 对比度增强
Mat image_processing::enhance_image(Mat& img)
{
	Mat enhanced_image;
	double alpha = 1.0;  // 对比度增益
	int beta = 0;        // 亮度增益
	Mat lut(1, 256, CV_8U);
	for (int i = 0; i < 256; i++) {
		lut.at<uchar>(i) = saturate_cast<uchar>(alpha * i + beta);
	}
	LUT(img, lut, enhanced_image);

	return enhanced_image;
}


// 数字模板预处理
bool image_processing::digital_template_preprocessing(QString& template_path)
{
	Mat image;
	QFile file(template_path);
	if (!file.open(QFile::ReadOnly))
	{
		cout << "template_path read failed";
		return false;
	}
	else {
		QByteArray ba = file.readAll();
		image = imdecode(vector<char>(ba.begin(), ba.end()), 1);
		file.close();
	}

	Mat gray_image;
	cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);

	Mat edge_image;
	bilateralFilter(gray_image, edge_image, 5, 75, 75);

	Mat binary_image;
	threshold(edge_image, binary_image, 200, 255, THRESH_BINARY_INV);

	Mat closing_image;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(binary_image, closing_image, MORPH_CLOSE, kernel);

	vector <vector<Point >> contours;
	vector<Vec4i> hierarchy;
	findContours(closing_image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	cout << "Number of contours detected: " << contours.size() << std::endl;

	for (size_t i = 0; i < contours.size(); i++) {
		Rect boundContours = cv::boundingRect(contours[i]);
		// rectangle(image, boundContours, Scalar(0, 255, 0), 2);
		Mat roi = image(boundContours);
		templates_cards.push_back({ roi,boundContours });
	}
	if (templates_cards.empty())return false;

	//进行字符排序
	for (int i = 0; i < templates_cards.size() - 1; i++)
	{
		for (int j = 0; j < templates_cards.size() - 1 - i; j++)
		{
			if (templates_cards[j].rect.x > templates_cards[j + 1].rect.x)
			{
				Card temp = templates_cards[j];
				templates_cards[j] = templates_cards[j + 1];
				templates_cards[j + 1] = temp;
			}
		}
	}
	/* for (int i = 0; i < templates_cards.size(); i++)
	 {
		 imshow(QString("card%1").arg(i).toStdString(), templates_cards[i].mat);
	 }*/

	return true;

}


// 身份证号拆分
bool image_processing::ID_card_preprocessing(Mat& image)
{
	Mat gray_image;
	cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);
	//imshow("gray_image", gray_image);

	Mat edge_image;
	bilateralFilter(gray_image, edge_image, 5, 75, 75);
	//imshow("edge_image", edge_image);

	Mat binary_image;
	threshold(edge_image, binary_image, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);
	//imshow("binary_image", binary_image);

	Mat closing_image;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(binary_image, closing_image, MORPH_CLOSE, kernel);
	//imshow("closing_image", closing_image);

	vector <vector<Point >> contours;
	vector<Vec4i> hierarchy;
	findContours(closing_image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	cout << "Number of contours detected: " << contours.size() << std::endl;

	ID_number.clear();
	for (size_t i = 0; i < contours.size(); i++) {
		Rect boundContours = cv::boundingRect(contours[i]);
		cv::rectangle(image, boundContours, Scalar(0, 255, 0), 2);
		double ratio = (double)boundContours.width / (double)boundContours.height;
		if(boundContours.area() >= 100 && boundContours.area() <= 10000)
		if (ratio >= 0.2 && ratio <= 0.8)
		{
			Mat roi = image(boundContours);
			ID_number.push_back({ roi,boundContours });
		}
	}
	//imshow("ID_card", image);
	if (ID_number.empty())return false;

	for (int i = 0; i < ID_number.size() - 1; i++)
	{
		for (int j = 0; j < ID_number.size() - 1 - i; j++)
		{
			if (ID_number[j].rect.x > ID_number[j + 1].rect.x)
			{
				Card temp = ID_number[j];
				ID_number[j] = ID_number[j + 1];
				ID_number[j + 1] = temp;
			}
		}
	}

	//for (int i = 0; i < ID_number.size(); i++)
	//{
	//	cout << "ID_number:" << i  << " " << (double)ID_number[i].rect.width / (double)ID_number[i].rect.height <<"  "<< ID_number[i].rect.area() << endl;
	//}

	// for (int i = 0; i < ID_number.size(); i++)
	// {
	//	 imshow(QString("card%1").arg(i).toStdString(), ID_number[i].mat);
	// }

	return true;
}

bool image_processing::recognise_ID_number()
{
	if (ID_number.size() != 18) return false;
	for (int i = 0; i < 18; i++)
	{
		Mat roi = ID_number[i].mat;
		resize(roi, roi, Size(30, 40), 0, 0, INTER_AREA);
		Mat gray_image;
		cvtColor(roi, gray_image, cv::COLOR_BGR2GRAY);
		Mat binary_image;
		threshold(gray_image, binary_image, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);

		int maxIndex = 0;
		double Max = 0.0;

		for (int j = 0; j < 11; j++)
		{
			Mat template_num = templates_cards[j].mat;
			resize(template_num, template_num, Size(30, 40), 0, 0, INTER_AREA);
			Mat gray_template_num;
			cvtColor(template_num, gray_template_num, cv::COLOR_BGR2GRAY);
			Mat binary_template_num;
			threshold(gray_template_num, binary_template_num, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);

			Mat result;
			//matchTemplate(gray_image, gray_template_num , result, TM_CCOEFF_NORMED);
			matchTemplate(binary_image, binary_template_num, result, TM_CCOEFF_NORMED);
			double minVal, maxVal;
			Point minLoc, maxLoc;

			minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

			if (maxVal > Max)
			{
				Max = maxVal;
				maxIndex = j;
			}
		}
		result[i] = maxIndex;
	}

	return true;
}

char* image_processing::get_ID_number(int* raw_ID_number)
{
	char ID_number[19];
	for (int i = 0; i < 18; i++)
	{
		if (raw_ID_number[i] == 0)
			ID_number[i] = '1';
		else if (raw_ID_number[i] == 1)
			ID_number[i] = '2';
		else if (raw_ID_number[i] == 2)
			ID_number[i] = '3';
		else if (raw_ID_number[i] == 3)
			ID_number[i] = '4';
		else if (raw_ID_number[i] == 4)
			ID_number[i] = '5';
		else if (raw_ID_number[i] == 5)
			ID_number[i] = '6';
		else if (raw_ID_number[i] == 6)
			ID_number[i] = '7';
		else if (raw_ID_number[i] == 7)
			ID_number[i] = '8';
		else if (raw_ID_number[i] == 8)
			ID_number[i] = '9';
		else if (raw_ID_number[i] == 9)
			ID_number[i] = '0';
		else if (raw_ID_number[i] == 10)
			ID_number[i] = 'X';

	}
	return ID_number;
}

QString image_processing::char_to_QString()
{
	QString ID;
	for (int i = 0; i < 18; i++)
	{
		ID += get_ID_number(result)[i];
	}
	return ID;
}

QString image_processing::get_provience()
{
	QString provience;
	string temp;
	for (int i = 0; i < 2; i++)
	{
		temp += get_ID_number(result)[i];
	}
	provience = id_rule->get_provience(temp);
	return provience;
}

QString image_processing::get_birthday()
{
	QString birthday;
	string temp;
	for (int i = 5; i < 14; i++)
	{
		temp += get_ID_number(result)[i];
	}
	birthday = id_rule->get_birthday(temp);
	return birthday;
}

QString image_processing::get_sex()
{
	QString sex;
	string temp;
	for (int i = 16; i < 17; i++)
	{
		temp += get_ID_number(result)[i];
	}
	sex = id_rule->get_sex(temp);
	return sex;
}




