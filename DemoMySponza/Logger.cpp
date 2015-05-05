#include "Logger.hpp"


Logger::Logger()
{
	
}

void Logger::Open()
{
	stream.open("profileData.txt", std::ios::trunc);
	printf("open: %s\n", stream.is_open() ? "true" : "false");
}

void Logger::Log(std::string input_)
{
	input.append(input_ + "\t");
}

void Logger::Flush()
{
	stream << input + "\n";
	input = std::string();
}

void Logger::Close()
{
	stream.close();
}