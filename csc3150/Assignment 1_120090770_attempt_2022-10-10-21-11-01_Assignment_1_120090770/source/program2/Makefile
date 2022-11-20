obj-m	:= program2.o
KVERSION := $(shell uname -r)
PWD	:= $(shell pwd)
all:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
# execute: 
# 	./program2 abort
# 	./program2 alarm
# 	./program2 bus
# 	./program2 floating
# 	./program2 hangup
# 	./program2 illegal_instr
# 	./program2 interrupt
# 	./program2 kill
# 	./program2 normal
# 	./program2 pipe
# 	./program2 quit
# 	./program2 segment_fault
# 	./program2 stop
# 	./program2 terminate
# 	./program2 trap