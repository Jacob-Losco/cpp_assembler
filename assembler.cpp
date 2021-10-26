#include <iostream>
#include <fstream>
#include <map>
#include <bitset>
#include <string>
using namespace std;

void errorOutAndExit(ofstream& output, int lineNumber, string reason);

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

    //input and output file stream setups
    cout << "Hello! Please enter the name of the file you want to read input from: ";
    while(!fileValid) {
        cin >> assemblyFileName;
        assemblyInput = ifstream(assemblyFileName);
        if(assemblyInput.good())
            fileValid = true;
        else 
            cout << "\nno file found with that name... please enter a valid file name: ";
    }
    machineOutput = ofstream(assemblyFileName.substr(0, assemblyFileName.find(".")) + "_output.txt");
    
    string commandString;
    //while there is still content to read from file
    while(!assemblyInput.eof()) {
        lineCount++; //on next line
        assemblyInput >> commandString;
        cout << commandString << endl;
        bitset<11> opcode = opcodeMap[commandString]; //get binary opcode in 11 bits.
        if(opcode != 0x0) { //if the opcode exists
            string finalLineOutput; //the final string that will be printed to a line of the output file
            switch(opcode.to_ulong()) {
                //REGISTER FORMAT:
                case 0x458:
                case 0x558:
                case 0x658:
                case 0x758: 
                case 0x69B: 
                case 0x69A: {
                    //push size 11 opcode to output
                    finalLineOutput += opcode.to_string();

                    //TODO, for now trash input
                    string trash;
                    assemblyInput >> trash;
                    assemblyInput >> trash;
                    assemblyInput >> trash;
                    break;
                }

                //IMMEDIATE FORMAT:
                case 0x488:
                case 0x588:
                case 0x688:
                case 0x788: {
                    //reformat the opcode so that it is in 10 bits
                    bitset<10> reducedOpcode = bitset<10> (0);
                    while(!opcode[opcode.size() -1]) //while the MSB of testInit is not 1, shift left
                        opcode <<= 1;
                    reducedOpcode = bitset<10> (opcode.to_string().substr(0, opcode.size() - (opcode.size() - reducedOpcode.size()))); //remove zeros off the end until it fits new size
                    finalLineOutput += reducedOpcode.to_string();

                    //TODO, for now trash input
                    string trash;
                    assemblyInput >> trash;
                    assemblyInput >> trash;
                    assemblyInput >> trash;
                    break;
                }

                //DATA FORMAT:
                case 0x7C2:
                case 0x7C0: {
                    //push size 11 opcode to output
                    finalLineOutput += opcode.to_string();

                    //TODO, for now trash input
                    string trash;
                    assemblyInput >> trash;
                    assemblyInput >> trash;
                    assemblyInput >> trash;
                    break;
                }

                //BRANCH FORMAT:
                case 0x0A0: {
                    finalLineOutput += "000101"; //hardcoded since reduced opcode for b does not follow standard rules

                    //TODO, for now trash input
                    string trash;
                    assemblyInput >> trash;
                    break;
                }

                //CONDITIONAL BRANCH FORMAT:
                case 0x5A0:
                case 0x5A8: {
                    //reformat the opcode so that it is in 8 bits
                    bitset<8> reducedOpcode = bitset<8> (0);
                    while(!opcode[opcode.size() -1]) //while the MSB of testInit is not 1, shift left 1
                        opcode <<= 1;
                    reducedOpcode = bitset<8> (opcode.to_string().substr(0, opcode.size() - (opcode.size() - reducedOpcode.size()))); //remove zeros off the end until it fits new size
                    finalLineOutput += reducedOpcode.to_string();

                    //TODO, for now trash input
                    string trash;
                    assemblyInput >> trash;
                    assemblyInput >> trash;
                    break;
                }
            }
            machineOutput << finalLineOutput << endl; //print to line in file
        }
        //handle comments: trash this and next input
        else if(commandString == "#"){
            string trash;
            assemblyInput >> trash;
            lineCount--;
        }
        //if not a proper instruction/comment/label, error out for reason invalid instruction/label
        else if(commandString.back() != ':') {
            errorOutAndExit(machineOutput, lineCount, "\'" + commandString + "\' is not a valid Instruction Name or Label.");
        }
    }
    assemblyInput.close();
    machineOutput << "-- End of Program --";
    machineOutput.close();
    cout << "Success! Machine code written to " << assemblyFileName.substr(0, assemblyFileName.find(".")) + "_output.txt!" << endl;
}

//precondition: output is the stream to the output file
//postcondition: writes to output and cl that there was an error at line lineNumber, and includes a reason.
void errorOutAndExit(ofstream& output, int lineNumber, string reason) {
    output << "ERROR on line " << lineNumber << ": " << reason << endl;
    cout << "ERROR on line " << lineNumber << ": " << reason << endl;
    exit(0);
}