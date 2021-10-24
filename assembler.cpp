#include <iostream>
#include <fstream>
#include <map>
#include <bitset>
using namespace std;

int main() {
    ifstream assemblyInput;
    ofstream machineOutput;
    map<string, bitset<11>> opcodeMap = {
        {"ADD", 0x458},
        {"ADDI", 0x488},
        {"ADDIS", 0x588},
        {"ADDS", 0x558},
        {"SUB", 0x658},
        {"SUBI", 0x688},
        {"SUBIS", 0x788},
        {"SUBS", 0x758},
        {"B", 0x0A0},
        {"CBZ", 0x5A0},
        {"CBNZ", 0x5A8},
        {"LDUR", 0x7C2},
        {"STUR", 0x7C0},
        {"LSL", 0x69B},
        {"LSR", 0x69A}
    };
    string assemblyFileName;
    int lineCount = 0;
    bool fileValid = false;

    cout << "Hello! Please enter the name of the file you want to read input from: ";
    while(!fileValid) {
        cin >> assemblyFileName;
        assemblyInput = ifstream(assemblyFileName, ifstream::in);
        if(assemblyInput.good())
            fileValid = true;
        else 
            cout << "\nno file found with that name... please enter a valid file name: ";
    }
    machineOutput = ofstream(assemblyFileName.substr(0, assemblyFileName.find(".")) + "_output.txt");
    string commandString;
    while(!assemblyInput.eof()) {
        lineCount++;
        assemblyInput >> commandString;
        bitset<11> opcode = opcodeMap[commandString];
        if(opcode != 0x0) {
            string finalLineOutput = opcode.to_string();
            switch(opcode.to_ulong()) {
                //REGISTER FORMAT: TO BE DONE, FOR NOW TRASH INPUT
                case 0x458:
                case 0x558:
                case 0x658:
                case 0x758: 
                case 0x69B: 
                case 0x69A: {
                    string trash;
                    assemblyInput >> trash;
                    assemblyInput >> trash;
                    assemblyInput >> trash;
                    break;
                }

                //IMMEDIATE FORMAT: TO BE DONE, FOR NOW TRASH INPUT
                case 0x488:
                case 0x588:
                case 0x688:
                case 0x788: {
                    string trash;
                    assemblyInput >> trash;
                    assemblyInput >> trash;
                    assemblyInput >> trash;
                    break;
                }

                //DATA FORMAT: TO BE DONE, FOR NOW TRASH INPUT
                case 0x7C2:
                case 0x7C0: {
                    string trash;
                    assemblyInput >> trash;
                    assemblyInput >> trash;
                    assemblyInput >> trash;
                    break;
                }

                //BRANCH FORMAT: TO BE DONE, FOR NOW TRASH INPUT
                case 0x0A0: {
                    string trash;
                    assemblyInput >> trash;
                    break;
                }

                //CONDITIONAL BRANCH FORMAT: TO BE DONE, FOR NOW TRASH INPUT
                case 0x5A0:
                case 0x5A8: {
                    string trash;
                    assemblyInput >> trash;
                    assemblyInput >> trash;
                    break;
                }
            }
            machineOutput << finalLineOutput << endl;
        }
        //handle comments: trash this and next input
        else if(commandString == "#"){
            string trash;
            assemblyInput >> trash;
            lineCount--;
        }
        //if not a proper instruction/comment/label, error out and terminate
        else if(commandString.back() != ':') {
            machineOutput << "ERROR on line " << lineCount << ": not a valid Instruction or Label." << endl;
            cout << "ERROR on line " << lineCount << ": not a valid Instruction or Label." << endl;
            exit(0);
        }
    }
    assemblyInput.close();
    machineOutput << "-- End of Program --";
    machineOutput.close();
    cout << "Success! Machine code written to " << assemblyFileName.substr(0, assemblyFileName.find(".")) + "_output.txt!" << endl;
}