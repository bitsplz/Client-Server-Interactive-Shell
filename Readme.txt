#Muhammad Mujtaba Afzal 14882
Compile the server: gcc -o <name your exe> myserver.c -lpthread
Run the server: ./<name of your server exe>
The port number assigned would be visible.

Compile the client: gcc -o <name your exe> myclient.c -lpthread
Run the client: ./<name of your client exe> localhost <portnumber>

*exit the client by only pressing enter when prompted for input.
*delimiter for multiple commands in a line is ;
*input must not end with the delimiter.
*no space should be left when delimiting.
*not case sensitive

Valid commands on client side:
1. add <list of numbers separated by space>
2. sub <list of numbers separated by space>
3. mul <list of numbers separated by space>
4. div <list of numbers separated by space>
5. run <app name>
6. kill <pid/app name>
7. list
8. print <message>

Valid commands on server side:
1. print <message for all>
   sends a message to every client
2. send <pid> <message>
   sends message to a specific client
3. list
   displays list of all clients connected
4. listof <pid>
   displays list of processes run by a client

*valid Client commands: add 2 3
                        rUn gnome-calculator
                        kill 23098
                        kill firefox;print hello
			add 2 3;run gedit;sub 4 5
*Invalid Client command:add2 3
                        kill
			add 2 3;
			add 2 3 ; run gedit
                        print
*Valid Server Commands: list
		        print hello
                        send 1234 hello
			listof 1234
