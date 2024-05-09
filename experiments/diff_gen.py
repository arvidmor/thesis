
# Script for generating the diff string/file for simulating dsu's

init_addr = 0x10002

# @: address, #: no_words
shift_op = "S@-?:#w"

# @: address, #: no_words, $: data
write_op = "W@:#w$"

# print(write_op.replace("@", hex(init_addr)[2:]).replace("#", "0010").replace("$", "FFFF"*16))

# print(shift_op.replace("@", hex(init_addr)[2:]).replace("?", hex(init_addr+1022)[2:]).replace("#", "0001"))

# max = 32
# for i in range(max):
#     str = shift_op\
#             .replace("@", hex(init_addr + i*2)[2:])\
#             .replace("#", "0001") \
#         + ";"

#     if i == max-1:
#         print(str.replace(";", "") + "\";", end="")
#     elif i % 8 == 0:
#         print("\"" + str, end="")
#     elif i % 8 == 7:
#         print( str + "\"\\")
#     else :
#         print(str, end="")
