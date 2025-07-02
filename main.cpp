#include <iostream>
#include <fstream>
#include <string>

using namespace std;

bool adminLogin() {
    ifstream file;
    string savedUser, savedPass, inputUser, inputPass;
    int attempts = 0;

    while (attempts < 3) {
        file.open("admin.txt");
        file >> savedUser >> savedPass;
        file.close();

        cout << "\n--- Admin Login ---" << endl;
        cout << "Enter admin username: ";
        cin >> inputUser;
        cout << "Enter admin password: ";
        cin >> inputPass;

        if (inputUser == savedUser && inputPass == savedPass) {
            cout << "Login successful.\n";
            return true;
        } else {
            cout << "Incorrect username or password. Try again.\n";
            attempts++;
        }
    }

    cout << "Too many failed attempts. Access denied.\n";
    return false;
}


void registerUser() {
    ofstream file("usersDB.txt", ios::app); 

    string username, password;
    cout << "\n--- Register New User ---" << endl;
    cout << "Enter new username: ";
    cin >> username;
    cout << "Enter new password: ";
    cin >> password;

    int initialDeposit = 10500;
    string borrowRequest = "none";
    string returnRequest = "none";

    
    file << username << " " << password << " " << initialDeposit << " "
         << borrowRequest << " " << returnRequest << endl;

    cout << "User registered successfully with deposit of " << initialDeposit << " shillings.\n";
}

bool userLogin(string &usernameOut) {
    string username, password;
    string storedUser, storedPass;
    int balance;
    string borrowReq, returnReq;

    while (true) {
        cout << "\n--- User Login ---" << endl;
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;

        ifstream file("usersDB.txt");
        bool found = false;

        while (file >> storedUser >> storedPass >> balance >> borrowReq >> returnReq) {
            if (username == storedUser && password == storedPass) {
                cout << "Login successful.\n";
                usernameOut = storedUser;
                file.close();
                return true;
            }
        }

        file.close();
        cout << "Incorrect username or password. Try again.\n";
    }
}

void viewProfile(string username) {
    ifstream file("usersDB.txt");
    string storedUser, storedPass;
    int balance;
    string borrowReq, returnReq;

    while (file >> storedUser >> storedPass >> balance >> borrowReq >> returnReq) {
        if (storedUser == username) {
            cout << "\n--- Your Profile ---" << endl;
            cout << "Username: " << storedUser << endl;
            cout << "Account Balance: " << balance << " shillings\n";
            cout << "Car Borrow Request: " << borrowReq << endl;
            cout << "Car Return Request: " << returnReq << endl;
            return;
        }
    }

    cout << "User profile not found.\n";
}

void requestCar(string username) {
    ifstream carFile("carsDB.txt");
    string brand[10];
    int stock[10];
    int count = 0;

    cout << "\n--- Available Cars ---\n";

    while (carFile >> brand[count] >> stock[count]) {
        cout << (count + 1) << ". " << brand[count] << " (" << stock[count] << " available)\n";
        count++;
    }
    carFile.close();

    int choice;
    cout << "Enter the number of the car you want to rent: ";
    cin >> choice;

    if (choice < 1 || choice > count) {
        cout << "Invalid choice.\n";
        return;
    }

    string selectedCar = brand[choice - 1];
    if (stock[choice - 1] == 0) {
        cout << "Sorry, " << selectedCar << " is not available.\n";
        return;
    }

    
    ifstream userFile("usersDB.txt");
    ofstream tempFile("temp.txt");

    string user, pass, borrowReq, returnReq;
    int balance;
    bool updated = false;

    while (userFile >> user >> pass >> balance >> borrowReq >> returnReq) {
        if (user == username) {
            if (balance < 5000) {
                cout << "Insufficient balance. You need at least 5000 shillings.\n";
                tempFile << user << " " << pass << " " << balance << " "
                         << borrowReq << " " << returnReq << endl;
            } else {
                borrowReq = selectedCar;
                cout << "Request to rent " << selectedCar << " sent to admin.\n";
                updated = true;
                tempFile << user << " " << pass << " " << balance << " "
                         << borrowReq << " " << returnReq << endl;
            }
        } else {
            tempFile << user << " " << pass << " " << balance << " "
                     << borrowReq << " " << returnReq << endl;
        }
    }

    userFile.close();
    tempFile.close();
    remove("usersDB.txt");
    rename("temp.txt", "usersDB.txt");

    if (!updated) {
        cout << "Request not successful.\n";
    }
}


void approveBorrowRequests() {
    ifstream userFile("usersDB.txt");
    ofstream tempUser("temp_users.txt");

    string user, pass, borrowReq, returnReq;
    int balance;
    bool changeMade = false;

    
    const int MAX_CARS = 50;
    string brand[MAX_CARS];
    int stock[MAX_CARS];
    int carCount = 0;

    
    ifstream carFile("carsDB.txt");
    while (carFile >> brand[carCount] >> stock[carCount]) {
        carCount++;
    }
    carFile.close();

    
    while (userFile >> user >> pass >> balance >> borrowReq >> returnReq) {
        if (borrowReq != "none") {
            
            bool carFound = false;
            for (int i = 0; i < carCount; i++) {
                if (brand[i] == borrowReq) {
                    carFound = true;
                    if (stock[i] > 0) {
                        stock[i]--; 
                        borrowReq = "none";
                        changeMade = true;
                        cout << "Approved: " << user << " has borrowed " << brand[i] << endl;
                    } else {
                        cout << "Car not available for " << user << ": " << brand[i] << " is out of stock.\n";
                    }
                    break;
                }
            }
            if (!carFound) {
                cout << "Car brand '" << borrowReq << "' not found in database.\n";
            }
        }

        
        tempUser << user << " " << pass << " " << balance << " " << borrowReq << " " << returnReq << endl;
    }

    userFile.close();
    tempUser.close();
    remove("usersDB.txt");
    rename("temp_users.txt", "usersDB.txt");

    
    if (changeMade) {
        ofstream newCarFile("carsDB.txt");
        for (int i = 0; i < carCount; i++) {
            newCarFile << brand[i] << " " << stock[i] << endl;
        }
        newCarFile.close();
    }

    cout << "Done processing borrow requests.\n";
}
void requestReturn(string username) {
    ifstream userFile("usersDB.txt");
    ofstream tempFile("temp.txt");

    string user, pass, borrowReq, returnReq;
    int balance;
    bool found = false;

    while (userFile >> user >> pass >> balance >> borrowReq >> returnReq) {
        if (user == username) {
            found = true;
            if (borrowReq == "none") {
                cout << "You have no borrowed car to return.\n";
                tempFile << user << " " << pass << " " << balance << " "
                         << borrowReq << " " << returnReq << endl;
            } else {
                returnReq = borrowReq;
                cout << "Return request for " << returnReq << " sent to admin.\n";
                tempFile << user << " " << pass << " " << balance << " "
                         << borrowReq << " " << returnReq << endl;
            }
        } else {
            tempFile << user << " " << pass << " " << balance << " "
                     << borrowReq << " " << returnReq << endl;
        }
    }

    userFile.close();
    tempFile.close();
    remove("usersDB.txt");
    rename("temp.txt", "usersDB.txt");

    if (!found) {
        cout << "User not found.\n";
    }
}

void approveReturnRequests() {
    ifstream userFile("usersDB.txt");
    ofstream tempUser("temp_users.txt");

    string user, pass, borrowReq, returnReq;
    int balance;
    bool changeMade = false;

    
    const int MAX_CARS = 50;
    string brand[MAX_CARS];
    int stock[MAX_CARS];
    int carCount = 0;

    ifstream carFile("carsDB.txt");
    while (carFile >> brand[carCount] >> stock[carCount]) {
        carCount++;
    }
    carFile.close();

    
    while (userFile >> user >> pass >> balance >> borrowReq >> returnReq) {
        if (returnReq != "none") {
            bool carFound = false;

            for (int i = 0; i < carCount; i++) {
                if (brand[i] == returnReq) {
                    stock[i]++; 
                    borrowReq = "none";
                    returnReq = "none";
                    changeMade = true;
                    cout << "Approved: " << user << " returned " << brand[i] << endl;
                    break;
                }
            }

            if (!carFound) {
                cout << "Car brand '" << returnReq << "' not found.\n";
            }
        }

        tempUser << user << " " << pass << " " << balance << " "
                 << borrowReq << " " << returnReq << endl;
    }

    userFile.close();
    tempUser.close();
    remove("usersDB.txt");
    rename("temp_users.txt", "usersDB.txt");

    if (changeMade) {
        ofstream newCarFile("carsDB.txt");
        for (int i = 0; i < carCount; i++) {
            newCarFile << brand[i] << " " << stock[i] << endl;
        }
        newCarFile.close();
    }

    cout << "Done processing return requests.\n";
}

void changeUserPassword(string username) {
    ifstream inFile("usersDB.txt");
    ofstream outFile("temp.txt");

    string user, pass, newPass;
    int balance;
    string borrowReq, returnReq;
    string oldPassInput;

    bool updated = false;

    while (inFile >> user >> pass >> balance >> borrowReq >> returnReq) {
        if (user == username) {
            cout << "Enter your current password: ";
            cin >> oldPassInput;
            if (oldPassInput == pass) {
                cout << "Enter new password: ";
                cin >> newPass;
                pass = newPass;
                updated = true;
                cout << "Password updated successfully.\n";
            } else {
                cout << "Incorrect current password. Password not changed.\n";
            }
        }

        outFile << user << " " << pass << " " << balance << " "
                << borrowReq << " " << returnReq << endl;
    }

    inFile.close();
    outFile.close();
    remove("usersDB.txt");
    rename("temp.txt", "usersDB.txt");

    if (!updated) {
        cout << "Password update failed.\n";
    }
}

void changeAdminPassword() {
    ifstream inFile("admin.txt");
    string adminUser, adminPass;
    string oldPassInput, newPass;

    inFile >> adminUser >> adminPass;
    inFile.close();

    cout << "Enter your current password: ";
    cin >> oldPassInput;

    if (oldPassInput != adminPass) {
        cout << "Incorrect password. Try again.\n";
        return;
    }

    cout << "Enter new password: ";
    cin >> newPass;

    ofstream outFile("admin.txt");
    outFile << adminUser << " " << newPass;
    outFile.close();

    cout << "Admin password changed successfully.\n";
}

int main() {
    int mainChoice;

    while (true) {
        cout << "\n========== CAR RENTAL SYSTEM ==========\n";
        cout << "1. Admin Login\n";
        cout << "2. User Login\n";
        cout<<"3. User Signup\n";
        cout << "4. Exit\n";
        cout << "Choose an option: ";
        cin >> mainChoice;

        if (mainChoice == 1) {
            if (adminLogin()) {
                
                int adminChoice;
                do {
                    cout << "\n--- ADMIN MENU ---\n";
                    // cout << "1. Register User\n";
                    cout << "2. Approve Borrow Requests\n";
                    cout << "3. Approve Return Requests\n";
                    cout << "4. Change Admin Password\n";
                    cout << "5. Logout\n";
                    cout << "Choose an option: ";
                    cin >> adminChoice;

                    switch (adminChoice) {
                        // case 1: registerUser(); break;
                        case 2: approveBorrowRequests(); break;
                        case 3: approveReturnRequests(); break;
                        case 4: changeAdminPassword(); break;
                        case 5: cout << "Logging out...\n"; break;
                        default: cout << "Invalid choice.\n"; break;
                    }
                } while (adminChoice != 5);
            }

        } else if (mainChoice == 2) {
            string loggedUser;
            if (userLogin(loggedUser)) {
                
                int userChoice;
                do {
                    cout << "\n--- USER MENU ---\n";
                    cout << "1. View Profile\n";
                    cout << "2. Request Car\n";
                    cout << "3. Return Car\n";
                    cout << "4. Change Password\n";
                    cout << "5. Logout\n";
                    cout << "Choose an option: ";
                    cin >> userChoice;

                    switch (userChoice) {
                        case 1: viewProfile(loggedUser); break;
                        case 3: requestReturn(loggedUser); break;
                        case 2: requestCar(loggedUser); break;
                        case 4: requestReturn(loggedUser); break;
                        case 5: changeUserPassword(loggedUser); break;
                        case 6: cout << "Logging out...\n"; break;
                        default: cout << "Invalid choice.\n"; break;
                    }
                } while (userChoice != 5);
            }

        } else if (mainChoice == 4) {
            cout << "Exiting the system. Goodbye!\n";
            break;
        } else if(mainChoice==3){
            registerUser();
        }
        else {
            cout << "Invalid choice. Please enter 1, 2,3, 4\n";
        }
    }

    return 0;
}

