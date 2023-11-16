#pragma once
#include <iostream>
#include <pqxx/pqxx>
#include "ExceptDBClass.h"
#include<Windows.h>

#ifdef EX_DBC
#define DBC __declspec(dllexport)
#else
#define DBC __declspec(dllimport)
#endif

class Dbclass
{
public:
	DBC Dbclass(const std::string& constring);
	DBC ~Dbclass();
	DBC void add_user(const std::string& FirstName, const std::string& LastName, const std::string& Email, std::string Phonenumber = "");
	DBC void add_user_phoneNumber(const std::string& FirstName, const std::string& LastName, const std::string& Email, const std::string& Phonenumber);
	DBC void delete_user_phoneNumber(const std::string& FirstName, const std::string& LastName, const std::string& Email, const std::string& Phonenumber);
	DBC void edit_user(const std::string& FirstName, const std::string& LastName, const std::string& Email, const std::string& NewFirstName, const std::string& NewLastName, const std::string& NewEmail);
	DBC void delete_user(const std::string& FirstName, const std::string& LastName, const std::string& Email);
	DBC std::list<std::map<std::string, std::string>> find_user_by_FirstName(const std::string& FirstName);
	DBC std::list<std::map<std::string, std::string>> find_user_by_LastName(const std::string& LastName);
	DBC std::map<std::string, std::string> find_user_by_Email(const std::string& Email);
	DBC std::map<std::string, std::string> find_user_by_PhoneNumber(const std::string& Phonenumber);

private:
	pqxx::connection conn;
	int get_userID(const std::string& FirstName, const std::string& LastName, const std::string& Email);
	int get_userID(const std::string& Email);
	int get_userID_phoneNumber(const std::string& Phonenumber);
	void get_phoneNumbers(const int& id, std::map<std::string, std::string>& m);
	void check_private_user_data(const std::string& FirstName, const std::string& LastName, const std::string& Email);
	bool check_phoneNumber(const std::string& Phonenumber, const int& user_id);
};