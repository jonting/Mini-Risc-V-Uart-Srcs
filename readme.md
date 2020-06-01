# Mini Risc-V with UART

This is an FPGA implementation of a RISC-V processor, reprogrammable via serial communication. <!-- Instructions and scripts for programming it can be found in my other repository [here](https://github.com/gbruner7607/Mini-Risc-V-gcc) -->
<br>
The original Mini-Risc-V Core was designed by Md Badruddoja Majumder. I've made some modifications including implementing a reprogrammable instruction memory (currently disabled), as well as restructuring the memory module to pave the way for a more complex and comprehensive memory hierarchy. The memory structure has been altered such that instructions and data are in a shared memory space. This is a work in progress.

<hr>

## Software Toolchain

Also included in the `gcc` folder is a python script, called `pycompile.py`, that allows you to compile c code into a coe file for use with the core.  This requires the riscv toolchain to be installed, targeted for rv32i. Additionally you'll need to use the included `boot.S` and `test.ld` files, but you may supply a bootloader or linker script of your own if you wish.

`./pycompile.py <options> files`

<hr>

### Installing the Software Toolchain

In order to compile c code, you must install the RISC-V toolchain. We use the one provided by UC-Berkely for their rocket chip, because it also includes the spike simulator.

**Note that this will take a long time**

*Installing the toolchain is as follows*

```bash
$ git clone https://github.com/freechipsproject/rocket-tools
$ git submodule update --init --recursive
$ export RISCV=path/to/where/you/want/toolchain/installed
```

Next, open `build-rv32ima.sh` and edit the two lines starting with `build_project`. Change the `--with-isa=rv32ima` parameter to `--with-isa=rv32i`. Our core does not support the `m` or `a` extensions of RISC-V yet. Then just run that script.

```bash
$ ./build-rv32ima.sh
```

Now that the toolchain is installed, make sure that it's in your PATH. Type the following line, or put it in your `~/.bashrc` to make the change permanent:

```bash
export PATH="path/to/toolchain/bin:$PATH"
```

<hr>

## IP Cores

A number of IP cores are used that must be included and configured as below:

```
Clocking Wizard

Clocking Options
  Primitive: MCMM
  Clocking Features: Frequency Synthesis
  Jitter Optimization: Balanced
  Input Clock Information:
    Port Name: clk
    Input Frequency: 100
    Input Jitter 0.010

Output Clocks
  clk_out1:
    port name: clk_50M
    output freq: 50
    Duty Cycle: 50
  clk_out2
    port name: clk_5M
    output freq: 5
    Duty Cycle: 50
```

```
Block Memory Generator

Component name: blk_mem_gen_0

Basic
  Interface Type: Native
  Memory Type: True Dual Port RAM
  Check generate address interface with 32 bits
  ECC Type: No ECC
  Write Enable:
    Byte Write Enable: Checked
    Byte size: 8
  Algorithm Options:
    Minimum area

Port A Options:
  Write & Read Width: 32
  Write & Read Depth: 131072 (For Basys3: 50176)
  Operating Mode: Write First
  Enable Port Type: Use ENA pin
  Uncheck Primitves output register & RSTA pin

Port B Options:
  Set up identical to Port A

Other Options:
  Load init file: Check, and include a coe file of your choice
  Fill Remaining Memory Locations: 0
```

```
Fifo Generator

Basic
  Interface Type: Native
  Implementation: Common Clock Builtin FIFO

Native Ports
  Read Mode: Standard FIFO
  Data Port Parameters
    Write Width: 8
    Write Depth: 512
```

<hr> 

## Mini-RISC-V Program Kernel 

This feature is currently in development. It is a software kernel that is loaded by default onto the core, that allows binaries to be loaded over a serial port and executed. 

The kernel can be compiled using the `pycompile.py` script as so: 
```
./pycompile.py -s -l kernel.ld -b kboot.S -o kernel kernel.c uart.c 
```
Steps summary:

Implementation on Nexys4:
Top module file: rv_uart_top.sv
Constraint file: riscvcore.xdc

Implementation on Basys3:
Top module file: rv_uart_top_basys3.sv
Constraint file: riscvcore_basys3.xdc
Note: disable contrandicting files from the project hierarchy: rv_uart_top.sv, risvcore.xdc



->Before synthesizing
	Load the resulting `kernel.coe` file into the Block RAM IP. 
->Synthesize, Implement and generate bitstream
->Load the bit file to the FPGA using hardware manager
->After bitstream generation
	Compile the program you want to load using pycompile normally, then use `reprogram.py` to transmit the hex file to the board.
	see the help menu for reprogram.py for usage. The default port for usb is /dev/ttyUSB1 and default baudrate is 115200. You can also 		input the usb port and baud rate from the terminal while running the program. The required argument is the hex file of the program you 		want to load to the miniRISC-V core. 
  
Testing print() on 7 seg display:
Printed out integers can be seen on the 7 seg display. The debug_input[4:0] signal can be used to display printed value according to their order in the code e.g. debug_input=5'b00000 shows the most recent printed value, 5'b00001 shows the second most recent printed value and so on.

UART test:
For a quick test of the UART interface, there is a c code named uartTest.c and python application named pyterminal.py. The c code simply reads a byte from the uart receiver and writes back to the transmitter. You have to program the core with uartTest.hex using reprogram.py. On the PC end, run the pyterminal.py. It will asks user to input a character and then echo back the character sent by the uartTest.c program in the miniRISC-V core.  
