# RPI3 OS Kernel
### 已實作的功能
* 使用mini UART實現基本的輸入輸出
* Simple shell (user space)
* Bootloader & Bootloader Self Relocation
  * 使用mini UART傳輸本地kernel image至RPI3的記憶體中
* 實現系統呼叫和硬體中斷處理(e.g.CPU Core Timer Interrupt Handler)
* 實現Process Context Switch & Scheduler並以round-robin作為排程演算法
* 實現Virtual memory management
  * 實作User program loader將Simple shell code載入記憶體並為其建立page table
  * 實作User library為主程式提供進入點(entry point)以及系統呼叫(system call)並在主程式結束後呼叫exit
  * 為每一個user task建立屬於它的虛擬定址空間
# Reference
* OSDI2020作業文件：https://grasslab.github.io/osdi/en/index.html
* OSDI2023作業文件：https://oscapstone.github.io/index.html#
* 交大修課心得 - 作業系統設計與實作：https://blog.ykzheng.com/notes/nctu/osdi/
* Learning operating system development using Linux kernel and Raspberry Pi: https://github.com/s-matyukevich/raspberry-pi-os/tree/master
