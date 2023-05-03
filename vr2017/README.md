Name: Kunal Ostwal  
Unikey: 500668171  

# COMP2017 - ASSIGNMENT 1

All file operations are done by reading byte by byte using getc(). This continues till EOF is detected.  

## File Operations on Example 1 (01.bin)


At 0:  
&nbsp;&nbsp;&nbsp;&nbsp;read 5 bytes continuously  
At 5:  
&nbsp;&nbsp;&nbsp;&nbsp;matches with _byte0_  
&nbsp;&nbsp;&nbsp;&nbsp;checks for _byte1_ by reading 1 byte  
At 6:  
&nbsp;&nbsp;&nbsp;&nbsp;matches with _byte1_  
&nbsp;&nbsp;&nbsp;&nbsp;checks for _byte2_ by reading 1 byte  
At 7:  
&nbsp;&nbsp;&nbsp;&nbsp;matches with _byte2_  
&nbsp;&nbsp;&nbsp;&nbsp;checks for _checksum_ by reading 1 byte  
At 8:  
&nbsp;&nbsp;&nbsp;&nbsp;matches with _checksum_  
&nbsp;&nbsp;&nbsp;&nbsp;trailer found  
&nbsp;&nbsp;&nbsp;&nbsp;chunk sent to chunk_processor() for analyzing and printing output  
&nbsp;&nbsp;&nbsp;&nbsp;offset value updated to 9  
EOF found. Loop broken.  


## File Operations on Example 2 (02.bin)


At 0:  
&nbsp;&nbsp;&nbsp;&nbsp;read 5 bytes continuously  
At 5:  
&nbsp;&nbsp;&nbsp;&nbsp;increase _packet_count_  
&nbsp;&nbsp;&nbsp;&nbsp;read 5 bytes continously  
At 10:  
&nbsp;&nbsp;&nbsp;&nbsp;increase _packet_count_  
&nbsp;&nbsp;&nbsp;&nbsp;read 5 bytes continously  
At 15:  
&nbsp;&nbsp;&nbsp;&nbsp;matches with _byte0_  
&nbsp;&nbsp;&nbsp;&nbsp;checks for _byte1_ by reading 1 byte  
At 16:  
&nbsp;&nbsp;&nbsp;&nbsp;matches with _byte1_  
&nbsp;&nbsp;&nbsp;&nbsp;checks for _byte2_ by reading 1 byte  
At 17:  
&nbsp;&nbsp;&nbsp;&nbsp;matches with _byte2_  
&nbsp;&nbsp;&nbsp;&nbsp;checks for _checksum_ by reading 1 byte  
At 18:  
&nbsp;&nbsp;&nbsp;&nbsp;matches with _checksum_  
&nbsp;&nbsp;&nbsp;&nbsp;trailer found  
&nbsp;&nbsp;&nbsp;&nbsp;chunk sent to chunk_processor() for analyzing and printing output  
&nbsp;&nbsp;&nbsp;&nbsp;offset value updated to 19  
At 19:  
&nbsp;&nbsp;&nbsp;&nbsp;chunk_count is increased  
&nbsp;&nbsp;&nbsp;&nbsp;read 5 bytes continuously  
At 24:  
&nbsp;&nbsp;&nbsp;&nbsp;EOF detected. while loop ended  
&nbsp;&nbsp;&nbsp;&nbsp;chunk 1 sent to chunk_processor() for analyzing and printing output  