#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <fstream>
#include <string>

class Logger
{
public:
    Logger(const char* filename);
    ~Logger();
    void addstr(std::string& msg);
    void getstr(std::vector<std::string>& output);
private:
    std::string m_filename;
    std::fstream m_file;
    std::shared_mutex m_sm;
};

Logger::Logger(const char* filename) : m_filename(filename)
{
    m_sm.lock();
    m_filename += ".txt";
    m_file.open(m_filename, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);     
    if (m_file.is_open())
    {
        std::cout << "File opened\n";
    }
    else
    {
        m_file.open(m_filename, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
        if (m_file.is_open()) std::cout << "File created\n";
        else std::cout << "Can't create new file\n";
    }
    m_file.close();
    m_sm.unlock();
}

Logger::~Logger()  
{
    m_sm.lock();
    if (m_file.is_open()) m_file.close();
    std::cout << "File closed\n";
    m_sm.unlock();
}

void Logger::addstr(std::string& msg)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));    
    m_sm.lock();
    m_file.open(m_filename, std::ios_base::in | std::ios_base::out);
    if (m_file.is_open())
    {
    
        m_file.seekp(0, std::ios_base::end); 
        m_file << msg << "\n";
    
    }
    else
    {
        std::cout << "File is NOT opened\n";
    }
    m_file.close();
    m_sm.unlock();
}

void Logger::getstr(std::vector<std::string>& output)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));   

    m_sm.lock();            
    m_file.open(m_filename, std::ios_base::in | std::ios_base::out);
    if (m_file.is_open())
    {
        
        m_file.seekg(0, std::ios_base::beg); 
        std::string tmp;
        while (std::getline(m_file, tmp))
        {
            output.push_back(tmp);
        }

    }
    else
    {
        std::cout << "File is NOT opened\n";
    }
    m_file.close();

    m_sm.unlock();              
}

void print_v(std::vector<std::string>& output)
{
    for (auto i : output)
    {
        std::cout << i << "\n";
    }
}

int main()
{
    Logger log("log");
    std::string one = "1";
    std::string two = "2";
    std::string three = "3";
    std::string four = "4";
    std::string five = "5";
    std::string six = "6";
    std::string seven = "7";

    std::vector<std::string> output1;
    std::vector<std::string> output2;
    std::vector<std::string> output3;

   
    std::thread write1(&Logger::addstr, &log, std::reference_wrapper<std::string>(one));
    std::thread write2(&Logger::addstr, &log, std::reference_wrapper<std::string>(two));
    std::thread write3(&Logger::addstr, &log, std::reference_wrapper<std::string>(three));

    std::thread read1(&Logger::getstr, &log, std::reference_wrapper<std::vector<std::string> >(output1));
    std::thread read2(&Logger::getstr, &log, std::reference_wrapper<std::vector<std::string> >(output2));

    std::thread write4(&Logger::addstr, &log, std::reference_wrapper<std::string>(four));
    std::thread write5(&Logger::addstr, &log, std::reference_wrapper<std::string>(five));
    std::thread write6(&Logger::addstr, &log, std::reference_wrapper<std::string>(six));
    std::thread write7(&Logger::addstr, &log, std::reference_wrapper<std::string>(seven));

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    write1.join();
    write2.join();
    write3.join();
    write4.join();
    write5.join();
    write6.join();
    write7.join();
    read1.join();
    read2.join();

    std::thread read3(&Logger::getstr, &log, std::reference_wrapper<std::vector<std::string> >(output3));
    read3.join();

    std::cout << "\nOutput 1:\n";
    print_v(output1);

    std::cout << "\nOutput 2:\n";
    print_v(output2);

    std::cout << "\nOutput 3:\n";
    print_v(output3);

    return 0;
}