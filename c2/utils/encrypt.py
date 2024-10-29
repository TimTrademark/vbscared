def encrypt_decrypt(data: str, key: str):
    output_text = ""
    key_length = len(key)

    for i in range(len(data)):
        # Get the ASCII code of the current character in input_text
        char_code = ord(data[i])

        # Get the ASCII code of the corresponding character in the key (cycled)
        key_code = ord(key[i % key_length])

        # XOR the character code with the key code and convert back to a character
        xor_result = chr(char_code ^ key_code)

        # Append the result to output_text
        output_text += xor_result

    return output_text