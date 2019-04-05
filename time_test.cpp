#include <chrono>
#include <ctime>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	auto c = std::chrono::system_clock::now();
	std::time_t t = c.time_since_epoch().count();
	std::cout << "chrono::system_clock::now since epoch " << t << "\n";
	std::time_t t2 = std::chrono::system_clock::to_time_t(c);
	std::cout << "chrono::system_clock::now to_time_t " << t2 << "\n";
	std::time_t now = std::time(nullptr);
	std::cout << "time_t " << now << "\n";
	auto hi = std::chrono::high_resolution_clock::now();
	std::time_t t3 = hi.time_since_epoch().count();
	std::cout << "chrono::high_resolution_clock::now since epoch " << t3 << "\n";
	char str[100];
	//  Date and time representation appropriate for locale
	std::strftime(str, sizeof(str), "%c", std::localtime(&t2));
	std::strftime(str, sizeof(str), "%c", std::localtime(&t));
}