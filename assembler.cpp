#include <iostream>
#include <fstream>
#include <sstream>
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
    map<string, int> labelMap;
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
                    finalLineOutput += opcode.to_string() + " ";

                    // get rd value
                    bitset<5> rdValue;
                    string rdString;
                    assemblyInput >> rdString;
                    if(rdString.length() != 3  && rdString.length() != 4)
                        errorOutAndExit(machineOutput, lineCount, "\'" + rdString + "'\' invalid length for register reference");
                    else if(rdString.at(0) != 'R' && rdString.at(0) != 'X')
                        errorOutAndExit(machineOutput, lineCount, "\'" + rdString + "'\' is not a proper way to reference a register");
                    else if(rdString.at(rdString.length() - 1) != ',')
                        errorOutAndExit(machineOutput, lineCount, "\'" + rdString + "'\' missing comma");
                    if(isdigit(rdString.at(1))) {
                        string rdValueString;
                        if(rdString.length() == 4) {
                            if(isdigit(rdString.at(2)))
                                rdValueString = rdString.substr(1, 2);
                            else
                                errorOutAndExit(machineOutput, lineCount, "\'" + rdString + "'\' is not a proper way to reference a register");
                        }
                        else
                            rdValueString = rdString.substr(1, 1);
                        stringstream rdValueStream;
                        int rdValueInt;
                        rdValueStream << rdValueString;
                        rdValueStream >> rdValueInt;
                        rdValue = bitset<5> (rdValueInt);
                        if(rdValue.to_ulong() < 0 || rdValue.to_ulong() > 31)
                            errorOutAndExit(machineOutput, lineCount, "\'" + rdString + "'\' referenced a register number that does not exist");
                    }
                    else
                        errorOutAndExit(machineOutput, lineCount, "\'" + rdString + "'\' is not a proper way to reference a register");

                    //get rn value
                    bitset<5> rnValue;
                    string rnString;
                    assemblyInput >> rnString;
                    if(rnString.length() != 3  && rnString.length() != 4)
                        errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' invalid length for register reference");
                    else if(rnString.at(0) != 'R' && rnString.at(0) != 'X')
                        errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' is not a proper way to reference a register");
                    else if(rnString.at(rnString.length() - 1) != ',')
                        errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' missing comma");
                    if(isdigit(rnString.at(1))) {
                        string rnValueString;
                        if(rnString.length() == 4) {
                            if(isdigit(rnString.at(2)))
                                rnValueString = rnString.substr(1, 2);
                            else
                                errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' is not a proper way to reference a register");
                        }
                        else
                            rnValueString = rnString.substr(1, 1);
                        stringstream rnValueStream;
                        int rnValueInt;
                        rnValueStream << rnValueString;
                        rnValueStream >> rnValueInt;
                        rnValue = bitset<5> (rnValueInt);
                        if(rnValue.to_ulong() < 0 || rnValue.to_ulong() > 31)
                            errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' referenced a register number that does not exist");
                    }
                    else
                        errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' is not a proper way to reference a register");

                    //get rm value OR shamtValue, depending on the instruction
                    bitset<5> rmValue;
                    bitset<6> shamtValue;
                    string thirdString;
                    assemblyInput >> thirdString;
                    if(thirdString.length() != 2 || thirdString.length() != 3) {
                        if(thirdString.at(0) == 'R' || rdString.at(0) == 'X') {
                            shamtValue = bitset<6> (0);
                            if(isdigit(thirdString.at(1))) {
                                string rmValueString;
                                if(thirdString.length() == 3) {
                                    if(isdigit(thirdString.at(2)))
                                        rmValueString = thirdString.substr(1, 2);
                                    else
                                        errorOutAndExit(machineOutput, lineCount, "\'" + thirdString + "\' is not a proper way to reference a register");
                                }
                                else
                                    rmValueString = thirdString.substr(1, 1);
                                stringstream rmValueStream;
                                int rmValueInt;
                                rmValueStream << rmValueString;
                                rmValueStream >> rmValueInt;
                                rmValue = bitset<5> (rmValueInt);
                                if(rmValue.to_ulong() < 0 || rmValue.to_ulong() > 31)
                                    errorOutAndExit(machineOutput, lineCount, "\'" + thirdString + "'\' referenced a register number that does not exist");
                            }
                            else
                                errorOutAndExit(machineOutput, lineCount, "\'" + thirdString + "\' is not a proper way to reference a register");
                        }
                        else if(thirdString.at(0) == '#') {
                            rmValue = bitset<5> (0);
                            if(isdigit(thirdString.at(1))) {
                                string shamtValueString;
                                if(thirdString.length() == 3) {
                                    if(isdigit(thirdString.at(2)))
                                        shamtValueString = thirdString.substr(1, 2);
                                    else
                                        errorOutAndExit(machineOutput, lineCount, "\'" + thirdString + "\' is not a proper way to reference a register");
                                }
                                else
                                    shamtValueString = thirdString.substr(1, 1);
                                stringstream shamtValueStream;
                                int shamtValueInt;
                                shamtValueStream << shamtValueString;
                                shamtValueStream >> shamtValueInt;
                                shamtValue = bitset<6> (shamtValueInt);
                            }
                            else
                                errorOutAndExit(machineOutput, lineCount, "\'" + thirdString + "\' is not a proper way to reference a register");
                        }
                        else {
                            errorOutAndExit(machineOutput, lineCount, "\'" + thirdString + "\' is not a proper way to reference a register or an immediate");
                        }
                    }
                    else
                        errorOutAndExit(machineOutput, lineCount, "\'" + rdString + "\' invalid length for register reference");

                    finalLineOutput += rmValue.to_string() + " " + shamtValue.to_string() + " " + rnValue.to_string() + " " + rdValue.to_string();
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
                    finalLineOutput += reducedOpcode.to_string() + " ";

                    // get rd value
                    bitset<5> rdValue;
                    string rdString;
                    assemblyInput >> rdString;
                    if(rdString.length() != 3  && rdString.length() != 4)
                        errorOutAndExit(machineOutput, lineCount, "\'" + rdString + "'\' invalid length for register reference");
                    else if(rdString.at(0) != 'R' && rdString.at(0) != 'X')
                        errorOutAndExit(machineOutput, lineCount, "\'" + rdString + "'\' is not a proper way to reference a register");
                    else if(rdString.at(rdString.length() - 1) != ',')
                        errorOutAndExit(machineOutput, lineCount, "\'" + rdString + "'\' missing comma");
                    if(isdigit(rdString.at(1))) {
                        string rdValueString;
                        if(rdString.length() == 4) {
                            if(isdigit(rdString.at(2)))
                                rdValueString = rdString.substr(1, 2);
                            else
                                errorOutAndExit(machineOutput, lineCount, "\'" + rdString + "'\' is not a proper way to reference a register");
                        }
                        else
                            rdValueString = rdString.substr(1, 1);
                        stringstream rdValueStream;
                        int rdValueInt;
                        rdValueStream << rdValueString;
                        rdValueStream >> rdValueInt;
                        rdValue = bitset<5> (rdValueInt);
                        if(rdValue.to_ulong() < 0 || rdValue.to_ulong() > 31)
                            errorOutAndExit(machineOutput, lineCount, "\'" + rdString + "'\' referenced a register number that does not exist");
                    }
                    else
                        errorOutAndExit(machineOutput, lineCount, "\'" + rdString + "'\' is not a proper way to reference a register");

                    //get rn value
                    bitset<5> rnValue;
                    string rnString;
                    assemblyInput >> rnString;
                    if(rnString.length() != 3  && rnString.length() != 4)
                        errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' invalid length for register reference");
                    else if(rnString.at(0) != 'R' && rnString.at(0) != 'X')
                        errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' is not a proper way to reference a register");
                    else if(rnString.at(rnString.length() - 1) != ',')
                        errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' missing comma");
                    if(isdigit(rnString.at(1))) {
                        string rnValueString;
                        if(rnString.length() == 4) {
                            if(isdigit(rnString.at(2)))
                                rnValueString = rnString.substr(1, 2);
                            else
                                errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' is not a proper way to reference a register");
                        }
                        else
                            rnValueString = rnString.substr(1, 1);
                        stringstream rnValueStream;
                        int rnValueInt;
                        rnValueStream << rnValueString;
                        rnValueStream >> rnValueInt;
                        rnValue = bitset<5> (rnValueInt);
                        if(rnValue.to_ulong() < 0 || rnValue.to_ulong() > 31)
                            errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' referenced a register number that does not exist");
                    }
                    else
                        errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' is not a proper way to reference a register");

                    //get ALU_immediate value
                    bitset<12> immValue;
                    string immString;
                    assemblyInput >> immString;
                    if(immString.length() != 2  && immString.length() != 3)
                        errorOutAndExit(machineOutput, lineCount, "\'" + immString + "'\' invalid length for immediate reference");
                    else if(immString.at(0) != '#')
                        errorOutAndExit(machineOutput, lineCount, "\'" + immString + "'\' is not a proper way to reference an immediate");
                    if(isdigit(immString.at(1))) {
                        string immValueString;
                        if(immString.length() == 3) {
                            if(isdigit(immString.at(2)))
                                immValueString = immString.substr(1, 2);
                            else
                                errorOutAndExit(machineOutput, lineCount, "\'" + immString + "'\' is not a proper way to reference an immediate");
                        }
                        else
                            immValueString = immString.substr(1, 1);
                        stringstream immValueStream;
                        int immValueInt;
                        immValueStream << immValueString;
                        immValueStream >> immValueInt;
                        immValue = bitset<12> (immValueInt);
                        if(immValue.to_ulong() < 0 || immValue.to_ulong() > 31)
                            errorOutAndExit(machineOutput, lineCount, "\'" + immString + "'\' referenced a register number that does not exist");
                    }
                    else
                        errorOutAndExit(machineOutput, lineCount, "\'" + immString + "'\' is not a proper way to reference a register");

                    finalLineOutput += immValue.to_string() + " " + rnValue.to_string() + " " + rdValue.to_string();
                    break;
                }

                //DATA FORMAT:
                case 0x7C2:
                case 0x7C0: {
                    //push size 11 opcode to output
                    finalLineOutput += opcode.to_string() + " ";

                    //get rt value
                  bitset<5> rtValue;
                  string rtString;
                  assemblyInput >> rtString;
                  if (rtString.length() != 3 && rtString.length() != 4)
                      errorOutAndExit(machineOutput, lineCount, "\'" + rtString + "'\' invalid length for register reference");
                  else if (rtString.at(0) != 'R' && rtString.at(0) != 'X')
                      errorOutAndExit(machineOutput, lineCount, "\'" + rtString + "'\' is not a proper way to reference a register");
                  else if (rtString.at(rtString.length() - 1) != ',')
                      errorOutAndExit(machineOutput, lineCount, "\'" + rtString + "'\' missing comma");
                  if (isdigit(rtString.at(1))) {
                      string rtValueString;
                      if (rtString.length() == 4) {
                          if (isdigit(rtString.at(2)))
                              rtValueString = rtString.substr(1, 2);
                          else
                              errorOutAndExit(machineOutput, lineCount, "\'" + rtString + "'\' is not a proper way to reference a register");
                      }
                      else
                          rtValueString = rtString.substr(1, 1);
                      stringstream rtValueStream;
                      int rtValueInt;
                      rtValueStream << rtValueString;
                      rtValueStream >> rtValueInt;
                      rtValue = bitset<5>(rtValueInt);
                      if (rtValue.to_ulong() < 0 || rtValue.to_ulong() > 31)
                          errorOutAndExit(machineOutput, lineCount, "\'" + rtString + "'\' referenced a register number that does not exist");
                  }
                  else
                      errorOutAndExit(machineOutput, lineCount, "\'" + rtString + "'\' is not a proper way to reference a register");
                  //get rn value
                  bitset<5> rnValue;
                  string rnString;
                  assemblyInput >> rnString;
                  if (rnString.length() != 4 && rnString.length() != 5)
                      errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' invalid length for register reference");
                  else if (rnString.at(0) != '[')
                      errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' must include brackets for data format");
                  else if (rnString.at(1) != 'R' && rnString.at(1) != 'X')
                      errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' is not a proper way to reference a register");
                  else if (rnString.at(rnString.length() - 1) != ',')
                      errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' missing comma");
                  if (isdigit(rnString.at(2))) {
                      string rnValueString;
                      if (rnString.length() == 5) {
                          if (isdigit(rnString.at(3)))
                              rnValueString = rnString.substr(2, 3);
                          else
                              errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' is not a proper way to reference a register");
                      }
                      else
                          rnValueString = rnString.substr(2, 2);
                      stringstream rnValueStream;
                      int rnValueInt;
                      rnValueStream << rnValueString;
                      rnValueStream >> rnValueInt;
                      rnValue = bitset<5>(rnValueInt);
                      if (rnValue.to_ulong() < 0 || rnValue.to_ulong() > 31)
                          errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' referenced a register number that does not exist");
                  }
                  else
                      errorOutAndExit(machineOutput, lineCount, "\'" + rnString + "'\' is not a proper way to reference a register");
                  //get DT_address value
                  bitset<12> offsetValue;
                  string offsetString;
                  assemblyInput >> offsetString;
                  if (offsetString.length() != 3 && offsetString.length() != 4)
                      errorOutAndExit(machineOutput, lineCount, "\'" + offsetString + "'\' invalid length for offset reference");
                  else if (offsetString.at(0) != '#')
                      errorOutAndExit(machineOutput, lineCount, "\'" + offsetString + "'\' must include brackets for data format");
                  else if (offsetString.at(offsetString.length() - 1) != ']')
                      errorOutAndExit(machineOutput, lineCount, "\'" + offsetString + "'\' is not a proper way to reference an offset value");
                  if (isdigit(offsetString.at(1))) {
                      string offsetValueString;
                      if (offsetString.length() == 5) {
                          if (isdigit(offsetString.at(2)))
                              offsetValueString = offsetString.substr(1, 2);
                          else
                              errorOutAndExit(machineOutput, lineCount, "\'" + offsetString + "'\' is not a proper way to reference an offset");
                      }
                      else
                          offsetValueString = offsetString.substr(1, 1);
                      stringstream offsetValueStream;
                      int offsetValueInt;
                      offsetValueStream << offsetValueString;
                      offsetValueStream >> offsetValueInt;
                      offsetValue = bitset<12>(offsetValueInt);
                      if (offsetValue.to_ulong() < 0 || offsetValue.to_ulong() > 31)
                          errorOutAndExit(machineOutput, lineCount, "\'" + offsetString + "'\' referenced a register number that does not exist");
                  }
                  else
                      errorOutAndExit(machineOutput, lineCount, "\'" + offsetString + "'\' is not a proper way to reference a register");

                      finalLineOutput += offsetValue.to_string() + " " + rnValue.to_string() + " " + rtValue.to_string();
                      break;

                }

                //BRANCH FORMAT:
                case 0x0A0: {
                    finalLineOutput += "000101 "; //hardcoded since reduced opcode for b does not follow standard rules

                    string labelName;
                    assemblyInput >> labelName;
                    //if label has not been declared, leave a warning flag. Will error out at the end if this label is never declared
                    if(labelMap[labelName] == 0)
                        labelMap[labelName] == -1;
                    finalLineOutput += labelName;
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
                    finalLineOutput += reducedOpcode.to_string() + " ";
                    //get rt value
                    bitset<5> rtValue;
                    string rtString;
                    assemblyInput >> rtString;
                    if(rtString.length() != 3  && rtString.length() != 4)
                        errorOutAndExit(machineOutput, lineCount, "\'" + rtString + "'\' invalid length for register reference");
                    else if(rtString.at(0) != 'R' && rtString.at(0) != 'X')
                        errorOutAndExit(machineOutput, lineCount, "\'" + rtString + "'\' is not a proper way to reference a register");
                    else if(rtString.at(rtString.length() - 1) != ',')
                        errorOutAndExit(machineOutput, lineCount, "\'" + rtString + "'\' missing comma");
                    if(isdigit(rtString.at(1))) {
                        string rtValueString;
                        if(rtString.length() == 4) {
                            if(isdigit(rtString.at(2)))
                                rtValueString = rtString.substr(1, 2);
                            else
                                errorOutAndExit(machineOutput, lineCount, "\'" + rtString + "'\' is not a proper way to reference a register");
                        }
                        else
                            rtValueString = rtString.substr(1, 1);
                        stringstream rtValueStream;
                        int rtValueInt;
                        rtValueStream << rtValueString;
                        rtValueStream >> rtValueInt;
                        rtValue = bitset<5> (rtValueInt);
                        if(rtValue.to_ulong() < 0 || rtValue.to_ulong() > 31)
                            errorOutAndExit(machineOutput, lineCount, "\'" + rtString + "'\' referenced a register number that does not exist");
                    }
                    else
                        errorOutAndExit(machineOutput, lineCount, "\'" + rtString + "'\' is not a proper way to reference a register");

                    //retrieve label
                    string labelName;
                    assemblyInput >> labelName;
                    //if label has not been declared, leave a warning flag. Will error out at the end if this label is never declared
                    if(labelMap[labelName] == 0)
                        labelMap[labelName] == -1;
                    finalLineOutput += labelName + " " + rtValue.to_string();
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
        //if it is a label, save the label name and the index of the next line in the code
        else if(commandString.back() == ':')
            labelMap[commandString.substr(0, commandString.length() - 1)] = lineCount + 1;
        //if not a proper instruction/comment/label, error out for reason invalid instruction/label
        else
            errorOutAndExit(machineOutput, lineCount, "\'" + commandString + "\' is not a valid Instruction Name or Label.");
    }
    //check to see if any used labels were never declared
    for(map<string, int>::iterator i = labelMap.begin(); i != labelMap.end(); i++) {
        if(i->second == 0)
            errorOutAndExit(machineOutput, lineCount, "\'" + i->first + "\' was used as a label in code but was never declared.");
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
