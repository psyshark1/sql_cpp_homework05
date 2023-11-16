#include"DBClass.h"

int main(int argc, char** argv)
{
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	try
	{
		Dbclass dbc{ "dbname= host=127.0.0.1 port=5432 user= password=" };

	}
	catch (ExceptDBClass& esql)
	{
		std::cout << esql.what() << std::endl;
		return 23;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 66;
	}
	return 0;
}
