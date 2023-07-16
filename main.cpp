#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "config.hpp"

namespace fs = std::filesystem;
constexpr auto PROGRAM_NAME = "Kierancli++";

namespace kierancrossland {
	bool is_directory_empty(std::string dir_name) { // returns true if directory is empty and false if directory isnt empty.
		if (fs::is_empty(dir_name)) return true; //directory is empty
		if (!fs::is_empty(dir_name)) return false;//directory has data;
	}
	std::string read_from_text_file(std::string file_location) {
		std::ifstream file;
		if (file.fail()) {
			std::cout << "Error: Failed to open file " << file_location << std::endl;
			return "EPIC_FAIL";
		}
		file.open(file_location);
		std::stringstream file_stringstream;
		file_stringstream << file.rdbuf(); //read the file
		return file_stringstream.str(); //str holds the content of the file
	}
	void write_to_text_file(std::string file_location, std::string contents) {
		std::ofstream file(file_location);
		file << contents;
		file.close();
	}
	namespace kierancli {
		class User {
		public:
			std::string name;
			std::string password;
			
			void greet(std::string USER_DIR) {
				if (fs::exists(USER_DIR + this->name)) {
					std::cout << "\nGreetings " << this->name << ".\nType \"help/h\" to list avaliable commands.\n";
				}
			}

			int change_password(std::string USER_DIR) {
				std::cout << "What do you want your new password to be? (quit/q to cancel): ";
				std::string new_password; std::cin >> new_password;
				if (new_password == "quit" or new_password == "q") return 0;
				write_to_text_file(USER_DIR + this->name + "/" + this->name + "_password.txt", new_password);
				this->password = read_from_text_file(USER_DIR + this->name + "/" + this->name + "_password.txt");
				std::cout << "Password updated.\n";
				return 0;
			}
		};
		int	create_userdata(std::string USER_DIR) {
			std::cout << "Welcome to user creation! \nWhat do you want the username to be (no spaces, quit/q to cancel): ";
			std::string typed_username; std::cin >> typed_username;
			if (typed_username == "quit" or typed_username == "q") return 0;
			std::cout << "What do you want the password to be? (this can be any string,  quit/q to cancel): ";
			std::string typed_password;
			std::cin >> typed_password;
			if (typed_password == "quit" or typed_password == "q") return 0;
			if (fs::is_directory(USER_DIR + typed_username) || !fs::exists(USER_DIR + typed_username)) {
				fs::create_directory(USER_DIR + typed_username);
			}
			std::ofstream password_file(USER_DIR + typed_username + "/" + typed_username + "_password.txt");
			password_file << typed_password;
			password_file.close();
			if (!fs::is_empty(USER_DIR + typed_username)) {
				std::cout << "New userdata has been created (username: " << "\"" << typed_username << "\"" << " password: " << "\"" << typed_password << "\"" << ")\n";
			}
			else {
				std::cout << "Somehow failed to create new userdata\n";
			}
			return 0;
		}
		
		int shell_menu(User user, std::string USER_DIR) {
			while (true) {
				std::cout << user.name << "# ";
				std::string command;
				std::cin >> command;
				if (command == "quit" or command == "q") { std::cout << "Goodbye " << user.name << "." << std::endl; exit(0); }
				if (command == "clear" or command == "cl") std::cout << "\033[2J\033[1;1H";
				if (command == "pwd") std::cout << fs::current_path() << std::endl;
				if (command == "owd") {
#ifdef __linux__
					system("xdg-open .");
#elif _WIN32
					system("explorer.exe .");
				}
#endif
				if (command == "switchuser" or command == "su") return 0;
				if (command == "showpassword") std::cout << user.password << std::endl;
				if (command == "createuser" or command == "cu") kierancli::create_userdata(USER_DIR);
				if (command == "changepassword") user.change_password(USER_DIR);
		
				if (command == "help" || command == "h") {
					std::cout << "Avaliable commands:\n";
					std::cout << "help/h\t\t Lists the avaliable commands.\n";
					std::cout << "quit/q\t\t Quits the program.\n";
					std::cout << "clear\t\t Clears the terminal window.\n";
					std::cout << "pwd\t\t Prints the current working directory.\n";
					std::cout << "owd\t\t Opens the running directory in the file explorer\n";
					std::cout << "switchuser\t Returns to the login screen.\n";
					std::cout << "showpassword\t Prints current users password and password_path.\n";
					std::cout << "createuser\t allows for the creation of new users in " << USER_DIR << "\n";
					std::cout << "changepassword\t allows user to change their password.\n";
				}
			}
		}
		void login_menu(kierancli::User user, std::string USER_DIR) {
			while (true) {
				//the username is needed so that we can know what user directory and passwordfile is called.
				std::cout << "username: ";
				std::cin >> user.name;
#ifdef DEBUG 
				if (user.name == DEBUG_USERNAME) kierancli::shell_menu(user, USER_DIR);
#endif
				std::string password_file = USER_DIR + user.name + "/" + user.name + "_password.txt"; //the path of the users password_file.
				std::cout << "password: ";
				std::string typed_password; 
				std::cin >> typed_password;
				/*typed_password is a  temp password that is checked against the actual password read into user.password.
				If the typed_password matches user.password the user is authenticated.*/

				user.password = read_from_text_file(password_file); //plain text authentication backend.
				
				/*The authentication system is written so that user.password need only exist as a string.
				It isn't concerned with HOW user.password is read into the program,
				making it easy to add alternate authentication backends.
				*/
				if (user.password != typed_password) std::cout << "Incorrect username or password\n";

				if (user.password == typed_password) { user.greet(USER_DIR); kierancli::shell_menu(user, USER_DIR); }
			}
		}
	}
}

int main() {
	using namespace kierancrossland; 

	if (!fs::exists(USER_DIR_CONFIG_FILE)) { std::cerr << "ERROR! " << USER_DIR_CONFIG_FILE << " does not exist." << std::endl; return 1 ; }
	auto USER_DIR = read_from_text_file(USER_DIR_CONFIG_FILE);	

	std::cout << "[compiled at Time: " << __TIME__ << " Date:" << __DATE__ << " USER_DIR_NAME = " << USER_DIR << "] " << std::endl;
	std::cout << "Welcome to " << PROGRAM_NAME << std::endl;

	if (!fs::exists(USER_DIR)) {
		std::cout << "The USER_DIR does not exist, creating it now.\n";
		fs::create_directories(USER_DIR); 
	}
	if (std::filesystem::is_empty(USER_DIR)) {
		std::cout << "The " << USER_DIR << " directory contains no userdata.\n";
		std::cout << "Do you want to create new userdata(yes / no) :";
		std::string choice; std::cin >> choice;
		if (choice == "no") return 0;
		kierancli::create_userdata(USER_DIR);
	}
	kierancli::User user;
	kierancli::login_menu(user, USER_DIR);


	return 0;
}
