# Green-Threads:
Work only on X86-64, tested only on Ubuntu 20.04
### Screencast:
![alt text](https://github.com/Acool4ik/Green-threads/blob/master/images/Screencast.gif)
### Then to run project:
- `sudo apt -y install fasm`
- `make`
### Note:
- Main functiomality for switching context implemented in `*.asm` file
- Instead of interrupts using mechanism of signals with signal `SIGALARM`
- For generating signals used systemcall `setitimer`
- Handler of signal acts as function, which call switch function written in assembly
- the main thing to understand is that the handler work in thread context
### Bugs:
- Don't work custom spinlock, calling of function `lock` break down stack of thread 
- Don't work user input from console
