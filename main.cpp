//standard c++ headers
#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <cmath>
#include <cstring>


struct Instruction
{
    int16_t opcode = 0;
    int16_t arg1 = 0;
    int16_t arg2 = 0;
};

struct Arguments
{
    int8_t type = 0;
    int16_t a1Value = 0;
    int16_t a2Value = 0;
};

struct JumpLabel
{
    std::string name;
    int linenumber = 0;

    JumpLabel(std::string _name, int _linenumber)
    {
        name = _name;
        linenumber = _linenumber;
    }
};

struct Constant
{
    std::string name;
    int value = 0;

    Constant(std::string _name, int _value)
    {
        name = _name;
        value = _value;
    }
};

struct assetFileStruct
{
    int wordCount = 0;
    int startAddress = 0;

    assetFileStruct(int _W,int _A)
    {
        wordCount = _W;
        startAddress = _A;
    }
};

std::vector<JumpLabel> jumps;
std::vector<assetFileStruct> assetFilesData;
std::vector<Constant> constants;


enum{
    ADDRESS_ONLY = -3,
    NESTED_ONLY = -2,
    IMMEDIATE_ONLY = -1,

    ADD_IMM = 0,
    ADD_ADD = 1,
    ADD_NEST = 2,

    NEST_IMM = 3,
    NEST_ADD = 4,
    NEST_NEST = 5,

    IMM_IMM = 6,
    IMM_ADD = 7,
    IMM_NEST = 8,
    JUMP_LABEL = 9
};

std::vector<std::string> splitString(std::string ogstring)
{
    //splitting the string into instruction and arguments
    std::istringstream instructionStream(ogstring);
    std::string foundValue;
    std::vector<std::string> streamVector;

    while (std::getline(instructionStream, foundValue, ' '))
    {
        streamVector.push_back(foundValue);
    }

    return streamVector;
}

int16_t singleHexDigitToInt(char h)
{
    int16_t converted = 0;
    switch (h)
    {
    case '0':
        converted = 0;
        break;
    case '1':
        converted = 1;
        break;
    case '2':
        converted = 2;
        break;
    case '3':
        converted = 3;
        break;
    case '4':
        converted = 4;
        break;
    case '5':
        converted = 5;
        break;
    case '6':
        converted = 6;
        break;
    case '7':
        converted = 7;
        break;
    case '8':
        converted = 8;
        break;
    case '9':
        converted = 9;
        break;
    case 'A':
    case 'a':
        converted = 10;
        break;
    case 'B':
    case 'b':
        converted = 11;
        break;
    case 'C':
    case 'c':
        converted = 12;
        break;
    case 'D':
    case 'd':
        converted = 13;
        break;
    case 'E':
    case 'e':
        converted = 14;
        break;
    case 'F':
    case 'f':
        converted = 15;
        break;
    default:
        break;
    }

    return converted;
}

int16_t hexToInt(std::string hexvalue)
{
    int converted = 0;
    int lookup[4] = {1, 16, 256, 4096};
    for (int i = 0; i < (int)hexvalue.length(); i++)
    {
        converted += lookup[i] * singleHexDigitToInt(hexvalue[(hexvalue.length() - 1) - i]);
    }
    return converted;
    
}

int16_t floatToInt(std::string floatvalue)
{
    _Float16 f = (_Float16)std::stof(floatvalue);
    int16_t i = 0;
    memcpy(&i, &f, sizeof(i));
    return i;
}

int16_t intToInt(std::string intvalue)
{
    return (int16_t)std::stoi(intvalue);
}

int16_t getLinenumberFromLabel(std::string label)
{
    for (int i = 0; i < (int)jumps.size(); i++)
    {
        if(jumps[i].name == label)
        {
            return jumps[i].linenumber;
        }
    }

    return 0;
    
}

int16_t getValueFromConstant(std::string constant)
{
    for (int i = 0; i < (int)constants.size(); i++)
    {
        if(constants[i].name == constant)
        {
            return constants[i].value;
        }
    }

    return 0;
    
}

int16_t getAddressFromFile(std::string i)
{
    int fIndex = (int)std::stoi(i);
    if(fIndex < (int)assetFilesData.size())
    {
        return assetFilesData[fIndex].startAddress;
    }
    return 0;
}

int16_t getSizeFromFile(std::string i)
{
    int fIndex = (int)std::stoi(i);
    if(fIndex < (int)assetFilesData.size())
    {
        return assetFilesData[fIndex].wordCount;
    }
    return 0;
}

Arguments argumentSolver(std::string argument1, std::string argument2)
{
    Arguments args;

    int type1 = 0;
    int type2 = 0;

    if(argument1 != "")
    {
        switch (argument1[0])
        {
        case '$':
            type1 = 1;
            break;
        case '&':
            type1 = 2;
            break;
        case '%':
            type1 = 3;
            break;
        case '#':
            type1 = 4;
            args.a1Value = getLinenumberFromLabel(argument1);
            break;
        default:
            break;
        }

    
        switch (argument1[1])
        {
        case 'h':
            args.a1Value = hexToInt(argument1.substr(2, argument1.size() - 2));
            break; 
        case 'i':
            args.a1Value = intToInt(argument1.substr(2, argument1.size() - 2));
            break; 
        case 'f':
            args.a1Value = floatToInt(argument1.substr(2, argument1.size() - 2));
            break; 
        case 'a':
            args.a1Value = getAddressFromFile(argument1.substr(2, argument1.size() - 2));
            std::cout << "address 1: " << args.a1Value << "\n";
            break;
        case 's':
            args.a1Value = getSizeFromFile(argument1.substr(2, argument1.size() - 2));
            std::cout << "size 1: " << args.a1Value << "\n";
            break;
        case '?':
            args.a1Value = getValueFromConstant(argument1.substr(2, argument1.size() - 2));
            std::cout << "constant 1: \" " << argument1 << "\" value: " << args.a1Value << "\n";
            break;
        default:
            break;
        }
        
    }

    if(argument2 != "")
    {

        switch (argument2[0])
        {
        case '$':
            type2 = 1;
            break;
        case '&':
            type2 = 2;
            break;
        case '%':
            type2 = 3;
            break;
        case '#':
            type2 = 4;
            args.a2Value = getLinenumberFromLabel(argument2);
            break;
        default:
            break;
        }

        switch (argument2[1])
        {
        case 'h':
            args.a2Value = hexToInt(argument2.substr(2, argument2.size() - 2));
            break; 
        case 'i':
            args.a2Value = intToInt(argument2.substr(2, argument2.size() - 2));
            break; 
        case 'f':
            args.a2Value = floatToInt(argument2.substr(2, argument2.size() - 2));
            break; 
        case 'a':
            args.a2Value = getAddressFromFile(argument2.substr(2, argument2.size() - 2));
            std::cout << "address 2: " << args.a2Value << "\n";
            break;
        case 's':
            args.a2Value = getSizeFromFile(argument2.substr(2, argument2.size() - 2));
            std::cout << "size 2: " << args.a2Value << "\n";
            break;
        case '?':
            args.a2Value = getValueFromConstant(argument2.substr(2, argument2.size() - 2));
            std::cout << "constant 2: \" " << argument2 << "\" value: " << args.a2Value << "\n";
            break;
        default:
            break;
        }
    
    }

    if(type1 == 4 || type2 == 4){args.type = JUMP_LABEL;} //#

    if(type1 == 1 && type2 == 0){args.type = ADDRESS_ONLY;} //$
    if(type1 == 2 && type2 == 0){args.type = NESTED_ONLY;} //&
    if(type1 == 3 && type2 == 0){args.type = IMMEDIATE_ONLY;} //%

    if(type1 == 1 && type2 == 3){args.type = ADD_IMM;} //$%
    if(type1 == 1 && type2 == 1){args.type = ADD_ADD;} //$$
    if(type1 == 1 && type2 == 2){args.type = ADD_NEST;} //$&

    if(type1 == 2 && type2 == 3){args.type = NEST_IMM;} //&%
    if(type1 == 2 && type2 == 1){args.type = NEST_ADD;} //&$
    if(type1 == 2 && type2 == 2){args.type = NEST_NEST;} //&&
    
    if(type1 == 3 && type2 == 3){args.type = IMM_IMM;} //%%
    if(type1 == 3 && type2 == 1){args.type = IMM_ADD;} //%$
    if(type1 == 3 && type2 == 2){args.type = IMM_NEST;} //%&

    return args;
}


Instruction encodeInstruction(std::string splitInst[3], int linenumber)
{
    Instruction encodedInstruction;
    Arguments solvedArgs = argumentSolver(splitInst[1], splitInst[2]);

    encodedInstruction.arg1 = solvedArgs.a1Value;
    encodedInstruction.arg2 = solvedArgs.a2Value;


    if(splitInst[0] == "NOP"){return encodedInstruction;}
    else if(splitInst[0] == "SFR")
    {
        if(solvedArgs.type == IMMEDIATE_ONLY){encodedInstruction.opcode = 1;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "GFR")
    {
        if(solvedArgs.type == ADDRESS_ONLY){encodedInstruction.opcode = 2;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "CITF")
    {
        if(solvedArgs.type == ADDRESS_ONLY){encodedInstruction.opcode = 3;}
        else if(solvedArgs.type == NESTED_ONLY){encodedInstruction.opcode = 4;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "CFTI")
    {
        if(solvedArgs.type == ADDRESS_ONLY){encodedInstruction.opcode = 5;}
        else if(solvedArgs.type == NESTED_ONLY){encodedInstruction.opcode = 6;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "RAND")
    {
        if(solvedArgs.type == IMM_IMM){encodedInstruction.opcode = 7;}
        else if(solvedArgs.type == IMM_ADD){encodedInstruction.opcode = 8;}
        else if(solvedArgs.type == IMM_NEST){encodedInstruction.opcode = 9;}
        else if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 10;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 11;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 12;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 13;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 14;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 15;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "COMP")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 16;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 17;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 18;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 19;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 20;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 21;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "LODX")
    {
        if(solvedArgs.type == IMMEDIATE_ONLY){encodedInstruction.opcode = 22;}
        else if(solvedArgs.type == ADDRESS_ONLY){encodedInstruction.opcode = 23;}
        else if(solvedArgs.type == NESTED_ONLY){encodedInstruction.opcode = 24;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "LODY")
    {
        if(solvedArgs.type == IMMEDIATE_ONLY){encodedInstruction.opcode = 25;}
        else if(solvedArgs.type == ADDRESS_ONLY){encodedInstruction.opcode = 26;}
        else if(solvedArgs.type == NESTED_ONLY){encodedInstruction.opcode = 27;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "LODZ")
    {
        if(solvedArgs.type == IMMEDIATE_ONLY){encodedInstruction.opcode = 28;}
        else if(solvedArgs.type == ADDRESS_ONLY){encodedInstruction.opcode = 29;}
        else if(solvedArgs.type == NESTED_ONLY){encodedInstruction.opcode = 30;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "STOX")
    {
        if(solvedArgs.type == ADDRESS_ONLY){encodedInstruction.opcode = 31;}
        else if(solvedArgs.type == NESTED_ONLY){encodedInstruction.opcode = 32;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "STOY")
    {
        if(solvedArgs.type == ADDRESS_ONLY){encodedInstruction.opcode = 33;}
        else if(solvedArgs.type == NESTED_ONLY){encodedInstruction.opcode = 34;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "STOZ")
    {
        if(solvedArgs.type == ADDRESS_ONLY){encodedInstruction.opcode = 35;}
        else if(solvedArgs.type == NESTED_ONLY){encodedInstruction.opcode = 36;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "LODI")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 37;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 38;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "LODF")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 39;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 40;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "MOVE")
    {
        if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 41;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 42;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 43;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 44;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "SWAP")
    {
        if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 45;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 46;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 47;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 48;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "ADDI")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 49;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 50;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 51;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 52;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 53;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 54;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "ADDF")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 55;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 56;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 57;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 58;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 59;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 60;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "SUBI")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 61;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 62;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 63;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 64;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 65;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 66;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "SUBF")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 67;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 68;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 69;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 70;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 71;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 72;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "MULI")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 73;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 74;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 75;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 76;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 77;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 78;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "MULF")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 79;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 80;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 81;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 82;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 83;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 84;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "DIVI")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 85;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 86;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 87;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 88;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 89;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 90;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "DIVF")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 91;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 92;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 93;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 94;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 95;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 96;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "XOR")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 97;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 98;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 99;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 100;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 101;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 102;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "OR")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 103;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 104;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 105;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 106;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 107;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 108;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "AND")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 109;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 110;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 111;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 112;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 113;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 114;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "NOT")
    {
        if(solvedArgs.type == ADDRESS_ONLY){encodedInstruction.opcode = 115;}
        else if(solvedArgs.type == NESTED_ONLY){encodedInstruction.opcode = 116;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "ARSH")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 117;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 118;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 119;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 120;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 121;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 122;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "ALSH")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 123;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 124;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 125;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 126;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 127;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 128;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "LRSH")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 129;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 130;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 131;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 132;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 133;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 134;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "LLSH")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 135;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 136;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 137;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 138;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 139;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 140;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "RRSH")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 141;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 142;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 143;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 144;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 145;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 146;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "RLSH")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 147;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 148;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 149;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 150;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 151;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 152;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "PUSH")
    {
        if(solvedArgs.type == ADDRESS_ONLY){encodedInstruction.opcode = 153;}
        else if(solvedArgs.type == NESTED_ONLY){encodedInstruction.opcode = 154;}
        else if(solvedArgs.type == IMMEDIATE_ONLY){encodedInstruction.opcode = 155;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "POP")
    {
        if(solvedArgs.type == ADDRESS_ONLY){encodedInstruction.opcode = 156;}
        else if(solvedArgs.type == NESTED_ONLY){encodedInstruction.opcode = 157;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "BRNU")
    {
        if(solvedArgs.type == IMMEDIATE_ONLY){encodedInstruction.opcode = 158;}
        else if(solvedArgs.type == JUMP_LABEL){encodedInstruction.opcode = 158;encodedInstruction.arg1 = (encodedInstruction.arg1 - linenumber) * 3;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "BRNP")
    {
        if(solvedArgs.type == IMMEDIATE_ONLY){encodedInstruction.opcode = 159;}
        else if(solvedArgs.type == JUMP_LABEL){encodedInstruction.opcode = 159;encodedInstruction.arg1 = (encodedInstruction.arg1 - linenumber) * 3;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "BRNZ")
    {
        if(solvedArgs.type == IMMEDIATE_ONLY){encodedInstruction.opcode = 160;}
        else if(solvedArgs.type == JUMP_LABEL){encodedInstruction.opcode = 160;encodedInstruction.arg1 = (encodedInstruction.arg1 - linenumber) * 3;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "BRNN")
    {
        if(solvedArgs.type == IMMEDIATE_ONLY){encodedInstruction.opcode = 161;}
        else if(solvedArgs.type == JUMP_LABEL){encodedInstruction.opcode = 161;encodedInstruction.arg1 = (encodedInstruction.arg1 - linenumber) * 3;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "JMPU")
    {
        if(solvedArgs.type == IMMEDIATE_ONLY){encodedInstruction.opcode = 162;}
        else if(solvedArgs.type == JUMP_LABEL){encodedInstruction.opcode = 162;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "JMPP")
    {
        if(solvedArgs.type == IMMEDIATE_ONLY){encodedInstruction.opcode = 163;}
        else if(solvedArgs.type == JUMP_LABEL){encodedInstruction.opcode = 163;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "JMPZ")
    {
        if(solvedArgs.type == IMMEDIATE_ONLY){encodedInstruction.opcode = 164;}
        else if(solvedArgs.type == JUMP_LABEL){encodedInstruction.opcode = 164;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "JMPN")
    {
        if(solvedArgs.type == IMMEDIATE_ONLY){encodedInstruction.opcode = 165;}
        else if(solvedArgs.type == JUMP_LABEL){encodedInstruction.opcode = 165;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "CALL")
    {
        if(solvedArgs.type == IMMEDIATE_ONLY){encodedInstruction.opcode = 166;encodedInstruction.arg1 = (encodedInstruction.arg1 - linenumber) * 3;}
        else if(solvedArgs.type == JUMP_LABEL){encodedInstruction.opcode = 166;encodedInstruction.arg1 = (encodedInstruction.arg1 - linenumber) * 3;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "OUT")
    {
        if(solvedArgs.type == IMM_IMM){encodedInstruction.opcode = 167;}
        else if(solvedArgs.type == IMM_ADD){encodedInstruction.opcode = 168;}
        else if(solvedArgs.type == IMM_NEST){encodedInstruction.opcode = 169;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }

    else if(splitInst[0] == "IN")
    {
        if(solvedArgs.type == IMM_ADD){encodedInstruction.opcode = 170;}
        else if(solvedArgs.type == IMM_NEST){encodedInstruction.opcode = 171;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "RET")
    {
        encodedInstruction.opcode = 172;
        return encodedInstruction;
    }
    else if(splitInst[0] == "WVB")
    {
        encodedInstruction.opcode = 173;
        return encodedInstruction;
    }
    else if(splitInst[0] == "CALLJ")
    {
        if(solvedArgs.type == IMMEDIATE_ONLY){encodedInstruction.opcode = 174;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "SOUND")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 175;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
        return encodedInstruction;
    }
    else if(splitInst[0] == "COS")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 176;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 177;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 178;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 179;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 180;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 181;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
    }
    else if(splitInst[0] == "SIN")
    {
        if(solvedArgs.type == ADD_IMM){encodedInstruction.opcode = 182;}
        else if(solvedArgs.type == ADD_ADD){encodedInstruction.opcode = 183;}
        else if(solvedArgs.type == ADD_NEST){encodedInstruction.opcode = 184;}
        else if(solvedArgs.type == NEST_IMM){encodedInstruction.opcode = 185;}
        else if(solvedArgs.type == NEST_ADD){encodedInstruction.opcode = 186;}
        else if(solvedArgs.type == NEST_NEST){encodedInstruction.opcode = 187;}
        else{std::cout << "INVALID SYNTAX ON LINE " << linenumber + 1 << "\n";}
    }
    else if(splitInst[0] == "ENDPROG")
    {
        encodedInstruction.opcode = (int16_t)0xFFFF;
        encodedInstruction.arg1 = (int16_t)0xFFFF;
        encodedInstruction.arg2 = (int16_t)0xFFFF;
        return encodedInstruction;
    }

    return encodedInstruction;
}

int main(int argc, char *argv[])
{
  
    
    if (argc != 3)
    {
        std::cout << "NOT ENOUGH ARGUMENTS";
        return -1;
    }


    std::vector<Instruction> finalCode;
    std::vector<std::string> sourceLines;
    std::vector<std::string> assetFiles;

    int totalassetsOffset = 12352;

    std::ifstream sourcecode;
    sourcecode.open(argv[1]);
    if(sourcecode.is_open())
    {
        std::string line;

        while (std::getline(sourcecode, line))
        {
            bool isAsset = false;
            if(line.length() > 6)
            {
                if(
                    line[0] == 'A' && 
                    line[1] == 'S' && 
                    line[2] == 'S' && 
                    line[3] == 'E' && 
                    line[4] == 'T' && 
                    line[5] == ' '
                )
                {
                    assetFiles.push_back(line.substr(6, line.size() - 6));
                    isAsset = true;
                }
            }
            
            if(isAsset == false)
            {
                sourceLines.push_back(line);
                totalassetsOffset += 3;
            }
        }

        int assetCount = assetFiles.size();

        if(assetCount > 0)
        {
            for (int i = 0; i < assetCount; i++)
            {
                //std::cout << assetFiles[i] << " ";
                std::filesystem::path p{assetFiles[i]};
                int sizeOfFile = (int)ceil(std::filesystem::file_size(p)/2.0f);
                //std::cout << sizeOfFile << "\n";
                assetFilesData.push_back(assetFileStruct(sizeOfFile, totalassetsOffset));
                totalassetsOffset += sizeOfFile;

            }
            
        }

        for (int i = 0; i < (int)sourceLines.size(); i++)
        {
            if(sourceLines[i][0] == '#')
            {  
                jumps.push_back(JumpLabel(sourceLines[i], i));
            }
            else if(sourceLines[i].length() > 7)
            {  
                if(
                    sourceLines[i][0] == 'D' && 
                    sourceLines[i][1] == 'E' && 
                    sourceLines[i][2] == 'F' && 
                    sourceLines[i][3] == 'I' && 
                    sourceLines[i][4] == 'N' && 
                    sourceLines[i][5] == 'E' &&
                    sourceLines[i][6] == ' '
                )
                {
                    std::vector<std::string> splitConst = splitString(sourceLines[i]);
    
                    if(splitConst.size() > 2)
                    {
    
                        int actualValue = 0;
                        
                        switch (splitConst[2][0])
                        {
                            case 'h':
                                actualValue = hexToInt(splitConst[2].substr(1, splitConst[2].size() - 1));
                                break; 
                            case 'i':
                                actualValue = intToInt(splitConst[2].substr(1, splitConst[2].size() - 1));
                                break; 
                            case 'f':
                                actualValue = floatToInt(splitConst[2].substr(1, splitConst[2].size() - 1));
                                break;
                            default:
                                break;
                        }
                        
                        constants.push_back(Constant(splitConst[1], actualValue));
                        
                    }
                }
            }
            
        }
        std::cout << "Instruction count: " << (int)sourceLines.size() << "\n";
        for (int L = 0; L < (int)sourceLines.size(); L++)
        {
            std::string splitInstruction[3] = {"", "", ""};
            
            std::vector<std::string> splitvector = splitString(sourceLines[L]);
            for (int i = 0; i < (int)splitvector.size(); i++)
            {
                splitInstruction[i] = splitvector[i];
            }
            
            

            //std::cout << splitInstruction[0] << "," << splitInstruction[1] << "," << splitInstruction[2] << "\n";
            Instruction ei = encodeInstruction(splitInstruction, L);
            //std::cout << ei.opcode << "," << ei.arg1 << "," << ei.arg2 << "\n\n";

            finalCode.push_back(ei);
        }
        
        sourcecode.close();
        
        
    }
    else{std::cout<<"Failed to open file: \""<<argv[1]<<"\""; return 0;}

    std::ofstream outputStream;
    std::ifstream assetInputStream;

    outputStream.open(argv[2], std::ofstream::out);
    if(outputStream.is_open())
    {
        for(int i = 0; i < (int)finalCode.size(); i++)
        {   
            //instruction
            char highBits = (finalCode[i].opcode >> 8) & 0xFF;
            char lowBits = finalCode[i].opcode & 0xFF;

            outputStream<<highBits;
            outputStream<<lowBits;

            //arg 1
            highBits = (finalCode[i].arg1 >> 8) & 0xFF;
            lowBits = finalCode[i].arg1 & 0xFF;

            outputStream<<highBits;
            outputStream<<lowBits;

            //arg 2
            highBits = (finalCode[i].arg2 >> 8) & 0xFF;
            lowBits = finalCode[i].arg2 & 0xFF;

            outputStream<<highBits;
            outputStream<<lowBits;
            
        }
        char inputdata = 0;
        for (int i = 0; i < (int)assetFiles.size(); i++)
        {
            assetInputStream.open(assetFiles[i], std::ifstream::in);
            if(assetInputStream.is_open())
            {
                while (assetInputStream >> inputdata)
                {
                    if(assetInputStream.eof()){break;}
                    outputStream << inputdata;
                }
                
            }
            assetInputStream.close();
        }
        
        outputStream<<(char)0xFF;
        outputStream<<(char)0xFF;
        outputStream<<(char)0xFF;
        outputStream<<(char)0xFF;
        outputStream<<(char)0xFF;
        outputStream<<(char)0xFF;
        outputStream<<(char)0xFF;
        outputStream<<(char)0xFF;
        outputStream<<(char)0xFF;
        outputStream<<(char)0xFF;
        outputStream<<(char)0xFF;
        outputStream<<(char)0xFF;

        outputStream.close();
        std::cout << "Finished writing to file: \"" << argv[2] << "\"";
    }
    else{std::cout<<"Failed to write to file: \""<<argv[2]<<"\""; return 0;}
    
    
    return 0; 
    
}

