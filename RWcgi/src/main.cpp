#include <cstdlib>
#include <cstdio>
#include <exception>
#include <iostream>
using namespace std;


#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>


void process_cgi_request()
{
    try{
        cgicc::Cgicc cgi;
        ofstream outfile("/var/log/strava.log",ios::app | ios::out | ios::ate | ios::binary);

    outfile << "State " << cgi("state") << endl;        
    outfile << "Code " << cgi("code") << endl;        
    outfile.close();

    cout << "State " << cgi("state") << endl;        
    cout << "Code " << cgi("code") << endl;        
    
    //call curl
    
        
        
    }
    catch (exception& e){
        printf("Exception occured: %s\n",e.what());    
    }

}



int main(int argc, char** argv)
{
    cout << cgicc::HTTPHTMLHeader() << endl;
    process_cgi_request();

    return 0;
}