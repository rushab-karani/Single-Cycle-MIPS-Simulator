#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.
// #define ADDU 1
// #define SUBU 3
// #define AND 4
// #define OR  5
// #define NOR 7
#define M 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the 
// memory is still 32-bit addressable.
#define N 1000 // dumpout dmem size

struct IFStruct {
    bitset<32>  PC;
    bool        nop;
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem;
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
    bool        wrt_enable;
    bool        nop;
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem;
    bool        wrt_enable;
    bool        nop;
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
public:
    bitset<32> Reg_data;
    RF()
    {
        Registers.resize(32);
        Registers[0] = bitset<32>(00000000000000000000000000000000);
        // TODO: For Testing add-add Dependencies // Remove
        //Registers[2] = bitset<32>(00000000000000000000000000000001);

    }

    bitset<32> readRF(bitset<5> Reg_addr)
    {
        Reg_data = Registers[Reg_addr.to_ulong()];
        return Reg_data;
    }

    void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
    {
        Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
    }

    void outputRF()
    {
        ofstream rfout;
        rfout.open("RFresult.txt", std::ios_base::app);
        if (rfout.is_open())
        {
            rfout << "State of RF:\t" << endl;
            for (int j = 0; j < 32; j++)
            {
                rfout << Registers[j] << endl;
            }
        }
        else cout << "Unable to open file";
        rfout.close();
    }

private:
    vector<bitset<32> >Registers;
};

class ALU
{
public:
    bitset<32> ALUresult;
    bitset<32> ALUOperation(bool ALUOP, bitset<32> oprand1, bitset<32> oprand2)
    {
        unsigned int result = 0;
        // switch (ALUOP.to_ulong())
        // {

        // case ADDU: result = oprand1.to_ulong() + oprand2.to_ulong(); break;
        // case SUBU: result = oprand1.to_ulong() - oprand2.to_ulong(); break;
        // }

        if (ALUOP)
            result = oprand1.to_ulong() + oprand2.to_ulong();
        else
            result = oprand1.to_ulong() - oprand2.to_ulong();

        return ALUresult = bitset<32>(result);
    }
};

class INSMem
{
public:
    bitset<32> Instruction;
    INSMem()
    {
        IMem.resize(MemSize);
        ifstream imem;
        string line;
        int i = 0;
        imem.open("imem.txt");
        if (imem.is_open())
        {
            while (getline(imem, line))
            {
                IMem[i] = bitset<8>(line);
                i++;
            }
        }
        else cout << "Unable to open file";
        imem.close();
    }

    bitset<32> readInstr(bitset<32> ReadAddress)
    {
        string insmem;
        insmem.append(IMem[ReadAddress.to_ulong()].to_string());
        insmem.append(IMem[ReadAddress.to_ulong() + 1].to_string());
        insmem.append(IMem[ReadAddress.to_ulong() + 2].to_string());
        insmem.append(IMem[ReadAddress.to_ulong() + 3].to_string());
        Instruction = bitset<32>(insmem);       //read instruction memory
        return Instruction;
    }

private:
    vector<bitset<8> > IMem;
};

class DataMem
{
public:
    bitset<32> ReadData;
    DataMem()
    {
        DMem.resize(MemSize);
        ifstream dmem;
        string line;
        int i = 0;
        dmem.open("dmem.txt");
        if (dmem.is_open())
        {
            while (getline(dmem, line))
            {
                DMem[i] = bitset<8>(line);
                i++;
            }
        }
        else cout << "Unable to open file";
        dmem.close();
    }

    bitset<32> readDataMem(bitset<32> Address)
    {
        string datamem = "";
        datamem.append(DMem[Address.to_ulong()].to_string());
        datamem.append(DMem[Address.to_ulong() + 1].to_string());
        datamem.append(DMem[Address.to_ulong() + 2].to_string());
        datamem.append(DMem[Address.to_ulong() + 3].to_string());
        ReadData = bitset<32>(datamem);     //read data memory
        return ReadData;
    }

    void writeDataMem(bitset<32> Address, bitset<32> WriteData)
    {
        DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0, 8));
        DMem[Address.to_ulong() + 1] = bitset<8>(WriteData.to_string().substr(8, 8));
        DMem[Address.to_ulong() + 2] = bitset<8>(WriteData.to_string().substr(16, 8));
        DMem[Address.to_ulong() + 3] = bitset<8>(WriteData.to_string().substr(24, 8));
    }

    void outputDataMem()
    {
        ofstream dmemout;
        dmemout.open("dmemresult.txt");
        if (dmemout.is_open())
        {
            for (int j = 0; j < 1000; j++)
            {
                dmemout << DMem[j] << endl;
            }

        }
        else cout << "Unable to open file";
        dmemout.close();
    }

private:
    vector<bitset<8> > DMem;
};

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate << "State after executing cycle:\t" << cycle << endl;

        printstate << "IF.PC:\t" << state.IF.PC.to_ulong() << endl;
        printstate << "IF.nop:\t" << state.IF.nop << endl;

        printstate << "ID.Instr:\t" << state.ID.Instr << endl;
        printstate << "ID.nop:\t" << state.ID.nop << endl;

        printstate << "EX.Read_data1:\t" << state.EX.Read_data1 << endl;
        printstate << "EX.Read_data2:\t" << state.EX.Read_data2 << endl;
        printstate << "EX.Imm:\t" << state.EX.Imm << endl;
        printstate << "EX.Rs:\t" << state.EX.Rs << endl;
        printstate << "EX.Rt:\t" << state.EX.Rt << endl;
        printstate << "EX.Wrt_reg_addr:\t" << state.EX.Wrt_reg_addr << endl;
        printstate << "EX.is_I_type:\t" << state.EX.is_I_type << endl;
        printstate << "EX.rd_mem:\t" << state.EX.rd_mem << endl;
        printstate << "EX.wrt_mem:\t" << state.EX.wrt_mem << endl;
        printstate << "EX.alu_op:\t" << state.EX.alu_op << endl;
        printstate << "EX.wrt_enable:\t" << state.EX.wrt_enable << endl;
        printstate << "EX.nop:\t" << state.EX.nop << endl;

        printstate << "MEM.ALUresult:\t" << state.MEM.ALUresult << endl;
        printstate << "MEM.Store_data:\t" << state.MEM.Store_data << endl;
        printstate << "MEM.Rs:\t" << state.MEM.Rs << endl;
        printstate << "MEM.Rt:\t" << state.MEM.Rt << endl;
        printstate << "MEM.Wrt_reg_addr:\t" << state.MEM.Wrt_reg_addr << endl;
        printstate << "MEM.rd_mem:\t" << state.MEM.rd_mem << endl;
        printstate << "MEM.wrt_mem:\t" << state.MEM.wrt_mem << endl;
        printstate << "MEM.wrt_enable:\t" << state.MEM.wrt_enable << endl;
        printstate << "MEM.nop:\t" << state.MEM.nop << endl;

        printstate << "WB.Wrt_data:\t" << state.WB.Wrt_data << endl;
        printstate << "WB.Rs:\t" << state.WB.Rs << endl;
        printstate << "WB.Rt:\t" << state.WB.Rt << endl;
        printstate << "WB.Wrt_reg_addr:\t" << state.WB.Wrt_reg_addr << endl;
        printstate << "WB.wrt_enable:\t" << state.WB.wrt_enable << endl;
        printstate << "WB.nop:\t" << state.WB.nop << endl;
    }
    else cout << "Unable to open file";
    printstate.close();
}

unsigned long shiftbits(bitset<32> inst, int start)
{
    unsigned long ulonginst;
    return ((inst.to_ulong()) >> start);

}

bitset<32> signextend(bitset<16> imm)
{
    string sestring;
    if (imm[15] == 0) {
        sestring = "0000000000000000" + imm.to_string<char, std::string::traits_type, std::string::allocator_type>();
    }
    else {
        sestring = "1111111111111111" + imm.to_string<char, std::string::traits_type, std::string::allocator_type>();
    }
    return (bitset<32>(sestring));

}


int main()
{
    stateStruct state, newState;

    // instruction
   // bitset<32> pc=0;
   // bitset<32> instruction;
    bitset<6> opcode;
    bitset<6> funct;
    bitset<16> imm;

    //control signals
    // bitset<1> IType;
    // bitset<1> JType;
    bitset<1> RType;
    bitset<1> IsBranch;
    // bitset<1> IsLoad;
    // bitset<1> IsStore;
    // bitset<1> WrtEnable;
    bool stall = false;
    bool if_stall_done = false;
    bool load_add_dep = false;
    bool add_add_dep = false;
    bool add_inst_add_dep = false;
    bool add_ldr_dep = false;
    bool add_inst_ldr_dep = false;
    bool ldr_inst_add_dep = false;
    bool ldr_str_dep = false;
    bool add_str_dep = false;
    bool add_inst_str_dep = false;
    bool branch_done = false;

    // RF signals
    bitset<5> RReg1;
    bitset<5> RReg2;
    bitset<5> WReg;
    bitset<32> WData;

    // pc signals
    bitset<1> IsEq;
    bitset<32> pcplusfour;
    bitset<32> braddr;

    // ALU signals
    bitset<32> ALUin1;
    bitset<32> ALUin2;
    bitset<32> signext;

    int cycle = 0;

    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;

    state.WB.nop = 1;
    state.MEM.nop = 1;
    state.EX.nop = 1;
    state.ID.nop = 1;
    state.IF.nop = 1;
    newState.WB.nop = 1;
    newState.MEM.nop = 1;
    newState.EX.nop = 1;
    newState.ID.nop = 1;
    newState.IF.nop = 1;


    while (1) {

        /* --------------------- WB stage --------------------- */

        if (state.WB.nop == 0)
        {
            if (state.WB.wrt_enable)
            {
                myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
                // myRF.outputRF();
            }
            myRF.outputRF();
        }

        /* --------------------- MEM stage --------------------- */

        if (state.MEM.nop == 0)
        {
            if (add_str_dep == 0 && ldr_str_dep == 0)
            {
                newState.WB.Wrt_data = (state.MEM.rd_mem) ? myDataMem.readDataMem(state.MEM.ALUresult) : state.MEM.ALUresult;
            }
            //Add-store dependency // Mem-Mem
            if (add_str_dep)
            {
                if (state.MEM.Rt == state.WB.Wrt_reg_addr)
                {
                    state.MEM.Store_data = state.WB.Wrt_data;
                }
                add_str_dep = false;
            }
            /*else
            {
                newState.WB.Wrt_data = (state.MEM.rd_mem) ? myDataMem.readDataMem(state.MEM.ALUresult) : state.MEM.ALUresult;
            }*/

            //Load-store dependency // Mem-Mem
            if (ldr_str_dep)
            {
                if (state.MEM.Rt == state.WB.Wrt_reg_addr)
                {
                    state.MEM.Store_data = state.WB.Wrt_data;
                }
                ldr_str_dep = false;
            }
            //newState.WB.Wrt_data = (state.MEM.rd_mem) ? myDataMem.readDataMem(state.MEM.ALUresult) : state.MEM.ALUresult;
            if (state.MEM.wrt_mem)
                myDataMem.writeDataMem(state.MEM.ALUresult, state.MEM.Store_data);

            newState.WB.Rs = state.MEM.Rs;
            newState.WB.Rt = state.MEM.Rt;
            newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
            newState.WB.wrt_enable = state.MEM.wrt_enable;
        }

        newState.WB.nop = newState.MEM.nop;

        /* --------------------- EX stage --------------------- */
        if (state.EX.nop == 0)
        {
            //add-store Dependency // MEM-Ex 
            if (add_inst_str_dep)
            {
                if (state.EX.Rt == state.WB.Wrt_reg_addr)
                {
                    state.EX.Read_data2 = state.WB.Wrt_data;
                }
                if (state.MEM.Rs == state.WB.Wrt_reg_addr)
                {
                    state.EX.Read_data1 = state.WB.Wrt_data;
                }
                add_inst_str_dep = false;
            }
            if (load_add_dep)
            {
                load_add_dep = false;
                //load-Add Dependencies // MEM-EX
                if (state.EX.Rs == state.WB.Wrt_reg_addr)
                    state.EX.Read_data1 = state.WB.Wrt_data;
                else if (state.EX.Rt == state.WB.Wrt_reg_addr)
                    state.EX.Read_data2 = state.WB.Wrt_data;
            }
            if (add_add_dep)
            {
                add_add_dep = false;
                //Add-Add Dependencies // EX-EX
                if (state.EX.Rs == state.MEM.Wrt_reg_addr)
                    state.EX.Read_data1 = state.MEM.ALUresult;
                else if (state.EX.Rt == state.MEM.Wrt_reg_addr)
                    state.EX.Read_data2 = state.MEM.ALUresult;
            }
            if (add_inst_add_dep)
            {
                add_inst_add_dep = false;
                //Add-Add Dependencies // MEM-EX
                if (state.EX.Rs == state.WB.Wrt_reg_addr)
                    state.EX.Read_data1 = state.WB.Wrt_data;
                else if (state.EX.Rt == state.WB.Wrt_reg_addr)
                    state.EX.Read_data2 = state.WB.Wrt_data;
            }
            if (add_ldr_dep)
            {
                add_ldr_dep = false;
                //Add-ldr Dependencies // EX-EX
                if (state.EX.Rs == state.MEM.Wrt_reg_addr)
                    state.EX.Read_data1 = state.MEM.ALUresult;
                else if (state.EX.Rt == state.MEM.Wrt_reg_addr)
                    state.EX.Read_data2 = state.MEM.ALUresult;
            }
            if (add_inst_ldr_dep)
            {
                add_inst_ldr_dep = false;
                //Add-Add Dependencies // MEM-EX
                if (state.EX.Rs == state.WB.Wrt_reg_addr)
                    state.EX.Read_data1 = state.WB.Wrt_data;
                else if (state.EX.Rt == state.WB.Wrt_reg_addr)
                    state.EX.Read_data2 = state.WB.Wrt_data;
            }
            if (ldr_inst_add_dep)
            {
                ldr_inst_add_dep = false;
                //Add-Add Dependencies // MEM-EX
                if (state.EX.Rs == state.WB.Wrt_reg_addr)
                    state.EX.Read_data1 = state.WB.Wrt_data;
                else if (state.EX.Rt == state.WB.Wrt_reg_addr)
                    state.EX.Read_data2 = state.WB.Wrt_data;
            }


            ALUin1 = state.EX.Read_data1;
            ALUin2 = (state.EX.is_I_type) ? signext : state.EX.Read_data2;
            newState.MEM.ALUresult = myALU.ALUOperation(state.EX.alu_op, ALUin1, ALUin2);
            newState.MEM.Store_data = state.EX.Read_data2;
            newState.MEM.Rs = state.EX.Rs;
            newState.MEM.Rt = state.EX.Rt;
            newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
            newState.MEM.rd_mem = state.EX.rd_mem;
            newState.MEM.wrt_mem = state.EX.wrt_mem;
            newState.MEM.wrt_enable = state.EX.wrt_enable;
        }
        //load-store Dependency // Mem-Mem
        if (state.MEM.rd_mem == 1 && newState.MEM.wrt_mem == 1 && state.MEM.wrt_enable == 1 && newState.MEM.wrt_enable == 0)
        {
            ldr_str_dep = 1;
        }
        //Add-store Dependency // Mem-Mem
        if (state.MEM.wrt_enable == 1 && newState.EX.wrt_mem == 1 && state.MEM.wrt_mem == 0 && newState.EX.wrt_enable == 0)
        {
            add_str_dep = 1;
        }
        newState.MEM.nop = newState.EX.nop;

        /* --------------------- ID stage --------------------- */

        opcode = bitset<6>(shiftbits(state.ID.Instr, 26));

        if (state.ID.nop == 0)
        {

            if_stall_done = false;
            RType = (opcode.to_ulong() == 0) ? 1 : 0;
            newState.EX.is_I_type = (opcode.to_ulong() != 0 && opcode.to_ulong() != 2) ? 1 : 0;
            // JType = (opcode.to_ulong()==2)?1:0;
            IsBranch = (opcode.to_ulong() == 4) ? 1 : 0;
            newState.EX.rd_mem = (opcode.to_ulong() == 35) ? 1 : 0;
            newState.EX.wrt_mem = (opcode.to_ulong() == 43) ? 1 : 0;
            newState.EX.wrt_enable = (newState.EX.wrt_mem || IsBranch.to_ulong()) ? 0 : 1;


            funct = bitset<6>(shiftbits(state.ID.Instr, 0));
            newState.EX.Rs = bitset<5>(shiftbits(state.ID.Instr, 21));
            newState.EX.Rt = bitset<5>(shiftbits(state.ID.Instr, 16));
            newState.EX.Wrt_reg_addr = (newState.EX.is_I_type) ? newState.EX.Rt : bitset<5>(shiftbits(state.ID.Instr, 11));
            // state.EX.alu_op = (opcode.to_ulong()==35 || opcode.to_ulong()==43)?(bitset<3>(string("001"))):bitset<3> (shiftbits(state.ID.Instr, 0));
            if (newState.EX.rd_mem || newState.EX.wrt_mem || (funct.to_ulong() == 33))
                newState.EX.alu_op = 1;
            else if (funct.to_ulong() == 35)
                newState.EX.alu_op = 0;

            newState.EX.Imm = bitset<16>(shiftbits(state.ID.Instr, 0));
            signext = signextend(newState.EX.Imm);


            newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
            newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt);


            IsEq = (newState.EX.Read_data1.to_ulong() == newState.EX.Read_data2.to_ulong()) ? 1 : 0;
            if (IsEq == 0 && IsBranch == 1)
            {
                //state.ID.nop = 1;
                braddr = bitset<32>(
                    state.IF.PC.to_ulong()
                    + (bitset<32>((bitset<30>(shiftbits(signext, 0))).to_string<char, std::string::traits_type, std::string::allocator_type>() + "00")).to_ulong());
                //newState.IF.PC = 1;
            }

        }
        //Add-Add Dependency    // Ex-Ex    
        if (newState.EX.wrt_enable == 1 && state.EX.wrt_enable == 1 && state.EX.rd_mem == 0 && newState.EX.rd_mem == 0)
        {
            add_add_dep = 1;
        }
        // Add-Add dependency // Mem-Ex
        if (newState.EX.wrt_enable == 1 && state.MEM.wrt_enable == 1 && state.EX.rd_mem == 0 && state.MEM.rd_mem == 0)
        {
            add_inst_add_dep = 1;
        }
        //Add-Ldr Dependency    // EX-EX
        if (state.EX.wrt_enable == 1 && newState.EX.rd_mem == 1 && newState.EX.wrt_enable == 1 && state.EX.rd_mem == 0)
        {
            add_ldr_dep = 1;
        }
        //Add-Ldr Dependency // Mem-Ex
        if (state.MEM.wrt_enable == 1 && newState.EX.rd_mem == 1 && state.EX.rd_mem == 0 && newState.MEM.wrt_enable == 1)
        {
            add_inst_ldr_dep = 1;
        }
        //Load-Add dependency // Mem-Ex
        if (state.MEM.rd_mem == 1 && newState.EX.wrt_enable == 1 && state.MEM.wrt_enable == 1 && newState.EX.rd_mem == 0)
        {
            ldr_inst_add_dep = 1;
        }
        //Add-store Dependency // Mem-EX
        if (state.WB.wrt_enable == 1 && newState.EX.wrt_mem == 1 && newState.EX.wrt_enable == 0)
        {
            add_inst_str_dep = 1;
        }

        //Load-add Dependency // stall + Mem-EX 
        if (if_stall_done == false)
        {
            if (newState.EX.wrt_enable == 1 && state.EX.rd_mem == 1 && state.EX.wrt_enable == 1 && newState.EX.rd_mem == 0)
            {
                if ((newState.EX.Rs == newState.MEM.Wrt_reg_addr) || (newState.EX.Rt == newState.MEM.Wrt_reg_addr))
                {
                    stall = true;
                    if_stall_done = true;
                    load_add_dep = true;
                }
            }
        }
        if (stall)
        {
            newState.ID = state.ID;
            newState.IF = state.IF;
            newState.EX.nop = 1;
            //newState.ID.nop = 1;
            newState.IF.nop = 0;
            state.IF.nop = 1;
            //state.ID.nop = 1;
            stall = false;
        }
        else
        {
            newState.EX.nop = newState.ID.nop;
        }
        /* --------------------- IF stage --------------------- */

        if (state.IF.PC == 0)
        {
            state.IF.nop = 0;
        }
        else if (branch_done)
        {
            newState.IF.PC = braddr;
            state.IF.nop = 0;
            branch_done = false;
            IsBranch = 0;
        }
        if (state.IF.nop == 0)
        {
            newState.ID.Instr = myInsMem.readInstr(state.IF.PC);
            if (newState.ID.Instr.to_string<char, std::string::traits_type, std::string::allocator_type>() == "11111111111111111111111111111111")
            {
                newState.IF.nop = 1;
            }
            else if (IsEq == 0 && IsBranch == 1)
            {
                newState.IF.PC = braddr;
                newState.IF.nop = 1;
                branch_done = true;
                newState.ID.Instr = state.ID.Instr;
            }
            else
            {
                newState.IF.PC = state.IF.PC.to_ulong() + 4;
                newState.IF.nop = 0;
            }

            // pc
            //pcplusfour = state.IF.PC.to_ulong() + 4;

            //newState.IF.PC = (IsBranch.to_ulong() == 1 && IsEq.to_ulong() == 0) ? braddr : pcplusfour);

        }
        newState.ID.nop = newState.IF.nop;

        printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 

        state = newState; /*The end of the cycle and updates the current state with the values calculated in this cycle */

        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
            break;

        cycle += 1;
    }

    //myRF.outputRF(); // dump RF;    
    myDataMem.outputDataMem(); // dump data mem 

    return 0;
}
