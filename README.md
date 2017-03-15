Copyright © 2009-2010, 2016-2017 Dmitry Schelkunov. All rights reserved.
Contacts: <d.schelkunov@gmail.com>, <http://dschelkunov.blogspot.com/>

EVHEN
-----
EVHEN is a fastest asymmetric white-box cipher which is based on the chaos theory, 
the theory of symmetric SPN-ciphers and on the method of concealing of a linear relationship 
(read more in LRC-method.pdf or at https://eprint.iacr.org/2010/419). 

It allows both encryption and signing of messages with a speed of a classical block cipher.

EVHEN is named in honor of two greatest mathematicians: Evariste Galois and Jules Henri Poincare.

DEPENDENCIES
------------
A method of generating of chaotic s-boxes is based on Asim, M., Jeoti, V.: Efficient and simple method for designing chaotic S-boxes. ETRI Journal 30(1), 170–172 (2008)

For opearions over high precision floating point numbers MPIR library is used: http://www.mpir.org/


HOW TO COMPILE
--------------
Open white_box.sln witn Microsoft Visual Studio 2013 or later. Build wb_creator. Enjoy.


USAGE
-----

USAGE: wb_creator.exe number_of_rounds min_number_of_mixes max_number_of_mixes

SAMPLE (with best practice params): wb_creator.exe 10 50 100

When the program successfully ends, it creates public and private keys (wb_encr_tbl.h and wb_decr_tbl.h header files) in the current directory.
Having wb_encr_tbl.h only (public key) it's hard to recover inverse lookup tables (private key) and to decrypt an encrypted with public key message.

To check generated keys copy wb_encr_tbl.h and wb_decr_tbl.h to the wb_sample project's directory. Then rebuild wb_sample and start it.

Note, if you change number_of_rounds, don't forget to adapt wb_sample (number of calls of encr_r and decr_r in main.cpp).

