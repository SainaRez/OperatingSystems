OperatingSystems
Project 1 checkpoint
Author: Saina Rezvani, Josh Desmond
Date: 01/24/2020

This project includes the boring.c and the custom.c files.

Phase 1: 
The boring.c runs the following commands: whoami, last, ls -al /home

Phase:
The custom.c is taking in the commands as parameters from command line. (This file is not yet complete, but can be run)

the following was the output directly from command line:

########################

```
Running command: whoami 
saina

-- Statistics ---
Time elapsed: 0.002026 seconds
Page Faults: 1
Page Faults (reclaimed): 92
-- End of Statistics --

Running command: last 
saina    tty7         :0               Fri Jan 24 19:56    gone - no logout
reboot   system boot  4.4.203srezvani  Fri Jan 24 19:56   still running
saina    tty7         :0               Fri Jan 24 09:40 - crash  (10:15)
reboot   system boot  4.4.203srezvani  Fri Jan 24 09:40   still running
saina    tty7         :0               Thu Jan 23 12:02 - down   (05:42)
reboot   system boot  4.4.203srezvani  Thu Jan 23 12:01 - 17:44  (05:42)
saina    tty7         :0               Wed Jan 22 00:12 - crash (1+11:49)
reboot   system boot  4.4.203srezvani  Wed Jan 22 00:12 - 17:44 (1+17:31)
saina    tty7         :0               Tue Jan 21 23:41 - down   (00:02)
reboot   system boot  4.4.203srezvani  Tue Jan 21 23:41 - 23:44  (00:03)
saina    tty7         :0               Tue Jan 21 23:20 - crash  (00:20)
reboot   system boot  4.4.203srezvani  Tue Jan 21 23:20 - 23:44  (00:23)
saina    tty7         :0               Tue Jan 21 20:35 - down   (01:39)
reboot   system boot  4.4.203srezvani  Tue Jan 21 20:35 - 22:15  (01:39)
saina    tty7         :0               Tue Jan 21 12:53 - down   (07:41)
reboot   system boot  4.4.0-171-generi Tue Jan 21 12:53 - 20:35  (07:41)
saina    tty7         :0               Tue Jan 21 11:15 - down   (01:18)
reboot   system boot  4.4.0-171-generi Tue Jan 21 11:14 - 12:33  (01:18)
saina    tty7         :0               Tue Jan 21 10:25 - down   (00:31)
reboot   system boot  4.4.0-171-generi Tue Jan 21 10:25 - 10:57  (00:31)
saina    tty7         :0               Tue Jan 21 09:31 - crash  (00:53)
reboot   system boot  4.4.0-31-generic Tue Jan 21 09:31 - 10:57  (01:25)
saina    tty7         :0               Tue Jan 21 01:56 - crash  (07:34)
reboot   system boot  4.4.0-31-generic Tue Jan 21 01:56 - 10:57  (09:00)
saina    tty7         :0               Tue Jan 21 01:26 - down   (00:29)
reboot   system boot  4.4.0-31-generic Tue Jan 21 01:26 - 01:55  (00:29)

wtmp begins Tue Jan 21 01:26:33 2020

-- Statistics ---
Time elapsed: 0.004135 seconds
Page Faults: 1
Page Faults (reclaimed): 113
-- End of Statistics --

Running command: ls -al /home 
total 12
drwxr-xr-x  3 root  root  4096 Jan 21 01:22 .
drwxr-xr-x 23 root  root  4096 Jan 23 13:15 ..
drwxr-xr-x 24 saina saina 4096 Jan 24 19:56 saina

-- Statistics ---
Time elapsed: 0.001400 seconds
Page Faults: 0
Page Faults (reclaimed): 134
-- End of Statistics --

```
########################

Running boring:

$ make
$ ./boring

Running custom:

$ make
$ ./custom