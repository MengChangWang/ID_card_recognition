#include "ID_rule.h"

QString ID_rule::get_provience(string id)
{
	QString provience = "0";
	provience = QString::fromStdString(un_map[id]);
	return provience;
}

QString ID_rule::get_sex(string id)
{
	int ID = stoi(id);
	if (ID % 2 == 0)
		return "女";
	else
		return "男";
}

QString ID_rule::get_birthday(string id)
{
	QString year = QString::fromStdString(id.substr(1, 4));
	QString month = QString::fromStdString(id.substr(5, 2));
	QString day = QString::fromStdString(id.substr(7, 2));
	return year + "年" + month + "月" + day + "日";
}

ID_rule::ID_rule()
{
	vector<string> key = {
	"11", "12", "13", "14", "15",
	"21", "22", "23",
	"31", "32", "33", "34", "35", "36", "37",
	"41", "42", "43",
	"44", "45", "46",
	"51", "52", "53", "54", "50",
	"61", "62", "63", "64", "65",
	"71", "81", "82"
	};

	vector<string> value = {
		"北京","天津","河北","山西","内蒙古",
		"辽宁","吉林","黑龙江",
		"上海","江苏","浙江","安徽","福建","江西","山东",
		"河南","湖北","湖南",
		"广东","广西","湖南",
		"四川","贵州","云南","西藏","重庆",
		"陕西","甘肃","青海","宁夏","新疆",
		"台湾","香港","澳门"
	};
	for (int i = 0; i < key.size(); i++)
		un_map[key[i]] = value[i];
}
