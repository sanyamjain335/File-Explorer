#include<iostream>
#include<bits/stdc++.h>
#include<dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include<string>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h> 
#include <errno.h>  
#include <sys/wait.h>
#include<fstream>
#include <sys/ioctl.h>

using namespace std;
stack<string>visited;
stack<string>parent;

vector<string> get_directory_list(string curr_path){
    vector<string> directory_list;
    DIR *dir = opendir(curr_path.c_str());  //Pointer to first element of directory
    dirent *d_structure;    //Dirent structure
    while((d_structure=readdir(dir))!=NULL){
        string filename((d_structure->d_name)); //Convert pathname to filename
        directory_list.push_back(filename);
    }
    closedir(dir);
    return directory_list;
}

int getch(void)
{
 int ch;
 struct termios oldt;
 struct termios newt;
 tcgetattr(STDIN_FILENO, &oldt); 
 newt = oldt;  
 newt.c_lflag &= ~(ICANON | ECHO); 
 tcsetattr(STDIN_FILENO, TCSANOW, &newt);
 ch = getchar(); 
 tcsetattr(STDIN_FILENO, TCSANOW, &oldt); 
 return ch; 
}

void print(string filename){
    struct stat buf;
    const char *months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    cout<<"\u001b[33m";
    stat(filename.c_str(),&buf);
    if(filename.length()>10){
        filename.resize(10);
        cout << filename << ".." << std::string(1,'\t');;    
    }
    else
        cout << filename << std::string(1,'\t');
    cout << filename.size() << std::string(1,'\t');
    cout<<( (S_ISDIR(buf.st_mode)) ? "d" : "-");
    cout<<( (buf.st_mode & S_IRUSR) ? "r" : "-");
    cout<<( (buf.st_mode & S_IWUSR) ? "w" : "-");
    cout<<( (buf.st_mode & S_IXUSR) ? "x" : "-");
    cout<<( (buf.st_mode & S_IRGRP) ? "r" : "-");
    cout<<( (buf.st_mode & S_IWGRP) ? "w" : "-");
    cout<<( (buf.st_mode & S_IXGRP) ? "x" : "-");
    cout<<( (buf.st_mode & S_IROTH) ? "r" : "-");
    cout<<( (buf.st_mode & S_IWOTH) ? "w" : "-");
    cout<<( (buf.st_mode & S_IXOTH) ? "x" : "-") << std::string(1,'\t');
    
    struct tm *tym;
    tym = gmtime(&(buf.st_mtime));
    cout<<tym->tm_mday<<" "<<months[(tym->tm_mon)-1]<<" "<<tym->tm_hour<<":"<<tym->tm_min;
        
    cout<<"\u001b[0m";
}

void display_directories_and_files(vector<string> &list_of_directories,int start,int end){
    cout<<"\033c";
    if(list_of_directories.size() <= 25)
        end = list_of_directories.size()-1;
    // for(auto x:list_of_directories)
    //     cout << x << endl;
    for(int i=start;i<=end;i++)
    {
        print(list_of_directories[i]);
        cout<<endl;
    }
    cout<<"\u001b[1A";
}

bool is_directory(string filename){
    struct stat buf;
    stat(filename.c_str(),&buf);
    return S_ISDIR(buf.st_mode);  
}

void moveCursor(std::ostream& os, int col, int row)
{
  os << "\033[" << col << ";" << row << "H";
}

string copyFiles(string src,string dest)
{
    ifstream fs;
    ofstream ft;
    char ch;
    string mesg;
    fs.open(src);
    if(!fs)
    {
        mesg="Error in opening source file..!!";
    }
    
    ft.open(dest);
    if(!ft)
    {
        mesg="Error in opening target file..!!";
        fs.close();
        
    }
    while(fs.eof()==0)
    {
        fs>>ch;
        ft<<ch;
    }

    fs.close();
    ft.close();
    return mesg;
}

string createdirectory(string src)
{
    string mesg;
    if (mkdir(src.c_str(),0777) != 0)
    {
        mesg="directory not created successfully";
    }
    return mesg;
}

void copy_directory(string dir,string dest)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    char buffer[256];
    string  basePath = getcwd(buffer, 256);
    if((dp = opendir(dir.c_str())) == NULL) 
    {
    
    }
    chdir(dir.c_str());
    while((entry = readdir(dp)) != NULL) 
    {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)) 
        {
        if(strcmp(".",entry->d_name) == 0 ||strcmp("..",entry->d_name) == 0)
        continue;
        cout<<dir+"/"+string(entry->d_name)<<dest+"/"+string(entry->d_name);
        createdirectory(dest+"/"+string(entry->d_name));
        copy_directory(dir+"/"+string(entry->d_name),dest+"/"+string(entry->d_name));
        }
        else 
        {       
            copyFiles(dir+"/"+string(entry->d_name),dest+"/"+string(entry->d_name));
        }
    }
    chdir(basePath.c_str());
    closedir(dp);
    
}   

string createfile(string src)
{
    ofstream ftsrc;
    char ch;
    string mesg;
    
    ftsrc.open(src);
    if(!ftsrc)
    {
        mesg="Error in creating  file..!";
    }
    else 
    {
        mesg="File Created Successfully";
    }
    ftsrc.close();
    return mesg;
}

vector<string> searchFile(string dir,vector<string>&search1,string file)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    char buffer[256];
    string  basePath = getcwd(buffer, 256);
    if((dp = opendir(dir.c_str())) == NULL) 
    {
    //return search1;
    }
    chdir(dir.c_str());
    while((entry = readdir(dp)) != NULL) 
    {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)) 
        {
        if(strcmp(".",entry->d_name) == 0 ||
        strcmp("..",entry->d_name) == 0)
        continue;
        search1=searchFile(dir+"/"+string((entry->d_name)),search1,file);
        }
        else 
        {
            if(string(entry->d_name)==file)
            
                {vector<string> tokens;
                    
                    stringstream check1(dir);
                    string rawcode;
                    while(getline(check1, rawcode, '/'))
                    {
                        tokens.push_back(rawcode);
                    }   
                    search1.push_back(tokens[tokens.size()-1]);}
        }
    }
    chdir(basePath.c_str());
    closedir(dp);
    return search1;
}

string deletefile(string src)
{                              
    string mesg; 
    if( remove(src.c_str()) != 0 )
        mesg="Error deleting file" ;
    else
        mesg= "File successfully deleted" ;
    return mesg;
}

string delete_dir(string src)
{
    // DIR *dp;
    // struct dirent *entry;
    // struct stat statbuf;
    // char buffer[256];
    // string  basePath = getcwd(buffer, 256);
    // if((dp = opendir(dir.c_str())) == NULL) 
    // {
    // //return search1;
    // }
    // chdir(dir.c_str());
    // while((entry = readdir(dp)) != NULL) 
    // {
    //     lstat(entry->d_name,&statbuf);
    //     if(S_ISDIR(statbuf.st_mode)){
    //         if(remove(src.c_str()) != 0 ){
    //             mesg="Error deleting file" ;
    //             break;
    //         }
    //         else
    //             mesg= "File successfully deleted" ;    
    //     } 
        
    // }
    // return mesg;
    string mesg;
    if (rmdir(src.c_str()) != 0)
        mesg= "Error deleting folder";
    else
        mesg= "Folder successfully deleted" ;
    return mesg;
}

void setCommandModeStatus(vector<char>&commandBuffer,string mesg)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
    //cout<<"\033["<<w.ws_row-4<<";25H";
    cout<<"\u001b["<<1<<"K";
    //cout << '\b';
    //cout<<"\u001b["<<w.ws_row-7<<"A";
    cout<<"\033["<<w.ws_row<<";1H";
    //cout<<"\033["<<1<<";1H";
    cout<<"\u001b[33m";
    cout<<"Enter Command:";
    //out<<"\u001b[0m";

    cout<<mesg;
    
    cout<<"\033["<<w.ws_row<<";"<<15+mesg.length()<<"H";
    commandBuffer.clear();
}

int main(int argc, char **argv){
    cout<<"\u001b[2J"; //Clear Screen

    char buffer[256];
    string curr_path = getcwd(buffer, 256);
    const string root_path = curr_path;
    vector<string> list_of_directories;
    vector<char>commandBuffer;
    string terminal_mode = "NORMAL";
    char esc_start[] = { 0x1b, ']', '0', ';', 0 };
    char esc_end[] = { 0x07, 0 };

    list_of_directories = get_directory_list(curr_path);
    int c=chdir(curr_path.c_str());     //Set it to current directory
    if(c<0)     //Error changing directory
        cout<<"Error in changing directory";

    int start=0,end=25;
    display_directories_and_files(list_of_directories,start,end);
    int curr_cursor = list_of_directories.size();
    moveCursor(std::cout, curr_cursor,0);
    while(1){
        // for(auto x:list_of_directories)
        //     cout << x << endl;
        if(terminal_mode == "NORMAL"){
            cout << esc_start <<"************** NORMAL MODE **************"<< esc_end;
            char ch = getch();
            if (ch == '\033') { // if the first value is esc
                getch(); // skip the [
                char third_ch = getch();
                if(third_ch == 'A') { // the real value
                    // code for arrow up
                    curr_cursor--;
                    if(curr_cursor>=0)
                        moveCursor(std::cout, curr_cursor,0);
                }
                if(third_ch == 'B'){
                    curr_cursor++;
                    if(curr_cursor<=list_of_directories.size())
                        moveCursor(std::cout, curr_cursor,0);
                    // code for arrow down
                }
                if(third_ch == 'C'){}
                if(third_ch == 'D'){}
            }
            if(ch == '\n'){     //Enter Key
                if(is_directory(list_of_directories[curr_cursor-1])){
                    parent.push(curr_path);
                    curr_path = curr_path + "/" + list_of_directories[curr_cursor-1];
                    list_of_directories = get_directory_list(curr_path);
                    c=chdir(curr_path.c_str());     //Set it to current directory
                    if(c<0)     //Error changing directory
                        cout<<"Error in changing directory";
                    display_directories_and_files(list_of_directories,start,end);
                    curr_cursor = list_of_directories.size();
                    moveCursor(std::cout, curr_cursor,0);
                }
                else{
                    if (fork() == 0) {
                        execl("/usr/bin/xdg-open", "xdg-open", (list_of_directories[curr_cursor-1]).c_str(), (char *)0);
                            
                        exit(1);
                    }
                }
            }
            if(ch == 'h'){      //Home page
                curr_path = root_path;
                list_of_directories = get_directory_list(curr_path);
                c=chdir(curr_path.c_str());     //Set it to current directory
                if(c<0)     //Error changing directory
                    cout<<"Error in changing directory";
                display_directories_and_files(list_of_directories,start,end);
                curr_cursor = list_of_directories.size();
                moveCursor(std::cout, curr_cursor,0);
            }
            if(ch == 127){      //Backspace
                if(!parent.empty()){
                    curr_path = parent.top();
                    parent.pop();
                    list_of_directories = get_directory_list(curr_path);
                    c=chdir(curr_path.c_str());     //Set it to current directory
                    if(c<0)     //Error changing directory
                        cout<<"Error in changing directory";
                    display_directories_and_files(list_of_directories,start,end);
                    curr_cursor = list_of_directories.size();
                    moveCursor(std::cout, curr_cursor,0);   
                }
            }
            if(ch==58){       //Colon key
                struct winsize w;
                ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
                //cout<<"\u001b[J";
                cout<<"\u001b["<<w.ws_row-3<<"B";
                //cout<<"\033["<<50<<"B";
                //commandBuffer.clear();
                terminal_mode="COMMAND";
                cout<<"\u001b[33m";
                cout<<"Enter Command:";
                cout<<"\u001b[0m";
            }
        }
        else{
            cout << esc_start <<"************** COMMAND MODE **************"<< esc_end;
            char ch = getch();
            while(ch=getch())
            {
                if(ch!=10 && ch!=27 && ch!=127)
                {
                    cout<<ch;
                    commandBuffer.push_back(ch);
                }
                
                else if(ch == 27)       //escape key
                {
                    terminal_mode="NORMAL";

                    struct winsize w;
                    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
                    //cout<<"\u001b["<<1<<"A";
                    //cout<<"\033["<<w.ws_row-10<<";1H";
                    cout<<"\u001b["<<1<<"K";
                    cout<<"\033["<<3<<";1H";
                    display_directories_and_files(list_of_directories,start,end);
                    int curr_cursor = list_of_directories.size();
                    moveCursor(std::cout, curr_cursor,0);
                    break;
                }

                else if(ch==127)      //Backspace Key
                {
                    string mesg="";
                    setCommandModeStatus(commandBuffer,mesg);
                }
                
                else if(ch==10)         //enter key (Check for command)
                {   
                    string mesg;
                    string str=string(commandBuffer.begin(),commandBuffer.end());
                    vector<string> tokens;
                    tokens.clear();
                    stringstream check1(str);
                    string rawcode;
                    
                    while(getline(check1, rawcode, ' '))
                    {
                        tokens.push_back(rawcode);
                    }   
                    if(tokens.size()>2)
                    {           
                        if(tokens[0]=="copy")
                        {
                            string dest,basePath;
                            char buffer[256];
                            basePath = getcwd(buffer, 256);
                            dest=tokens[tokens.size()-1];
                            
                            for(unsigned int i=1;i<tokens.size()-1;i++)
                            {
                                string compSrc=basePath+"/"+tokens[i];
                                string compDest=dest+"/"+tokens[i];
                                mesg = "File copied successfully";
                                mesg=copyFiles(compSrc,compDest);
                            }
                            //  mesg="File Successfully Copied";
                            setCommandModeStatus(commandBuffer,mesg);
                        }
                        
                        else if(tokens[0]=="move")
                        {
                            string compSrc;
                            char buffer[256];
                            string basePath = getcwd(buffer, 256);
                            string dest=tokens[tokens.size()-1];
                            for(unsigned int i=1;i<tokens.size()-1;i++)
                            {
                                compSrc=basePath+"/"+tokens[i];
                                string compDest=dest+"/"+tokens[i];
                                copyFiles(compSrc,compDest);
                            
                            }
                            for(unsigned int i=1;i<tokens.size()-1;i++)
                            {
                                 compSrc=basePath+"/"+tokens[i];
                                if(remove(compSrc.c_str()) != 0)
                                    mesg="Error deleting file" ;
                                else
                                    mesg="File moved successfully";
                                
                            }
                            
                            setCommandModeStatus(commandBuffer,mesg);
                        }
                                                
                        else if(tokens[0]=="create_dir")
                        {
                            char buffer[256];
                            string basePath = getcwd(buffer, 256);
                            
                            string compSrc;
                            string dest=tokens[2];
                            if(dest==".")
                            {
                                compSrc=basePath+"/"+tokens[1];
                            }
                            else{
                                compSrc=tokens[2]+"/"+tokens[1];
                            }   
                            mesg=createdirectory(compSrc);
                            //mesg="Directory Created Successfully";
                            setCommandModeStatus(commandBuffer,mesg);
                        }
                                
                        else if(tokens[0]=="copy_dir")
                        {
                            char buffer[256];
                            string basePath = getcwd(buffer, 256);
                            
                            string compSrc,compDest;
                            string dest=tokens[2];
                        
                            compSrc=basePath+"/"+tokens[1];
                        
                            compDest=basePath+"/"+dest;
                            mesg=createdirectory(compDest);
                            copy_directory(compSrc,compDest);
                            mesg="Directory copied Successfully";
                            setCommandModeStatus(commandBuffer,mesg);
                        }
                        
                        else if(tokens[0]=="rename")
                        {
                            char buffer[256];
                            string basePath = getcwd(buffer, 256);
                            string src,dest;
                            src=tokens[1];
                            dest=tokens[2];
                        
                            string compSrc=basePath+"/"+src;
                            string compDest=basePath+"/"+dest;
                            
                            copyFiles(compSrc,compDest);
                            mesg= compSrc=basePath+"/"+src;
                            if(remove(compSrc.c_str()) != 0)
                                mesg= "Error deleting file";
                            else
                                mesg="File successfully renamed";
                            
                            
                            setCommandModeStatus(commandBuffer,mesg);
                        }
                        
                        else if(tokens[0]=="create_file")
                        {
                        
                            char buffer[256];
                            string basePath = getcwd(buffer, 256);
                            
                            string compSrc;
                            string dest=tokens[2];
                            if(dest==".")
                            {
                                compSrc=basePath+"/"+tokens[1];
                            }
                            else{
                                compSrc=tokens[2]+"/"+tokens[1];
                            }
                            mesg=createfile(compSrc);
                            //  mesg="File Created Successfully";
                            setCommandModeStatus(commandBuffer,mesg);
                        }
                    }
                    else if(tokens.size()==2)
                    {
                        string basePath; 
                            
                        if(tokens[0]=="delete_file")
                        {
                        
                            char buffer[256];
                            basePath = getcwd(buffer, 256);
                            string dest;
                            string compSrc;
                            dest=tokens[1];
                            compSrc=basePath+"/"+dest;
                            mesg=deletefile(compSrc);
                            //mesg="File Deleted Successfully";
                            setCommandModeStatus(commandBuffer,mesg);
                        }
                        
                        else if(tokens[0]=="delete_dir")
                        {
                        
                            char buffer[256];
                            basePath = getcwd(buffer, 256);
                        
                            string compSrc;
                            string dest=tokens[1];
                            compSrc=basePath+"/"+dest;
                            mesg= delete_dir(compSrc);
                            //mesg="Directory Deleted Successfully";
                            setCommandModeStatus(commandBuffer,mesg);
                        }   
                        
                        else if(tokens[0]=="goto")
                        {
                        
                            char buffer[256];
                            basePath = getcwd(buffer, 256);
                        
                            string compSrc;
                            string dest=tokens[1];
                            compSrc=basePath+"/"+dest;
                            terminal_mode="NORMAL";
                            mesg="Directory Goto Successfully";
                            //setCommandModeStatus(commandBuffer,mesg);
                            //cout<<compSrc;
                            //chdir("..");
                            chdir(compSrc.c_str());
                            //directory_list.clear();
                            //vector<string> directory_list=get_directory_list(compSrc);
                            list_of_directories=get_directory_list(compSrc);
                            int start=0,end=25;
                            cout<<"\033[3;1H";
                            display_directories_and_files(list_of_directories,start,end);
                            curr_cursor = list_of_directories.size();
                            moveCursor(std::cout, curr_cursor,0);
                            break;
                        }   
                            
                        else if(tokens[0]=="search")
                        {
                            string inputFil=tokens[1];
                            vector<string>searchResult;
                            char buffer[256];
                            basePath = getcwd(buffer, 256);
                            
                            string path=basePath;
                            searchResult=searchFile(basePath,searchResult,inputFil);
                            if(searchResult.size()==0)
                            {
                                mesg="File not Found";
                            }
                            else
                            {
                                //cout<<searchResult.size();
                                for (int i=0;i<searchResult.size();i++)
                                {
                                    mesg+=searchResult[i]+" "+"Found";
                                }
                                terminal_mode="NORMAL";
                                display_directories_and_files(searchResult,0,searchResult.size());
                                cout<<"\033[3;1H";
                                chdir(basePath.c_str());
                                //DirectryList=searchResult;
                                break;
                                
                            } 
                            chdir(basePath.c_str());
                            setCommandModeStatus(commandBuffer,mesg);
                        }
                    }
                    

                }
            
            }

        }

    }
}