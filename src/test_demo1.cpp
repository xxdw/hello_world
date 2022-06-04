//
// Created by 79203 on 2022/5/28.
//

#include "test_demo1.h"
#include<iostream>
using namespace std;

void TestDemo1::ExceptionDemo1() throw(){
    cout<<"1--before try block..."<<endl;
    try{
        cout<<"2--Inside try block..."<<endl;
        throw "exception";
        //cout<<"3--After throw ...."<<endl;
    } catch (int i) {
        cout<<"4--In catch block1 ... exception..errcode  is.."<<i<<endl;
    } catch (const char * s) {
        cout<<"5--In catch block2 ... exception..errcode is.."<<s<<endl;
    }
    cout<<"6--After Catch...";
    throw 123;
}
