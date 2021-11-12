#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <bitset>
#include <string>
using namespace std;

//precondition:
  //output - an output stream for writing error messages to
  //lineNumber - the index of what line of the machine code we are writing
  //registerString - the string containing some code that needs to be translated to a register binary
  //isEnd - determines whether or not we should check to see if there is a comma at the end of the string
bitset<5> getRegister(ofstream& output, int lineNumber, string registerString, bool isEnd);

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
    map<string, int> labelDeclarationMap;
    map<string, int> labelCallMap;
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
    string finalFileOutput; //the final string that will be printed to the output file
    //while there is still content to read from file
    while(!assemblyInput.eof()) {
        lineCount++; //on next line
        commandString = "";
        assemblyInput >> commandString;
        bitset<11> opcode = opcodeMap[commandString]; //get binary opcode in 11 bits.
        if(opcode != 0x0) { //if the opcode exists

            switch(opcode.to_ulong()) {
                //REGISTER FORMAT:
                case 0x458:
                case 0x558:
                case 0x658:
                case 0x758:
                case 0x69B:
                case 0x69A: {
                    //push size 11 opcode to output
                    finalFileOutput += opcode.to_string() + " ";

                    // get rd value
                    bitset<5> rdValue;
                    string rdString;
                    assemblyInput >> rdString;
                    rdValue = getRegister(machineOutput, lineCount, rdString, false);

                    //get rn value
                    bitset<5> rnValue;
                    string rnString;
                    assemblyInput >> rnString;
                    rnValue = getRegister(machineOutput, lineCount, rnString, false);

                    //get rm value OR shamtValue, depending on the instruction
                    bitset<5> rmValue;
                    bitset<6> shamtValue;
                    string thirdString;
                    assemblyInput >> thirdString;
                    if(thirdString.length() != 2 || thirdString.length() != 3) {
                        if(thirdString.at(0) == 'R' || rdString.at(0) == 'X') {
                            shamtValue = bitset<6> (0);
                            rmValue = getRegister(machineOutput, lineCount, thirdString, true);
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

                    finalFileOutput += rmValue.to_string() + " " + shamtValue.to_string() + " " + rnValue.to_string() + " " + rdValue.to_string() + "\n";
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
                    finalFileOutput += reducedOpcode.to_string() + " ";

                    // get rd value
                    bitset<5> rdValue;
                    string rdString;
                    assemblyInput >> rdString;
                    rdValue = getRegister(machineOutput, lineCount, rdString, false);

                    //get rn value
                    bitset<5> rnValue;
                    string rnString;
                    assemblyInput >> rnString;
                    rnValue = getRegister(machineOutput, lineCount, rnString, false);

                    //get ALU_immediate value
                    bitset<12> immValue;
                    string immString;
                    assemblyInput >> immString;
                    if(immString.length() != 2  && immString.length() != 3)
                        errorOutAndExit(machineOutput, lineCount, "\'" + immString + "\' invalid length for immediate reference");
                    else if(immString.at(0) != '#')
                        errorOutAndExit(machineOutput, lineCount, "\'" + immString + "\' is not a proper way to reference an immediate");
                    if(isdigit(immString.at(1))) {
                        string immValueString;
                        if(immString.length() == 3) {
                            if(isdigit(immString.at(2)))
                                immValueString = immString.substr(1, 2);
                            else
                                errorOutAndExit(machineOutput, lineCount, "\'" + immString + "\' is not a proper way to reference an immediate");
                        }
                        else
                            immValueString = immString.substr(1, 1);
                        stringstream immValueStream;
                        int immValueInt;
                        immValueStream << immValueString;
                        immValueStream >> immValueInt;
                        immValue = bitset<12> (immValueInt);
                        if(immValue.to_ulong() < 0 || immValue.to_ulong() > 31)
                            errorOutAndExit(machineOutput, lineCount, "\'" + immString + "\' referenced a register number that does not exist");
                    }
                    else
                        errorOutAndExit(machineOutput, lineCount, "\'" + immString + "\' is not a proper way to reference a register");

                    finalFileOutput += immValue.to_string() + " " + rnValue.to_string() + " " + rdValue.to_string() + "\n";
                    break;
                }

                //DATA FORMAT:
                case 0x7C2:
                case 0x7C0: {
                  //push size 11 opcode to output
                  finalFileOutput += opcode.to_string() + " ";

                  //get rt value
                  bitset<5> rtValue;
                  string rtString;
                  assemblyInput >> rtString;
                  rtValue = getRegister(machineOutput, lineCount, rtString, false);

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
                      errorOutAndExit(machineOutput, lineCount, "\'" + offsetString + "\' invalid length for offset reference");
                  else if (offsetString.at(0) != '#')
                      errorOutAndExit(machineOutput, lineCount, "\'" + offsetString + "\' must include brackets for data format");
                  else if (offsetString.at(offsetString.length() - 1) != ']')
                      errorOutAndExit(machineOutput, lineCount, "\'" + offsetString + "\' is not a proper way to reference an offset value");
                  if (isdigit(offsetString.at(1))) {
                      string offsetValueString;
                      if (offsetString.length() == 5) {
                          if (isdigit(offsetString.at(2)))
                              offsetValueString = offsetString.substr(1, 2);
                          else
                              errorOutAndExit(machineOutput, lineCount, "\'" + offsetString + "\' is not a proper way to reference an offset");
                      }
                      else
                          offsetValueString = offsetString.substr(1, 1);
                      stringstream offsetValueStream;
                      int offsetValueInt;
                      offsetValueStream << offsetValueString;
                      offsetValueStream >> offsetValueInt;
                      offsetValue = bitset<12>(offsetValueInt);
                      if (offsetValue.to_ulong() < 0 || offsetValue.to_ulong() > 31)
                          errorOutAndExit(machineOutput, lineCount, "\'" + offsetString + "\' referenced a register number that does not exist");
                  }
                  else
                      errorOutAndExit(machineOutput, lineCount, "\'" + offsetString + "\' is not a proper way to reference a register");
                  finalFileOutput += offsetValue.to_string() + " " + rnValue.to_string() + " " + rtValue.to_string() + "\n";
                  break;
                }

                //BRANCH FORMAT:
                case 0x0A0: {
                    finalFileOutput += "000101 "; //hardcoded since reduced opcode for b does not follow standard rules

                    string labelName;
                    assemblyInput >> labelName;
                    char callTag = 65;
                    char labelFormat = 'B'; //used later to reduce bits for conditional branch
                    bool set = false;
                    do {
                      if(labelCallMap[labelName + callTag + labelFormat] == 0) {
                        map<string, int>::iterator changeElement = labelCallMap.find(labelName + callTag + labelFormat);
                        if(changeElement != labelCallMap.end())
                          changeElement->second = lineCount;
                        else
                          labelCallMap.insert(pair<string, int> (labelName + callTag + labelFormat, lineCount));
                        set = true;
                      }
                      else
                        callTag++;
                    } while(!set);
                    finalFileOutput += labelName + callTag  + labelFormat + "\n";
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
                    finalFileOutput += reducedOpcode.to_string() + " ";

                    //get rt value
                    bitset<5> rtValue;
                    string rtString;
                    assemblyInput >> rtString;
                    rtValue = getRegister(machineOutput, lineCount, rtString, false);

                    //retrieve label
                    string labelName;
                    assemblyInput >> labelName;
                    char callTag = 65;
                    char labelFormat = 'C'; //used later to reduce bits for conditional branch
                    bool set = false;
                    do {
                      if(labelCallMap[labelName + callTag + labelFormat] == 0) {
                        map<string, int>::iterator changeElement = labelCallMap.find(labelName + callTag + labelFormat);
                        if(changeElement != labelCallMap.end())
                          changeElement->second = lineCount;
                        else
                          labelCallMap.insert(pair<string, int> (labelName + callTag + labelFormat, lineCount));
                        set = true;
                      }
                      else
                        callTag++;
                    } while(!set);
                    finalFileOutput += labelName + callTag + labelFormat + " " + rtValue.to_string() + "\n";
                }
            }
        }
        //handle comments and eof: trash this and next input
        else if(commandString == "#" || commandString == ""){
            string trash;
            assemblyInput >> trash;
            lineCount--;
        }
        //if it is a label, save the label name and the index of the next line in the code
        else if(commandString.back() == ':') {
          labelDeclarationMap[commandString.substr(0, commandString.length() - 1)] = lineCount;
          lineCount--;
        }
        //if not a proper instruction/comment/label, error out for reason invalid instruction/label
        else
            errorOutAndExit(machineOutput, lineCount, "\'" + commandString + "\' is not a valid Instruction Name or Label.");
    }
    //resolve label names, or error out if a label was called but never declared
    for(map<string, int>::iterator i = labelCallMap.begin(); i != labelCallMap.end(); i++) {
      string labelName = i->first.substr(0, i->first.length() - 2); //get the actual label name without the id and format tag
      //if a label was called but never declared, error out
      if(labelDeclarationMap[labelName] == 0)
        errorOutAndExit(machineOutput, lineCount, "\'" + i->first.substr(0, i->first.length() - 2) + "\' was used as a label in code but was never declared.");
      bitset<21> resolvedLabelCode;
      int instructionDistance = labelDeclarationMap[labelName] - i->second;
      //if the distance is negative, do extra operations to get two's complement of number
      if(instructionDistance < 0) {
        resolvedLabelCode = bitset<21>(instructionDistance * -1);
        resolvedLabelCode.flip();
        int i = 0;
        while(resolvedLabelCode.test(i)) {
          resolvedLabelCode.reset(i);
          i++;
        }
        resolvedLabelCode.set(i);
      }
      else
        resolvedLabelCode = bitset<21>(instructionDistance); //create binary for the number of instructions that the program must jump to branch
      //reduce bits from 26 to 21 if this is a conditional branch label
      if(i->first.at(i->first.length() - 1) == 'B') {
        bitset<26> resolvedBranchingLabelCode = bitset<26> (resolvedLabelCode.to_ulong());
        //replace the label string in the final output with the resolved binary
        int indexOfLabelCall = finalFileOutput.find(i->first);
        finalFileOutput.replace(indexOfLabelCall, i->first.length(), resolvedBranchingLabelCode.to_string());
      }
      else {
        //replace the label string in the final output with the resolved binary
        int indexOfLabelCall = finalFileOutput.find(i->first);
        finalFileOutput.replace(indexOfLabelCall, i->first.length(), resolvedLabelCode.to_string());
      }
    }
    assemblyInput.close();
    machineOutput << finalFileOutput << "\n-- End of Program --";
    machineOutput.close();
    cout << "Success! Machine code written to " << assemblyFileName.substr(0, assemblyFileName.find(".")) + "_output.txt!" << endl;
}

bitset<5> getRegister(ofstream& output, int lineNumber, string registerString, bool isEnd) {
  bitset<5> registerValue;
  if(isEnd) {
    if(registerString.length() != 2 && registerString.length() != 3)
      errorOutAndExit(output, lineNumber, "\'" + registerString + "\' invalid length for register reference");
  }
  else {
    if(registerString.length() != 3 && registerString.length() != 4)
      errorOutAndExit(output, lineNumber, "\'" + registerString + "\' invalid length for register reference");
  }
  if(registerString.at(0) != 'R' && registerString.at(0) != 'X')
      errorOutAndExit(output, lineNumber, "\'" + registerString + "\' is not a proper way to reference a register");
  else if(!isEnd && registerString.at(registerString.length() - 1) != ',')
      errorOutAndExit(output, lineNumber, "\'" + registerString + "\' missing comma");
  if(isdigit(registerString.at(1))) {
      string registerValueString;
      if(registerString.length() == 4) {
          if(isdigit(registerString.at(2)))
              registerValueString = registerString.substr(1, 2);
          else
              errorOutAndExit(output, lineNumber, "\'" + registerString + "\' is not a proper way to reference a register");
      }
      else
          registerValueString = registerString.substr(1, 1);
      stringstream registerValueStream;
      int registerValueInt;
      registerValueStream << registerValueString;
      registerValueStream >> registerValueInt;
      registerValue = bitset<5> (registerValueInt);
      if(registerValue.to_ulong() < 0 || registerValue.to_ulong() > 31)
          errorOutAndExit(output, lineNumber, "\'" + registerString + "\' referenced a register number that does not exist");
  }
  else
      errorOutAndExit(output, lineNumber, "\'" + registerString + "\' is not a proper way to reference a register");
  return registerValue;
}

//precondition: output is the stream to the output file
//postcondition: writes to output and cl that there was an error at line lineNumber, and includes a reason.
void errorOutAndExit(ofstream& output, int lineNumber, string reason) {
    output << "ERROR on line " << lineNumber << ": " << reason << endl;
    cout << "ERROR on line " << lineNumber << ": " << reason << endl;
    exit(0);
}
