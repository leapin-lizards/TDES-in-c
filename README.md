# TDES-in-c
This is a program that loads 3 8-byte binary data files and encrypts or decryps it depending on the file name, using Triple-DES with 2 keys.   
* If the file name is set to ```plaintext.txt``` the program performs encryption.   
* If the file name is set to ```ciphertext.txt``` the program performs decryption.   
<!-- end of the list -->
The other 2 files are the 2 keys needed for Triple-DES. They must be named ```key1.txt``` and ```key2.txt```.   
The program also adds another 8 bytes to the input data as padding before encrypting/decrypting.
