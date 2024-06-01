#pragma once
#include <vector>
#include <string>
#include <QString>
#include <unordered_map>
using namespace std;
class ID_rule
{
private:
	vector<string> index;
	vector<string> provience;
	unordered_map<string, string> un_map;
public:
	QString get_provience(string id);
	QString get_sex(string id);
	QString get_birthday(string id);
	ID_rule();

};

