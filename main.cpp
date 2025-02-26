#include <iostream>
#include <limits>
#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <fstream>
#include <termios.h>
using namespace std;

void ClearScreen() {
    cout << "\033[2J\033[1;1H";
}

void PrintPaddedValue(const string& label, int value, const string& action) {
    const int TOTAL_WIDTH = 55;

    string valueStr;
    int padding;
    bool checkValueRequired = false;
    if(value == -1) {
        valueStr = action;

        padding = 55;
    }
    else {
        valueStr = to_string(value);
        checkValueRequired = true;

        padding = TOTAL_WIDTH - valueStr.size();
    }
    padding = padding - label.size();

    string result = checkValueRequired ? valueStr : "";
    cout << label << result;
    for(int i = 0; i < padding; i++) {
        cout << " ";
    }
    cout << "[" << action << "]\n";
}

class RepetitionSums {
private:
    int minRepetition;
    int maxRepetition;
    bool printValue;

    void SetRepetitionValues(bool minmax) {
        string minmaxStr = "minimum";
        if(minmax) minmaxStr = "maximum";
        int repValue;
        while(true) {
            cout << "What should the " << minmaxStr << " value be?: ";
            cin >> repValue;
            if(cin.fail()) {
                cout << "\nInput is invalid, please try again.";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                ClearScreen();
                continue;
            }

            if(repValue < 0) {
                cout << "\n" << minmaxStr << " can't be lower than 0";
                continue;
            }

            if(minmax) {
                if(repValue <= minRepetition) {
                    cout << "\nmaxRepetition can't be lower or equal to minRepetition.";
                    continue;
                }
                cout << "\nThe " << minmaxStr << " value is now " << repValue << "\n";
                maxRepetition = repValue;
                break;
            }
            cout << "\nThe " << minmaxStr << " value is now " << repValue << "\n";
            minRepetition = repValue;
            break;
        }
    }

public:
    RepetitionSums(int min = 0, int max = 10, bool printValue = false) : minRepetition(min), maxRepetition(max), printValue(printValue) {}

    void SetMinRepetition() {
        SetRepetitionValues(false);
    }

    void SetMaxRepetition() {
        SetRepetitionValues(true);
    }

    void SetPrintValue() {
        printValue = !printValue;
    }

    int GetMinRepetition() const {
        return minRepetition;
    }

    int GetMaxRepetition() const {
        return maxRepetition;
    }

    bool GetPrintValue() const {
        return printValue;
    }
};

class Menu {
private:
    RepetitionSums rSums;
    int minRep;
    int maxRep;
    bool printValueBool;
    string printValue;

public:
    void SetRepetitionSums(RepetitionSums rSumsGet) {
        rSums = rSumsGet;
    }

    void DrawMenu() {
        minRep = rSums.GetMinRepetition();
        maxRep = rSums.GetMaxRepetition();

        cout << "Welcome to my Collatz Conejecture calculator. It's not the best, but it's still cool!\n";

        PrintPaddedValue("Minimum repetition value: ", minRep, "SetMinRepetition");
        PrintPaddedValue("Maximum repetition value: ", maxRep, "SetMaxRepetition");
        printValueBool = rSums.GetPrintValue();
        printValue = "";
        if(printValueBool) {
            printValue = "Print to file: yes";
        }
        else {
            printValue = "Print to file: no";
        }
        PrintPaddedValue(printValue, -1, "Print");
        PrintPaddedValue("Run!", -1, "Run");
        PrintPaddedValue("Quit!", -1, "Quit");
        cout << "\nWhat would you like to do?: ";
    }

    RepetitionSums GetRepetitionSums() {
        return rSums;
    }
};

class CollatzConjecture {
private:
    int min;
    int max;

    string getCurrentDateTime() {
        auto now = chrono::system_clock::now();
        auto now_time_t = chrono::system_clock::to_time_t(now);
        tm now_tm = *localtime(&now_time_t);

        std::ostringstream oss;
        oss << std::put_time(&now_tm, "%Y%m%d_%H%M%S");

        return oss.str();
    }

public:
    bool SetMinMax(RepetitionSums rSums) {
        min = rSums.GetMinRepetition();
        max = rSums.GetMaxRepetition();
        if(min >= max) {
            cout << "maxRepetition cannot be less than or equal to minRepetition";
            return false;
        }
        return true;
    }

    void Collatz(bool printValue) {
        int sum;
        string print;

        ofstream outFile;
        if(printValue) {
            string dateTimeString = getCurrentDateTime() + ".txt";
            outFile.open(dateTimeString);
        }

        for(int i = min; i <= max; i++) {
            sum = i;
            print = "Starting with " + to_string(sum) + ": {";
            while(sum != 1) {
                print += to_string(sum) + ", ";
                // even
                if(!(sum % 2)) {
                    sum = sum / 2;
                }
                // odd
                else {
                    sum = sum * 3 + 1;
                }
            }
            print += to_string(sum) + " }";
            if(outFile.is_open()) {
                cout << "New file opened\n";
                outFile << print << endl;
            }
            cout << print << "\n";
        }

        if(outFile.is_open()) {
            outFile.close();
        }
    }
};

class ManagerClass {
private:
    RepetitionSums rSums;
    Menu menu;
    CollatzConjecture colz;

    void PauseTerminal() {
        cout << "\nPress any key to continue...\n";
        termios oldt;
        tcgetattr(STDIN_FILENO, &oldt);

        termios newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        char ch;
        read(STDIN_FILENO, &ch, 1);

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
public:
    short menuNavigatorClass() {
        string optionSelect;
    
        while(true) {
            cin >> optionSelect;
    
            for(int i = 0; i < optionSelect.size(); i++) {
                optionSelect[i] = tolower(optionSelect[i]);
            }
    
            if(optionSelect == "setminrepetition" || optionSelect == "min") {
                ClearScreen();
                rSums.SetMinRepetition();
                menu.SetRepetitionSums(rSums);
                menu.DrawMenu();
            }
            else if(optionSelect == "setmaxrepetition" || optionSelect == "max") {
                ClearScreen();
                rSums.SetMaxRepetition();
                menu.SetRepetitionSums(rSums);
                menu.DrawMenu();
            }
            else if(optionSelect == "print") {
                ClearScreen();
                rSums.SetPrintValue();
                menu.SetRepetitionSums(rSums);
                menu.DrawMenu();
            }
            else if(optionSelect == "run") {
                ClearScreen();
                bool check = colz.SetMinMax(rSums);
                if(!check) {
                    return -1;
                }
                colz.Collatz(rSums.GetPrintValue());
                PauseTerminal();
                ClearScreen();
                menu.DrawMenu();
            }
            else if(optionSelect == "quit") {
                return 0;
            }
            else {
                ClearScreen();
                cout << "Invalid option, please try again.\n";
                menu.DrawMenu();
                continue;
            }
        }
    }
};

int main() {
    ManagerClass mClass;
    Menu menu;

    bool escape;
    short callCode;
    while(true) {
        ClearScreen();
        menu.DrawMenu();
        callCode = mClass.menuNavigatorClass();
        if(callCode == 0) break;
    }

    return 0;
}

