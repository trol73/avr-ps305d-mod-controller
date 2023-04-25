name = 'power-supply-controller'

src = [
   'src/*.c',
]

mcu = 'atmega8a'

frequency = 16*1000000

defines = [
   'F_CPU=16000000',
   'DEBUG=0',
]

compiler_options = ['-g2  -Os']

linker_options = []

configurations = {}

