#include <stdio.h>
#include "sqlite3.h"
#include <iostream>
#include <conio.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
using namespace std::rel_ops;
typedef std::string		string_t;
//typedef std::vector<string_t>			string_vector;
typedef std::vector<size_t>				size_vector;
typedef std::ifstream					ifstream_t;

const char* name = "my_database.dblite";
const char* tname = "myDataBase";

const char* SQL = "CREATE TABLE IF NOT EXISTS foo(a,b,c); INSERT INTO FOO VALUES(1,2,3); INSERT INTO FOO SELECT * FROM FOO;";
const char* sql = "CREATE TABLE IF NOT EXISTS myDataBase( id integer,type varchar(20),pubName varchar(40),pubDate varchar(11),place varchar(20),authName varchar(20))";

const char* insert = "INSERT into myDataBase(id,type,pubName,pubDate,place,authName) values (1, 'Detection', 'lunch at the cannibal', '2012-20-6','Moldova','Daria Dontsova'); ";
//"INSERT into myDataBase(id,type,pubName,pubDate,place,authName) values (2, 'Roman', 'War and Piece', '2010-3-5','Romania','Tolstoi L.N.');";
//"INSERT into myDataBase(id,type,pubName,pubDate,place,authName) values (3, 'Satira', 'Dead Spirits', '1995-7-4','Russia','Gogoli N.V.');";
//"INSERT into myDataBase(id,type,pubName,pubDate,place,authName) values (4, 'Fantastics', 'Master and Margarita', '2004-26-11','England','Bulgacov');";
//"INSERT into myDataBase(id,type,pubName,pubDate,place,authName) values (5, 'Story', 'Heart of a Dog', '2006-7-3','Russia','Bulgacov');";
//"INSERT into myDataBase(id,type,pubName,pubDate,place,authName) values (6, 'Satira', 'Revizor', '2011-12-11','Belarusi','Gogoli N.V.');";
//"INSERT into myDataBase(id,type,pubName,pubDate,place,authName) values (7, 'Roman', 'Anna Karenina', '2003-1-1','Amerika','Tolstoi L.N.');";
//"INSERT into myDataBase(id,type,pubName,pubDate,place,authName) values (8, 'Story', 'Childhood', '1995-7-4','Portugalia','Gogoli N.V.');";
const char* findByName = "SELECT id,type,name,pubDate,authName1 FROM myDataBase where authName = ?";
const char* del = "drop table myDataBase";
const char* select = "SELECT * from myDataBase";


const char* query = "SELECT id,type,name,pubDate,authName1 FROM example";
const char* insert_query = "INSERT INTO example VALUES (?, ?, ?)";

sqlite3* db;
sqlite3_stmt* stmt;

struct Record {
	size_t id;
	string_t type;			// Тип публикации
	string_t name;			// Имя публикации
	string_t date;			// Дата публикации
	string_t place;			// Место публикации
	string_t authors;	// Авторы публикации

	Record() : id(0), type(""), name(""), date(""), place("") , authors(""){
	}

	Record(size_t _id, string_t _type, string_t _name, string_t _date, string_t _place, string_t _authors) {//констпуктор копирования
		init(_id, _type, _name, _date, _place, _authors);
	}

	Record(const Record& r) {
		init(r.id, r.type, r.name, r.date, r.place, r.authors);
	}

	void init(size_t _id, string_t _type, string_t _name, string_t _date, string_t _place, string_t _authors) {
		id = _id;
		type = _type;
		name = _name;
		date = _date;
		place = _place;
		authors = _authors;
	}


	////////////////////////////////////////////////////////////
	bool operator==(const Record& r) const { return id == r.id; }
	bool operator <(const Record& r) const { return id < r.id; }
	~Record() {};
};

std::ostream& operator<<(std::ostream& os, const Record& r) {
	os << r.id << "  " << r.type << "\t" 
		<< r.name << "  " << r.date << "\t" 
		<< r.place <<" " <<  r.authors;
	os << std::endl;
	return os;
}

struct Table {
	size_t n;	// Количество записей
	std::vector<Record> records;

	Table() {}

	Table(const char* database, const char* tableName) {
		int rezult = sqlite3_open(database, &db);
		const char* query = "SELECT * FROM myDataBase";
		sqlite3_prepare_v2(db, query, 255, &stmt, NULL);
		//sqlite3_bind_text(stmt, 1, tableName, -1, NULL);
		//sqlite3_bind_text(stmt, 1, authName, -1, NULL);
		//sqlite3_bind_int(stmt, 1, 2);
		Record tmp;
		
		++n;
		records.clear();
		while ((rezult = sqlite3_step(stmt)) == SQLITE_ROW){
			tmp.id = sqlite3_column_int(stmt, 0);
			tmp.type = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
			tmp.name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
			tmp.date = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
			tmp.place = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
			tmp.authors = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
			records.push_back(tmp);
		}
		sqlite3_finalize(stmt);
		sqlite3_close(db);

		n = 0;
		
	}

	void add() {
		Record tmp;
		string_t buf;

		tmp.id = ++n;
		std::cout << "Enter type: ";			std::cin >> tmp.type;
		std::cout << "Enter name: ";			std::cin >> tmp.name;
		std::cout << "Enter date(MM/DD/YYY): ";	std::cin >> tmp.date;
		std::cout << "Enter place: ";			std::cin >> tmp.place;
		std::cout << "Enter Author: ";			std::cin >> tmp.authors;
		records.push_back(tmp);
	} //добавление записи

	void change(size_t _id) {//изменение записи
		auto it = records.begin();
		for (it; it != records.end(); ++it)
			if (it->id == _id) break;		// Нашли необходимую запись

		int n = 1;
		while (n) {
			size_t			num_tmp;
			string_t		str_tmp;

			std::cout << "1 - Change id" << std::endl;
			std::cout << "2 - Change type" << std::endl;
			std::cout << "3 - Change name" << std::endl;
			std::cout << "4 - Change date" << std::endl;
			std::cout << "5 - Change place" << std::endl;
			std::cout << "6 - Change author" << std::endl;

			size_t choise = 0;
			std::cout << ">";	std::cin >> choise;
			switch (choise) {
			case 1:
				std::cout << "Enter the ID to change : ";
				std::cin >> num_tmp;
				it->id = num_tmp;
				break;

			case 2:
				std::cout << "Enter the type to change: ";
				std::cin >> str_tmp;
				it->type = str_tmp;
				break;

			case 3:
				std::cout << "Enter the name to change: ";
				std::cin >> str_tmp;
				it->name = str_tmp;
				break;

			case 4:
				std::cout << "Enter the date to change(MM/DD/YYYY): ";
				std::cin >> str_tmp;
				it->date = str_tmp;
				break;

			case 5:
				std::cout << "Enter the place to change: ";
				std::cin >> str_tmp;
				it->place = str_tmp;
				break;

			case 6:
				std::cout << "Enter the place to change: ";
				std::cin >> num_tmp;
				it->authors = num_tmp;
				break;

			default:
				std::cout << "Enter the number between 1-6" << std::endl;
				break;
			}

			std::cout << "Continue? (1 - yes / 0 - no)" << std::endl;
			std::cin >> n;
		}
	}

	void del(string_t _name) { //удаление записи
		auto it = records.begin();
		for (it; it != records.end(); ++it)
			if (it->name == _name) {
				records.erase(it);
				break;
			}
		if (it == records.end()) {
			std::cout << "No such record" << std::endl;
			return;
		}
	}

	void searchByName(string_t _name){//поиск по названию публикации
		for (auto it = records.begin(); it != records.end(); ++it)
			if (it->name == _name){
				std::cout << *it;
				break;
			}
	}

	void searchByAuthor(string_t _author){//поиск по автору
		for (auto it = records.begin(); it != records.end(); ++it)
				if (it->authors == _author)
					std::cout << *it;
	}

	void commit() { //запись в файл
		const char* query = "SELECT * FROM myDataBase";
		const char* del = "drop table myDataBase";
		const char* sql = "CREATE TABLE IF NOT EXISTS myDataBase( id integer,type varchar(20),pubName varchar(40),pubDate varchar(11),place varchar(20),authName varchar(20))";
		sqlite3_prepare_v2(db, del, 255, &stmt, NULL);
		sqlite3_prepare_v2(db, sql, 255, &stmt, NULL);
		const char* insert = "INSERT into myDataBase(id,type,pubName,pubDate,place,authName) values (?, ?, ?, ?, ?, ?); ";
		for (auto it = records.begin(); it != records.end(); ++it){
			sqlite3_prepare_v2(db, insert, 255, &stmt, NULL);
			sqlite3_bind_int(stmt, 1, it->id);
			sqlite3_bind_text(stmt, 2, it->type.c_str(), -1, NULL);
			sqlite3_bind_text(stmt, 3, it->name.c_str(), -1, NULL);
			sqlite3_bind_text(stmt, 4, it->date.c_str(), -1, NULL);
			sqlite3_bind_text(stmt, 5, it->place.c_str(), -1, NULL);
			sqlite3_bind_text(stmt, 6, it->authors.c_str(), -1, NULL);
			
		}

	}

	~Table() {}
};

std::ostream& operator<<(std::ostream& os, const Table& t) {
	for (auto it = t.records.begin(); it != t.records.end(); ++it)
		os << *it;
	return os;
}

void menu(const char* databaseName, const char* tableName) {
	Table t(databaseName,tableName);
	int n;
	int m = 0;

	while (m != 7){
		std::cout << "What do you want to do?:" << std::endl;
		std::cout << "1 - Add record;" << std::endl;
		std::cout << "2 - Edit record;" << std::endl;
		std::cout << "3 - Delete record;" << std::endl;
		std::cout << "4 - Search by name of record;" << std::endl;
		std::cout << "5 - Search by name of Author;" << std::endl;
		std::cout << "6 - Print;" << std::endl;
		std::cout << "7 - Exit" << std::endl;
		std::cin >> n;
		string_t NAME;
		switch (n){
		case 1:
			t.add();
			break;
		case 2:
			size_t ID;
			std::cout << "Enter the ID of record, you wish to edit: "; std::cin >> ID;
			t.change(ID);
			t.commit();
			break;
		case 3:
			std::cout << "Enter the NAME that contains record to delete: ";
			std::cin >> NAME;
			t.del(NAME);
			t.commit();
			break;
		case 4:
			std::cout << "Enter the NAME of record you wish to find: ";
			std::cin >> NAME;
			t.searchByName(NAME);
			break;
		case 5:
			std::cout << "Enter the Name of Author you wish to find: ";
			std::cin >> NAME;
			t.searchByAuthor(NAME);
			break;
		case 6:
			for (auto it = t.records.begin(); it != t.records.end(); it++){
				Record r = *it;
				std::cout << r;
			}
			break;
		case 7:
			m = 7;
		}
	}
}



int main(int argc, char** argv) {
	menu(name,tname);
	system("PAUSE");
	return 0;
	

}