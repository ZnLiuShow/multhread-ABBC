// multhread.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <mutex>
#include <condition_variable>

void thread1();
void thread2();
void thread3();
void thread4();

std::mutex mtx_sc;//截图线程锁
std::condition_variable cvmtx_start;//截图线程锁条件
std::condition_variable cvmtx_sc;//截图线程锁条件
std::condition_variable cvmtx_end;//截图线程锁条件
int imgoperation = 0;//线程锁状态

int main()
{
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread1, NULL, 0, NULL);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread2, NULL, 0, NULL);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread3, NULL, 0, NULL);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread4, NULL, 0, NULL);
    getchar();
    std::cout << "Hello World!\n";
}

void thread1() {
    while (1) {
        std::unique_lock<std::mutex> lock(mtx_sc);
        if (cvmtx_start.wait_for(lock, std::chrono::milliseconds(5), [] {  Sleep(1); return  imgoperation == 0; }))
        {
            imgoperation = 0b11;
            std::cout << "this is 1!" << std::endl;;
        }
        cvmtx_sc.notify_all();
        lock.unlock();
        Sleep(10);
    }
}
void thread2() {
    int count = 0;
    while (1) {
        std::unique_lock<std::mutex> lock(mtx_sc);
        cvmtx_sc.wait(lock, [] {  return (imgoperation & 0b1) == 0b1; });
        count++;
        imgoperation |= 0b100;
        imgoperation ^= 0b1;
        std::cout << "this is 2! count:" << count << " value:" << imgoperation << std::endl;
        cvmtx_end.notify_one();
        lock.unlock();      

        Sleep(1);
    }
}
void thread3() {
    int count = 0;
    while (1) {
        std::unique_lock<std::mutex> lock(mtx_sc);
        cvmtx_sc.wait(lock, [] {  return (imgoperation & 0b10) == 0b10; });
        count++;
        imgoperation |= 0b1000;
        imgoperation ^= 0b10;
        std::cout << "this is 3! count:" << count << " value:" << imgoperation << std::endl;
        cvmtx_end.notify_one();
        lock.unlock();
        Sleep(15);
    }
}
void thread4() {
    while (1) {
        std::unique_lock<std::mutex> lock(mtx_sc);
        cvmtx_end.wait(lock, [] {  return (imgoperation & 0b1100) == 0b1100; });
        imgoperation = 0;
        std::cout << "this is 4!\n";
        cvmtx_start.notify_one();
        lock.unlock();      
        Sleep(1);
    }
}
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
