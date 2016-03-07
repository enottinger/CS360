all: webtest
webtest: timeclient.cpp
	g++ -o webtest -g timeclient.cpp

client: client.cpp
	g++ -o client client.cpp

timeclient: timeclient.cpp
	g++ -o timeclient -g timeclient.cpp

run: 
	./timeclient mclement.us 80 /foo.html 20

debug:
	./timeclient mclement.us 80 /foo.html 20 -d
