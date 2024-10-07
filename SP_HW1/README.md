1. Compile Program: 
$ make

2. Exercise 4-1 is written in my_tee.c, and Exercise 4-2 is written in my_cp.c.
   seek_io.c is used to create a file with holes.
   It is the similar code as Chapter 4 P.84, so it is the same way to use this executable file like the one in the textbook.
   
3. How to use my_tee:
e.g.
	$ cat file | ./my_tee new_file
	$ cat file | ./my_tee -a new_file
	$ ./my_tee -a new_file  # read from stdin, press Ctrl+D to end

4. How to use my_cp:
In my way, I first create a file with holes by 
	$ ./seek_io tfile s100 wabc # create 100 bytes file holes, and write "abc" after 100 bytes.
Then, see the bytes in tfile:
	$ ls -l tfile
Next, use my_cp to copy tfile into new_tfile:
	$ ./my_cp tfile new_tfile
See the bytes in new_tfile:
	$ ls -l new_tfile
Finally, use the following command to compare them and ensure they are the same:
	$ diff tfile new_tfile

