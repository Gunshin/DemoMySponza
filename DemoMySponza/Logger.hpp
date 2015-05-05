#pragma once

#include <string>
#include <iostream>
#include <fstream>

class Logger
{

public:
	Logger();

	void Open();
	void Log(std::string input_);
	void Flush();
	void Close();

private:

	std::ofstream stream;
	std::string input;

};