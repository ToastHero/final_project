#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <limits>
#include <unordered_set>
#include <conio.h>
#include <cstdlib> //Para clear sa screen
#include <windows.h> //Windows API functions
#include <cctype>
#include <algorithm>

using namespace std;
const int SHIFT = 3;

string encrypt(const string& text, int shift) {
    string encryptedText = text;
    for (char& c : encryptedText) {
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            c = (c - base + shift) % 26 + base;
        } else if (isdigit(c)) {
            c = (c - '0' + shift) % 10 + '0';
        } else {
            const string specialChars = "!@#$%^&*.{}";
            size_t index = specialChars.find(c);
            if (index != string::npos) {
                index = (index + shift) % specialChars.size();
                c = specialChars[index];
            }
        }
    }
    return encryptedText;
}

const string accountDirectory = "C:\\logs\\"; //Butanganan sa mga data

//Pang check kung naa ang directory
bool directoryExists(const string& path) {
    DWORD fileAttributes = GetFileAttributesA(path.c_str());
    return (fileAttributes != INVALID_FILE_ATTRIBUTES && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY));
}

//Pang himo ug bag o na directory
bool createDirectory(const string& path) {
    return CreateDirectoryA(path.c_str(), nullptr);
}

//Pang clear sa screen
void clearScreen() {
    system("cls");
}

//Main Menu
void displayMainMenu() {
    cout << "=== Notes Manager ===" << endl;
    cout << "1. Register" << endl;
    cout << "2. Log in" << endl;
    cout << "3. Close program" << endl;
    cout << "Please input the number of your choice: ";
}

//Global unordered set na butanganan sa account names
unordered_set<string> existingAccountNames;

//Pang display sa Account Registration Page
void displayAccountRegistration() {
    clearScreen();
    cout << "=== Account Registration ===" << endl;

    string accountName;
    string accountPassword;

    //Pang input tas validate sa account name
    while (true) {
        cout << "Please input the account name: ";
        getline(cin, accountName);

        //Pang check if okay na ang name
        if (accountName.length() >= 3 && accountName.find_first_not_of("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-") == string::npos) {
            //Pang check if na gamit na ang name
            if (existingAccountNames.find(accountName) != existingAccountNames.end()) {
                cout << "Account name already taken. Please choose a different name." << endl;
            } else {
                existingAccountNames.insert(accountName);
                break;
            }
        } else {
            cout << "Invalid account name. Account name should have at least 3 characters and contain only letters, numbers, and spaces." << endl;
        }
    }

    //Pang input tas validate sa account password
    while (true) {
        cout << "Please input account password: ";
        getline(cin, accountPassword);

        //Pang check if okay na ang password
        if (accountPassword.length() >= 3 && accountPassword.find_first_not_of("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") == string::npos)
            break;
        else
            cout << "Invalid account password. Account password should have at least 3 characters and contain only letters, numbers, and spaces." << endl;
    }

    //Pang tago or encrypt sa password
    string encryptedPassword = encrypt(accountPassword, SHIFT);

    //Pang himo ug account directory kung wala
    if (!directoryExists(accountDirectory))
        createDirectory(accountDirectory);

    //Pang butang sa account data sa file
    string accountFile = accountDirectory + accountName + ".txt";
    ofstream outfile(accountFile);

    if (outfile.is_open()) {
        outfile << "Account name: " << accountName << endl;
        outfile << "Account password: " << encryptedPassword << endl;
        outfile.close();
    }

    clearScreen();
    cout << "=== Account Successfully Registered ===" << endl;
    cout << "You may now log in to your account using the information below." << endl;
    cout << "Account name: " << accountName << endl;
    cout << "Account password: " << encryptedPassword << endl;
    cout << "(Press enter to continue...)" << endl;
    cin.ignore();
}

//Para display sa Log in Page
void displayLogInPage();

//Para sa choices sa Main Menu
void handleMainMenuChoice(int choice) {
    switch (choice) {
        case 1:
            displayAccountRegistration();
            break;
        case 2:
            displayLogInPage();
            break;
        case 3:
            exit(0);
        default:
            clearScreen();
            cout << "Invalid choice. Please enter a valid menu option." << endl;
            cout << "(Press enter to continue...)" << endl;
            cin.ignore();
            break;
    }
}

//Para add sa Notes
void addNotes(const string& accountName) {
    clearScreen();
    cout << "You may now write your notes." << endl;
    cout << "(Press enter after each note. Leave a blank note to end.)" << endl;

    string note;
    string notesFile = accountDirectory + accountName + "_notes.txt";
    ofstream outfile(notesFile, ios::app);

    while (true) {
        getline(cin, note);

        if (note.empty())
            break;

        outfile << note << endl;
    }

    outfile.close();
    clearScreen();
}

//Para view sa Notes
void viewNotes(const string& accountName) {
    clearScreen();
    string notesFile = accountDirectory + accountName + "_notes.txt";
    ifstream infile(notesFile);

    if (infile.is_open()) {
        cout << "=== Notes ===" << endl;

        string note;
        int noteCount = 1;

        while (getline(infile, note)) {
            cout << noteCount << ". " << note << endl;
            noteCount++;
        }

        infile.close();
    } else {
        cout << "No notes found." << endl;
    }

    cout << "(Press enter to continue...)" << endl;
    cin.ignore();
}

//Para edit sa Notes
void editNote(const string& accountName) {
    clearScreen();
    string notesFile = accountDirectory + accountName + "_notes.txt";
    ifstream infile(notesFile);

    if (infile.is_open()) {
        vector<string> notes;
        string note;
        while (getline(infile, note)) {
            notes.push_back(note);
        }
        infile.close();

        cout << "=== Edit Note ===" << endl;
        for (size_t i = 0; i < notes.size(); ++i) {
            cout << i + 1 << ". " << notes[i] << endl;
        }

        cout << "Choose the number of the note that you want to edit (or press enter to cancel): ";
        string choice;
        getline(cin, choice);

        if (!choice.empty()) {
            int noteIndex = stoi(choice) - 1;

            if (noteIndex >= 0 && static_cast<size_t>(noteIndex) < notes.size()) {
                clearScreen();
                cout << "Enter the new content for the note (or press enter to cancel): ";
                string newContent;
                getline(cin, newContent);

                if (!newContent.empty()) {
                    notes[noteIndex] = newContent;

                    ofstream outfile(notesFile);

                    for (const auto& n : notes)
                        outfile << n << endl;

                    outfile.close();
                }
            }
        }
    } else {
        cout << "No notes found." << endl;
    }

    cout << "(Press enter to continue...)" << endl;
    cin.ignore();
}

//Para delete sa Notes
void deleteNote(const string& accountName) {
    clearScreen();
    string notesFile = accountDirectory + accountName + "_notes.txt";
    ifstream infile(notesFile);

    if (infile.is_open()) {
        vector<string> notes;
        string note;
        while (getline(infile, note)) {
            notes.push_back(note);
        }
        infile.close();

        cout << "=== Delete Note ===" << endl;
        for (size_t i = 0; i < notes.size(); ++i) {
            cout << i + 1 << ". " << notes[i] << endl;
        }

        cout << "Choose the number of the note that you want to delete (or press enter to cancel): ";
        string choice;
        getline(cin, choice);

        if (!choice.empty()) {
            int noteIndex = stoi(choice) - 1;

            if (noteIndex >= 0 && static_cast<size_t>(noteIndex) < notes.size()) {
                notes.erase(notes.begin() + noteIndex);

                ofstream outfile(notesFile);

                for (const auto& n : notes)
                    outfile << n << endl;

                outfile.close();
            }
        }
    } else {
        cout << "No notes found." << endl;
    }

    cout << "(Press enter to continue...)" << endl;
    cin.ignore();
}

//Para display sa Log in Page
void displayLogInPage() {
    clearScreen();
    cout << "=== Log In ===" << endl;

    string accountName;
    string accountPassword;

    cout << "Please input the account name: ";
    getline(cin, accountName);

    cout << "Please input the account password: ";
    getline(cin, accountPassword);

    //Pang tago sa password na gi input
    string encryptedPassword = encrypt(accountPassword, SHIFT);

    //Pang check if ni exist ang account
    string accountFile = accountDirectory + accountName + ".txt";
    ifstream infile(accountFile);

    if (infile.is_open()) {
        string storedAccountName;
        string storedPassword;

        getline(infile, storedAccountName);
        getline(infile, storedPassword);

        infile.close();

        //Pang kuha sa name ug password sa user
        storedAccountName = storedAccountName.substr(storedAccountName.find(": ") + 2);
        storedPassword = storedPassword.substr(storedPassword.find(": ") + 2);

        if (accountName == storedAccountName && encryptedPassword == storedPassword) {
            clearScreen();

            int choice;
            do {
                cout << "=== Notes Menu ===" << endl;
                cout << "1. Add note" << endl;
                cout << "2. View notes" << endl;
                cout << "3. Edit note" << endl;
                cout << "4. Delete note" << endl;
                cout << "5. Log out" << endl;
                cout << "Please input the number of your choice: ";
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore the newline character

                switch (choice) {
                    case 1:
                        addNotes(accountName);
                        break;
                    case 2:
                        viewNotes(accountName);
                        break;
                    case 3:
                        editNote(accountName);
                        break;
                    case 4:
                        deleteNote(accountName);
                        break;
                    case 5:
                        clearScreen();
                        cout << "Logged out successfully." << endl;
                        break;
                    default:
                        clearScreen();
                        cout << "Invalid choice. Please enter a valid menu option." << endl;
                        break;
                }
            } while (choice != 5);
        } else {
            clearScreen();
            cout << "Invalid account credentials." << endl;
            cout << "(Press enter to continue...)" << endl;
            cin.ignore();
        }
    } else {
        clearScreen();
        cout << "Account does not exist." << endl;
        cout << "(Press enter to continue...)" << endl;
        cin.ignore();
    }
}

int main() {
    int choice;

    do {
        displayMainMenu();
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); //Pang ignore sa newline character
        handleMainMenuChoice(choice);
    } while (choice != 3);

    return 0;
}
