#include"DBClass.h"

Dbclass::Dbclass(const std::string& constring) : conn{constring}
{
	pqxx::transaction tx{ conn };
	tx.exec("create table if not exists Users ("
		"id SERIAL PRIMARY KEY,"
		"FirstName VARCHAR(80) NOT NULL,"
		"LastName VARCHAR(80) NOT NULL,"
		"Email VARCHAR(80) NOT NULL UNIQUE)");
	tx.exec("create table if not exists User_PhoneNumber("
		"User_id INTEGER REFERENCES Users(id),"
		"PhoneNumber VARCHAR(12) NOT NULL UNIQUE)");
	tx.commit();

	conn.prepare("add_user", "insert into Users (FirstName,LastName,Email)values($1,$2,$3)");
	conn.prepare("edit_user", "UPDATE users SET FirstName = $1, LastName = $2, Email = $3 WHERE id = $4");
	conn.prepare("add_phonenumber", "insert into User_PhoneNumber(User_id,PhoneNumber)values($1,$2)");
	conn.prepare("delete_phonenumber", "DELETE FROM User_PhoneNumber WHERE user_id = $1 AND PhoneNumber = $2");
	conn.prepare("delete_phonenumbers", "DELETE FROM User_PhoneNumber WHERE user_id = $1");
	conn.prepare("delete_user", "DELETE FROM users WHERE id = $1");
	conn.prepare("get_userID", "Select id FROM Users WHERE FirstName = $1 AND LastName = $2 AND Email = $3");
	conn.prepare("get_userID_email", "Select id FROM Users WHERE Email = $1");
	conn.prepare("get_userID_phoneNumber", "Select user_id FROM User_PhoneNumber WHERE phonenumber = $1");
	conn.prepare("get_phoneNumber", "Select phonenumber FROM User_PhoneNumber WHERE user_id = $1 AND phonenumber = $2");
	conn.prepare("get_phoneNumbers", "Select phonenumber FROM User_PhoneNumber WHERE user_id = $1");
}
void Dbclass::add_user(const std::string& FirstName, const std::string& LastName, const std::string& Email, std::string Phonenumber)
{
	check_private_user_data(FirstName, LastName, Email);
	int id{ get_userID(FirstName,LastName,Email) };
	if (id) { throw ExceptDBClass("User " + FirstName + " " + LastName + " email: " + Email + " already exists!"); }

	pqxx::transaction tx{ conn };
	tx.exec_prepared("add_user", FirstName, LastName, Email);
	tx.commit();

	if (!Phonenumber.empty())
	{
		add_user_phoneNumber(FirstName, LastName, Email, Phonenumber);
	}
}

void Dbclass::edit_user(const std::string& FirstName, const std::string& LastName, const std::string& Email, const std::string& NewFirstName, const std::string& NewLastName, const std::string& NewEmail)
{
	check_private_user_data(NewFirstName, NewLastName, NewEmail);
	int id{ get_userID(FirstName,LastName,Email) };
	if (!id) { throw ExceptDBClass("User " + FirstName + " " + LastName + " email: " + Email + " not found!"); }
	
	pqxx::transaction tx{ conn };
	tx.exec_prepared("edit_user", NewFirstName, NewLastName, NewEmail, id);
	tx.commit();
}

void Dbclass::add_user_phoneNumber(const std::string& FirstName, const std::string& LastName, const std::string& Email, const std::string& Phonenumber)
{
	int id{ get_userID(FirstName,LastName,Email) };
	if (!id) { throw ExceptDBClass("User " + FirstName + " " + LastName + " email: " + Email + " not found!"); }

	pqxx::transaction tx{ conn };
	tx.exec_prepared("add_phonenumber", id, Phonenumber);
	tx.commit();
}

void Dbclass::delete_user_phoneNumber(const std::string& FirstName, const std::string& LastName, const std::string& Email, const std::string& Phonenumber)
{
	int id{ get_userID(FirstName,LastName,Email) };
	if (!id) { throw ExceptDBClass("User " + FirstName + " " + LastName + " email: " + Email + " not found!"); }

	if (!check_phoneNumber(Phonenumber, id)) { throw ExceptDBClass("User " + FirstName + " " + LastName + " Phone Number: " + Phonenumber + " not found!"); }

	pqxx::transaction tx{ conn };
	tx.exec_prepared("delete_phonenumber", id, Phonenumber);
	tx.commit();
}

void Dbclass::delete_user(const std::string& FirstName, const std::string& LastName, const std::string& Email)
{
	int id{ get_userID(FirstName,LastName,Email) };
	if (!id) { throw ExceptDBClass("User " + FirstName + " " + LastName + " email: " + Email + " not found!"); }

	pqxx::transaction tx{ conn };
	tx.exec_prepared("delete_phonenumbers", id);
	tx.exec_prepared("delete_user", id);
	tx.commit();
}

int Dbclass::get_userID(const std::string& FirstName, const std::string& LastName, const std::string& Email)
{
	pqxx::transaction tx{ conn };
	pqxx::result r = tx.exec_prepared("get_userID", FirstName, LastName, Email);
	tx.commit();
	if (r.empty()) { return 0; }
	return std::stoi(r[0][0].c_str());
}

int Dbclass::get_userID(const std::string& Email)
{
	pqxx::transaction tx{ conn };
	pqxx::result r = tx.exec_prepared("get_userID_email", Email);
	tx.commit();
	if (r.empty()) { return 0; }
	return std::stoi(r[0][0].c_str());
}

int Dbclass::get_userID_phoneNumber(const std::string& Phonenumber)
{
	pqxx::transaction tx{ conn };
	pqxx::result r = tx.exec_prepared("get_userID_phoneNumber", Phonenumber);
	tx.commit();
	if (r.empty()) { return 0; }
	return std::stoi(r[0][0].c_str());
}

bool Dbclass::check_phoneNumber(const std::string& Phonenumber, const int& user_id)
{
	pqxx::transaction tx{ conn };
	pqxx::result r = tx.exec_prepared("get_phoneNumber", user_id, Phonenumber);
	tx.commit();
	if (r.empty()) { return false; }
	return true;
}

void Dbclass::check_private_user_data(const std::string& FirstName, const std::string& LastName, const std::string& Email)
{
	if (FirstName.empty()) { throw ExceptDBClass("User's First Name cannot be empty!"); }
	if (LastName.empty()) { throw ExceptDBClass("User's Last Name cannot be empty!"); }
	if (Email.empty()) { throw ExceptDBClass("User's Email cannot be empty!"); }
}

std::list<std::map<std::string, std::string>> Dbclass::find_user_by_FirstName(const std::string& FirstName)
{
	pqxx::transaction tx{ conn };
	auto col = tx.query<int, std::string, std::string, std::string>("Select id, FirstName, LastName, Email FROM Users WHERE FirstName = " + tx.esc(FirstName));
	tx.commit();
	std::list<std::map<std::string, std::string>> lst;
	std::map<std::string, std::string> m;
	for (const auto& [id, FirstName, LastName, Email] : col)
	{
		m["FirstName"] = FirstName;
		m["LastName"] = LastName;
		m["Email"] = Email;
		get_phoneNumbers(id, m);
		lst.push_back(m);
		m.clear();
	}
	return lst;
}

std::list<std::map<std::string, std::string>> Dbclass::find_user_by_LastName(const std::string& LastName)
{
	pqxx::transaction tx{ conn };
	auto col = tx.query<int, std::string, std::string, std::string>("Select id, FirstName, LastName, Email FROM Users WHERE FirstName = " + tx.esc(LastName));
	tx.commit();
	std::list<std::map<std::string, std::string>> lst;
	std::map<std::string, std::string> m;
	for (const auto& [id, FirstName, LastName, Email] : col)
	{
		m["FirstName"] = FirstName;
		m["LastName"] = LastName;
		m["Email"] = Email;
		get_phoneNumbers(id, m);
		lst.push_back(m);
		m.clear();
	}
	return lst;
}

std::map<std::string, std::string> Dbclass::find_user_by_Email(const std::string& Email)
{
	int id{ get_userID(Email) };
	if (!id) { throw ExceptDBClass("User with email: " + Email + " not found!"); }

	pqxx::transaction tx{ conn };
	auto col = tx.query<int, std::string, std::string, std::string>("Select id, FirstName, LastName, Email FROM Users WHERE id = " + tx.esc(std::to_string(id)));
	tx.commit();
	std::map<std::string, std::string> m;
	for (const auto& [id, FirstName, LastName, Email] : col)
	{
		m["FirstName"] = FirstName;
		m["LastName"] = LastName;
		m["Email"] = Email;
		get_phoneNumbers(id, m);
	}
	return m;
}

std::map<std::string, std::string> Dbclass::find_user_by_PhoneNumber(const std::string& Phonenumber)
{
	int id{ get_userID_phoneNumber(Phonenumber) };
	if (!id) { throw ExceptDBClass("User with phone number: " + Phonenumber + " not found!"); }

	pqxx::transaction tx{ conn };
	auto col = tx.query<int, std::string, std::string, std::string>("Select id, FirstName, LastName, Email FROM Users WHERE id = " + tx.esc(std::to_string(id)));
	tx.commit();
	std::map<std::string, std::string> m;
	for (const auto& [id, FirstName, LastName, Email] : col)
	{
		m["FirstName"] = FirstName;
		m["LastName"] = LastName;
		m["Email"] = Email;
		get_phoneNumbers(id, m);
	}
	return m;
}

void Dbclass::get_phoneNumbers(const int& id, std::map<std::string, std::string>& m)
{
	pqxx::transaction tx{ conn };
	int i{ 1 };
	auto col = tx.exec_prepared("get_phoneNumbers", id);
	tx.commit();
	if (!col.empty())
	{
		for (const auto& row : col)
		{
			m["PhoneNumber" + std::to_string(i)] = row[0].c_str(); ++i;
		}
	}
}

Dbclass::~Dbclass()
{
	conn.close();
}