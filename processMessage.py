import os
if __name__ == "__main__":
    raw_input = os.sys.argv[1]
    hex_string = ""
    target = "<p>"
    i = 0
    length = len(raw_input)
    while i < length:
        if raw_input[i] == "%":
            while i < length and raw_input[i] == "%":
                hex_string += raw_input[i:i+3]
                i += 3
            target += bytes.fromhex(hex_string.replace("%", "", len(hex_string))).decode('utf8')
            hex_string = ""
        elif raw_input[i] == "+":
            target += " "
            i += 1
        else:
            target += raw_input[i]
            i += 1
    target += "</p>"
    f = open("./message.txt", "a")
    f.write(target)
    f.close();

