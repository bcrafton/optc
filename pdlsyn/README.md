

# pdlsyn
This repository implements a tool to compile protocol description language to verilog
### Compiler
C++.</br>
1. PDL to AST
2. AST to FSM
3. FSM + FSM -> Combined FSM
4. FSM -> Verilog</br>
### Running the code
There is a dependency script that may or may not work for getting the dependencies.</br>
Dependencies:</br>
1. bison
2. flex
3. boost

make; make run</br>
You can modify the config file and choose which PDLs to merge
