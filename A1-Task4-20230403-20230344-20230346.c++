#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstring>
#include <bitset>
using namespace std;
!!!!!

class Instructor {
private:
    fstream file;
    vector<string> implement;

public:
    void Load_file() {
        while (true) {
            string fname;
            cout << "Enter the file name: ";
            getline(cin, fname);
            file.open(fname);

            if (file.is_open()) {
                cout << "The file has been opened." << endl;
                Read_file();
                file.close();
                break;
            } else {
                cout << "The file does not exist, please try again." << endl;
            }
        }
    }

    void Read_file() {
        string word;
        implement.clear();
        while (file >> word) {
            implement.push_back(word);
            if (word.substr(0, 4) == "C000") break; 
        }
    }

    bool validation() {
        for (const auto& instr : implement) {
            if (instr.size() > 4) {
                cout << "Error: invalid instruction length: " << instr << endl;
                return false;
            }
            if (!((instr[0] >= '0' && instr[0] <= '9') || (instr[0] == 'A' || instr[0] == 'B' || instr[0] == 'C' || instr[0] == 'D'))) {
                cout << "Error: invalid character in instruction: " << instr << endl;
                return false;
            }
        }
        return true;
    }

    vector<string>& getImplement() {
        return implement;
    }
};

class Register {
private:
    vector<string> reg;

public:
    Register() : reg(16, "00") {}

    vector<string>& Reg() {
        return reg;
    }
};

class Memory {
private:
    vector<string> mem;

public:
    Memory() : mem(256, "00") {}

    vector<string>& Memo() {
        return mem;
    }
};

class Machine {
public:
    Instructor inst;
    Register reg;
    Memory mem;

public:
    Machine() {
        inst.Load_file();
        if (!inst.validation()) {
            cout << "Instruction validation failed." << endl;
            exit(1);
        }
        load_instructions_to_memory();
    }

    vector<string>& instructions = inst.getImplement();
    vector<string>& REG = reg.Reg();
    vector<string>& Mem = mem.Memo();

    void load_instructions_to_memory() {
        int memory_address = 0x10;
        for (const auto& instruction : instructions) {
            Mem[memory_address] = instruction;
            memory_address += 2;
        }
    }

    string decimal_to_binary (int val) {

        string binary = "" ;
        while(val > 0){

            binary = to_string(val % 2) + binary;
            val /= 2;
        }

        if(binary.size() < 8) {
            for(int i = 0 ; i < 8-binary.size() ; i++) {
                binary.insert(0 , "0");
            }
        }

        return binary.empty() ? "0" : binary;
    }

    string hex_to_binary(const string& hex) {
        string binary = "";

        for (char hex_char : hex) {
            switch (toupper(hex_char)) {
                case '0': binary += "0000"; break;
                case '1': binary += "0001"; break;
                case '2': binary += "0010"; break;
                case '3': binary += "0011"; break;
                case '4': binary += "0100"; break;
                case '5': binary += "0101"; break;
                case '6': binary += "0110"; break;
                case '7': binary += "0111"; break;
                case '8': binary += "1000"; break;
                case '9': binary += "1001"; break;
                case 'A': binary += "1010"; break;
                case 'B': binary += "1011"; break;
                case 'C': binary += "1100"; break;
                case 'D': binary += "1101"; break;
                case 'E': binary += "1110"; break;
                case 'F': binary += "1111"; break;
                default:
                    cerr << "Invalid hexadecimal character: " << hex_char << endl;
                    return ""; 
            }
        }
        return binary;
    }

    float binary_to_float (string bin) {
        if (bin.size() != 8) {
            cerr << "Error: invalid binary string length!" << endl;
            return 0.0f;
        }

        int sign = (bin[0] == '1') ? 1 : 0;  
        int exponent = binary_to_decimal(bin.substr(1, 3)); 
        int mantissa = binary_to_decimal(bin.substr(4));  

        float result = pow(-1, sign) * pow(2, exponent - 4) * (mantissa / 16.0f); 
        return result;
    }

    int binary_to_decimal(const string& binary) {
        int decimal = 0;
        int base = 1; 

        for (int i = binary.size() - 1; i >= 0; --i) {
            if (binary[i] == '1') {
                decimal += base;
            }
            base *= 2; 
        }

        return decimal;
    }

    string decimal_to_binary_with_3bit (int expo , int n_bit) {
        string binary = "" ;
        for(int i = n_bit -1 ; i >= 0 ; i--) {

            binary += (expo & (1 << i)) ? '1' : '0';
        }
        return binary ;
    }

    string float_to_hexa(float num) {
        string sign = "0";
        int exponent = 0;
        string mantissa;
        string conversion = "0.";

        if (num == 0) {
            return "00"; 
        }

        if (num < 0) {
            sign = "1";
            num = -num;
        }

        while(num < 1.0f) {
            num *= 2.0f ;
            int length = conversion.size() ;

            if(num >= 1.0f) {
                if(num == 1.0f) {
                    conversion.insert(length , "1") ;
                    break ;
                }
                else {
                    conversion.insert(length , "1") ;
                    num = -1.0f ;
                }

            }
            else {
                conversion.insert(length , "0");
            }


        }

        int maxCount = 0;
        int currentCount = 0;

        for(char ch : conversion.substr(2)) {
            if (ch == '0') {
                currentCount++;
            } else {
                if (currentCount > maxCount) {
                    maxCount = currentCount;
                }
                currentCount = 0;
            }
        }

        if (currentCount > maxCount) {
            maxCount = currentCount;
        }

        exponent = -maxCount + 4;
        mantissa = conversion.substr(2 + maxCount);
        cout << mantissa << endl ;

        while (mantissa.size() < 4) {
            mantissa += "0";
        }

        string bin_expo = decimal_to_binary_with_3bit(exponent, 3);
        cout << bin_expo << endl ;
        string bin_hex = sign + bin_expo + mantissa;

        stringstream hexStream;
        hexStream << hex << uppercase << stoi(bin_hex, nullptr, 2); 
        string hex_result = hexStream.str();

        return hex_result;
    }


    void perform() {
        int decimal_num1, decimal_num2, decimal_num3;
        int instruction_pointer = 0x10; 

        while (instruction_pointer < Mem.size()) {
            const string& imp = Mem[instruction_pointer];
            cout << "Executing instruction at address " << hex << instruction_pointer << ": " << imp << endl;

            if (imp == "C000") {
                break; 
            }

            if (imp[0] == '1') { 
                decimal_num1 = stoi(string(1, imp[1]), nullptr, 16);
                decimal_num2 = stoi(imp.substr(2, 2), nullptr, 16);
                REG[decimal_num1] = Mem[decimal_num2];
            }
            else if (imp[0] == '2') { 
                decimal_num1 = stoi(string(1, imp[1]), nullptr, 16);
                REG[decimal_num1] = string(1, imp[2]) + imp[3];
            }
            else if (imp[0] == '3') { 
                decimal_num1 = stoi(string(1, imp[1]), nullptr, 16);
                decimal_num2 = stoi(imp.substr(2, 2), nullptr, 16);
                Mem[decimal_num2] = REG[decimal_num1];
            }
            else if (imp[0] == '4') { 
                decimal_num1 = stoi(string(1, imp[1]), nullptr, 16);
                decimal_num2 = stoi(string(1, imp[2]), nullptr, 16);
                REG[decimal_num2] = REG[decimal_num1];
            }
            else if (imp[0] == '5') { 
                decimal_num1 = stoi(string(1, imp[1]), nullptr, 16);
                decimal_num2 = stoi(string(1, imp[2]), nullptr, 16);
                decimal_num3 = stoi(string(1, imp[3]), nullptr, 16);
                int result = stoi(REG[decimal_num2], nullptr, 16) + stoi(REG[decimal_num3], nullptr, 16);
                if (result > 0xFF) result &= 0xFF; 
                REG[decimal_num1] = to_hex(result);
            }
            else if (imp[0] == '6') { 
                decimal_num1 = stoi(string(1, imp[1]), nullptr, 16); 
                decimal_num2 = stoi(string(1, imp[2]), nullptr, 16); 
                decimal_num3 = stoi(string(1, imp[3]), nullptr, 16); 

                string hexValue1 = REG[decimal_num2]; 
                string hexValue2 = REG[decimal_num3]; 

                int decimalValue1 = stoi(hexValue1, nullptr, 16); 
                int decimalValue2 = stoi(hexValue2, nullptr, 16); 

                string binValue1 = hex_to_binary(hexValue1); 
                string binValue2 = hex_to_binary(hexValue2); 

                float f1 = binary_to_float(binValue1); 
                float f2 = binary_to_float(binValue2); 

                float result = f1 + f2;

                REG[decimal_num1] = float_to_hexa(result); 
            }

            else if (imp[0] == '7') { 
                decimal_num1 = stoi(string(1, imp[1]), nullptr, 16);
                decimal_num2 = stoi(string(1, imp[2]), nullptr, 16);
                decimal_num3 = stoi(string(1, imp[3]), nullptr, 16);

                int de1 = stoull(REG[decimal_num2], nullptr, 16) ;
                int de2 = stoull(REG[decimal_num3], nullptr, 16) ;

                int result = de1 | de2 ;


                REG[decimal_num1] = to_hex(result);
            }
            else if (imp[0] == '8') { 
                decimal_num1 = stoi(string(1, imp[1]), nullptr, 16);
                decimal_num2 = stoi(string(1, imp[2]), nullptr, 16);
                decimal_num3 = stoi(string(1, imp[3]), nullptr, 16);

                int de1 = stoull(REG[decimal_num2], nullptr, 16) ;
                int de2 = stoull(REG[decimal_num3], nullptr, 16) ;

                int result = de1 & de2 ;

                REG[decimal_num1] = to_hex(result);
            }
            else if (imp[0] == '9') { 
                decimal_num1 = stoi(string(1, imp[1]), nullptr, 16);
                decimal_num2 = stoi(string(1, imp[2]), nullptr, 16);
                decimal_num3 = stoi(string(1, imp[3]), nullptr, 16);

                int de1 = stoull(REG[decimal_num2], nullptr, 16) ;
                int de2 = stoull(REG[decimal_num3], nullptr, 16) ;

                int result = de1 ^ de2 ;


                REG[decimal_num1] = to_hex(result); 
            }
            else if (imp[0] == 'A') { 
                decimal_num1 = stoi(string(1, imp[1]), nullptr, 16);
                decimal_num2 = stoi(imp.substr(2, 2), nullptr, 16);
                REG[decimal_num1] = Mem[decimal_num2]; 
            }
            else if (imp[0] == 'B') { 
                decimal_num1 = stoi(string(1, imp[1]), nullptr, 16);
                decimal_num2 = stoi(imp.substr(2, 2), nullptr, 16);
                if (REG[decimal_num1] == REG[0]) {
                    instruction_pointer = decimal_num2;
                    continue; 
                }
            }

            instruction_pointer += 2; 
        }
    }

    string to_hex(int value) {
        stringstream ss;
        ss << hex << uppercase << setw(2) << setfill('0') << (value & 0xFF);
        return ss.str();
    }

    void print() {
        cout << "Registers:" << endl;
        for(const auto& i : REG) {
            cout << i << " ";
        }
        cout << endl;
        cout << "Memory:" << endl;
        for(const auto& j : Mem) {
            cout << j << " ";
        }
        cout << endl;
    }
};

int main() {
    Machine m;
    while (true) {
        cout << "Select an option:" << endl;
        cout << "1. Load a file" << endl;
        cout << "2. Run step-by-step" << endl;
        cout << "3. Run the whole program" << endl;
        cout << "4. Exit" << endl;

        int choice;
        cin >> choice;
        cin.ignore();  

        switch (choice) {
            case 1:
                m.inst.Load_file();  
                break;
            case 2:
                m.perform(); 
                break;
            case 3:
                m.perform();
                m.print();
                break;
            case 4:
                cout << "Exiting..." << endl;
                return 0;
            default:
                cout << "Invalid choice, please try again." << endl;
                break;
        }
    }
}
