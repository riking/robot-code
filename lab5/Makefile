F_CPU = 16000000

robot:
	avr-gcc -mmcu=atmega328p -DF_CPU=$(F_CPU) -O2 -o test.elf robot.c
	avr-objcopy -O ihex test.elf test.hex
	avr-size test.elf
sender: sender.c
	avr-gcc -mmcu=atmega328p -DF_CPU=$(F_CPU) -O2 -o test.elf sender.c
	avr-objcopy -O ihex test.elf test.hex
	avr-size test.elf

test:
	avr-gcc -mmcu=atmega328p -DF_CPU=$(F_CPU) -O2 -o test.elf motor_strength.c
	avr-objcopy -O ihex test.elf test.hex
	avr-size test.elf

clean:
	rm -rf *.elf *.o *.hex

download_mac:
	#may need to change /dev/tty to correct port on your computer
	avrdude -p m328p -P /dev/tty.usbmodem1411 -c arduino -F -u -U flash:w:test.hex

download_linux:
	# may need to change /dev/tty to correct port on your computer
	avrdude -p m328p -P /dev/ttyACM0 -c arduino -F -u -U flash:w:test.hex

download_windows:
	#may need to change COM port to correct one on your computer
	avrdude -p m328p -P com4 -c arduino -F -u -U flash:w:test.hex 
