#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

using namespace std;

#define MAX_LENGTH 4096
#define PATH_DEP "db_dep.txt"
#define PATH_CR "db_cr.txt"

class IllegalInputException {
public:
	char* val;

	IllegalInputException(const char* exc)
	{
		val = new char[strlen(exc) + 1];
		strcpy_s(val, strlen(exc) + 1, exc);
	}

	friend ostream& operator<<(ostream& os, const IllegalInputException& exc) {
		os << exc.val;
		return os;
	}
};

class WrongSumException {
public:
	char* val;

	WrongSumException(const char* exc)
	{
		val = new char[strlen(exc) + 1];
		strcpy_s(val, strlen(exc) + 1, exc);
	}

	friend ostream& operator<<(ostream& os, const WrongSumException& exc) {
		os << exc.val;
		return os;
	}
};

class NothingHasFoundException {
public:
	char* val;

	NothingHasFoundException(const char* exc)
	{
		val = new char[strlen(exc) + 1];
		strcpy_s(val, strlen(exc) + 1, exc);
	}

	friend ostream& operator<<(ostream& os, const NothingHasFoundException& exc) {
		os << exc.val;
		return os;
	}
};

class Fio { // класс ФИО

private: // приватные переменные
	char* surname;
	char* name;
	char* patronymic;

public:
	Fio() { // конструктор по умолчанию
		set_surname((char*)"I");
		set_name((char*)"I");
		set_patronymic((char*)"I");
	}

	Fio(const Fio& fio) { // конструктор копирования
		set_surname(fio.surname);
		set_name(fio.name);
		set_patronymic(fio.patronymic);
	}

	Fio& operator=(const Fio& fio) { // оператор присваивания
		set_surname(fio.surname);
		set_name(fio.name);
		set_patronymic(fio.patronymic);
		return *this;
	}

	// сеттеры и геттеры
	void set_surname(char* surname) {
		this->surname = new char[strlen(surname) + 1];
		strcpy_s(this->surname, strlen(surname) + 1, surname);
	}

	void set_name(char* name) {
		this->name = new char[strlen(name) + 1];
		strcpy_s(this->name, strlen(name) + 1, name);
	}

	void set_patronymic(char* patronymic) {
		this->patronymic = new char[strlen(patronymic) + 1];
		strcpy_s(this->patronymic, strlen(patronymic) + 1, patronymic);
	}

	char* get_surname() {
		return surname;
	}

	char* get_name() {
		return name;
	}

	char* get_patronymic() {
		return patronymic;
	}

	// перегруженный оператор побитового сдвига (вывод)
	friend ostream& operator<<(ostream& out, const Fio& fio);

	bool operator==(const Fio& fio) {
		return strcmp(surname, fio.surname) == 0 && strcmp(name, fio.name) == 0 && strcmp(patronymic, fio.patronymic) == 0;
	}

	friend Fio enter_fio(); // дружественная (поэтому определение потом) функция ввода с клавы ФИО
};

class Account {
private:
	float sum;
	Fio client;

public:
	Account() {
		sum = 100;
	}

	Account(const Account& account) {
		sum = account.sum;
		client = account.client;
	}

	Account& operator+(float amount) {
		this->sum += amount;
		return *this;
	}

	float get_sum() const {
		return sum;
	}

	Fio get_client() const {
		return client;
	}

	void set_client(Fio client) {
		this->client = client;
	}

	void set_sum(float sum) {
		this->sum = sum;
	}

};

class Credit : public Account {
private:
	int ccn;
	float percent;
	int months;

public:
	static int count; // статический счетчик

	Credit() : Account() { // конструктор по умолчанию
		ccn = 1234;
		percent = 1;
		months = 1;
	}

	Credit(const Credit& credit) : Account(credit) { // конструктор копирования
		set_ccn(credit.ccn);
		set_percent(credit.percent);
		set_months(credit.months);
	}

	Credit& operator=(const Credit& credit) { // оператор присваивания
		set_client(credit.get_client());
		set_sum(credit.get_sum());
		set_ccn(credit.ccn);
		set_percent(credit.percent);
		set_months(credit.months);
		return *this;
	}

	Credit& operator+(float amount) {
		set_sum(get_sum() + amount);
		return *this;
	}

	int get_ccn() const {
		return ccn;
	}

	int get_months() const {
		return months;
	}

	float get_percent() const {
		return percent;
	}

	float get_payment() const {
		return months == 0 ? 0 : get_sum() / months;
	}

	void set_ccn(int ccn) {
		this->ccn = ccn;
	}

	void set_months(int months) {
		this->months = months;
	}

	void set_percent(float percent) {
		this->percent = percent;
	}

	friend int find_by_ccn(Credit* list, Credit& credit, int ccn);

	friend Credit* find_by_fio(Credit* list, int& am, Fio fio);

	friend Credit enter_credit(); // функция ввода счета с клавиатуры

	friend ostream& operator<<(ostream& out, const Credit& credit);

	friend void save(Credit* list); // объявление функции сохранения

	friend Credit* read_credits(); // объявление фукнции чтения из файла

	friend Credit* sort_by_sum(Credit* list, int& am); // объявление сортировки по балансу на счете

	friend void print_list(Credit* list, int size); // объявление функции вывода листа аккаунтов

	friend void pay_by_credit(Credit* list, int ccn) {
		Credit cr;
		int i = find_by_ccn(list, cr, ccn);

		if (cr.months <= 0) {
			throw WrongSumException("The credit is no more!");
		}

		cr.set_sum(cr.get_sum() - cr.get_payment());
		cr.set_months(cr.get_months() - 1);
		cout << "Money is correctly taken off!" << endl;
		list[i] = cr;
	}
};

class Deposit : public Account {

private:
	int id;

public:
	static int count; // статический счетчик

	Deposit() : Account() { // конструктор по умолчанию
		id = 1;
	}

	Deposit(const Deposit& bank) : Account(bank) { // конструктор копирования
		set_id(bank.id);
	}

	Deposit& operator=(const Deposit& bank) { // оператор присваивания
		set_client(bank.get_client());
		set_sum(bank.get_sum());
		set_id(bank.id);
		return *this;
	}

	Deposit& operator+(float amount) {
		set_sum(get_sum() + amount);
		return *this;
	}

	// геттеры и сеттеры
	int get_id() const {
		return id;
	}

	void set_id(int id) {
		this->id = id;
	}

	// перегруженный побитовый сдвиг (вывод)
	friend ostream& operator<<(ostream& out, const Deposit& bank);

	friend Deposit enter_deposit(); // функция ввода счета с клавиатуры

	friend int find_by_id(Deposit* list, Deposit& bank, int id); // объявление поиска по номеру счета

	friend Deposit* find_by_fio(Deposit* list, int& am, Fio fio);

	friend Deposit* sort_by_sum(Deposit* list, int& am); // объявление сортировки по балансу на счете

	friend void save(Deposit* list); // объявление функции сохранения

	friend Deposit* read_deposits(); // объявление фукнции чтения из файла

	friend void print_list(Deposit* list, int size); // объявление функции вывода листа аккаунтов

	friend void take_money_by_id(Deposit* arr, int id, float money) {
		Deposit dep;
		int i = find_by_id(arr, dep, id);

		if (money > dep.get_sum()) {
			throw WrongSumException("The amount of money that you want to take is too much");
		}

		dep.set_sum(dep.get_sum() - money);
		cout << "Money is correctly taken off!" << endl;
		arr[i] = dep;
	}
};

void print_list(Deposit* list, int size) { // вывод листа определенного размера
	if (size == 0) {
		throw NothingHasFoundException("The list is empty, nothing to print");
	}
	for (int i = 0; i < size; i++) {
		cout << list[i];

		if (i != size - 1) {
			cout << endl << endl;
		}
	}
}

void print_list(Credit* list, int size) { // вывод листа определенного размера
	if (size == 0) {
		throw NothingHasFoundException("The list is empty, nothing to print");
	}
	for (int i = 0; i < size; i++) {
		cout << list[i];

		if (i != size - 1) {
			cout << endl << endl;
		}
	}
}

ostream& operator<<(ostream& out, const Fio& fio) { // вывод фио
	out << "Person: " << fio.surname << " " << fio.name << " " << fio.patronymic;
	return out;
}

ostream& operator<<(ostream& out, const Deposit& bank) { // вывод счета
	out << "Client - " << bank.get_client() << endl <<
		"Account ID - " << bank.get_id() << endl << "Account sum - " << bank.get_sum();
	return out;
}

ostream& operator<<(ostream& out, const Credit& credit) { // вывод кредита
	out << "Client - " << credit.get_client() << endl <<
		"CCN - " << credit.get_ccn() << endl << "Credit sum - " << credit.get_sum() << endl <<
		"Credit percent - " << credit.get_percent() << endl <<
		"Months - " << credit.get_months() << ';' << " Payement/month - " << credit.get_payment();
	return out;
}

Fio enter_fio() { // ввод фио с клавы
	Fio fio;

	char* buff = new char[MAX_LENGTH];

	cout << "Enter surname: ";
	cin.getline(buff, MAX_LENGTH);
	fio.set_surname(buff);

	cout << "Enter name: ";
	cin.getline(buff, MAX_LENGTH);
	fio.set_name(buff);

	cout << "Enter patronymic: ";
	cin.getline(buff, MAX_LENGTH);
	fio.set_patronymic(buff);

	delete[] buff;
	return fio;
}

void cin_int(int& n) {
	if (!(cin >> n)) {
		throw IllegalInputException("Waiting int");
	}
}

int require_int() {
	int to_ret;
	do {
		try {
			cin_int(to_ret);
			break;
		}
		catch (IllegalInputException exc) {
			cin.clear();
			cin.ignore(4096, '\n');
			cout << exc << endl;
		}
	} while (true);
	return to_ret;
}

void cin_float(float& n) {
	if (!(cin >> n)) {
		throw IllegalInputException("Waiting float");
	}
}

int require_float() {
	float to_ret;
	do {
		try {
			cin_float(to_ret);
			break;
		}
		catch (IllegalInputException exc) {
			cin.clear();
			cin.ignore(4096, '\n');
			cout << exc << endl;
		}

	} while (true);
	return to_ret;
}

Deposit enter_deposit() { // ввод счета с клавы
	Deposit bank;

	cout << "Enter client" << endl;
	bank.set_client(enter_fio());

	/*cout << "Enter id: ";
	cin >> bank.id;*/
	bank.id = Deposit::count + 1;

	cout << "Enter sum: ";
	bank.set_sum(require_float());

	return bank;
}

Credit enter_credit() { // ввод счета с клавы
	Credit credit;

	cout << "Enter client" << endl;
	credit.set_client(enter_fio());

	/*
	cout << "Enter CCN: ";
	int i_t;
	cin >> i_t;
	credit.set_ccn(i_t);
	*/

	credit.set_ccn(Credit::count + 1);

	cout << "Enter sum: ";
	credit.set_sum(require_float());

	cout << "Enter percent: ";
	credit.set_percent(require_float());

	cout << "Enter months: ";
	credit.set_months(require_int());

	return credit;
}

int find_by_id(Deposit* list, Deposit& bank, int id) { // поиск по номеру депозита
	for (int i = 0; i < Deposit::count; ++i) {
		if (list[i].id == id) {
			bank = list[i];
			return i;
		}
	}
	throw NothingHasFoundException("Nothing has been found by id");
}

Deposit* find_by_fio(Deposit* list, int& am, Fio fio) {
	Deposit* to_ret = new Deposit[Deposit::count];
	am = 0;
	for (int i = 0; i < Deposit::count; ++i)
	{
		if (list[i].get_client() == fio)
		{
			to_ret[am] = list[i];
			++am;
		}
	}
	return to_ret;
}

int find_by_ccn(Credit* list, Credit& credit, int ccn) { // поиск по номеру кредита
	for (int i = 0; i < Credit::count; ++i) {
		if (list[i].get_ccn() == ccn) {
			credit = list[i];
			return i;
		}
	}
	throw NothingHasFoundException("Nothing has been found by ccn");
}

Credit* find_by_fio(Credit* list, int& am, Fio fio) {
	Credit* to_ret = new Credit[Credit::count];
	am = 0;
	for (int i = 0; i < Credit::count; ++i)
	{
		if (list[i].get_client() == fio)
		{
			to_ret[am] = list[i];
			++am;
		}
	}
	return to_ret;
}

Deposit* sort_by_sum(Deposit* list, int& am) { // сортировка депозитов вставкой по балансу
	for (int i = 1; i < Deposit::count; ++i) {
		for (int j = i - 1; j >= 0 && list[j].get_sum() < list[j + 1].get_sum(); --j) {
			std::swap<Deposit>(list[j], list[j + 1]);
		}
	}
	return list;
}

Credit* sort_by_sum(Credit* list, int& am) { // сортировка кредитов вставкой по балансу
	for (int i = 1; i < Credit::count; ++i) {
		for (int j = i - 1; j >= 0 && list[j].get_sum() < list[j + 1].get_sum(); --j) {
			std::swap<Credit>(list[j], list[j + 1]);
		}
	}
	return list;
}

void save(Deposit* list) { // сохранение депозитов в файл с названием "db_dep.txt"
	remove(PATH_DEP);
	ofstream file(PATH_DEP);

	file << Deposit::count << endl;
	for (int i = 0; i < Deposit::count; ++i) {
		Deposit t = list[i]; // surname;name;patr;id;sum
		file << t.get_client().get_surname() << ';' << t.get_client().get_name() << ';' << t.get_client().get_patronymic() << ';'
			<< t.id << ';' << t.get_sum() << ';' << endl;
	}
	file.close();
}

void save(Credit* list) { // сохранение кредитов в файл с названием "db_cr.txt"
	remove(PATH_CR);
	ofstream file(PATH_CR);

	file << Credit::count << endl;
	for (int i = 0; i < Credit::count; ++i) {
		Credit t = list[i]; // surname;name;patr;ccn;sum;percent;months;
		file << t.get_client().get_surname() << ';' << t.get_client().get_name() << ';' << t.get_client().get_patronymic() << ';'
			<< t.ccn << ';' << t.get_sum() << ';' << t.percent << ';' << endl << t.months << ';';
	}
	file.close();
}

Deposit* read_deposits() { // чтение из файла
	Deposit* list = new Deposit[100];

	ifstream file(PATH_DEP);
	if (!file) {
		return list;
	}

	file >> Deposit::count;

	// surname;name;patr;id;sum
	for (int i = 0; i < Deposit::count; ++i) {
		if (file.eof()) {
			Deposit::count = i;
			cout << "Something went wrong";
			return list;
		}
		Deposit t;
		Fio fio;

		file.ignore();

		char* buff = new char[MAX_LENGTH + 1];

		file.getline(buff, MAX_LENGTH, ';'); // читаем surname
		fio.set_surname(buff);

		file.getline(buff, MAX_LENGTH, ';'); // читаем name
		fio.set_name(buff);

		file.getline(buff, MAX_LENGTH, ';'); // читаем patronymic
		fio.set_patronymic(buff);

		int temp_int;
		file >> temp_int; // читаем номер счета
		t.set_id(temp_int);
		file.ignore();

		float sum;
		file >> sum; // читаем сумму на счете

		t.set_sum(sum);
		t.set_client(fio);
		file.ignore();

		list[i] = t;

		delete[] buff;
	}

	file.close();
	return list;
}

Credit* read_credits() { // чтение из файла
	Credit* list = new Credit[100];

	ifstream file(PATH_CR);
	if (!file) {
		return list;
	}

	file >> Credit::count;

	// surname;name;patr;ccn;sum;percent
	for (int i = 0; i < Credit::count; ++i) {
		if (file.eof()) {
			Credit::count = i;
			cout << "Something went wrong";
			return list;
		}
		Credit t;
		Fio fio;

		file.ignore();

		char* buff = new char[MAX_LENGTH + 1];

		file.getline(buff, MAX_LENGTH, ';'); // читаем surname
		fio.set_surname(buff);

		file.getline(buff, MAX_LENGTH, ';'); // читаем name
		fio.set_name(buff);

		file.getline(buff, MAX_LENGTH, ';'); // читаем patronymic
		fio.set_patronymic(buff);

		int temp_int;
		file >> temp_int; // читаем номер кредита
		t.set_ccn(temp_int);
		file.ignore();

		float temp_float;
		file >> temp_float; // читаем сумму кредита
		t.set_sum(temp_float);
		file.ignore();

		file >> temp_float; // читаем процент кредита
		t.set_percent(temp_float);
		file.ignore();

		file >> temp_int; // читаем месяцы кредита
		t.set_months(temp_int);
		file.ignore();


		t.set_client(fio);

		list[i] = t;

		delete[] buff;
	}

	file.close();
	return list;
}

int menu() { // опции выбора в меню
	int opt;
	cout << "1. Enter new bank account" << endl;
	cout << "2. Enter new credit" << endl;
	cout << "3. Print all the deposits" << endl;
	cout << "4. Print all the credits" << endl;
	cout << "5. Print all the elements" << endl;
	cout << "6. Sort deposits by decreasing sum" << endl;
	cout << "7. Sort credits by decreasing sum" << endl;
	cout << "8. Search deposit by ID" << endl;
	cout << "9. Search credit by CCN" << endl;
	cout << "10. Take money from deposit with id" << endl;
	cout << "11. Pay by credit" << endl;
	cout << "12. Find deposits by fio" << endl;
	cout << "13. Find credits by fio" << endl;
	cout << "14. Exit and save" << endl << endl;

	do {
		try {
			cout << "Enter option: ";
			opt = require_int();
			if (opt < 1 || opt > 14) {
				throw IllegalInputException("Wrong option, try again");
			}
			break;
		}
		catch (IllegalInputException exc) {
			cout << exc << endl;
		}
		catch (...) {
			cout << "Something definetly went wrong" << endl;
		}
	} while (true);
	cin.ignore();

	return opt;
}

int Deposit::count = 0; // инициализация статической переменной
int Credit::count = 0; // инициализация статической переменной

int main() {
	int opt;
	int am;
	int temp;
	Deposit bank;
	Deposit* list = read_deposits(), *t_list;
	Credit credit;
	Credit* cr_list = read_credits(), *t_cr_list;

	Deposit banker = Deposit();
	cout << "Was" << endl << banker << endl << endl;

	banker = banker + 205; // проверка работоспособности перегрузки операции сложения

	cout << "Became" << endl << banker << endl << endl;

	while (true) {
		opt = menu();
		cout << endl;
		switch (opt) {
		case 1: { // ввод нового счета с клавы
			list[Deposit::count] = enter_deposit();
			++Deposit::count;
			cout << endl << endl;
			break;
		}
		case 2: { // ввод нового кредита с клавы
			cr_list[Credit::count] = enter_credit();
			++Credit::count;
			cout << endl << endl;
			break;
		}
		case 3: { // напечатать все депозиты
			try {
				print_list(list, Deposit::count);
			}
			catch (NothingHasFoundException exc) {
				cout << exc << endl;
			}
			cout << endl << endl;
			break;
		}
		case 4: { // напечатать все кредиты
			try {
				print_list(cr_list, Credit::count);
			}
			catch (NothingHasFoundException exc) {
				cout << exc << endl;
			}
			cout << endl << endl;
			break;
		}
		case 5: { // напечатать всё
			cout << "Deposits:" << endl;
			try {
				print_list(list, Deposit::count);
			}
			catch (NothingHasFoundException exc) {
				cout << exc << endl;
			}
			cout << endl << endl;
			cout << "Credits:" << endl;
			try {
				print_list(cr_list, Credit::count);
			}
			catch (NothingHasFoundException exc) {
				cout << exc << endl;
			}
			cout << endl << endl;
			break;
		}
		case 6: { // сортировка по сумме депозита (от большего к меньшему, по уменьшению)
			sort_by_sum(list, am);
			try {
				print_list(list, Deposit::count);
			}
			catch (NothingHasFoundException exc) {
				cout << exc << endl;
			}
			catch (...) {
				cout << "Something went wrong" << endl;
			}
			cout << endl << endl;
			break;
		}
		case 7: { // сортировка по сумме кредита (от большего к меньшему, по уменьшению)
			sort_by_sum(cr_list, am);
			try {
				print_list(cr_list, Credit::count);
			}
			catch (NothingHasFoundException exc) {
				cout << exc << endl;
			}
			catch (...) {
				cout << "Something went wrong" << endl;
			}
			cout << endl << endl;
			break;
		}
		case 8: { // поиск по индексу
			cout << "Enter id: ";
			temp = require_int();
			try {
				find_by_id(list, bank, temp);
				cout << endl << bank << endl;
			}
			catch (NothingHasFoundException exc) {
				cout << exc << endl;
			}
			catch (...) {
				cout << "Something went wrong " << endl;
			}
			cout << endl << endl;
			break;
		}
		case 9: { // поиск по номеру кредита
			cout << "Enter ccn: ";
			temp = require_int();
			try {
				find_by_ccn(cr_list, credit, temp);
				cout << endl << credit << endl;
			}
			catch (NothingHasFoundException exc) {
				cout << exc << endl;
			}
			catch (...) {
				cout << "Something went wrong " << endl;
			}
			cout << endl << endl;
			break;
		}
		case 10: {
			cout << "Enter id: ";
			temp = require_int();
			cout << "Enter amount of money: ";
			try {
				take_money_by_id(list, temp, require_float());
			}
			catch (NothingHasFoundException exc) {
				cout << exc << endl;
			}
			catch (WrongSumException exc) {
				cout << exc << endl;
			}
			catch (...) {
				cout << "Smthng went wrong" << endl;
			}
			break;
		}
		case 11: {
			cout << "Enter ccn: ";
			temp = require_int();
			try {
				pay_by_credit(cr_list, temp);
			}
			catch (NothingHasFoundException exc) {
				cout << exc << endl;
			}
			catch (WrongSumException exc) {
				cout << exc << endl;
			}
			catch (...) {
				cout << "Smthng went wrong" << endl;
			}
			break;
		}
		case 12: {
			am = 0;
			t_list = new Deposit[Deposit::count];
			try {
				t_list = find_by_fio(list, am, enter_fio());
				cout << endl;
				print_list(t_list, am);
			}
			catch (NothingHasFoundException exc) {
				cout << exc << endl;
			}
			catch (...) {
				cout << "smthng went wrong" << endl;
			}
			cout << endl << endl;
			delete[] t_list;
			break;
		}
		case 13: {
			am = 0;
			t_cr_list = new Credit[Credit::count];
			try {
				t_cr_list = find_by_fio(cr_list, am, enter_fio());
				cout << endl;
				print_list(t_cr_list, am);
			}
			catch (NothingHasFoundException exc) {
				cout << exc << endl;
			}
			catch (...) {
				cout << "smthng went wrong" << endl;
			}
			cout << endl << endl;
			delete[] t_cr_list;
			break;
		}
		case 14: {
			save(list);
			save(cr_list);
			return 0;
		}
		default: {
			break;
		}
		}
	}
}