#ifndef JSTOOLS_H
#define JSTOOLS_H
#ifndef F_OK
#define F_OK 0
#endif
#include<string>
#include<sstream>
#include<iostream>
#include<chrono>
#include<vector>
#include<regex>
#ifdef WIN32
#if _MSC_VER >= 1600

#pragma execution_character_set("utf-8")

#endif
#include<direct.h>
#include<io.h>
#endif
#ifdef linux
#include<dirent.h>
#include<sys/stat.h>
#endif
#include"spdlog/spdlog.h"
class Utils
{
public:
	//文件操作类
	static bool checkExist(const std::string &path)
	{
#ifdef linux
		int result = access(path.c_str(), F_OK);
#endif
#ifdef WIN32
		int result = _access(path.c_str(), F_OK);
#endif
		if (result == -1) 
		{
			return false;
		}
		else if (result == 0) 
		{
			return true;
		}
	}
	static bool createDirectory(const std::string &path)
	{
#ifdef WIN32
		int result = _mkdir(path.c_str());
		if (result == -1) 
		{
			return false;
		}
		else if (result == 0) 
		{
			return true;
		}
		return false;
#endif
#ifdef linux
		int result = mkdir(path.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if( result== -1) 
		{
			return false;
		} else if(result == 0)
		{
			return true;
		} 
#endif
	}

	static std::vector<std::string>dir_list(const std::string &path)
	{
#ifdef WIN32
		std::vector<std::string>result;
		std::string fullpath = (path + "/*.csv");
		const char *to_search = fullpath.c_str(); //欲查找的文件，支持通配符
		long handle; //用于查找的句柄
		struct _finddata_t fileinfo; //文件信息的结构体
		handle = _findfirst(to_search, &fileinfo); //第一次查找
		if (-1 == handle) {
			return result;
		}
		else {
			result.push_back(std::string(fileinfo.name));
		}
		while (!_findnext(handle, &fileinfo)) {  //循环查找其他符合的文件，知道找不到其他的为止
			result.push_back(std::string(fileinfo.name));
		}
		_findclose(handle); //别忘了关闭句柄
		return result;
#endif
#ifdef linux
		DIR    *dir;
		struct    dirent    *ptr;
		std::vector<std::string>result;
		std::string fullpath = (path + "/*.csv");
		dir = opendir(fullpath.c_str());
		while((ptr = readdir(dir)) != NULL)
			result.push_back(std::string(ptr->d_name));
		closedir(dir);
		return result;
#endif
	}

        static void deletedir(const std::string &dir_param)
        {
#ifdef WIN32
			std::string temp = dir_param + "/*";
			const char * dir = temp.c_str();
			std::vector<std::string>namelist = Utils::split(dir, "/");
			if (namelist.size() >= 2){
				_finddata_t fileDir;
				long lfDir;

				if ((lfDir = _findfirst(dir, &fileDir)) == -1)
					printf("No file is found\n");
				else{
					do{
						std::string name = namelist[namelist.size() - 2]+"/"+ std::string(fileDir.name);
						int result = remove(name.c_str());
					} while (_findnext(lfDir, &fileDir) == 0);
				}
				_findclose(lfDir);
			}
#endif
        }
        //字符串转换
        static std::vector<std::string> split(std::string str, std::string pattern)
        {
            std::string::size_type pos;
		std::vector<std::string> result;
		str += pattern;
		std::string::size_type size = str.size();
		for (std::string::size_type i = 0; i < size; i++) {
			pos = str.find(pattern, i);
			if (pos < size) {
				std::string s = str.substr(i, pos - i);
				result.push_back(s);
				i = pos + pattern.size() - 1;
			}
		}
		return result;
	}
	//其他
	static std::string regexSymbol(const std::string &symbol) 
	{
		std::string s = symbol;
		std::regex regex("\\d");
		std::string trans_symbol = std::regex_replace(s, regex, "");
		return trans_symbol;
	}
#ifdef WIN32
	static std::string UTF8_2_GBK(const std::string &utf8Str)
	{
		std::string outGBK = "";
		int n = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
		WCHAR *str1 = new WCHAR[n];
		MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, str1, n);
		n = WideCharToMultiByte(CP_ACP, 0, str1, -1, NULL, 0, NULL, NULL);
		char *str2 = new char[n];
		WideCharToMultiByte(CP_ACP, 0, str1, -1, str2, n, NULL, NULL);
		outGBK = str2;
		delete[] str1;
		str1 = NULL;
		delete[] str2;
		str2 = NULL;
		return outGBK;
	}

	static std::string GBK_2_UTF8(const std::string& strGBK)
	{
		WCHAR * str1;
		int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
		str1 = new WCHAR[n];
		MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
		n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
		char * str2 = new char[n];
		WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
		std::string strOutUTF8;
		strOutUTF8.assign(str2, n);
		delete[]str1;
		str1 = NULL;
		delete[]str2;
		str2 = NULL;
		return strOutUTF8;
	}
#endif
	static std::string getCurrentDateTime()
	{
		time_t now = time(NULL);
		auto milliseconds = getMilliseconds();
		std::string lastmilliseconds = milliseconds.substr(milliseconds.size() - 3);
		tm local = *localtime(&now);//将UNIX时间戳转TM结构
		std::string datetime = Int2String((local.tm_year + 1900), 4) + "-" + Int2String((local.tm_mon + 1), 2) + "-" + Int2String((local.tm_mday), 2)
			+ " " + Int2String((local.tm_hour), 2) + ":" + Int2String((local.tm_min), 2) + ":" + Int2String((local.tm_sec), 2);
		return datetime + "." + lastmilliseconds;
	}

	static std::string getMilliseconds() 
	{
		auto milliseconds = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::system_clock::now().time_since_epoch()).count());
		return milliseconds;
	}

	static std::string getCurrentTime() 
	{
		time_t now = time(NULL);
		auto milliseconds = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::system_clock::now().time_since_epoch()).count());
		std::string lastmilliseconds = milliseconds.substr(milliseconds.size() - 3);
		tm local = *localtime(&now);//将UNIX时间戳转TM结构
		std::string datetime = Int2String((local.tm_hour), 2) + ":" + Int2String((local.tm_min), 2) + ":" + Int2String((local.tm_sec), 2);
		return datetime + "." + lastmilliseconds;
	}

	static std::string getCurrentDate() 
	{
		time_t now = time(NULL);
		tm local = *localtime(&now);//将UNIX时间戳转TM结构
		std::string datetime = Int2String((local.tm_year + 1900), 4) + "-" + Int2String((local.tm_mon + 1), 2) + "-" + Int2String((local.tm_mday), 2);
		return datetime;
	}

	static std::string Time_ttoString(time_t time)
	{
		auto tt = time;
		struct tm* ptm = localtime(&tt);
		char date[60] = { 0 };
		sprintf(date, "%d-%02d-%02d      %02d:%02d:%02d",
			(int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
			(int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
		return std::string(date);
	}

	inline static time_t timeTotime_t(int hour, int minute, int second) 
	{
		//时分秒转换成unix时间戳
		time_t now = time(NULL);
		tm local = *localtime(&now);//将UNIX时间戳转TM结构
		local.tm_hour = hour;
		local.tm_min = minute;
		local.tm_sec = second;
		time_t ft = mktime(&local);
		return ft;
	}

	inline static time_t datetimeToTime_t(const std::string &date,const std::string &time)
	{
		std::vector<std::string>datelist = Utils::split(date, "-");
		std::vector<std::string>timelist = Utils::split(time, ":");
		std::vector<std::string>seconds;
		if (timelist.at(2).find(".") != std::string::npos)
		{
			seconds = Utils::split(timelist.at(2), ".");
		}
		else
		{
			seconds.push_back(timelist.at(2));
		}

		if (datelist.size() == 3 && timelist.size() == 3) {
			tm l;
			l.tm_year = std::stoi(datelist.at(0)) - 1900;
			l.tm_mon = std::stoi(datelist.at(1)) - 1;
			l.tm_mday = std::stoi(datelist.at(2));
			l.tm_hour = std::stoi(timelist.at(0));
			l.tm_min = std::stoi(timelist.at(1));
			l.tm_sec = std::stoi(seconds.at(0));
			time_t ft = mktime(&l);
			return ft;
		}
	}

	inline  static std::string Int2String(int value, int digit)
	{
		char charstr[100];
		std::string str = std::string("%0" + std::to_string(digit) + "d");
		const char * format = str.c_str();
		sprintf(charstr, format, value);
		return std::string(charstr);
	}

	inline  static std::string Bool2String(bool var)
	{
		if (var == true)
		{
			return "true";
		}
		else
		{
			return "false";
		}
	}

        static int getWeekDay(const std::string &date)
	{
		std::vector<std::string>dateVec = Utils::split(date, "-");
		int y = 0, c = 0, m = 0, d = 0;
		int month, year, centrary, day;
		if (dateVec.size()>2)
		{
			centrary = std::stod(dateVec[0].substr(0, 2)) + 1;
			year = std::stod(dateVec[0].substr(2, 2));
			month = std::stod(dateVec[1]);
			day = std::stod(dateVec[2]);
		}
		else
		{
#ifdef WIN32
			std::cout << Utils::UTF8_2_GBK("getweekday日期出错");
			return -1;
#endif
#ifdef linux
                        std::cout << "getweekday日期出错";
                        return -1;
#endif
		}
		if (month == 1 || month == 2)
		{
			c = (year - 1) / 100;
			y = (year - 1) % 100;
			m = month + 12;
			d = day;
		}
		else
		{
			c = year / 100;
			y = year % 100;
			m = month;
			d = day;
		}

		int w = y + y / 4 + c / 4 - 2 * c + 26 * (m + 1) / 10 + d - 1;
		w = w >= 0 ? (w % 7) : (w % 7 + 7);
		if (w == 0)
		{
			w = 7;
		}
		return w;
	}
};
#endif
